#include "image_zgp.h"
#include "../graph/image.h"
#include "device/file.h"

//����Ϊ��ͨ�ã���ʹ��png�ĸ�ʽ��
//���ҷ�����Ҫzgp�ĸ�ʽ�����Ż�����pngҪ��һ����
//����pngû�п����Ϣ����Ϊ�˷ֿ�Ⱦɫ�������ڴ涯̬չ�������Ǳ����õġ�
//���ԡ�����

#include <windows.h>

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

DWORD color565_8888(DWORD color)
{
	return (color<<5&0x7fc00)|(color<<8&0xf80000)|((color<<3|(color&7))&0x3ff);
};

struct st_header{
	WORD id;
	WORD ver;
	WORD len;
};

//#define CHECK_BLOCK if (block < 0 || mask_type == block)
#define CHECK_BLOCK 

__inline DWORD MakeSrcDword(DWORD wb)
{
	return ((wb << 16) | wb) & 0x7e0f81f;
}

DECLARE_COUNTER(image_zgp)

bool sprite_zgp::create_sprite(WORD* pal,void* _data)
{
	WSpriteStruct *spt_src=(WSpriteStruct*)(_data);
	WSpriteStruct *spt_this=&m_data;
	spt_this->kx = spt_src->kx; 
	spt_this->ky = spt_src->ky; 
	spt_this->w = spt_src->w; 
	spt_this->h = spt_src->h; 
	spt_this->data=0;
	spt_this->sprite_userdata=spt_src->sprite_userdata;
	spt_this->line=(BYTE **)&spt_src->line;
	for (int i=0;i<spt_this->h;i++) {
		spt_this->line[i]+=(DWORD)_data;
		//BYTE p = *(spt_this->line[i]);
	}
	spt_this->pal=pal; 
	spt_type = 0;
	//sprite�ǽ��յģ�����Ҫ��trim�ˡ�
	return true;
}

bool sprite_zgp::create_empty()
{
	WSpriteStruct *spt_this=&m_data;
	spt_this->kx=0;
	spt_this->ky=0;
	spt_this->w=0;
	spt_this->h=0;
	spt_this->line=0;
	spt_this->data=0;
	spt_this->pal=0;
	spt_this->sprite_userdata = 0;	
	spt_type = 1;	
	return true;
}

//û�п��Ƿֿ���Ϣ�����Բ������ã�
void sprite_zgp::render8888(int x,int y,DWORD* buf8888,int pitch,int block,WORD* pal) const
{
	DWORD* deslineptr = buf8888 + pitch * y + x;
	if (pal == NULL)
		pal = m_data.pal;
	int w = m_data.w;
	int h = m_data.h;
	int off = 0;

	WORD tpixel;
	//��Ⱦ��buf
	for (int i=0; i<h; i++,deslineptr+=pitch)
	{
		DWORD* lineptr = deslineptr;
		BYTE* src = m_data.line[i];
		if(*src == 0xff && *(src+1)==0xff ) //mean�ظ�
			src=m_data.line[i-1];
		for (int j=0; j<w;)
		{
			BYTE mask_type, alpha;
			BYTE flag, style;
			style=*src++;
			if (style==0) 
				break;
			flag=style&3;
			style=style>>2;
			int k;
			int m;
			switch(flag)
			{
			case 0://ȫ͸��
				{
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						for (int z = 0; z < m-k; z++)
						{
							//*lineptr++ = tpixel;
							*lineptr = 0;								
							lineptr++;
						}
						//lineptr += m - k;
						j=m;
					}
					else
						j = k;
					break;
				}
			case 1://���������͸������color��ͬ����
				{
					mask_type = *src++;
					tpixel=pal[*src+mask_type*256];
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						for (int z = 0; z < m-k; z++)
						{
							//*lineptr++ = tpixel;
							CHECK_BLOCK
								*lineptr = color565_8888(tpixel) | 0xff000000;								
							lineptr++;
						}
						j = m;
					}
					else
						j = k;

					++src;
					break;
				}
			case 2://�����͸������color��ͬ����
				{
					alpha= *src++;
					mask_type = alpha>>5;
					alpha&=31;
					tpixel=pal[*src+mask_type*256];
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						//DWORD sd = MakeSrcDword(tpixel);
						//alpha = 32 - (alpha+1);
						for (int z = 0; z < m-k; z++)
						{
							//MakeAlpha32Inverse((WORD*)lineptr,sd,alpha);
							CHECK_BLOCK
								*lineptr = color565_8888(tpixel) | (alpha << (24+3));								
							lineptr++;
						}
						j = m;
					}
					else
						j = k;

					++src;
					break;
				}
			case 3://���������͸������color����ͬ����
				{
					mask_type = *src++;
					int mask_offset = mask_type*256;

					for (k=j;k<j+style && k<off;k++,++src);
					for (;k<j+style && k<w;k++,++lineptr,++src) 
					{
						//*lineptr=pal[*src+mask_offset];
						CHECK_BLOCK
							*lineptr=color565_8888(pal[*src+mask_offset]) | 0xff000000;
					}
					j=k;

					break;
				}
			}
		}
	}
}


void mread(void* dest,char*& src,size_t size)
{
	memcpy(dest,src,size);
	src += size;
}


const int pal512 = 256 * 2; //256ɫ��word


//���ֻ���ͼƬ������png 256ɫ�ģ�ÿ��������һ�У�����һ��
bool image_zgp::loadzgp_file(const char* zgp)
{
	FILE* f = fopen(get_resourcefile(zgp),"rb");
	if (!f)
		return false;
	int seek = fseek(f,0,SEEK_END);
	size_t size = ftell(f);
	fseek(f,0,SEEK_SET);
	//char* _data = new char[size];
	_data = new char[size];
	fread(_data,size,1,f);
	fclose(f);
	bool rt = loadzgp_memory(zgp,_data);
	//delete _data;
	return true;
}

bool image_zgp::loadzgp_memory(const char* zgp,char* _data)
{
	st_header header;
	char* data = (char* )_data;
	mread(&header,data,sizeof(header));
	mread(&pak,data,sizeof(pak));

	if (pak.extern_len)
	{
		char *ex_data = new char[pak.extern_len];
		mread(ex_data, data,pak.extern_len);
		delete []ex_data;
	}

	int allframe = pak.frame * pak.dir;
	size_t offset_size = allframe * sizeof(DWORD);

	//int m_Size=size + (pak->pal_num-1)*pal512 - header.len - sizeof(header)-pak->extern_len - offset_size;

	pal = new WORD[256 * pak.pal_num];
	mread(pal,data,pal512);
	for(int i=1;i<pak.pal_num;i++)
	{
		//�ֿ��ˡ�
		memcpy((char *)pal+pal512*i, pal, pal512);
	}

	DWORD* offtbl = new DWORD[offset_size];
	mread(offtbl,data,offset_size);

	sprites = new sprite_zgp[allframe];
	int frame = 0;

	int width = 0;
	int height = 0;
	int y = 0;
	for (int i=0; i<pak.dir; i++)
	{
		int x = 0;
		int h = 0;
		for (int j=0; j<pak.frame; j++,frame++)
		{
			sprite_zgp& spr = sprites[frame];
			if (offtbl[frame])
			{
				char* off = data + offtbl[frame] - offset_size;
				spr.create_sprite(pal,off);
			}
			else
			{
				spr.create_empty();
			}

			//sprites[frame].set_pos(x,y);
			x += spr.m_data.w;
			if (h < spr.m_data.h)
				h = spr.m_data.h;
		}
		if (width < x)
			width = x;
		y += h;
	}
	height = y;

	width = ((width + 3) / 4) * 4;


	//delete [] pal;
	delete []offtbl;
	//delete []m_Data;
	//delete []sprites;

	//delete []_data;
	return true;
}

bool regist_zgp(const char* file,int& dir,int& frame)
{
	FILE* f = fopen(get_resourcefile(file),"rb");
	if (!f)
		return false;
	st_header header;
	fread(&header,sizeof(header),1,f);
	WSpritePakFileHeader pak;
	fread(&pak,sizeof(pak),1,f);
	fclose(f);
	dir = pak.dir;
	frame = pak.frame;
	return true;
}

image_zgp::~image_zgp()
{
	delete [] pal;
	delete []sprites;
	delete _data;
}

image* image_zgp::get_sprite_image(int frame,int block ,WORD* pal ) const
{
	sprite_zgp* s = get_sprite(frame);

	image* i = new image();
	i->create_image_dynamic(s->m_data.w,s->m_data.h,4);
	s->render8888(0,0,(DWORD*)i->get_buffer(),i->get_width(),block,pal);
	i->set_cg(s->get_cg());
	i->rgb2bgr();
	return i;
}