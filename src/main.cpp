/*
Author: Mohammad Adnaan
Class: ECE6122
Last Date Modified: 07 Dec, 2021
Description:
main function renders all objects to screen
polls the location of UAVs every 30 millisec
few blocks of codes are taken from
source: http://www.opengl-tutorial.org/beginners-tutorials/
*/
#include <iostream>
#include<thread>
#include<mutex>
#include<array>
#include<vector>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.h"
#include<ECE_UAV.h>
using namespace glm;

int main(void)
{
	GLFWwindow* window;
	mat4 myMatrix;
	vec4 myVector;
	char vertexShaderPath[] = "shaders/vertexshader.shader";
	char fragmentShaderPath[] = "shaders/fragmentshader.shader";
	char fragmentShaderSpherePath[] = "shaders/fragmentshader_sphere.shader";

	if (!glfwInit())
	{
		std::cout<<" Failed to initialize GLFW " << stderr << std::endl;
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 800, "GATECH BUZZY BOWL !", NULL, NULL);


	if (window == NULL)
	{
		std::cout << " Failed to open GLFW window. " << stderr << std::endl;
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.95f, 0.95f, 0.95f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1,&VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders(vertexShaderPath, fragmentShaderPath);
	GLuint programID_sphere = LoadShaders(vertexShaderPath, fragmentShaderSpherePath);

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	mat4 Projection = perspective(radians(60.0f), 1.0f / 1.0f, 0.1f, 120.0f);
	

	mat4 View = glm::lookAt(
		vec3(0, -1.5,1.5), // Camera is at (4,3,3), in World Space vec3(0, -1.6, 1.6)
		vec3(0,1, 0), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	mat4 Model = mat4(1.0f);

	mat4 MVP = Projection * View * Model;

	GLuint field = loadBMP_custom("models/ff.bmp");

	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLuint colorMultiplier = glGetUniformLocation(programID, "multiplier");
	glUniform1f(colorMultiplier, 1.0f);

	// Read sphere .obj file
	std::vector<glm::vec3> vertices_sphere;
	std::vector<glm::vec2> uvs_sphere;
	std::vector<glm::vec3> normals_sphere; 
	bool res = loadOBJ("models/sphere.obj", vertices_sphere, uvs_sphere, normals_sphere);

	std::vector<unsigned short> indices_sphere;
	std::vector<glm::vec3> indexed_vertices_sphere;
	std::vector<glm::vec2> indexed_uvs_sphere;
	std::vector<glm::vec3> indexed_normals_sphere;
	indexVBO(vertices_sphere, uvs_sphere, normals_sphere, indices_sphere, indexed_vertices_sphere, indexed_uvs_sphere, indexed_normals_sphere);

	glm::mat4 ModelMatrixSphere = glm::mat4(1.0);
	ModelMatrixSphere = glm::scale(ModelMatrixSphere, glm::vec3(0.045f, 0.045f, 0.045f));
	ModelMatrixSphere = glm::translate(ModelMatrixSphere, glm::vec3(-0.57f, 0.0f, 20.57613169f)); //
	glm::mat4 MVPSphere = Projection * View * ModelMatrixSphere;


	// Read suzanne .obj file
	std::vector<glm::vec3> vertices_suzi;
	std::vector<glm::vec2> uvs_suzi;
	std::vector<glm::vec3> normals_suzi; 
	bool res_suzi = loadOBJ("models/suzanne.obj", vertices_suzi, uvs_suzi, normals_suzi);
	GLuint Texture_suzi = loadDDS("models/uvmap.DDS");
	std::vector<unsigned short> indices_suzi;
	std::vector<glm::vec3> indexed_vertices_suzi;
	std::vector<glm::vec2> indexed_uvs_suzi;
	std::vector<glm::vec3> indexed_normals_suzi;
	indexVBO(vertices_suzi, uvs_suzi, normals_suzi, indices_suzi, indexed_vertices_suzi, indexed_uvs_suzi, indexed_normals_suzi);



	static const GLfloat field_vertex_buffer_data[] =
	{
		-1.0f, -0.2839351852f, 0.0f,
		1.0f, -0.2839351852f, 0.0f,
		1.0f, 0.2839351852f, 0.0f,
		-1.0f, 0.2839351852f, 0.0f,
	};


	static const GLfloat field_uv_buffer_data[] =
	{
	0.0f + 0.045f + 0.01171875f, 0.0234375f,
	1.0f + 0.045f - 0.01171875f,  0.0234375f,
	1.0f + 0.045f - 0.01171875f, 1.0f - 0.0234375f,
	0.0f + 0.045f + 0.01171875f, 1.0f - 0.0234375f
	};

	GLuint indices_field[] = {0,1,2,0,2,3};


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(field_vertex_buffer_data), field_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(field_uv_buffer_data), field_uv_buffer_data, GL_STATIC_DRAW);

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_field), &indices_field[0], GL_STATIC_DRAW);

	int colorflicker = 0;
	float k = 0.0f;
	double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double pollTime = glfwGetTime();

	std::array<ECE_UAV,15> UAV;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//setting initial position of uavs 

			UAV[i*3+j].initialPos[0] = 50.0-25*i;
			UAV[i*3+j].initialPos[1] = 26.665-j*26.665;	
			UAV[i * 3 + j].accelerationCalc();
		}
	}

	int startMoving = true;
	bool started = false;
	bool valueUpdated = false;
	float translateX[15] = { 0 }; float translateY[15] = { 0 }; float translateZ[15] = { 0 };
	float simulationStopTime = 90.0;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window)==0)
	{
		//renders all objects 

		currentTime = glfwGetTime();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// begin field rendering
		glUseProgram(programID);
		MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		TextureID = glGetUniformLocation(programID, "myTextureSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, field);
		glUniform1i(TextureID, 0);

		colorMultiplier = glGetUniformLocation(programID, "multiplier");
		glUniform1f(colorMultiplier, 1.0f);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(field_vertex_buffer_data), field_vertex_buffer_data, GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(field_uv_buffer_data), field_uv_buffer_data, GL_STATIC_DRAW);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_field), &indices_field[0], GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES,sizeof(indices_field) / sizeof(indices_field[0]),GL_UNSIGNED_INT,(void*)0);

		// field rendering done

		// begin sphere rendering
		glUseProgram(programID_sphere);
		MatrixID = glGetUniformLocation(programID_sphere, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPSphere[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices_sphere.size() * sizeof(glm::vec3), &indexed_vertices_sphere[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_sphere.size() * sizeof(unsigned short), &indices_sphere[0], GL_STATIC_DRAW);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		// Draw the sphere
		glDrawElements(GL_TRIANGLES, indices_sphere.size(), GL_UNSIGNED_SHORT, (void*)0);
		glDisable(GL_BLEND);

		//Sphere draw finished

		//Drawing suzis
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 3; j++)
			{

				if (currentTime - lastTime >= 5.0)
				{
					
					if (startMoving)
					{
						for (int uavNumber = 0; uavNumber < 15; uavNumber++)
						{
							//initiating threads for each uav

							UAV[uavNumber].start(&UAV[uavNumber]);
						}
						started = true;
						startMoving = false;
					}

				}


				if (started && (currentTime - pollTime) >= 0.03  && (currentTime - lastTime) <= simulationStopTime)
				{
					translateX[i * 3 + j] = UAV[i * 3 + j].translation[0];
					translateY[i * 3 + j] = UAV[i * 3 + j].translation[1];
					translateZ[i * 3 + j] = UAV[i * 3 + j].translation[2];
					valueUpdated = true;
					
				}

				glm::mat4 ModelMatrixSuzi = glm::mat4(1.0);
				ModelMatrixSuzi = glm::scale(ModelMatrixSuzi, glm::vec3(0.012f, 0.012f, 0.012f));


				ModelMatrixSuzi = glm::translate(ModelMatrixSuzi, glm::vec3(translateX[i * 3 + j] -2.5f + 64.37f - i * 32.185f, translateY[i * 3 + j] -20.71228833f + j * 20.71228833f, 0.5f + translateZ[i * 3 + j]));
				
				glm::mat4 MVPSuzi = Projection * View * ModelMatrixSuzi;

				glUseProgram(programID);
				MatrixID = glGetUniformLocation(programID, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPSuzi[0][0]);

				TextureID = glGetUniformLocation(programID, "myTextureSampler");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture_suzi);
				glUniform1i(TextureID, 0);

				colorMultiplier = glGetUniformLocation(programID, "multiplier");

				if (!colorflicker && (currentTime - lastTime) <= simulationStopTime)
				{
					glUniform1f(colorMultiplier, 1.0f);
					colorflicker = 1;
				}
				else
				{
					glUniform1f(colorMultiplier, 0.5f);
					colorflicker = 0;
				}
				// 1rst attribute buffer : vertices
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glBufferData(GL_ARRAY_BUFFER, indexed_vertices_suzi.size() * sizeof(glm::vec3), &indexed_vertices_suzi[0], GL_DYNAMIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				// 2nd attribute buffer : UVs
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
				glBufferData(GL_ARRAY_BUFFER, indexed_uvs_suzi.size() * sizeof(glm::vec3), &indexed_uvs_suzi[0], GL_DYNAMIC_DRAW);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

				// Index buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_suzi.size() * sizeof(unsigned short), &indices_suzi[0], GL_DYNAMIC_DRAW);

				glDrawElements(GL_TRIANGLES, indices_suzi.size(), GL_UNSIGNED_SHORT, (void*)0);
			}

		}
		//suzi drawing finished
		if (valueUpdated)
		{
			pollTime = currentTime;
			valueUpdated = false;
		}
		
		//
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (currentTime - lastTime >= 120.0)
		{
			break;
		}

	}

	for (int i = 0; i < 15; i++)
	{
		UAV[i].stopThread=1;
	}
	for (int i = 0; i < 15; i++)
	{
		// joining all threads
		if (started)
		{
			UAV[i].uav.join();
		}
	}
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	glfwTerminate();


	return 0;
}