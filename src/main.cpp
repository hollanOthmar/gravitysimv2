#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include <cmath>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/normal.hpp>
//gdal library
#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <cpl_conv.h>
#include <chrono>
#include "cube.hpp"
#include "camera.hpp"
#include <vector>

//CGAL STUFF!
//===============================================================
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL/IO/facets_in_complex_2_to_triangle_mesh.h>
#include <CGAL/IO/output_surface_facets_to_polyhedron.h>
#include <CGAL/Surface_mesh.h>

using namespace std;
using namespace chrono;

// default triangulation for Surface_mesher
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
// c2t3
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;
typedef Tr::Geom_traits GT;
typedef GT::Sphere_3 Sphere_3;
typedef GT::Point_3 Point_3;
typedef GT::FT FT;
typedef FT (*Function)(Point_3);
typedef CGAL::Implicit_surface_3<GT, Function> Surface_3;
FT sphere_function (Point_3 p) {
  const FT x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
  return x2+y2+z2-1;
}
typedef GT::Vector_3  Vector_3;
typedef CGAL::Simple_cartesian<double>                       Kernel;
//typedef Kernel::Point_3                                      Point;
typedef CGAL::Surface_mesh<Point_3> 							Mesh;
typedef CGAL::Polyhedron_3<Kernel>								Polyhedron;
//typedef Mesh::Vertex_index 								vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::face_descriptor   face_descriptor;
//typedef GT::Vector_3 Vector;
//=======================================================


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

vector<Vertex> sphereVertices;
//vector<unsigned int> sphereIndices;




//SETTINGS
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

Camera camera;
float lastFrame;
float deltaTime;


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void update();


std::vector<glm::vec3> spacegrid;
std::vector<int> gridIndicies;
std::vector<glm::vec3> testspacegrid;

void genSpaceGrid(std::vector<glm::vec3>& spacegrid);
void genSpaceGridWithIndicies(std::vector<glm::vec3>& spacegrid, std::vector<int>& indicies);

int main()
{
	Tr tr;            // 3D-Delaunay triangulation
	C2t3 c2t3 (tr);   // 2D-complex in 3D-Delaunay triangulation
	Mesh mesh;			//For storing mesh data
	Polyhedron p;
	// defining the surface
	Surface_3 surface(sphere_function,             // pointer to function
	Sphere_3(CGAL::ORIGIN, 2.)); // bounding sphere
	// Note that "2." above is the *squared* radius of the bounding sphere!
	// defining meshing criteria
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(30.,  // angular bound
														 0.1,  // radius bound
														 0.1); // distance bound
	// meshing surface
	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());
	std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";
	
	 // for each facet (triangle), print the vertices
    for(C2t3::Facet_iterator fi = c2t3.facets_begin();
            fi != c2t3.facets_end(); fi++) {

       C2t3::Cell_handle cell    = fi->first;
       int opposite_vertex_index = fi->second;
	   //compute normals
	   const Point_3& p0 = cell->vertex((opposite_vertex_index)&3)->point();
		const Point_3& p1 = cell->vertex((opposite_vertex_index+1)&3)->point();
		const Point_3& p2 = cell->vertex((opposite_vertex_index+2)&3)->point();
		const Point_3& p3 = cell->vertex((opposite_vertex_index+3)&3)->point();
		Vector_3 n = ( opposite_vertex_index % 2 == 1) ?
                           CGAL::normal(p1, p2, p3) :
                           CGAL::normal(p1, p3, p2);
		Vector_3 v1 = p1 - p0; 
						
		n = n /CGAL::sqrt(n * n);
		
		//if ( n * v1 < 0 ) { n = -n; } 
		
		
       for(int i = 0; i < 4; i++)
           if(i != opposite_vertex_index) {
//               std::cout << "(" << cell->vertex(i)->point().x()
//                         << "," << cell->vertex(i)->point().y()
//                         << "," << cell->vertex(i)->point().z() << ")"
//                         << std::endl;
						 Vertex v;
						 v.Position = glm::vec3(cell->vertex(i)->point().x(),cell->vertex(i)->point().y(),cell->vertex(i)->point().z());
						 
						
						const Point_3& av = cell->vertex(i)->point();
						//const Point_3& av = cell->vertex((opposite_vertex_index)&3)->point();
						
						auto eps = 0.1f;
						
						//std::cout << "Sphere function result: " << sphere_function(p1) << "\n";
						
						//OLD TEST
						n = (sphere_function(av + (eps * n)) < 0.0) ? -n : n;
						//NEW TEST
						//Vector_3 v1 = p1 - p0; 
						//n = (CGAL::angle(n, v1) == CGAL::ACUTE) ? -n:n;
						v.Normal = glm::vec3(n.x(),n.y(),n.z());
						sphereVertices.push_back(v);
						//v.Normal = cn;
						   
						 //CGAL::facets_in_complex_2_to_triangle_mesh(c2t3,p);
						 //vertex_descriptor = 
						 //Vector_3 vc = CGAL::Polygon_mesh_processing::compute_vertex_normal(i,mesh);
//						std::map<face_descriptor,Vector> fnormals;
//						std::map<vertex_descriptor,Vector> vnormals;
//						 CGAL::Polygon_mesh_processing::compute_normals(mesh,
//                                                 boost::make_assoc_property_map(vnormals),
//                                                 boost::make_assoc_property_map(fnormals));
           }
    }
	
	for (auto& a:sphereVertices)
     {
         std::cout<<glm::to_string(a.Position)<< "\n";
     }
	 
	 
	//========================================================================
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//creating window object
	GLFWwindow *window = glfwCreateWindow(800, 600, "GravitySim",NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glViewport(0,0,800,600);
	
	//Registering callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    //configure global opengel state
    glEnable(GL_DEPTH_TEST);
    //setup shader program
    unsigned int programID = loadShader();
    //new shader class
    Shader ourShader("../shaders/shader.vs","../shaders/shader.fs");
    Shader lightShader("../shaders/light.vs","../shaders/light.fs");
    Shader objectShader("../shaders/object.vs","../shaders/object.fs");
	Shader sphereShader("../shaders/sphere.vs","../shaders/sphere.fs");

    int nrt;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrt); 
    std::cout << "Maximum nr of vertex attributes supported: " << nrt << std::endl; 
    

    
    //vertex array 
    // float vertices[] = { 
    //     //positions      //color
    // 		0.5f, 0.5f, 0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,
    //  		0.5f, -0.5f, 0.0f,0.0f,1.0f,0.0f,1.0f,0.0f,
    //  		-0.5f,  -0.5f, 0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
    //         -0.5f, 0.5f, 0.0f,1.0f,0.0f,1.0f,0.0f,1.0f
	// };

    // unsigned int indices[] = {
    //     0,1,3,
    //     1,2,3
    // };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    unsigned int VBO, VAO,nVBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1, &VBO);
    //for storing normals, my own hack!
    glGenBuffers(1,&nVBO);

    //Bind VAO first, then bind and set vertex buffer(s) and then configure vertex attributes
    glBindVertexArray(VAO);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	//glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Vertex), &sphereVertices[0], GL_STATIC_DRAW);
    
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    
    //color attribute
    // glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,8*sizeof(float),(void *)(3*sizeof(float)));
    // glEnableVertexAttribArray(1);

    //texture attribute
    glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //normals
    glBindBuffer(GL_ARRAY_BUFFER, nVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(2);

    //load and create a texture
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D, texture);//all upcoming GL_TEXTURE_2D operations now have effect on this texture
    
    //set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    //the FileSystem::getPath(...) is part of the github repository so we can find files on any IDE/platform;
    //unsigned char* data = stbi_load(FileSystem::getPath("../textures/container.jpg").c_str(), &width, &height, &nrChannels,0);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("../textures/container.jpg", &width, &height, &nrChannels,0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    //TEXTURE 2 STUFF!
    unsigned int mytexture2;
    glGenTextures(1,&mytexture2);
    glBindTexture(GL_TEXTURE_2D, mytexture2);
    
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
	
	//SETTING UP THE SPHERE DATA
	glBindVertexArray(0);
	glm::vec3 spherePos = glm::vec3(9.0,0.0,0.0);
    //glm::vec3 lightColor = glm::vec3(1.0,1.0,1.0);

    unsigned int sphereVAO,sphereVBO;
    glGenVertexArrays(1,&sphereVAO);
    glBindVertexArray(sphereVAO);
	glGenBuffers(1, &sphereVBO);

    // we only need to bind to the VBO, the container's VBO's data already contains the correct data.
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Vertex), &sphereVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glBindVertexArray(0);
	

    //LIGHT SOURCE and other light settings
    glm::vec3 lightPos = glm::vec3(5.0,0.0,2.0);
    glm::vec3 lightColor = glm::vec3(1.0,1.0,1.0);

    unsigned int lightingVAO,lightVBO;
    glGenVertexArrays(1,&lightingVAO);
    glBindVertexArray(lightingVAO);

    // we only need to bind to the VBO, the container's VBO's data already contains the correct data.
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    // set the vertex attributes (only position data for our lamp)
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glBindVertexArray(0);

    unsigned int uniVAO, uniVBO, uniEBO;
    glGenVertexArrays(1,&uniVAO);
    glBindVertexArray(uniVAO);
    
    //gen space data
    genSpaceGrid(spacegrid);
    //test
    genSpaceGridWithIndicies(testspacegrid,gridIndicies);
	
    glGenBuffers(1,&uniVBO);
    glBindBuffer(GL_ARRAY_BUFFER,uniVBO);
    glBufferData(GL_ARRAY_BUFFER,3*4*testspacegrid.size(), &testspacegrid[0], GL_STATIC_DRAW);

    glGenBuffers(1,&uniEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,uniEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*gridIndicies.size(), &gridIndicies[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
								
    glBindVertexArray(0);

    //SETTING UP OBJECT OLD
	
    glm::vec3 objectCentre = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 objectColor = glm::vec3(0.0f,0.0f,1.0f);
    unsigned int objectVAO;
    glGenVertexArrays(1,&objectVAO);
    glBindVertexArray(objectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    // set the vertex attributes (only position data for our lamp)
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	
	//SETTING UP SPHERE
	/*
	glBindVertexArray(0);
	glm::vec3 spherePos = glm::vec3(9.0,0.0,0.0);
    //glm::vec3 lightColor = glm::vec3(1.0,1.0,1.0);

    unsigned int sphereVAO,sphereVBO;
    glGenVertexArrays(1,&sphereVAO);
    glBindVertexArray(sphereVAO);
	glGenBuffers(1, &sphereVBO);

    // we only need to bind to the VBO, the container's VBO's data already contains the correct data.
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Vertex), &sphereVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glBindVertexArray(0);*/
	

    


    /*
glGenVertexArrays(1,&VAO2);
    glGenBuffers(1,&VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, 192, modelvert, GL_STATIC_DRAW);

    //Setting attrib pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);*/
    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    //SETTING TEXTURES!!
    // ourShader.use();
    // ourShader.setInt("texture1",0);
    // ourShader.setInt("texture2",1);

    //init time frames
	lastFrame = 0.0f;
    deltaTime = 0.0f;

	while(!glfwWindowShouldClose(window))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    
        // glDepthMask(GL_TRUE);
        // glEnable(GL_DEPTH_TEST);
		
        
        //bind Texture
	
        /*glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,mytexture2);*/
	

        //SETTING TRANSFORMATION MATRIX
        /*
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        //QUERY LOCATION IN SHADER PROGRAM
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));*/
        
	
        // create transformations
        
        ourShader.use();
        // glm::mat4 model = glm::mat4(1.0f);
        // glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        
        // view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 view = glm::mat4(1.0f);
        // float radius = 10.0f;
        // float camX   = sin(glfwGetTime()) * radius;
        // float camZ   = cos(glfwGetTime()) * radius;
        // view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        //CHANGE OBJ CENTRE
        float radius = 0.5f;
        float objX   = sin(glfwGetTime()) * radius;
        float objZ   = cos(glfwGetTime()) * radius;
        objectCentre = glm::vec3(objX,0.0f,objZ);

        // std::cout<<glm::to_string(objectCentre)<< "\n";

        update();
        view = camera.getView();

        //projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        projection = camera.getProjection();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model,glm::vec3(1.0,1.0,1.0));
        // // retrieve the matrix uniform locations
        // // unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        // unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
        // unsigned int projLoc  = glGetUniformLocation(ourShader.ID, "projection");
        // // pass them to the shaders (3 different ways)
        // // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(projLoc,1,GL_FALSE, glm::value_ptr(projection));


        ourShader.setMat4f("view",view);
        ourShader.setMat4f("projection",projection);
        ourShader.setMat4f("model",model);
        ourShader.setVec3f("objectCenter",objectCentre);
        ourShader.setFloat("spaceConst",0.09f);

        // ourShader.setVec3f("lightColor",lightColor);
        // ourShader.setVec3f("lightPos",lightPos);
        // ourShader.setVec3f("viewPos", camera.getPos());

        //DRAWING SPACE
		
        glBindVertexArray(uniVAO);
        // glDrawArrays(GL_TRIANGLES,0,36);
        // glDrawArrays(GL_LINES,0,spacegrid.size());
        glDrawElements(GL_LINES, gridIndicies.size(), GL_UNSIGNED_INT, (void *)0);

        //DRAWING OBJECT
        glBindVertexArray(objectVAO);
//		glBindVertexArray(sphereVAO);
        objectShader.use();

        glm::mat4 objmodel = glm::mat4(1.0f);
        objmodel = glm::translate(objmodel,objectCentre);
        objmodel = glm::scale(objmodel,glm::vec3(0.05,0.05,0.05));
        
        objectShader.setMat4f("model",objmodel);
        //Revolution around y-axis!
        
        objectShader.setMat4f("view",view);
        objectShader.setMat4f("projection",projection);
		objectShader.setVec3f("lightColor",lightColor);
        objectShader.setVec3f("lightPos",lightPos);
        objectShader.setVec3f("viewPos", camera.getPos());
		objectShader.setVec3f("objectColor",objectColor);
		//normal matrix for object!
		glm::mat4 normalMatrix = glm::inverse(objmodel);
		normalMatrix = glm::transpose(normalMatrix);
		objectShader.setMat4f("normalMatrix",normalMatrix);

        //glDrawArrays(GL_TRIANGLES,0,36);
		glDrawArrays(GL_TRIANGLES,0,3*sphereVertices.size());

        
        //Bind vertex array and draw
        /*glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // ourShader.setMat4("model", model);
            unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glm::mat4 normalMatrix = glm::inverse(model);
            normalMatrix = glm::transpose(normalMatrix);
            ourShader.setMat4f("normalMatrix",normalMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        // drawing light
        glBindVertexArray(lightingVAO);
//		glBindVertexArray(sphereVAO);
        lightShader.use();
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        lightShader.setMat4f("model",lightModel);
        lightShader.setMat4f("view",view);
        lightShader.setMat4f("projection",projection);

        //glDrawArrays(GL_TRIANGLES,0,36);
		glDrawArrays(GL_TRIANGLES,0,3*sphereVertices.size());
		
		//drawing sphere
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(sphereVAO);
        sphereShader.use();
        glm::mat4 sphereModel = glm::mat4(1.0f);
        sphereModel = glm::translate(sphereModel, spherePos);
        sphereModel = glm::scale(sphereModel, glm::vec3(0.2f));
        sphereShader.setMat4f("model",sphereModel);
        sphereShader.setMat4f("view",view);
        sphereShader.setMat4f("projection",projection);

        glDrawArrays(GL_TRIANGLES,0,3*sphereVertices.size());
		


        // glDrawArrays(GL_TRIANGLES, 0, 36);

         //checking errors
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            cerr << "OpenGL error: " << err << endl;
        }

		glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1,&lightingVAO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    //Deleting landscape points
    //glDeleteVertexArrays(1,&VAO2);
    //glDeleteBuffers(1,&VBO2);
	
	
	// glDeleteBuffers(1, &VBO_gdal);
	// glDeleteBuffers(1, &EBO_gdal);
	
	//clear resources that were allocated!
    glfwTerminate();
    //delete the data array
    //delete[] modelvert;
	return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveFoward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.moveUp(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        camera.moveDown(deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.updateCameraDirection(xpos,ypos);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.zoom(yoffset);
}

void update()
{
    camera.update();
}

void genSpaceGrid(std::vector<glm::vec3>& spacegrid)
{
    //X-Y plane
    int N = 10;
    for (int i=0;i<N+1;++i)
        for (int j=0;j<N+1;++j)
        {
            spacegrid.push_back(glm::vec3(-0.5 + (1.0/N)*i,-0.5 + (1.0/N)*j,0.5));
            spacegrid.push_back(glm::vec3(-0.5 + (1.0/N)*i,-0.5 + (1.0/N)*j,-0.5));
        }
    //Y-Z Plane
    for (int j=0;j<N+1;++j)
        for (int k=0;k<N+1;++k)
        {
            spacegrid.push_back(glm::vec3(0.5,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
            spacegrid.push_back(glm::vec3(-0.5,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
        }
    //X-Z Plane
    for (int i=0;i<N+1;++i)
        for (int k=0;k<N+1;++k)
        {
            spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,0.5,-0.5+(1.0/N)*k));
            spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,-0.5,-0.5+(1.0/N)*k));
        }
    
    // for (auto& a:spacegrid)
    // {
    //     std::cout<<glm::to_string(a)<< "\n";
    //     std::cout << sizeof(a) << "\n";
    // }
    // std::cout << "array size: " << spacegrid.size() << "\n";
}

void genSpaceGridWithIndicies(std::vector<glm::vec3>& spacegrid, std::vector<int>& indicies)
{
    //X-Y plane
    int N = 10;
    int COUNT = 0;
    for (int i=0;i<N+1;++i)
        for (int j=0;j<N+1;++j)
        {
            int pair = 0;
            int pairCount = 0;
            int k = 0;
            while (pairCount < N){
                if(pair < 2){
                    spacegrid.push_back(glm::vec3(-0.5 + (1.0/N)*i,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
                    indicies.push_back(COUNT);
                    ++COUNT;
                    ++k;
                    ++pair;
                }
                else{
                    pair = 0;
                    ++pairCount;
                    if(pairCount == N)
                        break;
                    indicies.push_back(COUNT-1);
                    ++pair;
                }
            }
            // spacegrid.push_back(glm::vec3(-0.5 + (1.0/N)*i,-0.5 + (1.0/N)*j,0.5));
            // spacegrid.push_back(glm::vec3(-0.5 + (1.0/N)*i,-0.5 + (1.0/N)*j,-0.5));
        }
    //Y-Z Plane
    for (int j=0;j<N+1;++j)
        for (int k=0;k<N+1;++k)
        {
            int pair = 0;
            int pairCount = 0;
            int i = 0;
            while (pairCount < N){
                if(pair < 2){
                    spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
                    indicies.push_back(COUNT);
                    ++COUNT;
                    ++i;
                    ++pair;
                }
                else{
                    pair = 0;
                    ++pairCount;
                    if(pairCount == N)
                        break;
                    indicies.push_back(COUNT-1);
                    ++pair;
                }
            }
            // spacegrid.push_back(glm::vec3(0.5,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
            // spacegrid.push_back(glm::vec3(-0.5,-0.5 + (1.0/N)*j,-0.5+(1.0/N)*k));
        }
    //X-Z Plane
    for (int i=0;i<N+1;++i)
        for (int k=0;k<N+1;++k)
        {
            int pair = 0;
            int pairCount = 0;
            int j = 0;
            while (pairCount < N){
                if(pair < 2){
                    spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,-0.5+(1.0/N)*j,-0.5+(1.0/N)*k));
                    indicies.push_back(COUNT);
                    ++COUNT;
                    ++j;
                    ++pair;
                }
                else{
                    pair = 0;
                    ++pairCount;
                    if(pairCount == N)
                        break;
                    indicies.push_back(COUNT-1);
                    ++pair;
                }
            }
            // spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,0.5,-0.5+(1.0/N)*k));
            // spacegrid.push_back(glm::vec3(-0.5+(1.0/N)*i,-0.5,-0.5+(1.0/N)*k));
        }
    for (auto& a:indicies)
    {
//        std::cout<< a << "\n";
    }
    std::cout << "array size: " << spacegrid.size() << "\n";
    std::cout << "indicies size: " << indicies.size() << "\n";
}
