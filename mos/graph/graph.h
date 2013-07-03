#ifndef __GRAPH_GRAPH_H_
#define __GRAPH_GRAPH_H_

#include "rect.h"
#include <unordered_map>
#include <string>

//file���ܲ���ʵ�ʵ�file��������һ��key
//����file��frame����λtexture(file and rect)
//button.jpg 1 --> ui.jpg ui (200,300,64,32) true uiΪ��drawbatch������true
//walk.ani 2 -->walk.jpg walk2 (200,300,64,32) false walk��һ֡��һ֡��û���ˣ�����false
class texture;
class image;
class st_cell;
struct stFont;
class texture_font;
struct text_char;
class image_zgp;

#define TIME_NOTUSE(A,B) ((int)(time - A->m_time_use) / 1000 > B)
#define TIME(A) ((time - A->m_time_use) / 1000)

class file_source
{
public:
	virtual image* find_image_file(const char* file,int frame,const unsigned long* parts_pal_hsv) = 0;
	virtual const char* get_texture_file(const char* _file,int frame,const unsigned long* parts_pal_hsv) const = 0 ;
	virtual void auto_clear_resource() = 0;
	virtual void close_resource() = 0;
	virtual const char* get_file_ext() const = 0 ;
	virtual void dump_resource() const = 0 ;
};

class graph
{
public:
	graph();
	
	void regist_file_source(file_source* source);
	std::unordered_map<std::string,file_source* > source_map;
	file_source* find_file_source(const char* file) const;

	//������Դ��
	std::unordered_map<std::string,image*> image_map;
	image* find_image(const char* file,int frame,const unsigned long* parts_pal_hsv);
	
	bool get_image_size(const char* file,int frame,g_size& sz);
	bool get_image_sizecg(const char* file,int frame,g_size& sz,g_point& cg);

	std::unordered_map<std::string,texture*> texture_map;
	texture* find_texture(const char* file,int frame);

	std::unordered_map<int,texture_font*> texture_font_map;

	//clear 
	int m_compress_image,m_clear_image,m_clear_texture,m_clear_texturefont;
	void auto_clear_resource();
	void close_resource();
	void dump_resource(const std::string& type) const;

	//����
	int draw_image(const st_cell& cell,const char* file,int frame);
	int draw_box(const st_cell& cell,int w,int h);
	
	//����Ļ��Ƶ�ͼ
	int draw_image_map(const st_cell& cell,const char* file);
	
	//text
	int draw_text(const st_cell& cell,const st_cell& text,const g_rect& rc_father);
	//����sz_father����Ϊ����Ҫwrap
	int get_text_line(const st_cell& text,const g_size& sz_father);
	g_size get_text_size(const st_cell& text,const g_size& sz_father);

	//loop
	void render_begin();
	void render_end();
	
	void draw_win_begin(int x,int y,int w,int h,const st_cell& win);
	void draw_win_end();

	//device
	void init_graph();
	void close_graph();
	image* find_image_raw(const char* file,int frame,const unsigned long* parts_pal_hsv);
protected:
	int get_text_line(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str);
	g_size get_text_size(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str);


	texture_font* find_texture_font(int font,bool bold);
	bool find_texture_font_rc(const st_cell& text,int char_value,text_char& tc);
public:
	texture* find_texture(const char* file);
	void maped_texture(const char* file,texture* p);
};

graph* get_graph();

#endif