#include "director.h"
#include "window_render_gl.h"
#include "OGLES/GL/glew.h"
#include "window.h"
#include "gl_macro.h"


director::director(window_render_gl* w)
{
	m_wgl = w;
}

void director::create_director()
{
	glEnable(GL_TEXTURE_2D);
	set_alpha_blending(true);
	set_depth_test(false);
	set_projection();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void director::set_alpha_blending(bool bOn)
{

	if (bOn)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	CHECK_GL_ERROR_DEBUG();
}

void director::set_depth_test(bool bOn)
{
	if (bOn)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	CHECK_GL_ERROR_DEBUG();
}

void director::set_projection()
{
	int w = m_wgl->m_window->get_width();
	int h = m_wgl->m_window->get_height();
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w,h,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

