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
	 * Load an image into a 2D texture, creating a new texture id
	 * The texture unit desired for this texture should be set active
	 * before loading the texture as it will be bound during the loading process
	 * Can also optionally pass width & height variables to return the width
	 * and height of the loaded image
	 */
	GLuint load_texture(const std::string &file, size_t *width = nullptr, size_t *height = nullptr);
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
