
<h1>Model Loader</h1>

<h2>Function</h2>

This program simply loads an .obj* file and renders it in a window

Developed with Microsoft Visual Studio Proessional 2019 (version 16.2.3) using OpenGL Version 4.0.


*https://en.wikipedia.org/wiki/Wavefront_.obj_file

<h2>User Guide</h2>

Step 1: Locate the "SampleProjects.sln" file and open it (Note: you must have Visual Studio installed with nupengl.core and glm imported)

Step 2: Click the green "Local Windows Debugger" button to start the program

Step 3: Type in the name of the file you want into the loader e.g. Creeper (Note: Do not include the extension name e.g. “.obj”), then press enter


Step 4: 
To close the window and loader a new model, press “q” in the console, it will then prompt you to enter a new object name.

<h2>Code Structure</h2>

The loader and render are both in a single file (ObjLoader.cpp). <br>
There are six main functions:

loadTexture()<br>
init()<br>
display()<br>
loadFile()<br>
loadMTL()<br>
main()

<b>Overview</b><br>
In main() the user is asked to input a file name, this is then passed into loadFile() and loadMTL() which extracts all the data from the files. The data is then passed into init() to create and populate buffers. After that, in a while loop, display() is called that displays the model in a window. 

<b>How data is read from an obj file</b><br>
Using fstream the file is opened. Then in a while loop each line is read individually and checked for what piece of data it holds e.g. a line the begins with "vt" holds data about the texture coordinates.<br>
<i>Example:</i>
<pre>
 if(line.substr(0, 2) == "v ")                         //this line checks for vertices using .substr()
 { 
    glm::vec3 vertex;                                  //a temp variable
    istringstream s(line.substr(2));                   //removes the first two <i>char</i> i.e. "v "
    s >> vertex.x; s >> vertex.y; s >> vertex.z;       //extract data and add into temp vec3
    temp_vertices.push_back(vertex);                   //pushes back into vector
 }</pre>
