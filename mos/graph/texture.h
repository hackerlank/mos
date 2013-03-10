#ifndef __GRAPH_TEXTURE_H_
#define __GRAPH_TEXTURE_H_

//wrap opengl texture not directx.
class image;
#include <string>

class texture
{
public:
	~texture();
	image* m_image;

	bool create_texture(image* img,const g_rect* rc);
	int draw_cell(const st_cell& cell,const g_rect* rc = NULL);

	unsigned int m_time_use;
	void mark_use(unsigned int time){
		m_time_use = time;
	}

	//const g_rect* m_image_rect;
};

//texture class: 
//texture_whole	������ui�����ܵ�ƴ��ͬһ��ͼ��������˵icon
//texture_part	ÿ��part��whole��һ��rect
//texture,		���綯����Щ��ƴ��
//texture_font	�������ֺϲ���ͬһ����ͼ��������ʹ�õ�Ƶ�������£�
//texture_char	����part����font��һ��rect


#endif