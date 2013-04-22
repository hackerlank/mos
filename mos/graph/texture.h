#ifndef __GRAPH_TEXTURE_H_
#define __GRAPH_TEXTURE_H_

//wrap opengl texture not directx.
struct g_rect;
class st_cell;

class texture
{
public:
	virtual void set_alpha(bool alpha) = 0;
	virtual bool create_texture_dynamic(int width,int height,int m_bits_component) = 0;
	virtual int draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) = 0;
	virtual bool create_texture(image* img,const g_rect* rc) = 0;

	int draw_cell(const st_cell& cell,const g_rect* rc = 0);

	unsigned int m_time_use;
	void mark_use_texture(unsigned int time){
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