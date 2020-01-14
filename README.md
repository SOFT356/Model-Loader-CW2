
<h1>Improved Model Loader</h1>

<h2>Function</h2>
This program  is an updated version of a model loader I previously worked on. As before, it loads an .obj* file and renders it in a window. 
<br>Addition features in this version:
<br>Added more shader types and option to select a shader to load with
<br>Can choose between a few object types
<br>Full camera movement implemented
<br>Can spawn an object up to 10 times
<br>Hold ‘P’ to view wireframes of objects

<br>Developed with Microsoft Visual Studio Professional 2019 using OpenGL Version 4.0.

*https://en.wikipedia.org/wiki/Wavefront_.obj_file

<h2>User Guide</h2>
<h2>How to Load Object</h2>
Step 1: Locate the "SampleProjects.sln" file and open it (Note: you must have Visual Studio installed with nupengl.core and glm imported).

Step 2: Click the green "Local Windows Debugger" button to start the program.

Step 3: Choose which object you who like to load by entering the corresponding number and press enter.

Step 4: Choose how times you what to spawn this object then enter the amount (from 1 to 10) and press enter.

Step 5: Now choose which shader you would like to use by entering the corresponding number and press enter, a window should pop up.


<h2>Controls in Render Window</h2>
<b>Camera:</b> 
<br>W - Forward 
<br>A - Left
<br>S - Back
<br>D - Right
<br>Mouse - Look around
<br>Scroll Wheel - Zoom in/out
<br><b>Other:</b>
<br>P - Hold to view wireframe
<br>ESC - Close render window

<h2>Video Walkthrough</h2>
Here is a video walkthrough of the features
<a href="http://www.youtube.com/watch?feature=player_embedded&v=DxliVPOOwY0
" target="_blank"><img src="http://img.youtube.com/vi/DxliVPOOwY0/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>



<h2>Project Starting Point</h3>
As implied by the title, this project is a continuation of a model loader I have made, however this is less focused on the “loader” part and focuses on adding more features/interactivity to the program, therefore no extra file types have had any added support.

The original model loader was only able to read and obj file and an mtl file and render it with a texture. There was no lighting, no movement, and only a single default shader.

As a note, this program is based on code provided by Swen Gaudl (https://swen.fairrats.eu)

<h3>What’s new?</h3>
<h4>Lighting</h4>
The first thing was to implement lighting. This was done by implementing what I had already done into a different project provided by Swen Gaudl that had basic lighting implemented. This meant being able to load obj and mtl files and pass through the data into the rendering pipeline.
<h4>Camera</h4>
This was implemented with the help of a tutorial from learnopengl.com. It allows the user to move around with the WASD keys, look around with the mouse and zoom in and out with the scroll with.
Additionally the object rotates on its own axis.
<h4>Spawning More Than One Object</h4>
This basically redraws the same object set in memory based on the input by the user. The object is transformed into a different set position.
<h4>Multiple Shaders</h4>
It is now possible to switch between different shaders (Normal/Flat/Cel). The user can select one before loading/rendering the object.
As an extra the user can view the wireframes of objects by holding ‘P’ down.
<h2>Code Structure</h2>
The loader and render are both in a single file.
There are six main functions:

<br>loadTexture()
<br>init()
<br>display()
<br>loadFile()
<br>loadMTL()
<br>main()

<h3>Overview</h3>
In main() the user is asked to choose an object type, spawn amount and shader type. The corresponding obj and mtl files are loaded with loadFile() and loadMTL() which extracts all the data from the files. The data is then passed into init() to create and populate buffers. After that, in a while loop, display() is called that displays the model in a window.

<h3>How data is read from an obj file</h3>
Using fstream the file is opened. Then in a while loop each line is read individually and checked for what piece of data it holds e.g. a line the begins with "vt" holds data about the texture coordinates.
Example:
<pre>
 If (line.substr(0, 2) == "v ")                         	//this line checks for vertices using .substr()
 { 
    glm::vec3 vertex;                                  		//a temp variable
    istringstream s(line.substr(2));                   	//removes the first two char i.e. "v "
    s >> vertex.x; s >> vertex.y; s >> vertex.z;         //extract data and add into temp vec3
    temp_vertices.push_back(vertex);                    //pushes back into vector
 }
</pre>

<h3>Cel Shader</h3>
The bulk of this shader happens in the fragment shader so the vertex shader only need to pass through the normal and the light direction, using these two it calculates the intensity of the light and renders that pixel based on the intensity. So, the intensity is split into bands and a specific colour is rendered based on where the intensity falls in.
<h4>Vertex Shader</h4>
<pre>
void main()
{

// view-space coordinate
    vec4 P = mv_matrix * vec4(vPosition,1.0);
	
	vec3 L = lightPos - P.xyz;
	// calc the view vector
	
	//Normalise
	L = normalize(L);

	gl_Position = p_matrix * P;

	lightDir = L;
	
	normal = vec3(mv_matrix * vec4(vNormal,0.0));

	gl_Position = p_matrix * P;
}</pre>
<h4>Fragment Shader</h4>
<pre>
void main()
{
    float intensity;
	vec4 color;
	vec3 n = normalize(normal);
	intensity = dot(vec3(lightDir),n);

	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);
	gl_FragColor = color;
	
}</pre>

<h3>Flat Shader</h3>
There are two parts to this shader. The frag shader sets the colour of the object to a single solid colour while the vert shader it makes the object “2D” by just setting the z value of the vertex to zero.
<h4>Vertex Shader</h4>
<pre>
void main()
{
    void main(void)
{
	// view-space coordinate

	vec4 t = vec4(vPosition,1.0);
	t.z = 0.0; //Flatten

      	… (rest of code has been omitted as not needed to explain how the effect works)
}</pre>

<h4>Fragment Shader</h4>
<pre>
void main()
{
    fColor = vec4(0.5, 0.4, 0.8, 1.0);
}</pre>


<h2>Code Walkthrough</h2>
Here is a video walkthrough of the code
<a href="http://www.youtube.com/watch?feature=player_embedded&v=2oAjKgI3bcw
" target="_blank"><img src="http://img.youtube.com/vi/2oAjKgI3bcw/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>

<h3>References</h3>
https://learnopengl.com/
https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/toon-shading/

