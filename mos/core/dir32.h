#ifndef DIR32_H
#define DIR32_H

#include "rect.h"
#include "dir.h"

const int max_point32 = 100;

const int c_nNumDir8 = 8;		//8������

extern g_point c_aPointDir8[c_nNumDir8] ;

extern g_point c_aPoint32[32][max_point32];

void init_dir32();

int math_GetDir32(int x, int y);
inline int math_GetDir32(const g_point& ptTo,const g_point& ptFrom)
{
	g_point pt = ptTo - ptFrom;
	return math_GetDir32(pt.x,pt.y);
}

//pos �� dir��������ô
//dir ��32�����dir
//angle = pi/32 �ı��� must <= 8 =8 mean һ��
bool IsFan(const g_point& pos, int dir32, int angle);

#endif
