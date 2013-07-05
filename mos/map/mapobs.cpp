#include "mapobs.h"
#include "../device/file.h"
#include "core/AStar.h"
#include "core/cc.h"
#include "core/dir32.h"

int mapobs::is_block_safe(int x,int y) const
{
	if (x < 0 || x >= m_width)
		return true;
	if (y < 0 || y >= m_height)
		return true;
	return is_block(x,y);
}


mapobs::~mapobs()
{
	destory_obs();
}

void mapobs::destory_obs()
{
	delete m_buf;
	m_buf = NULL;
	delete m_astar;
	m_astar = NULL;
}

void mapobs::smooth_way(std::list<point2>& path)
{
	//from first to end
	std::list<point2> l;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		point2 pt = *it;
		if (l.empty())
			l.push_back(*it);
		else
		{
			if (is_block_seam_pixel(l.back(),pt))
			{
				auto it2 = it;
				it2 --;
				l.push_back(*it2);
			}
		}
	}
	l.pop_front();
	l.push_back(path.back());
	path.swap(l);
}


bool mapobs::is_block_point(const point2& pos) const
{
	return is_block_safe(pos.x,pos.y);
}

bool mapobs::is_block_point_pixel(const point2& pos) const
{
	point2 p1 = pos;
	pixel_2_cell(p1);
	return is_block_point(pos);
}

bool mapobs::is_block_seam_pixel(const point2& from,const point2& to) const
{
	point2 p1 = from;
	pixel_2_cell(p1);
	point2 p2 = to;
	pixel_2_cell(p2);
	return is_block_seam(p1,p2);
}

inline void ExchangeSameX(g_point& p1,g_point& p2)
{
	if (p1.x > p2.x)
	{
		g_point temp = p1;
		p1 = p2;
		p2 = temp;
	}
}

bool mapobs::is_block_seam(const g_point& _p1,const g_point& _p2) const
{
	g_point p1 = _p1;
	g_point p2 = _p2;
	ExchangeSameX(p1,p2);
	int dir = math_GetDir32(p2.x-p1.x,p2.y-p1.y);
	if (dir == -1)
		return false;

	int xmin = QIN_MIN(p1.x,p2.x);
	int xmax = QIN_MAX(p1.x,p2.x);
	int ymin = QIN_MIN(p1.y,p2.y);
	int ymax = QIN_MAX(p1.y,p2.y);
	//int bit = 1 << (e);
	for (int i=0;i<max_point32;i++)
	{
		g_point p = p1 + c_aPoint32[dir][i];
		//char str_point[32];
		//sprintf(str_point, "c_aPoint32[%d][%d](%d,%d)", dir, i, c_aPoint32[dir][i].x, c_aPoint32[dir][i].y);

		if (p.x > xmax || p.x < xmin || p.y > ymax || p.y < ymin)
		{
			//char str_point[32];
			//sprintf(str_point, "c_aPoint32[%d][%d](%d,%d)", dir, i, c_aPoint32[dir][i].x, c_aPoint32[dir][i].y);
			//cout << "point_dir:" << str_point << endl;
			return false;
		}	
		if (is_block_point(p))
			return true;
	}

	return true;
}

bool mapobs::find_path(const point2& from,const point2& to,std::list<point2>& path)
{
	//if (is_block_safe(from.x,from.y) || is_block_safe(to.x,to.y))
	//	return false;
	return m_astar->FindWay(from,to,path);
}

bool mapobs::find_path_pixel(const point2& from,const point2& _to,std::list<point2>& path)
{
	point2 to = _to;
	if (!is_block_seam_pixel(from,to))
	{
		path.push_back(to);
		return true;
	}

	point2 p1 = from;
	pixel_2_cell(p1);

	point2 p2 = to;
	pixel_2_cell(p2);

	int rt2 = find_notobs_seam(p1,p2);
	if (rt2 == -1)
		return false;
	if (rt2 == 1)
	{
		to = p2;
		cell_2_pixel(to);
	}

	bool rt = find_path(p1,p2,path);
	if (!rt)
		return rt;
	for (auto it=path.begin(); it != path.end(); ++it)
	{
		(*it).x *= m_scale;
		(*it).y *= m_scale;
	}

	//�������һ���㣬������ȷ�㡣
	path.push_back(to);

	//Բ����·����
	path.push_front(from);
	smooth_way(path);

	return rt;
}

int mapobs::find_notobs_seam(const point2& pos, point2& click) const
{
	if (click == pos)
		return 0;
	if (!is_block_point(click))
		return 0;
	//�г�ֱ�߷��� click-pos��
	float x1 = click.x;
	float y1 = click.y;
	float x2 = pos.x;
	float y2 = pos.y;

	if (click.x == pos.x)
	{
		for (int y = click.y-1; y != pos.y ; y -= (click.y-pos.y)/abs(click.y-pos.y) )
		{
			click.y = y;
			if (!is_block_point(click))
				return 1;
		}
		return -1;
	}
	else if (click.y == pos.y)
	{
		for (int x = click.x-1; x != pos.x ; x -= (click.x-pos.x)/abs(click.x-pos.x) )
		{
			click.x = x;
			if (!is_block_point(click))
				return 1;
		}
		return -1;
	}

	float k = (y2-y1)/(x2-x1);
	if (fabs(k) < 1.f)
	{
		float b = y1 - k * x1; 
		for (int x = click.x-1; x != pos.x ; x -= (click.x-pos.x)/abs(click.x-pos.x) )
		{
			click.x = x;
			click.y = k * x + b;
			if (!is_block_point(click))
				return 1;
		}
	}
	else
	{
		k = 1.f / k;
		float b = x1 - k * y1;
		for (int y = click.y-1; y != pos.y ; y -= (click.y-pos.y)/abs(click.y-pos.y) )
		{
			click.y = y;
			click.x = k * y + b;
			if (!is_block_point(click))
				return 1;
		}
	}

	return -1;
}


void mapobs::create_obs(int w,int h,unsigned char* buf,int scale)
{
	destory_obs();
	m_width = w;
	m_height = h;
	m_buf = buf;
	m_scale = scale;
	
	m_astar = new cAStar();
	m_astar->create_astar(g_size(w,h),buf,g_rect(0,0,w,h));
}
