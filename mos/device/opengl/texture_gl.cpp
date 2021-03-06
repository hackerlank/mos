#include "texture_gl.h"
#include "device/window_render.h"
#include "mos.h"
#include "graph/image.h"
#include <assert.h>
#include "gl_macro.h"
#include "window_render_gl.h"

DECLARE_COUNTER(texture_mul)

bool is_2_mi(unsigned int n)
{
	return (n &(n-1)) == 0;
}

//接近2的幂的数 比如30->32 33->64
unsigned long ccNextPOT(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}


texture_gl::texture_gl()
{
	m_textureId = 0;
	m_textureId_pal = 0;
	m_use_palette = false;
}

texture_gl::~texture_gl()
{
	if(m_textureId)
	{
		glDeleteTextures(1, &m_textureId);
		m_textureId = 0;
	}
	if(m_textureId_pal)
	{
		glDeleteTextures(1, &m_textureId_pal);
		m_textureId_pal = 0;
	}
}

bool texture_gl::create_texture_gl(const image* img) 
{
	if (get_render_gl()->is_thread())
	{
		st_texture st;
		st.op = op_create_texture_gl;
		st.tex = this;
		st.img = img;
		get_render_gl()->m_textures.push_back(st);
		return true;
	}
	return _create_texture_gl(img);
}

bool texture_gl::_create_texture_gl(const image* img) 
{
	m_obj_type = img->m_obj_type;
	int width = img->get_width();
	int height = img->get_height();

	bool rt ;
	if (img->m_bits_pixel == 1) 
	{
		//256色的调色板。
		//first we not use shader.
		if (img->is_256())
		{
			m_use_palette = img->use_palette();
			if (!img->use_palette())
			{
				colorbyte* buf = img->render_256_argb();
				rt = init_data(buf,img->get_width(),kCCTexture2DPixelFormat_RGBA8888,width,height,width,height);
				delete buf;
			}
			else
			{
				colorbyte* buf = img->get_buf_offset(0,0);
				//buf = img->render_256_index();
				rt = init_data(buf,img->get_width(),kCCTexture2DPixelFormat_A8,width,height,width,height);
				//delete buf;

				glGenTextures(1, &m_textureId_pal);
				glBindTexture(GL_TEXTURE_2D, m_textureId_pal);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, img->m_pal_color_num); 
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				
				CCTexture2DPixelFormat format = img->has_alpha() ? kCCTexture2DPixelFormat_RGBA8888 : kTexture2DPixelFormat_RGB888;
				const formatinfo* info = get_formatinfo(format);
				if (img->has_alpha())
				{
					colorbyte* pal = img->render_256_palette_alpha();
					glTexImage2D(GL_TEXTURE_2D, 0, info->internalformat, img->m_pal_color_num, 1, 0, info->glformat, info->gltype, pal);	
					delete pal;
				}
				else
					glTexImage2D(GL_TEXTURE_2D, 0, info->internalformat, img->m_pal_color_num, 1, 0, info->glformat, info->gltype, img->m_pal_color);	

				CHECK_GL_ERROR_DEBUG();
			}
		}
	}
	else
	{
		colorbyte* buf = img->get_buf_offset(0,0);
		CCTexture2DPixelFormat format = img->m_bits_pixel == 3 ? kCCTexture2DPixelFormat_RGB888 : kCCTexture2DPixelFormat_RGBA8888;
		rt = init_data(buf,img->get_width(),format,width,height,width,height);
	}
	//delete buf;
	return rt;
}

bool texture_gl::create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) 
{
	if (get_render_gl()->is_thread())
	{
		st_texture st;
		st.op = op_create_texture_dynamic;
		st.tex = this;
		st.width = width;
		st.height = height;
		st.format = format;
		get_render_gl()->m_textures.push_back(st);

		m_tex_width = width;
		m_tex_height = height;
		m_width = width;
		m_height = height;
		m_format = format;

		return true;
	}
	return _create_texture_dynamic(width,height,format);
}

bool texture_gl::_create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) 
{
	assert(is_2_mi(width) && is_2_mi(height));
	int bits = bitsPerPixelForFormat(format)/8;
	char* data = new char[width*height*bits];
	memset(data,0,sizeof(char)*width*height*bits);
	bool rt = init_data(data,0,format,width,height,width,height);
	delete data;
	return rt;
}

int texture_gl::draw_image_ontexture(int x,int y,const image* img,const g_rect* rc) 
{
	if (get_render_gl()->is_thread())
	{
		st_texture st;
		st.op = op_draw_image_ontexture;
		st.tex = this;
		st.x = x;
		st.y = y;
		st.img = img;
		st.rc = rc;
		if (rc)
			st.rect = *rc;
		get_render_gl()->m_textures.push_back(st);
		return true;
	}
	return _draw_image_ontexture(x,y,img,rc);
}

bool texture_gl::_draw_image_ontexture(int x,int y,const image* img,const g_rect* rc) 
{
	const formatinfo* info = get_formatinfo(m_format);
	if (!info)
		return false;

	g_rect r = rc ? *rc : img->get_rect();
	colorbyte* buf = img->get_buffer() + (info->bits)/8 * (r.l+r.t*img->m_width);
	return update_data(buf,img->m_width,x,y,r.width(),r.height());
}

bool texture_gl::update_data(const void* data, int rowLength,int offx,int offy,int width, int height)
{
	assert(width > 0 && width <= 10240 && height > 0 && height <= 10240);
	const formatinfo* info = get_formatinfo(m_format);
	if (!info)
		return false;

	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

	glTexSubImage2D(GL_TEXTURE_2D, 0, offx, offy, width, height, info->glformat, info->gltype, data);	

	CHECK_GL_ERROR_DEBUG();

	return true;
}

bool texture_gl::init_data(const void *data, int rowLength, CCTexture2DPixelFormat pixelFormat, int pixelsWide, int pixelsHigh, int width, int height)
{
	const formatinfo* info = get_formatinfo(pixelFormat);
	if (!info)
		return false;

	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glTexImage2D(GL_TEXTURE_2D, 0, info->internalformat, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, info->glformat, info->gltype, data);	

	CHECK_GL_ERROR_DEBUG();

	m_tex_width = pixelsWide;
	m_tex_height = pixelsHigh;
	m_width = width;
	m_height = height;
	m_format = pixelFormat;

	return true;
}	

formatinfo c_formatinfos[] = {
	{kCCTexture2DPixelFormat_RGBA8888,true,32,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,4},
	{kCCTexture2DPixelFormat_RGB888,false,32,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_RGB565,false,16,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,1},
	{kCCTexture2DPixelFormat_A8,true,8,GL_ALPHA8,GL_ALPHA,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_I8,false,8,GL_LUMINANCE,GL_LUMINANCE,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_AI88,true,16,GL_LUMINANCE_ALPHA,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_RGBA4444,true,16,GL_RGBA,GL_RGBA,GL_UNSIGNED_SHORT_4_4_4_4,1},
	{kCCTexture2DPixelFormat_RGB5A1,true,16,GL_RGBA,GL_RGBA,GL_UNSIGNED_SHORT_5_5_5_1,1},
};

const formatinfo* get_formatinfo(CCTexture2DPixelFormat format)
{
	return &c_formatinfos[format];
}

bool hasAlpha(CCTexture2DPixelFormat format)
{
	return get_formatinfo(format)->alpha;
}

unsigned int bitsPerPixelForFormat(CCTexture2DPixelFormat format)
{
	return get_formatinfo(format)->bits;
}

