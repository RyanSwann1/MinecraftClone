#pragma once

#include "glad.h"
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define glCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall())

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall()
{
	while (GLenum error = glGetError())
	{
		std::cout << "[Opengl Error] ( " << error << "): " << "\n";

		return false;
	}

	return true;
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[Opengl Error] ( " << error << "): " << function <<
			" " << file << ":" << line << "\n";

		return false;
	}

	return true;
}