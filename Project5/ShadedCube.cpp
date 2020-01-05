//////////////////////////////////////////////////////////////////////////////
//
//  SHaded Cube.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "LoadShaders.h"
#include <glm/glm.hpp> //includes GML
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>


//#include <Camera.h>

using namespace std;

// to use this example you will need to download the header files for GLM put them into a folder which you will reference in
// properties -> VC++ Directories -> libraries

enum VAO_IDs { Cube, NumVAOs = 2 };
enum Buffer_IDs { Triangles, Colours, Normals, Textures, Indices, NumBuffers = 5 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];
GLuint texture1;

GLuint shader;

GLuint NumVertices = 36;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//----------------------------------------------------------------------------
//
// loadfile
//

void loadfile(string fileName, vector<glm::vec3>& out_vertices, vector<glm::vec2>& out_uvs, vector<glm::vec3>& out_normals)
{


	string line;
	ifstream myFile("media/" + fileName + ".obj");


	vector<string> fileData;

	int verticesCount = 0;
	int texture_coordCount = 0;

	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec2> temp_uvs;
	vector<glm::vec3> temp_normals;
	vector<GLushort> elements;


	int vCount = 0;

	if (myFile.is_open())
	{


		while (getline(myFile, line))
		{
			//cout << line << endl;

			if (line.substr(0, 2) == "v ") //Vertices
			{
				glm::vec3 vertex;
				istringstream s(line.substr(2));
				s >> vertex.x; s >> vertex.y; s >> vertex.z;
				temp_vertices.push_back(vertex);

			}
			else if (line.substr(0, 2) == "vt") //Textures
			{

				glm::vec2 vertex;
				istringstream s(line.substr(3));
				s >> vertex.x; s >> vertex.y;
				temp_uvs.push_back(vertex);
			}
			else if (line.substr(0, 2) == "vn") //Normals
			{

				glm::vec3 vertex;
				istringstream s(line.substr(2));
				s >> vertex.x; s >> vertex.y; s >> vertex.z;
				temp_normals.push_back(vertex);
			}
			else if (line.substr(0, 2) == "f ") //Face Element
			{
				int counter = count(line.begin(), line.end(), '/');


				replace(line.begin(), line.end(), '/', ' ');
				istringstream fline(line.substr(2));
				GLuint num;


				GLint vTemp;
				GLint tTemp;
				GLint nTemp;

				GLint vTemp1;
				GLint tTemp1;
				GLint nTemp1;

				// Add & save 0
				fline >> num;
				vertexIndices.push_back(num);
				vTemp = num;

				fline >> num;
				uvIndices.push_back(num);
				tTemp = num;

				fline >> num;
				normalIndices.push_back(num);
				nTemp = num;

				//Add 1
				fline >> num;
				vertexIndices.push_back(num);

				fline >> num;
				uvIndices.push_back(num);

				fline >> num;
				normalIndices.push_back(num);


				//Add & save 2
				fline >> num;
				vertexIndices.push_back(num);
				vTemp1 = num;

				fline >> num;
				uvIndices.push_back(num);
				tTemp1 = num;

				fline >> num;
				normalIndices.push_back(num);
				nTemp1 = num;


				if (counter == 8) // If 8, it means there are 4 vertices for the face element
				{

					////Add 0
					vertexIndices.push_back(vTemp);
					uvIndices.push_back(tTemp);
					normalIndices.push_back(nTemp);

					////Add 2
					vertexIndices.push_back(vTemp1);
					uvIndices.push_back(tTemp1);
					normalIndices.push_back(nTemp1);

					//add 3
					fline >> num;
					vertexIndices.push_back(num);
					fline >> num;
					uvIndices.push_back(num);
					fline >> num;
					normalIndices.push_back(num);
				}

			}

		}



		for (unsigned int i = 0; i < vertexIndices.size(); i++) {


			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndices[i] - 1];
			glm::vec2 uv = temp_uvs[uvIndices[i] - 1];
			glm::vec3 normal = temp_normals[normalIndices[i] - 1];

			// Put the attributes in buffers
			out_vertices.push_back(vertex);
			out_uvs.push_back(uv);
			out_normals.push_back(normal);

		}


		cout << "\nOBJ File Read\n";
	}
	else {
		cout << "\nFile not found in media/";
	}

	myFile.close();
}


//----------------------------------------------------------------------------
//
// loadMTL
//

void loadMTL(string fileName, glm::vec4& colour, glm::vec3& out_diffuse, glm::vec3& out_specular, GLfloat& out_specularExponent, string& textureName) {

	glm::vec4 ambient; //Ka, RGB Colour
	glm::vec3 diffuse; //Kd
	glm::vec3 specular; //Ks
	GLfloat specularExponent; //Ns
	GLfloat transparency; //d
	string texture; //map_Kd, map_d
	const char* tex;

	string line;
	ifstream myMTLFile("media/" + fileName + ".mtl");

	if (myMTLFile.is_open())
	{


		while (getline(myMTLFile, line))
		{
			//cout << line << endl;

			if (line.substr(0, 2) == "Ka")
			{
				istringstream s(line.substr(2));
				s >> ambient.x; s >> ambient.y; s >> ambient.z; ambient.w = 1.0;
			}
			else if (line.substr(0, 2) == "d ")
			{
				istringstream s(line.substr(2));
				s >> ambient.w;
			}
			else if (line.substr(0, 2) == "Kd")
			{
				istringstream s(line.substr(2));
				s >> diffuse.x; s >> diffuse.y; s >> diffuse.z;
			}
			else if (line.substr(0, 2) == "Ks")
			{
				istringstream s(line.substr(2));
				s >> specular.x; s >> specular.y; s >> specular.z;
			}
			else if (line.substr(0, 2) == "Ns")
			{
				istringstream s(line.substr(2));
				s >> specularExponent;
			}
			else if (line.substr(0, 6) == "map_Kd")
			{
				istringstream s(line.substr(7));
				texture = s.str();

			}

		}

		colour = ambient;
		string texPath = "media/textures/" + texture;
		textureName = texPath;
		out_diffuse = diffuse;
		out_specular = specular;
		out_specularExponent = specularExponent;
		cout << "\nMTL File Read\n";
	}
	else {
		cout << ".mtl file not found" << endl;
	}

	myMTLFile.close();

}

//----------------------------------------------------------------------------
//
// loadTexture
//
void loadTexture(GLuint& texture, std::string texturepath)
{
	/// load and create a texture 
	// -------------------------
	
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(texturepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		cout << "Texture Loaded" << endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}


//----------------------------------------------------------------------------
//
// init
//
#define BUFFER_OFFSET(a) ((void*)(a))


void
init(vector<glm::vec3>& vertices, vector<glm::vec2>& uvs, vector<glm::vec3>& normals, glm::vec4& colour, glm::vec3& diffuse, glm::vec3& specular, GLfloat& specularExponent, string& textureName, GLuint shaderType)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Cube]);

	string fragShader = "";
	string vertShader = "";

	if (shaderType == 2)
	{ 
		//Texture - Flat
		vertShader = "media/flat.vert";
		fragShader = "media/triangles.frag";
	}
	else if (shaderType == 3)
	{
		//No Texture - Normal
		vertShader = "media/triangles.vert";
		fragShader = "media/flat.frag";
	}
	else if (shaderType == 4)
	{
		//No Texture - Flat
		vertShader = "media/flat.vert";
		fragShader = "media/flat.frag";
	}
	else if (shaderType == 5)
	{
		//Cel
		vertShader = "media/cel.vert";
		fragShader = "media/cel.frag";
	}
	else
	{
		//Textured - Normal
		vertShader = "media/triangles.vert";
		fragShader = "media/triangles.frag";
	}

	ShaderInfo  shaders[] =
	{
		{ GL_VERTEX_SHADER, vertShader.c_str() },
		{ GL_FRAGMENT_SHADER, fragShader.c_str() },
		{ GL_NONE, NULL }
	};

	shader = LoadShaders(shaders);
	glUseProgram(shader);
	
	//
    // configuring lighting
    //

    // ambient light
	glm::vec4 ambient = colour;
	//adding the Uniform to the shader
	GLuint aLoc = glGetUniformLocation(shader, "ambient");
	glUniform4fv(aLoc, 1, glm::value_ptr(ambient));

	// light object
	glm::vec3 lightPos = glm::vec3(100.0f, 25.0f, 100.0f);
	GLuint dLightPosLoc = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(dLightPosLoc, 1, glm::value_ptr(lightPos));


	// diffuse light
	glm::vec3 diffuseLight = diffuse;
	GLuint dLightLoc = glGetUniformLocation(shader, "dLight");
	glUniform3fv(dLightLoc, 1, glm::value_ptr(diffuseLight));
	
	// specular light
	glm::vec3 specularLight = specular;
	GLfloat shininess = specularExponent; //128 is max value
	GLuint sLightLoc = glGetUniformLocation(shader, "sLight");
	GLuint sShineLoc = glGetUniformLocation(shader, "sShine");
	glUniform3fv(sLightLoc, 1, glm::value_ptr(specularLight));
	glUniform1fv(sShineLoc, 1, &shininess);


	
	 NumVertices = vertices.size();
	vector<glm::vec4> color;
	for (size_t i = 0; i < NumVertices; i++)
	{
		color.push_back(colour);
	}

	

	glGenBuffers(NumBuffers, Buffers);
	
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	

	glVertexAttribPointer(Triangles, 3, GL_FLOAT,
		GL_FALSE, 0, BUFFER_OFFSET(0));
	
	//Colour Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Colours]);
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), &color[0], GL_STATIC_DRAW);
	//glBufferStorage(GL_ARRAY_BUFFER, sizeof(colours), colours, 0);

	glVertexAttribPointer(Colours, 4, GL_FLOAT,
		GL_FALSE, 0, BUFFER_OFFSET(0));


	//Colour Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normals]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(Normals, 3, GL_FLOAT,
		GL_FALSE, 0, BUFFER_OFFSET(0));

	//Texture Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Textures]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);	
	glVertexAttribPointer(Textures, 2, GL_FLOAT,
		GL_FALSE, 0, BUFFER_OFFSET(0));

	

	// load and create a texture 
	// -------------------------
	loadTexture(texture1, textureName);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);


	// creating the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	

	// creating the view matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

	// creating the projection matrix
	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 20.0f);

	// Adding all matrices up to create combined matrix
	glm::mat4 mv = view * model;


	//adding the Uniform to the shader
	int mvLoc = glGetUniformLocation(shader, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(shader, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));





	glEnableVertexAttribArray(Triangles);
	glEnableVertexAttribArray(Colours); 
	glEnableVertexAttribArray(Normals);
	glEnableVertexAttribArray(Textures);
	
}


//----------------------------------------------------------------------------
//
// display
//

void
display(GLfloat delta, GLuint objCount)
{
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	// bind textures on corresponding texture units
	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	// creating the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(delta), glm::vec3(0.2f, 1.0f, 0.7f));


	// creating the view matrix
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// Adding all matrices up to create combined matrix
	glm::mat4 mv = view * model;


	//adding the Uniform to the shader
	int mvLoc = glGetUniformLocation(shader, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(shader, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Cube 1
	glBindVertexArray(VAOs[Cube]);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glm::vec3 cubePositions[] = {
	   glm::vec3(2.0f,  5.0f, -15.0f),
	   glm::vec3(-1.5f, -2.2f, -2.5f),
	   glm::vec3(-3.8f, -2.0f, -12.3f),
	   glm::vec3(2.4f, -0.4f, -3.5f),
	   glm::vec3(-1.7f,  3.0f, -7.5f),
	   glm::vec3(1.3f, -2.0f, -2.5f),
	   glm::vec3(1.5f,  2.0f, -2.5f),
	   glm::vec3(1.5f,  0.2f, -1.5f),
	   glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	for (unsigned int i = 1; i < objCount; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, glm::vec3(0.5f));
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(delta), glm::vec3(i, 1, 10 - i)); // Have each object spin slightly differently
		glm::mat4 mv = view * model;
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}




}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	
		float cameraSpeed = 3 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


	// View Wireframe
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}


//----------------------------------------------------------------------------
//
// main
//


int
main(int argc, char** argv)
{
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	glm::vec4 colour;
	string textureName;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat specularExponent;
		



	do {

		cout << "******************" << endl;
		cout << "*  MODEL LOADER  *" << endl;
		cout << "******************" << endl;


		cout << "\nChoose Object to load: \n";
		cout << "\n1 - Creeper" << endl;
		cout << "2 - Cone" << endl;
		cout << "3 - Sphere" << endl;

		string path = "";
		//cin >> path;

		GLuint objType = 0;
		cin >> objType;
		if (objType == 1)
		{
			path = "Creeper";
		}
		else if (objType == 2)
		{
			path = "cone";
		}
		else if (objType == 3)
		{
			path = "sphere";
		}

		cout << "\nChoose how many time this object will be spawned (1 - 9): \n";
		GLuint objCount = 0;
		cin >> objCount;
		if (objCount < 1)
		{
			objCount = 1;
		}
		else if (objCount > 9)
		{
			objCount = 9;
		}

		cout << "\nChoose shader type:" << endl;
		cout << "\n1 - Textured - Normal" << endl;
		cout << "2 - Textured - Flat" << endl;
		cout << "3 - No Texture - Normal" << endl;
		cout << "4 - No Texture - Flat" << endl;
		cout << "5 - Cel" << endl;

		GLuint shaderType = -1; // = "Creeper";
		
		cin >> shaderType;

		
	
		loadfile(path, vertices, uvs, normals);
		loadMTL(path, colour, diffuse, specular, specularExponent, textureName);

		glfwInit();

		GLFWwindow* window = glfwCreateWindow(800, 600, "Shaded Cube", NULL, NULL);
		glClearColor(0, 0, 0, 0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-320, 319, -240, 239);
		glClear(GL_COLOR_BUFFER_BIT);

		glFlush();
		glfwMakeContextCurrent(window);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewInit();

		init(vertices, uvs, normals, colour, diffuse, specular, specularExponent, textureName, shaderType);

		bool isExit = false;
		GLfloat timer = 0.0f;
		while (!glfwWindowShouldClose(window))
		{

			processInput(window);
			// uncomment to draw only wireframe 
			// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			
				display(timer, objCount);
				glfwSwapBuffers(window);
				glfwPollEvents();
				timer += 1.0f;

				
		
		}

		cout << "\nFile Closed" << endl;

		glDeleteVertexArrays(1, VAOs);
		glDeleteBuffers(1, Buffers);
		glfwDestroyWindow(window);

		glfwTerminate();
		system("CLS");
		

	} while (true);
}
