#pragma once

//���ṹ, �Ƚ�����, �Ǵ�Ҷ�ڵ�����ڵ㷴�����ӣ������Ҷ�ڵ��ҵ����ڵ�
//#include "action/point2.h"
//#include "findpath/findpath_struct.h"
#include "rect.h"
#include <list>

typedef std::list<g_point> PATH;

typedef struct tree_node *TREE;

struct tree_node {
	int h;            //�ڵ����ڵĸ߶ȣ���ʾ����ʼ�㵽�ýڵ����еĲ���
	int tile;        //�ýڵ��λ��
	int dir;
	TREE father;    //�ýڵ����һ��
};

//���ӽṹ�����ڱ��洦����ĺ�û�д�����Ľ��
typedef struct link_node *LINK;

struct link_node {
	TREE node;
	int f;
	LINK next;
};

void drawxy(int x,int y,char c);
typedef unsigned char Obs_type;

class cAStar
{
protected:
	Obs_type* map;
	int offx,offy;
	unsigned long* dis_map;					//��������·��ʱ,�м�Ŀ������Ž�
	int map_w,map_h;						//��ͼ��͸�
	int start_x,start_y,end_x,end_y;		//�ص�,�յ�����
	LINK sort_queue;						// ����û�д�������߷����Ľڵ�
	LINK store_queue;			            // �����Ѿ�������Ľڵ� (��������ͷ�)

	void init_queue(void);
	void enter_queue(TREE node,int f);
	TREE get_from_queue(void);
	void pop_stack(void);
	void freetree(void);
	int judge(int x,int y);
	int trytile(int dir,int x,int y,TREE father);
	bool findpath(PATH& listWay);
	void showmap();
public:
	virtual ~cAStar();
	cAStar();
	void create_astar(g_size size,Obs_type* buffer,const g_rect& rc) ;

	bool FindWay(const g_point& Start,const g_point& Des, PATH& listWay);
	inline void draw_point(int x,int y,char c){
		drawxy(x+1+offx,y+1+offy,c);
	}};

void clrscr();
void gotoxy(int x,int y);
inline void goto_point(int x,int y){
	gotoxy(x+1,y+1);
}
inline void goto_point(const g_point& pt){
	goto_point(pt.x,pt.y);
}
inline void draw_point(int x,int y,char c){
	drawxy(x+1,y+1,c);
}
inline void draw_point(const g_point& pt,char c){
	draw_point(pt.x,pt.y,c);
}

