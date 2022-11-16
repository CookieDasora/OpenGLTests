// Cookie 2022

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/load_shader.h"
#include "common/obj_loader.h"

int main(void) {

	printf("[MAIN] - Hello! Startup.\n");

	// Initialize the library
	if (!glfwInit()) {
		fprintf(stderr, "[MAIN] - Failed to initialize GLFW\n");
		return -1;
	}

	// GLFW configurations
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Only OpenGL 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Creates a window and its OpenGL context
	window = glfwCreateWindow(1024, 768, "Hello world", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "[MAIN] - Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}

	// Window context
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "[MAIN] - Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}
	
	// Ensure we can capture the escape key
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0);

	// VAO - Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders\\SimpleVertexShader.vert", 
		"shaders\\SimpleFragmentShader.frag");

	// Projection matrix: 45° field of view, 4:3 ratio, display range: 0.1 unit <-> 100 units.
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Ortho camera
	// Left, right, bottom, top, zNear and zFar
	// glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 100.0f, 0.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(1, 1, 0),
		glm::vec3(0, 2, 0)
	);

	// Model matrix: an identity matrix (model will be at origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// ModelViewProjection (MVP): Multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model;

	// Get handle of our MVP uniform
	// Only during initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");


	// Read the .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("objects/cube.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);


	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// v-sync
		glfwSwapInterval(1);

		// Shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		// 1st attribute buffer: vertices!
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,			// attribute 0 to match the layout in the shader
			3,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride 
			(void*)0	// array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);


		// Draw the cube!
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}