#include <iostream>
#include <string>
#include <SDL.h>
#include "gl_core_3_3.h"
#include "render/render_target.h"
#include "driver.h"
#include "util.h"

const static std::string VERTEX_SHADER_SRC =
"#version 330 core\n\
const vec2 verts[4] = vec2[4](\n\
	vec2(-1, -1), vec2(1, -1), vec2(-1, 1), vec2(1, 1)\n\
);\n\
//y-inverted uvs so we don't need to flip the texture\n\
const vec2 uvs[4] = vec2[4](\n\
	vec2(0, 1), vec2(1, 1), vec2(0, 0), vec2(1, 0)\n\
);\n\
out vec2 fuv;\n\
void main(void){\n\
	fuv = uvs[gl_VertexID];\n\
	gl_Position = vec4(verts[gl_VertexID], 0, 1);\n\
}\n";
const static std::string FRAGMENT_SHADER_SRC =
"#version 330 core\n\
uniform sampler2D tex;\n\
in vec2 fuv;\n\
out vec4 color;\n\
void main(void){\n\
	color = texture(tex, fuv);\n\
}\n";

bool render_with_preview(Driver &driver){
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0){
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	const RenderTarget &target = driver.get_scene().get_render_target();
	SDL_Window *win = SDL_CreateWindow("CS6620", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, target.get_width(), target.get_height(),
		SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(win);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED){
		std::cerr << "ogl failed to load OpenGL functions" << std::endl;
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearDepth(1.f);

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(util::gldebug_callback, NULL);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
		NULL, GL_TRUE);
#endif

	GLint program = util::load_program(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
	if (program == -1){
		std::cerr << "GLSL shader failed to compile, aborting\n";
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}
	glUseProgram(program);
	GLint tex_unif = glGetUniformLocation(program, "tex");

	GLuint color;
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, target.get_width(), target.get_height(), 0, GL_RGB,
		GL_UNSIGNED_BYTE, NULL);
	//Disable linear filtering so the image is only as nice as the ray tracer renders it
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	GLuint depth;
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, target.get_width(), target.get_height(), 0, GL_RED,
		GL_UNSIGNED_BYTE, NULL);
	//Disable linear filtering so the image is only as nice as the ray tracer renders it
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//Set swizzle to give only the red channel so we see the depth map properly
	GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_RED};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);


	//A dummy vao, required for core profile but we don't really need it
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Run the driver so it renders while we update the texture with the
	//new output from it
	driver.render();
	bool quit = false;
	int shown_tex = 0;
	while (!quit){
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)){
				driver.cancel();
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN){
				switch (e.key.keysym.sym){
					case SDLK_d:
						shown_tex = 1;
						glUniform1i(tex_unif, shown_tex);
						break;
					case SDLK_c:
						shown_tex = 0;
						glUniform1i(tex_unif, shown_tex);
						break;
					default:
						break;
				}
			}
		}
		//Let the driver do some work
		SDL_Delay(16);
		//Update the texture with new data.
		//We could do better and only update blocks of updated pixels, but this is more
		//work than I want to put into this
		if (shown_tex == 0){
			glActiveTexture(GL_TEXTURE0);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, target.get_width(), target.get_height(), GL_RGB,
				GL_UNSIGNED_BYTE, target.get_colorbuf().data());
		}
		else {
			glActiveTexture(GL_TEXTURE1);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, target.get_width(), target.get_height(), GL_RED,
				GL_UNSIGNED_BYTE, target.generate_depth_img().data());
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		SDL_GL_SwapWindow(win);
	}
	glDeleteProgram(program);
	glDeleteTextures(1, &color);
	glDeleteTextures(1, &depth);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	SDL_Quit();
	//If the user aborted we may not have finished the render so report our status
	return driver.done();
}

