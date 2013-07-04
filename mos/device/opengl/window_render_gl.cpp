#include "window_render_gl.h"
#include "../window.h"
#include "../file.h"
#include "graph/texture.h"
#include "graph/image.h"
#include "graph/graph.h"
#include <windows.h>
#include "GL/glew.h"
#include "director.h"
#include "texture_gl.h"
#include "graph/cell.h"
#include "graph/color.h"
#include "gl_macro.h"
#include "glsl.h"

//using namespace cwc;

window_render_gl::window_render_gl(window* w):window_render(w)
{
	m_director = new director(this);
	m_shader_manager = 0;
}

window_render_gl::~window_render_gl()
{
}

static void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // preferred color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accum bits (ignored)
		24,                         // depth buffer
		8,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}
	

bool window_render_gl::create_render(int width,int height)
{
	m_hDC = GetDC((HWND)m_window->m_hWnd);
	SetupPixelFormat(m_hDC);
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);

	// check OpenGL version at first
	const GLubyte* glVersion = glGetString(GL_VERSION);
	printf("OpenGL version = %s\n", glVersion);

	if ( atof((const char*)glVersion) < 1.5 )
	{
		printf("OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card.\n",
			glVersion);
		return false;
	}

	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		printf("OpenGL error %s\n",(char *)glewGetErrorString(GlewInitResult));
		return false;
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		printf("Ready for GLSL\n");
	}
	else
	{
		printf("Not totally ready :(\n");
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		printf("Ready for OpenGL 2.0\n");
	}
	else
	{
		printf("OpenGL 2.0 not supported\n");
	}

	m_director->create_director();

	create_shaders();

	return true;
}

void window_render_gl::create_shaders() 
{
//-----------------------------------------------------------------------------
// Simple GLSL Vertex Shader:
	const char* palette_vertex_prog = "void main()\
									  {\
									  gl_TexCoord[0] = gl_MultiTexCoord0;\
									  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
									  }";

	const char* palette_fragment_prog = "uniform sampler2D texture;\
										uniform sampler2D palette;\
										void main()\
										{\
										gl_FragColor = texture2D(palette, vec2(texture2D(texture, gl_TexCoord[0].st).a, 0));\
										}";

	m_shader_manager = new glShaderManager();
	m_shader_palette = m_shader_manager->loadfromMemoryName("shader_palette",palette_vertex_prog,palette_fragment_prog);
	m_shader_manager->SetPath(get_resourcefile("shaders"));
}

texture* window_render_gl::create_texture() 
{
	return new texture_gl;
}

void window_render_gl::on_destroy()
{
	delete m_director;
	m_director = 0;
	delete m_shader_manager;
	m_shader_manager = 0;

	if (m_hDC != NULL && m_hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
	}
}

void window_render_gl::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void window_render_gl::render_end()
{
	if (m_hDC != NULL)
	{
		::SwapBuffers(m_hDC);
	}
}

struct s_f2
{    
	GLint x;
	GLint y;
};
struct s_uv
{
	GLfloat u;
	GLfloat v;
};

int window_render_gl::draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	s_texture_render++;
	return _draw_texture_cell(cell,_tex,rc);
}

int window_render_gl::_draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	return draw_texture(cell.x,cell.y,cell.room,cell.color,cell.alpha,cell.shader,cell.shader_param,_tex,rc);
}

bool get_cliped_rect2(g_rect& rect,const g_rect& clip,int& offx,int& offy)
{
	if (offx < clip.l)
	{
		rect.l -= offx - clip.l;
		if (rect.r <= rect.l)
			return false;
		offx = clip.l;
	}
	else if (offx >= clip.r)
		return false;
	if (offx + rect.width() > clip.r)
		rect.r -= offx + rect.width() - clip.r;

	if (offy < clip.t)
	{
		rect.t -= offy - clip.t;
		if (rect.b <= rect.t)
			return false;
		offy = clip.t;
	}
	else if (offy >= clip.b)
		return false;
	if (offy + rect.height() > clip.b)
		rect.b -= offy + rect.height() - clip.b;

	if (rect.is_empty())
		return false;

	return true;
}


//room做成shader算了。
int window_render_gl::draw_texture(int x,int y,float room,int color,int alpha,const char* shader,float shader_param, texture* _tex,const g_rect* rc_tex)
{
	texture_gl* tex = (texture_gl*)_tex;
	g_rect rect0 = rc_tex ? *rc_tex : tex->get_rect();

	float u0 = (float)rect0.l/tex->m_tex_width;
	float v0 = (float)rect0.t/tex->m_tex_height;
	g_rect rect = rect0;
	rect.r += rect.width() * (room - 1);
	rect.b += rect.height() * (room - 1);


	//这个地方不对，跟window clip了接着room，错了。
	
	//因为shader的缘故，这里可以去了。
	g_rect rc_window = g_rect(0,0,m_window->m_width,m_window->m_height);
	//if (m_rc_clip && !get_cliped_rect2(rect,*m_rc_clip,x,y))
	if (!get_cliped_rect(rect,rc_window,x,y,m_rc_clip))
		return -1;

	s_f2 f3[4];
	s_uv uv[4];

	f3[0].x = x;
	f3[0].y = y;
	uv[0].u = u0 + (float)(rect.l-rect0.l)/tex->m_tex_width/room;
	uv[0].v = v0 + (float)(rect.t-rect0.t)/tex->m_tex_height/room;
	f3[3].x = x + rect.width() ;
	f3[3].y = y + rect.height() ;
	uv[3].u = u0 + (float)(rect.r-rect0.l)/tex->m_tex_width/room;
	uv[3].v = v0 + (float)(rect.b-rect0.t)/tex->m_tex_height/room;

	f3[1].x = f3[0].x;
	f3[1].y = f3[3].y;
	uv[1].u = uv[0].u;
	uv[1].v = uv[3].v;
	f3[2].x = f3[3].x;
	f3[2].y = f3[0].y;
	uv[2].u = uv[3].u;
	uv[2].v = uv[0].v;


	glShader* cur_shader = NULL;
	if (shader)
		cur_shader = m_shader_manager->getShader(shader,tex->use_palette());
	if (cur_shader == NULL && tex->use_palette())
		cur_shader = m_shader_palette;

	if (cur_shader)
	{
		cur_shader->begin();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

		if (tex->use_palette())
		{
			int texid = cur_shader->GetUniformLocation("texture");
			glUniform1i(texid,0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex->m_textureId_pal);
			int palette = cur_shader->GetUniformLocation("palette");
			glUniform1i(palette,1);
		}

		int param = cur_shader->GetUniformLocation("param");
		//shader_param
		glUniform1f(param,shader_param);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);
	}

	unsigned char r,g,b;
	G_GET_RGB(color,r,g,b);
	glColor4ub(r,g,b,alpha);
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_TRIANGLE_STRIP);	
	for(int i=0;i<4;i++)
	{
		glTexCoord2f(uv[i].u,uv[i].v); 
		glVertex2i(f3[i].x,f3[i].y);	
	}
	glEnd();

	if (cur_shader)
	{
		cur_shader->end();
	}

	//glFlush();
	s_triangle_render += 2;

	CHECK_GL_ERROR_DEBUG();

	return 0;
}


int window_render_gl::draw_box_cell(const st_cell& cell,int w,int h)
{
	s_box_render ++;
	return draw_box(cell.x,cell.y,cell.color,cell.alpha,w,h);
}

int window_render_gl::draw_box(int x,int y,int color,int alpha,int w,int h)
{
	if (!get_cliped_box(x,y,w,h,m_window->m_width,m_window->m_height))
		return -1;

	s_triangle_render += 2;

	s_f2 f3[4];

	f3[0].x = x;
	f3[0].y = y;
	f3[3].x = x + w;
	f3[3].y = y + h;

	f3[1].x = f3[0].x;
	f3[1].y = f3[3].y;
	f3[2].x = f3[3].x;
	f3[2].y = f3[0].y;

	unsigned char r,g,b;
	G_GET_RGB(color,r,g,b);

	//glDisable(GL_TEXTURE_2D);

	glColor4ub(r,g,b,alpha);
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	//否则画不出box
	glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_TRIANGLE_STRIP);	
	for(int i=0;i<4;i++)
	{
		glVertex2i(f3[i].x,f3[i].y);	
	}
	glEnd();

	//glFlush();

	CHECK_GL_ERROR_DEBUG();

	return 0;
}

int window_render_gl::draw_text_cell(const st_cell& cell,texture* tex,const g_rect* rc)
{
	s_text_render++;
	return _draw_texture_cell(cell,tex,rc);
}

int window_render_gl::draw_image_cell(const st_cell& cell,image* img,const char* file,const g_rect* rc)
{
	s_image_render++;
	texture_gl* gl = (texture_gl*)get_graph()->find_texture(file);
	if (!gl)
	{
		gl = new texture_gl;
		if (!gl->create_texture_gl(img))
		{
			delete gl;
			return -1;
		}
		get_graph()->maped_texture(file,gl);
	}
	return _draw_texture_cell(cell,gl,rc);
}