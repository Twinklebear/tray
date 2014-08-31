#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <SDL.h>
#include "gl_core_3_3.h"
#include "util.h"

GLint util::load_shader(GLenum type, const std::string &text){
	GLuint shader = glCreateShader(type);
	const char *csrc = text.c_str();
	glShaderSource(shader, 1, &csrc, 0);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE){
		std::cerr << "load_shader: ";
		switch (type){
		case GL_VERTEX_SHADER:
			std::cerr << "Vertex shader: ";
			break;
		case GL_FRAGMENT_SHADER:
			std::cerr << "Fragment shader: ";
			break;
		case GL_GEOMETRY_SHADER:
			std::cerr << "Geometry shader: ";
			break;
		default:
			std::cerr << "Other shader type: ";
		}
		std::cerr << text << " failed to compile. Compilation log:\n";
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		char *log = new char[len];
		glGetShaderInfoLog(shader, len, 0, log);
		std::cerr << log << "\n";
		delete[] log;
		glDeleteShader(shader);
		return -1;
	}
	return shader;
}
GLint util::load_program(const std::vector<std::tuple<GLenum, std::string>> &shaders){
	std::vector<GLuint> glshaders;
	for (const std::tuple<GLenum, std::string> &s : shaders){
		GLint h = load_shader(std::get<0>(s), std::get<1>(s));
		if (h == -1){
			std::cerr << "load_program: A required shader failed to compile, aborting\n";
			for (GLuint g : glshaders){
				glDeleteShader(g);
			}
			return -1;
		}
		glshaders.push_back(h);
	}
	GLuint program = glCreateProgram();
	for (GLuint s : glshaders){
		glAttachShader(program, s);
	}
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE){
		std::cerr << "load_program: Program failed to link, log:\n";
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		char *log = new char[len];
		glGetProgramInfoLog(program, len, 0, log);
		std::cerr << log << "\n";
		delete[] log;
	}
	for (GLuint s : glshaders){
		glDetachShader(program, s);
		glDeleteShader(s);
	}
	if (status == GL_FALSE){
		glDeleteProgram(program);
		return -1;
	}
	return program;
}
bool util::log_glerror(const std::string &msg){
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		std::cerr << "OpenGL Error: ";
		switch (err){
		case GL_INVALID_ENUM:
			std::cerr << "Invalid enum";
			break;
		case GL_INVALID_VALUE:
			std::cerr << "Invalid value";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << "Invalid operation";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << "Out of memory";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << "Invalid FrameBuffer operation";
			break;
		default:
			std::cerr << std::hex << err << std::dec;
		}
		std::cerr << " - " << msg << "\n";
		return true;
	}
	return false;
}
#if _MSC_VER
void APIENTRY util::gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
	GLsizei len, const GLchar *msg, const GLvoid *user)
#else
void util::gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
	GLsizei len, const GLchar *msg, const GLvoid *user)
#endif
{
	//Print a time stamp for the message
	float sec = SDL_GetTicks() / 1000.f;
	int min = static_cast<int>(sec / 60.f);
	sec -= sec / 60.f;
	std::cerr << "[" << min << ":"
		<< std::setprecision(3) << sec << "] OpenGL Debug -";
	switch (severity){
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		std::cerr << " High severity";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		std::cerr << " Medium severity";
		break;
	case GL_DEBUG_SEVERITY_LOW_ARB:
		std::cerr << " Low severity";
	}
	switch (src){
	case GL_DEBUG_SOURCE_API_ARB:
		std::cerr << " API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		std::cerr << " Window system";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		std::cerr << " Shader compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		std::cerr << " Third party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		std::cerr << " Application";
		break;
	default:
		std::cerr << " Other";
	}
	switch (type){
	case GL_DEBUG_TYPE_ERROR_ARB:
		std::cerr << " Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		std::cerr << " Deprecated behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		std::cerr << " Undefined behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		std::cerr << " Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		std::cerr << " Performance";
		break;
	default:
		std::cerr << " Other";
	}
	std::cerr << ":\n\t" << msg << "\n";
}

