#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "load_shader.h"
#include "obj_loader.h"

using namespace std;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the vertex shader code from the file
    printf("[SHADER LOADER] - Reading shader contents.\n");
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf(); // Stores the buffer from the file
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("[SHADER LOADER] - Impossible to open %s. Make sure that the file path is correct.\n",
            vertex_file_path);
        return 0;
    }

    // Read the fragment shader code from the file
    printf("[SHADER LOADER] - Reading fragment shader contents.\n");
    printf("[SHADER LOADER] - Fragment shader file: %s.\n", fragment_file_path);
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf(); // Stores the buffer from fragment
        FragmentShaderCode = sstr.str();
        printf("[SHADER LOADER] - Done reading the fragment shader file.\n");
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile vertex shader
    printf("[SHADER LOADER] - Compiling vertex shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check vertex shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    else {
        printf("[SHADER LOADER] - Done compiling the vertex shader.\n");
    }

    // Compile fragment shader
    printf("[SHADER LOADER] - Compiling fragment shader: %s.\n", fragment_file_path);
    char const* FragmentShaderPointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentShaderPointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check fragment shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("[SHADER LOADER] - %s\n", &FragmentShaderErrorMessage[0]);
    }
    else {
        printf("[SHADER LOADER] - Done compiling fragment shader.\n");
    }

    // Link the program
    printf("[SHADER LOADER] - Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("[SHADER LOADER] - %s\n", &ProgramErrorMessage[0]);
    }
    printf("[SHADER LOADER] - Linked successfully.\n");

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}