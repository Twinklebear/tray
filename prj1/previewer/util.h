#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <array>
#include <string>
#include "gl_core_3_3.h"

namespace util {
	/*
	 * Load a GLSL shader from some shader program text
	 */
	GLint load_shader(GLenum type, const std::string &text);
	/*
	 * Build a shader program from the list of shaders passed
	 * the tuples should be { type, shader text }
	 */
	GLint load_program(const std::vector<std::tuple<GLenum, std::string>> &shaders);
	/*
	 * Check for an OpenGL error and log it along with the message passed
	 * if an error occured. Will return true if an error occured & was logged
	 */
	bool log_glerror(const std::string &msg);
	/*
	 * A debug callback for the GL_ARB_debug_out extension
	 */
#ifdef _WIN32
	void APIENTRY gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
		GLsizei len, const GLchar *msg, const GLvoid *user);
#else
	void gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
		GLsizei len, const GLchar *msg, const GLvoid *user);
#endif
}

#endif

