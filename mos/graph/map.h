#ifndef MAP_H
#define MAP_H

#include "rect.h"
#include <unordered_map>

class st_cell;

class map_source
{
public:
	virtual void draw_map_image(const st_cell& cell, const char* map_file,int frame) = 0;
	virtual bool load_map(const char* file,int frame,g_size& sz) = 0;
	virtual void draw_map_begin() = 0;
	virtual void draw_map_end() = 0;
	virtual void destory() = 0;
};

typedef map_source*(* map_source_func)();

//not called map confiict with std::map
class graph_map
{
public:
	graph_map();
	~graph_map();

	//׼�����Ƹ�image�ˣ��������֡�
	//void mask_drawing_image(const st_cell* cell);
	void draw_map_image(const st_cell& cell, const char* map_file,int frame);
	bool load_map(const char* file,int frame,g_size& sz);

	g_rect m_rc_map;
	const st_cell* m_in_map;

	void draw_map_begin(int x,int y,int w,int h,const st_cell& cell);
	void draw_map_end();

	map_source* m_source;

	void init_map();
	std::unordered_map<std::string,map_source_func> s_mapLoad;
	void register_map_source(const char* fileext,map_source_func func);
};


graph_map* get_map();

#endif