#include "mapdata.h"
#include "../graph/image.h"
#include "../graph/graph.h"

#include <assert.h>

#define ASSERT2(a,b)
#define ASSERT3(a,b,c)

void *alloc_mask(int w, int h) 
{
	return new char [up_div(w,4)*h];
}

int decompress (const void *in , void *out)
{
	unsigned char *op;
	const unsigned char *ip;
	unsigned long t;
	unsigned char *m_pos;

	op = (unsigned char *) out;
	ip = (const unsigned char *) in;

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		do *op++ = *ip++; while (--t > 0);
		goto first_literal_run;
	}

	for(;;) {
		t = *ip++;
		if (t >= 16) goto match;
		if (t == 0) {
			while (*ip == 0) {
				t += 255;
				ip++;
			}
			t += 15 + *ip++;
		}

		* (unsigned long *) op = * ( unsigned long *) ip;
		op += 4; ip += 4;
		if (--t > 0)
		{
			if (t >= 4)
			{
				do {
					* (unsigned long * ) op = * ( unsigned long * ) ip;
					op += 4; ip += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *ip++; while (--t > 0);
			}
			else do *op++ = *ip++; while (--t > 0);
		}

first_literal_run:

		t = *ip++;
		if (t >= 16)
			goto match;

		m_pos = op - 0x0801;
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;

		*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos;

		goto match_done;

		for(;;)
		{
match:
			if (t >= 64)
			{

				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;

				goto copy_match;

			}
			else if (t >= 32)
			{
				t &= 31;
				if (t == 0)	{
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 31 + *ip++;
				}

				m_pos = op - 1;
				m_pos -= (* ( unsigned short * ) ip) >> 2;
				ip += 2;
			}
			else if (t >= 16) {
				m_pos = op;
				m_pos -= (t & 8) << 11;
				t &= 7;
				if (t == 0) {
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 7 + *ip++;
				}
				m_pos -= (* ( unsigned short *) ip) >> 2;
				ip += 2;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			}
			else {
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				*op++ = *m_pos++; *op++ = *m_pos;
				goto match_done;
			}

			if (t >= 6 && (op - m_pos) >= 4) {
				* (unsigned long *) op = * ( unsigned long *) m_pos;
				op += 4; m_pos += 4; t -= 2;
				do {
					* (unsigned long *) op = * ( unsigned long *) m_pos;
					op += 4; m_pos += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *m_pos++; while (--t > 0);
			}
			else {
copy_match:
				*op++ = *m_pos++; *op++ = *m_pos++;
				do *op++ = *m_pos++; while (--t > 0);
			}

match_done:

			t = ip[-2] & 3;
			if (t == 0)	break;

match_next:
			do *op++ = *ip++; while (--t > 0);
			t = *ip++;
		}
	}

eof_found:
	//   if (ip != ip_end) return -1;
	return (op - (unsigned char*)out);
}

//int _load_jpeg_ex(WBitmap *bmp,void *jpeg_buf,DWORD buf_size);

bool map_block::load_whole(mapdata*head)
{
	int n = num;
	//if (((head->get_width()+BLOCKW-1)/BLOCKW)*((head->get_height()+BLOCKH-1)/BLOCKH)<=(unsigned)n)
	//	return false;	// ��ͼû����ô���
	//head may be release here!!!

	int len = 0;
	// load JPEG
	void* compress_data = head->get_block_image(n, len);
	
	//int jpeg_result = _load_jpeg_ex(m_Image, compress_data, len);

	//int w = (head->get_width()+BLOCKW-1)/BLOCKW;
	//int y = i / w;
	//int x = i % w;

	char filename[256];
	sprintf(filename,"%s_%02d%02d.jpg",head->m_map.c_str(),y,x);
	m_image_name = filename;
	m_Image = get_graph()->image_map[filename];
	if (!m_Image)
	{
		image* image888 = image::create_image_file_buffer(filename,compress_data,len);

		int m_MaskNum = head->get_mask_num(n);
		if( m_MaskNum > 0)
		{
			image* image8 = new image;
			image8->create_image_dynamic(BLOCKW,BLOCKH,1);
			//WMask* m_Mask = new WMask[m_MaskNum];
			const int *maskindex = head->get_block_maskindex(n, len);
			for(int i=0; i< m_MaskNum; i++)
			{
				//if (i != 0)
					//continue;
				const MASK_DATA *mask = head->get_mask(maskindex[i]);
				//printf("get mask %d %d\n",n,maskindex[i]);
				int size = up_div(mask->rect.w,4)*mask->rect.h;
				void* temp = alloc_mask(mask->rect.w, mask->rect.h);
				int result = decompress(mask->data,temp);
				ASSERT2(result == size,"decompress mask error!");
				
				WMask m;
				m.Create((int)mask->rect.x-BLOCKW*x, (int)mask->rect.y-BLOCKH*y,mask->rect.w,mask->rect.h, temp);
				m.Render(image8);
				
				delete temp;
			}

			m_Image = get_image8888_888_8(image888,image8);
			delete image888;
			delete image8;
		}
		else
		{
			m_Image = image888;
		}
		get_graph()->maped_image(filename,m_Image);
	}
	else
		m_Image ->image_add_ref();

	delete [] compress_data;
	ASSERT3(jpeg_result==0,"��ͼjpeg Error",jpeg_result);
	// load cell
	//compress_data = head->get_block_cell(n, len);
	//m_Cell = 
	//memcpy(m_Cell, compress_data, len);
	//delete[] compress_data;
	return true;
}

bool mapdata::load_map(const std::string& map)
{
	destory_mapdata();

	int i;
	f = mfopen(map.c_str(), "rb");
	if (!f)
		return false;

	mfread(&_map_header, sizeof(_map_header), 1, f);

	if(_map_header.mapid != 'M1.0')
	{
		mfclose(f);
		return false;
	}

	m_map = map;

	width = _map_header.width;
	height = _map_header.height;

	_map_position.block_num = get_block_width()*get_block_height();
	_blocks = new map_block[_map_position.block_num];
	for (int i=0; i<_map_position.block_num; i++)
	{
		int x,y;
		num_2_pos(i,x,y);
		_blocks[i].set_pos(x,y,i);
	}

	_map_position.block_offset = new int[_map_position.block_num];
	mfread(_map_position.block_offset, 4, _map_position.block_num, f);

	mfread(&_map_position.mask_seek, 4, 1, f);
	mfread(&_map_position.mask_num, 4, 1, f);

	if (_map_position.mask_num > 0)
	{
		_map_position.mask_offset = new int[_map_position.mask_num];
		mfread(_map_position.mask_offset, 4, _map_position.mask_num, f);

		_mask_data = new MASK_DATA[_map_position.mask_num];

		for(i = 0; i < _map_position.mask_num; i++)
		{
			mfseek(f, _map_position.mask_offset[i], SEEK_SET);
			mfread(&_mask_data[i].rect, sizeof(WRect), 1, f);
			mfread(&_mask_data[i].len, 4, 1, f);
			_mask_data[i].data = new char[_mask_data[i].len];
			mfread(_mask_data[i].data, _mask_data[i].len, 1, f);
		}
	}

	_block_info = new NORMAL_BLOCK_INFO[_map_position.block_num];
	for(i = 0; i < _map_position.block_num; i++)
	{
		NORMAL_BLOCK_INFO *p = &_block_info[i];
		mfseek(f, _map_position.block_offset[i], SEEK_SET);
		mfread(&p->mask_num, 4, 1, f);
		if(p->mask_num > 0)
		{
			p->mask_index = new int[p->mask_num];
			mfread(p->mask_index, 4, p->mask_num, f);
		}
		else
			p->mask_index = 0;
		mfseek(f, 4, SEEK_CUR);
		mfread(&p->jpeg_len, 4, 1, f);
		p->jpeg_offset = mftell(f);

		p->cell_offset = p->jpeg_offset + p->jpeg_len + 8;
		mfseek(f, p->cell_offset-4, SEEK_SET);
		mfread(&p->cell_len, 4, 1, f);

		p->bright_offset = p->cell_offset + p->cell_len + 8;
		mfseek(f, p->bright_offset-4, SEEK_SET);
		mfread(&p->bright_len, 4, 1, f);

		mfseek(f, 0, 0);
	}

	//loadblocks();
	return true;
}

void mapdata::load_all_blocks()
{
	//_blocks = new map_block[_map_position.block_num];
	for (int i=0; i<_map_position.block_num; i++)
	{
		_blocks[i].load_whole(this);
	}
}

void mapdata::destory_mapdata()
{
	if (f)
	{
		mfclose(f);
		f = 0;
	}

	delete []_mask_data;
	_mask_data = NULL;

	delete []_block_info;
	_block_info = NULL;

	delete []_blocks;
	_blocks = NULL;
}

mapdata::~mapdata()
{
	destory_mapdata();
}


void *mapdata::get_block_image(int n, int &len)
{
	mfseek(f, _block_info[n].jpeg_offset, SEEK_SET);
	len = _block_info[n].jpeg_len;
	char *data = new char[_block_info[n].jpeg_len];
	mfread(data, _block_info[n].jpeg_len, 1, f);
	return data;
}

//int save_jpg(const char* file,WBitmap *bmp);
//int save_png(const char* file,WMask *mask);
//
//void mapdata::save_jpg_dir(const char* dir)
//{
//	CreateDirectory(dir,NULL);
//	int w = (width+BLOCKW-1)/BLOCKW;
//	for (int i=0; i<_map_position.block_num; i++)
//	{
//		char buf[128];
//		int y = i / w;
//		int x = i % w;
//		sprintf(buf,"%s\\%02d%02d.jpg",dir,y,x);
//		printf("save... %s\n",buf);
//		map_block* p = &_blocks[i];
//		save_jpg(buf,p->m_Image);
//
//		for (int j=0; j<p->m_MaskNum;j++)
//		{
//			sprintf(buf,"%s\\%02d%02d_%02d.jpg",dir,y,x,j);
//			save_png(buf,&p->m_Mask[j]);
//		}
//	}
//
//}

map_block::~map_block()
{
	if (m_Image)
	{
		m_Image->image_release();
		m_Image = 0;
	}
}

void WMask::Render(image* img8)
{
	colorbyte* buf = img8->get_buf_offset(kx,ky);
	colorbyte* desline = buf;
	colorbyte* srcline = mask;
	BYTE mm[4] = {0x03,0x0c,0x30,0xc0};

	for (int y=0; y<h; y++)
	{
		colorbyte* des = desline;
		colorbyte* src = srcline;

		for (int x=0; x<w/4; x++,src++)
		{
			BYTE m = *src;
			for (int i=0; i<4; i++)
			{
				if ((m &mm[i]) == mm[i])
				{
					*des++ = 255;
				}
				else
				{
					*des++ = 0;
				}
			}
		}

		BYTE m = *src;
		for (int i=0; i<w%4; i++)
		{
			if ((m &mm[i]) == mm[i])
			{
				*des++ = 255;
			}
			else
			{
				*des++ = 0;
			}
		}

		desline += img8->get_line_pitch();
		srcline += pitch;
	}
}

void WMask::Create(int x,int y,int _w,int _h,void *ptr)
{
	kx = x;
	ky = y;
	w = _w;
	h = _h;

	pitch = up_div(w,4);
	//printf("WMask::Create x %d y %d w %d h %d\n",x,y,_w,_h);
	mask = (BYTE *) ptr; //alpha channel
	if (ky < 0)
	{
		h -= -ky;
		mask += -ky * pitch;
		ky = 0;
	}
	assert(kx >= 0 && ky >= 0 && w >=0 && h >= 0 );
	if (ky + h > BLOCKH)
		h = BLOCKH - ky;

	assert(kx + w <= BLOCKW && ky + h <= BLOCKH);
	//bmp = b;
}
