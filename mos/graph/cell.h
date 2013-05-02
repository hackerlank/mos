#ifndef __GRAPH_CELL_H_
#define __GRAPH_CELL_H_

struct lua_State;
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

#include "graph/rect.h"

class st_cell
{
public:
	int _not_use;

	const char* name;

	int x,y,z;
	int cx,cy; //����
	float room;
	int w,h;

	int color;
	int alpha;

	const char* image_file;
	int frame;
	const char* shade;

	//font
	int font;
	const char* text;
	int wrap,align,bold,underline;

	//int is_text;	//for text
	int is_box;		//for box
	int is_window;	//for window use��һ��window�ϵ�ͼ�ؿ���һ�λ��ơ�

	st_cell();
	void init();
	st_cell(const st_cell& r1,const st_cell r2);

	void set_kv(const char* key,lua_Number value);
	void set_kv(const char* key,const char* value);

	void st_print(int level) const;
};

int lua_render(lua_State *L);
void init_cell();

#endif