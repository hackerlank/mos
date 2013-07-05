#ifndef __GRAPH_CELL_H_
#define __GRAPH_CELL_H_

struct lua_State;
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

#include "core/rect.h"

#define CELL_MAX_PARTS 6

class st_cell
{
public:
	int _not_use;

	const char* name;

	int x,y,z;
	int cx,cy; //����

	float room;		//room�ı�offset��roomall�ı�render!

	int w,h;

	int color;
	int alpha;

	const char* image_file;
	int frame;
	int need_mask;

	const char* shader;
	float shader_param;

	//font
	int font;
	const char* text;
	int wrap,align,bold,underline;

	int is_box;		//for box
	
	int is_window;	//for window use
	//һ��window�ϵ�ͼ�ؿ���һ�λ��ƣ��ϲ����ϵ�image��text��Ȼ��һ���Ի���image��text��
	//��δʵ��
	
	int is_map;		//for map
	const char* map_file;
	//һ�������ʵ�֣���Ϊmap�ĸ�ʽ���ӣ�����map�����Ƕ�̬�ģ��������ϵ����֣�����Ҳ���ӡ�

	unsigned long part0,part1,part2,part3,part4,part5;
	//int shader_light;	//������������shader������part���У���Ϊpart�Ƿֿ���޷�֧�֡�

	st_cell();
	void init();
	//st_cell(const st_cell& r1,const st_cell& r2);
	void merge(const st_cell& r1,const st_cell& r2);

	void set_kv(const char* key,lua_Number value);
	void set_kv(const char* key,const char* value);

	void st_print(int level) const;
};

int lua_render(lua_State *L);
void init_cell();

#endif