
#include "stdafx.h"

#include "AStar.h"
#include "dir32.h"
#include <conio.h>
#include <ctype.h>
//#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

#define tile_num(x,y) ((y)*map_w+(x))  //�� x,y ����ת��Ϊ��ͼ�Ͽ�ı��
#define tile_x(n) ((n)%map_w)          //�ɿ��ŵó� x,y ����
#define tile_y(n) ((n)/map_w)

#define MAPMAXSIZE 180  //��ͼ������Ϊ 180x180�����û��64K�ڴ����ƿ��Ը���
#define c_MAXINT 32767

#define ASSERT(a)

//#define Trace_AStarFindPath

void init_astar()
{
}

void clear_astar()
{
}


void clrscr()
{
#ifdef WIN32
	HANDLE hCon = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO  csbi; 
	DWORD                       dwConSize;
	DWORD                       cCharsWritten; 
	COORD coordScreen = { 0, 0 };

	GetConsoleScreenBufferInfo( hCon, &csbi );
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
	FillConsoleOutputCharacter( hCon, TEXT(' '),
		dwConSize, 
		coordScreen,
		&cCharsWritten ); 
	GetConsoleScreenBufferInfo( hCon, &csbi );
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
	coordScreen.X = 0;
	coordScreen.Y = 0;
	FillConsoleOutputAttribute( hCon,
		csbi.wAttributes,
		dwConSize,
		coordScreen,
		&cCharsWritten ); 
	SetConsoleCursorPosition( hCon, coordScreen ); 
#endif
}

void gotoxy(int x,int y)
{
#ifdef WIN32
	HANDLE hCon = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(hCon,coord);
#endif
}

void drawxy(int x,int y,char ch)
{
	gotoxy(x,y);
	char c[2];
	c[0] = ch;
	c[1] = 0;
	cprintf(c);
}

cAStar::~cAStar()
{
	delete map;
	map = NULL;
	delete dis_map;
	dis_map = NULL;
}

bool cAStar::FindWay(const g_point& Start,const g_point& Des, PATH& listWay)
{
	start_x = Start.x - offx;
	start_y = Start.y - offy;
	end_x = Des.x - offx;
	end_y = Des.y - offy;
	//showmap();
	return findpath(listWay);
}

cAStar::cAStar()
{
	map = NULL;
	dis_map = NULL;
}

#define OBS 0xff

void cAStar::create_astar(g_size size,Obs_type* buffer,const g_rect& rc) 
{
	map_w = rc.width()+2;
	map_h = rc.height()+2;
	Obs_type* _map = new Obs_type[map_w*map_h];
	map = _map;
	offx = rc.l-1;
	offy = rc.t-1;
	Obs_type* src = buffer + rc.t * size.w + rc.l;
	_map += map_w + 1;
	for (int i=0; i<rc.height(); i++)
	{
		memcpy(_map,src,rc.width()*sizeof(Obs_type));
		_map += map_w;
		src += size.w;
	}
	//�ѵ�ͼ�����ܼ����赲
	for (int x=0; x<map_w; x++)
	{
		map[tile_num(x,0)] = OBS;
		map[tile_num(x,map_h-1)] = OBS;
	}
	for (int y=0; y<map_h; y++)
	{
		map[tile_num(0,y)] = OBS;
		map[tile_num(map_w-1,y)] = OBS;
	}

	dis_map = new unsigned long[map_w*map_h];
}

void cAStar::showmap(void)
{
#ifndef Trace_AStarFindPath
	return;
#endif
	int i,j;
	clrscr();
	for (i=0;i<map_h;i++) {
		gotoxy(1,i+1);
		for (j=0;j<map_w;j++)
		{
			switch(map[tile_num(j,i)])
			{
			case 0:
				cprintf(" ");
				break;
			case OBS:
				cprintf("B");
				break;
			default: 
				cprintf("O");
			}
		}
	}
	draw_point(start_x,start_y,'s');
	draw_point(end_x,end_y,'e');
}

// ��ʼ������
void cAStar::init_queue(void)
{
	sort_queue=new link_node;
	sort_queue->node=NULL;
	sort_queue->f=-1;
	sort_queue->next=new link_node;
	sort_queue->next->node=NULL;
	sort_queue->next->f=c_MAXINT;
	sort_queue->next->next=NULL;

	store_queue=new link_node;
	store_queue->node=NULL;
	store_queue->f=-1;
	store_queue->next=NULL;
}

// ������ڵ������, ������Ŀ�ĵع��۾����������
void cAStar::enter_queue(TREE node,int f)
{
	LINK p=sort_queue,father,q;
	while(f>p->f) {
		father=p;
		p=p->next;
		ASSERT(p);
	}
	q=new link_node;
	ASSERT(sort_queue);
	q->f=f,q->node=node,q->next=p;
	father->next=q;
}

// ����Ŀ�ĵع�������ķ���������
TREE cAStar::get_from_queue(void)
{
	LINK bestchoice=sort_queue->next;
	LINK next=sort_queue->next->next;
	sort_queue->next=next;

	bestchoice->next=store_queue->next;
	store_queue->next=bestchoice;
	return bestchoice->node;
}

// �ͷ�ջ���ڵ�
void cAStar::pop_stack(void)
{
	LINK s=store_queue->next;
	ASSERT(s);
	store_queue->next=store_queue->next->next;
	delete(s->node);
	delete(s);
}

// �ͷ�����������нڵ�
void cAStar::freetree(void)
{
	LINK p;
	while(store_queue){
		p=store_queue;
		delete(p->node);
		store_queue=store_queue->next;
		delete(p);
	}
	while (sort_queue) {
		p=sort_queue;
		delete(p->node);
		sort_queue=sort_queue->next;
		delete(p);
	}
}

// ���ۺ���,���� x,y ��Ŀ�ĵصľ���,����ֵ���뱣֤��ʵ��ֵС
int cAStar::judge(int x,int y)
{
	//return abs(end_x-x)+abs(end_y-y);
	int dx,dy;
	dx=abs(x-end_x),dy=abs(y-end_y);
	if (dx>dy) 
		return 10*dx+6*dy; 
	else 
		return 10*dy+6*dx;
}

// ������һ���ƶ��� x,y ���з�
int cAStar::trytile(int dir,int x,int y,TREE father)
{
	TREE p=father;
	int h;
	if (map[tile_num(x,y)]!=0) return 1; // ��� (x,y) �����ϰ�,ʧ��
	//��һ�������ж�(x,y)���Ƿ��Ѿ�������У��������ɾ������Ϊdis_map�Ѿ�
	//����õ��Ƿ��Ѿ�����
	//while (p) {
	//  if (x==tile_x(p->tile) && y==tile_y(p->tile)) return 1; //��� (x,y) ��������,ʧ��
	//  p=p->father;
	//}
	h=father->h+1;
	if (h>=dis_map[tile_num(x,y)]) return 1; // ��������и��õķ����ƶ��� (x,y) ʧ��
	dis_map[tile_num(x,y)]=h; // ��¼��ε� (x,y) �ľ���Ϊ��ʷ��Ѿ���

	// ���ⲽ����������������
	p=new tree_node;
	p->father=father;
	p->h=father->h+1;
	p->tile=tile_num(x,y);
	p->dir = dir;
	enter_queue(p,p->h+judge(x,y));
	return 0;
}

// ·��Ѱ�������� //from �Ʒ�
bool cAStar::findpath(PATH& listWay)
{
	TREE root;
	int i;
	memset(dis_map,0xff,map_h*map_w*sizeof(*dis_map));    //���dis_mapΪ0XFF����ʾ����δ������
	init_queue();
	root=new tree_node;
	root->tile=tile_num(start_x,start_y);
	root->h=0;
	root->dir = -1;
	root->father=NULL;
	enter_queue(root,judge(start_x,start_y));
	int dir = math_get_dir8(g_point(end_x,end_y),g_point(start_x,start_y));
	for (;;) {
		int x,y,child;
//		TREE p;
		root=get_from_queue();
		if (root==NULL) {
#ifdef Trace_AStarFindPath
			gotoxy(1,map_h+2);
			cout << "not find way" << endl;
#endif
			freetree();
			return false;
		}
		x=tile_x(root->tile);
		y=tile_y(root->tile);
#ifdef Trace_AStarFindPath
		//draw_point(x,y,'\'');
#endif
		if (x==end_x && y==end_y) break; // �ﵽĿ�ĵسɹ�����

		child=1;

		//�����������ƶ�ʱ���������﷽��,��ʱ���ﶼ�����ͨ�е�
		for (int i=0;i<8; ++i)
		{
			int current_dir = valid_dir8(dir+i);
			child&=trytile(current_dir, x+c_point_dir8_x[current_dir],y+c_point_dir8_y[current_dir],root);  
		}
		//child&=trytile(0,x,y-1,root);  //���������ƶ�
		//child&=trytile(1,x+1,y-1,root);//�����������ƶ�
		//child&=trytile(2,x+1,y,root); //���������ƶ�
		//child&=trytile(3,x+1,y+1,root); //�����������ƶ�
		//child&=trytile(4,x,y+1,root); //���������ƶ�
		//child&=trytile(5,x-1,y+1,root); //�����������ƶ�
		//child&=trytile(6,x-1,y,root); //���������ƶ�
		//child&=trytile(7,x-1,y-1,root); //�����������ƶ�

		if (child!=0)
			pop_stack();  // ����ĸ�����������ƶ�,�ͷ�������ڵ�
	}

	// ������������������·�������� path[] ��
	// cout << "find way:  ";
	g_point pt = g_point(end_x,end_y);
	g_point prev_pt = pt;

	int prev_dir = -1;

	if (root->dir < 0)   //����յ㷽���Ǹ��ģ�˵��û�ҵ�·��
	{
		return false;
	}
	for (i=0;root->dir>=0;i++) {
		//cout << root->dir << ",";
		int backdir = root->dir + 4;
		if (backdir >= 8)
			backdir -= 8;
		pt += c_aPointDir8[backdir];
#ifdef Trace_AStarFindPath
		draw_point(pt.x,pt.y,'X');
#endif
		if (root->dir != prev_dir)
		{
			prev_dir = root->dir ;
		}
		else
		{
			//�����ڶ����ڵ�Ҫ����
			if (root->father->dir>=0)
			{
				listWay.pop_front();
			}
		}
		if(root->father->dir < 0)
		{
			//���һ�㲻Ҫ���룬�����ⲿ���Ż�
			//g_point pt_edge = g_point((pt.x + prev_pt.x)/2, (pt.y + prev_pt.y)/2);
			//listWay.push_front(pt_edge + g_point(offx, offy));
			prev_pt = pt;
			root = root->father;
			continue;
		}

		listWay.push_front(pt + g_point(offx,offy));
		prev_pt = pt;
		//listWay.push_front(root->dir);
		root=root->father;
	}
#ifdef Trace_AStarFindPath
	cout << endl;
#endif
	freetree();
	return true;
}

