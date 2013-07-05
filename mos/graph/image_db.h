#ifndef __GRAPH_IMAGE_DB_H_
#define __GRAPH_IMAGE_DB_H_

#include "core/rect.h"
#include <string>

struct st_redirect
{
	std::string id_texture;
	std::string file_image;
	g_point cg; //Center of gravity
	g_rect rc;
};

int get_fileframe_hash(const char* file,int frame);
//button.jpg 1 --> ui.jpg (200,300,64,32)
const st_redirect* redirect_image_file(const char* file,int frame);
void regist_image_file(const char* file,int frame,st_redirect& rc);
bool regist_image_ini(const char* ini_file,int& dir,int& frame);

//image file's palette
//��ʵ��ð�file��pal��Ϊͬһ���ļ�������png��֧�֡�
//����Լ������ʽ�����¹������ܸ��ӣ��������ˣ���Ӹ��ļ���
void regist_image_palette(const char* image,const char* palette_file);
const char* get_image_palette(const char* image);
bool is_image_use_palette(const char* image);

//��Ϊ3d�Ĺ�ϵ��������һ�����飬����һ����ͼ��������һ�鶯����һ��ui��ƴ��ͬһ����ͼ�ϣ�����Ҳ��Ҫ<=1024*1024��
//����Ƕ����Ļ�����������һ���������
//�����2d��ԭ����ģʽ���ͱ��뿼�Ƕ�̬�Զ�ƴ��ԴͼΪһ��texture�Ĵ�ͼ���Ƚ��鷳�����ٶ�̬֧�֣�������������֧�֡�

#endif