#include "graph.h"
#include "cell.h"
#include "texture.h"
#include "texture_font.h"
#include "image.h"
#include "device/window_render.h"
#include "core/utf8.h"
#include "font.h"
#include "image_db.h"
#include "mos.h"
#include "image/image_zgp.h"
#include "map.h"
#include <iostream>

class auto_free
{
public:
	wchar_t* ptr;
	auto_free(wchar_t* buf){
		ptr = buf;
	}
	~auto_free(){
		delete ptr;
	}

};

//////////////////////////////////////////////////////////////////////////
graph::graph()
{
	m_compress_image = 10;
	m_clear_image  = 60;
	m_clear_texture = 10;
	m_clear_texturefont = 10;

	m_in_map = NULL;
}

void graph::regist_file_source(file_source* source)
{
	source_map[source->get_file_ext()] = source;
}

file_source* graph::find_file_source(const char* file) const
{
	const char* f = file + strlen(file) - 3;
	auto it = source_map.find(f);
	if (it != source_map.end())
		return it->second;
	//return source_map[f];
	return NULL;
}

image* graph::find_image_raw(const char* file,int frame,const unsigned long* parts_pal_hsv)
{
	file_source* source = find_file_source(file);
	if (source)
		return source->find_image_file(file,frame,parts_pal_hsv);
	//if (is_zgp(file))
	//	return find_image_zgp(file,frame,parts_pal_hsv);

	image* i = image_map[file];
	if (!i)
	{
		i = image::create_image_file(file);
		if (i)
			image_map[file] = i;
	}
	if (i)
		i->mark_use_image(g_time_now);
	return i;
}

image* graph::find_image(const char* file,int frame,const unsigned long* parts_pal_hsv)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();
	return find_image_raw(file,frame,parts_pal_hsv);
}

bool graph::get_image_size(const char* file,int frame,g_size& sz)
{
	g_point cg;
	return get_image_sizecg(file,frame,sz,cg);
}

bool graph::get_image_sizecg(const char* file,int frame,g_size& sz,g_point& cg)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r && !r->rc.is_empty())
	{
		sz = r->rc.get_size();
		cg = r->cg;
		return true;
	}
	if (r)
		file = r->file_image.c_str();

	image* i = find_image_raw(file,frame,NULL);
	if (i)
	{
		sz = i->get_size();
		cg = i->get_cg();
		return true;
	}
	return false;
}

void graph::maped_texture(const char* file,texture* p)
{
	p->mark_use_texture(g_time_now);
	texture_map[file] = p;
}

texture* graph::find_texture(const char* file)
{
	texture* t = texture_map[file];
	if (t)
		t->mark_use_texture(g_time_now);
	return t;
}

texture_font* graph::find_texture_font(int font,bool bold)
{
	if (bold)
		font += 1000;
	texture_font* tf = texture_font_map[font];
	if (tf)
		return tf;

	const stFont* st = get_font(font);
	if (!st)
		return NULL;

	tf = new texture_font;
	tf->create_texture_font(512,512,st,bold);
	texture_font_map[font] = tf;
	return tf;
}

bool graph::find_texture_font_rc(const st_cell& text,int char_value,text_char& tc)
{
	texture_font* tf = find_texture_font(text.font,text.bold != 0);
	if (tf)
	{
		tf->mark_use_texturefont(g_time_now);
		texture_char* t = tf->find_char(char_value);
		if (t)
		{
			t->mark_use_char(g_time_now);
			tc.rc_texture = &t->rc;
			tc.advance = t->advance;
			tc.texture = tf;
			return true;
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
void graph::auto_clear_resource()
{
	for (auto it = source_map.begin(); it != source_map.end(); ++it)
	{
		file_source* p = it->second;
		p->auto_clear_resource();
	}

	unsigned long time = get_time_now();

	for (auto it = image_map.begin(); it != image_map.end(); )
	{
		image* img = it->second;
		if (img && TIME_NOTUSE(img,m_clear_image) && img->m_ref == 1)
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear image " << it->first << " time: " << TIME(img) << std::endl;
#endif
			delete img;
			it = image_map.erase(it);
		}
		else
			++it;
//�������ԣ�ѹ��ûɶ�á�
//		else 
//		{
//			if (img && TIME_NOTUSE(img,m_compress_image) && !img->is_compress())			
//			{
//				img->compress();
//#ifdef _DEBUG_RESOURCE
//				std::cout << "compress image " << it->first << " time: " << TIME(img) 
//					<< " size " << img->get_buf_size()/1024 << "->" << img->get_compress_size()/1024 << std::endl;
//#endif
//			}			
//			++it;
//		}
	}	
	for (auto it = texture_map.begin(); it != texture_map.end();)
	{
		const texture* tex = it->second;
		if (tex && TIME_NOTUSE(tex,m_clear_texture))
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear texture " << it->first << " time: " << TIME(tex) << std::endl;
#endif
			delete tex;
			it = texture_map.erase(it);
		}
		else 
			++it;
	}	
	for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
	{
		texture_font* tf = it->second;
		for (auto it2 = tf->m_map_char.begin(); it2 != tf->m_map_char.end(); )
		{
			texture_char* tc = it2->second;
			if (TIME_NOTUSE(tc,m_clear_texturefont))
			{
#ifdef _DEBUG_RESOURCE
				std::cout << "clear font " << it->first << " char " << core::UnicodeCharToANSI(it2->first) << " time: " << TIME(tc) << std::endl;
#endif
				tf->m_char_free.push_back(tc);
				it2 = tf->m_map_char.erase(it2);
			}
			else 
				++it2;
		}
	}
}

void graph::close_resource()
{
	for (auto it = source_map.begin(); it != source_map.end(); ++it)
	{
		file_source* p = it->second;
		p->close_resource();
	}

	for (auto it = image_map.begin(); it != image_map.end(); ++it)
		delete it->second;
	image_map.clear();
	for (auto it = texture_map.begin(); it != texture_map.end(); ++it)
		delete it->second;
	texture_map.clear();
	for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
		delete it->second;
	texture_font_map.clear();
}

//why not image:dump ��ΪҲֻ�������ã�д������û��˼��
void graph::dump_resource(const std::string& type) const
{
	std::cout << "\n"__FUNCTION__" " << type << std::endl;
	unsigned long time = get_time_now();

	file_source* source = find_file_source(type.c_str());
	if (source)
		source->dump_resource();

	if (type == "all")
	{
		dump_resource("image");
		dump_resource("texture");
		dump_resource("texture_font");	
		dump_resource("font");
	}
	else if (type == "image")
	{
		for (auto it = image_map.begin(); it != image_map.end(); ++it)
		{
			const image* img = it->second;
			std::cout << it->first << " ref: " << img->m_ref << " time: " << TIME(img) << std::endl;
		}	
	}
	else if (type == "texture")
	{
		for (auto it = texture_map.begin(); it != texture_map.end(); ++it)
		{
			const texture* tex = it->second;
			std::cout << it->first << " time: " << TIME(tex) << std::endl;
		}	
	}
	else if (type == "texture_font")
	{
		for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
		{
			const texture_font* tf = it->second;
			std::cout << it->first << " time: " << TIME(tf) << std::endl;
			for (auto it2 = tf->m_map_char.begin(); it2 != tf->m_map_char.end(); ++it2)
			{
				const texture_char* tc = it2->second;
				std::cout << "\t" << core::UnicodeCharToANSI(it2->first) << " time: " << TIME(tc) << std::endl;
			}
		}
	}
	else if (type == "font")
	{
		dump_font();
	}
}

//////////////////////////////////////////////////////////////////////////
int graph::draw_image(const st_cell& cell,const char* file,int frame)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();

	image* img = find_image_raw(file,frame,&cell.part0);
	if (!img)
		return -1;

	const g_rect* rc = NULL;
	if (r && !r->rc.is_empty())
		rc = &r->rc;

	file_source* source = find_file_source(file);
	if (source)
		file = source->get_texture_file(file,frame,&cell.part0);

	return get_render()->draw_image_cell(cell,img,file,rc);
}

int graph::draw_box(const st_cell& cell,int w,int h)
{
	return get_render()->draw_box_cell(cell,w,h);
}

//////////////////////////////////////////////////////////////////////////
//text
static bool is_english(short c)
{
	return c <= 255;
}

void format_text(const st_cell& text,std::vector<text_char>& texts)
{
	//���ǵ�align���Զ����У��ͱ�㣬�ⲻ�Ǽ����׵����顣
	//����#x�ı���ɫ��bold���»��ߣ������ӡ�

	//textתΪutf16
}

//no wrap now
g_size graph::get_text_size(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str)
{
	text_char tc;

	int x = 0;
	int y = 0;

	int line = 1;
	int maxx = 0;

	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... �����ת���ַ�����ô���ˡ�
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space;
			line ++;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;
		x += tc.advance;
		if (maxx < x)
			maxx = x;
	}
	return g_size(maxx, line * (font->height + font->y_space) - font->y_space) ;
}

g_size graph::get_text_size(const st_cell& text,const g_size& sz_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return g_size(0,0);

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return g_size(0,0);
	g_size sz = get_text_size(text,sz_father,font,af.ptr);
	return sz;
}

//no wrap now
int graph::get_text_line(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str)
{
	text_char tc;

	int x = 0;
	int y = 0;

	int line = 1;
	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... �����ת���ַ�����ô���ˡ�
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space;
			line ++;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;
		x += tc.advance;
	}
	return line;
}

int graph::get_text_line(const st_cell& text,const g_size& sz_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return 0;

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return 0;
	int line = get_text_line(text,sz_father,font,af.ptr);
	return line;
}

int graph::draw_text(const st_cell& cell,const st_cell& text,const g_rect& rc_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return -1;

	get_render()->text_start(rc_father);
	st_cell c = cell;

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return -1;
	const wchar_t* str = af.ptr;

	text_char tc;

	int x = 0;
	int y = 0;
	float room = cell.room;

	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... �����ת���ַ�����ô���ˡ�
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space ;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;

		c.x = cell.x + x*room;
		c.y = cell.y + y*room;
		c.color = text.color;
		c.alpha = text.alpha;
		get_render()->draw_text_cell(c,tc.texture->m_texture,tc.rc_texture);

		x += tc.advance;
	}

	get_render()->text_end();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//loop
void graph::render_begin()
{
}

void graph::render_end()
{
	auto_clear_resource();
}

void graph::draw_win_begin(int x,int y,int w,int h,const st_cell& win)
{
	get_render()->window_start(x,y,w,h,win);
}

void graph::draw_win_end()
{
	get_render()->window_end();
}

//////////////////////////////////////////////////////////////////////////
//device
bool create_image_png(image_struct*,void* data,int size,const char* name);
bool create_image_jpg(image_struct*,void* data,int size,const char* name);

void graph::init_graph()
{
	init_cell();
	init_font();
	image::register_image_file("png",create_image_png);
	image::register_image_file("jpg",create_image_jpg);

	//image::register_image_file("zgp",create_image_zgp);
	//�򵥵��ļ�������register_image_file
	//���ӵĶ�֡�ļ�������regist_file_source
	regist_file_source(get_file_source_zgp());
}

void graph::close_graph()
{
	close_resource();
	close_font();
}

graph* get_graph()
{
	static graph s_graph;
	return &s_graph;
}

