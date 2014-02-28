#pragma once

#include <string>
#include "device/file.h"

typedef unsigned long      DWORD;
typedef unsigned char      BYTE;

struct NORMAL_MAP_HEADER
{
	int mapid;
	DWORD width;
	DWORD height;
};

struct NORMAL_MAP_POSITION
{
	int block_num;
	int *block_offset;
	int mask_seek;
	int mask_num;
	int *mask_offset;
	NORMAL_MAP_POSITION(){
		block_offset = 0;
		mask_offset = 0;
	}
	~NORMAL_MAP_POSITION()
	{
		delete block_offset;
		block_offset = 0;
		delete mask_offset;
		mask_offset = 0;
	}
};

struct NORMAL_MAP_BLOCK
{
	int mask_num;
	int *mask_index;
	int jpeg_len;
	char *jpeg_data;
	char *cell_data;
	char *bright_data;
};

struct NORMAL_BLOCK_INFO
{
	int mask_num;
	int *mask_index;
	int jpeg_len;
	int jpeg_offset;
	int cell_len;
	int cell_offset;
	int bright_len;
	int bright_offset;
	NORMAL_BLOCK_INFO(){mask_index = 0;}
	~NORMAL_BLOCK_INFO(){
		if(mask_index)
			delete mask_index;
	}
};

struct WRect {
	int x;
	int y;
	int w;
	int h;
	bool Contain(int x0,int y0) const
	{
		return x0>=x && x0<x+w && y0>=y && y0<y+h;
	}
};

struct MASK_DATA{
	WRect rect;
	int len;
	void *data;
	MASK_DATA(){data = 0;}
	~MASK_DATA(){
		if(data)
			delete data;
	}
};


#define BLOCKW	640
#define BLOCKH	480

class mapdata;

class WMask;
//class WBitmap;
class image;

class WMask
{
public:
	//const image* bmp;
	int kx,ky,w,h;
	BYTE * mask;

	int oky,oh;
	BYTE * omask;

	int id;
	int pitch;
	void Create(int i,int x,int y,int _w,int _h,void *ptr);
	void Render(image* img8);
	//BYTE* Render8888();
	//BYTE* Render888();
	bool IsCover(int x,int y) const;
	WMask()
	{
		omask = NULL;
	}
	~WMask();
};


class map_block
{
public:
	bool load_whole(mapdata *head);

	image* m_Image;
	std::string m_image_name;
	WMask* m_masks;

	int x,y,num;
	void set_pos(int _x,int _y,int _num)
	{
		x = _x;
		y = _y;
		num = _num;
	}

	map_block()
	{
		m_Image = 0;
		m_masks = 0;
	}
	~map_block();
};

inline int up_div(int a,int b)
{
	return (a + (b-1))/b;
}

class mapdata
{
public:
	MOS_FILE_HANDLE f;
	mapdata()
	{
		f = 0;
		_mask_data = 0;
		_block_info = 0;
		_blocks = 0;
	}
	~mapdata();
	void destroy_mapdata();

	NORMAL_MAP_HEADER _map_header;
	NORMAL_MAP_POSITION _map_position;
	MASK_DATA* _mask_data ;
	NORMAL_BLOCK_INFO* _block_info;
	int width,height;
	int get_width() const{return width;}
	int get_height() const{return height;}
	

	void *get_block_image(int n, int &len);
	//unsigned char *get_block_cell(int n, int &len);

	const int *get_block_maskindex(int n, int &len)
	{
		len = _block_info[n].mask_num;
		return _block_info[n].mask_index;
	}
	const MASK_DATA *get_mask(int index){ return &_mask_data[index];}
	int get_mask_num(int n) const{ return _block_info[n].mask_num;} 


	bool load_map(const std::string& map);	
	std::string m_map;

	void load_all_blocks();

	map_block* _blocks;
	int get_block_width() const{
		return up_div(width,BLOCKW);
	}
	int get_block_height() const{
		return up_div(height,BLOCKH);
	}
	void num_2_pos(int num,int& x,int& y){
		int w = get_block_width();
		y = num / w;
		x = num % w;
	}
	int pos_2_num(int x,int y){
		return y * get_block_width() + x;
	}	
	map_block* get_block(int x,int y){
		if (x < 0 || x >= get_block_width())
			return NULL;
		if (y < 0 || y >= get_block_height())
			return NULL;
		return &_blocks[pos_2_num(x,y)];
	}

	//void save_jpg_dir(const char* dir);
};