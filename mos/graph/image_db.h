#ifndef __GRAPH_IMAGE_DB_H_
#define __GRAPH_IMAGE_DB_H_

#include "rect.h"
#include <string>

struct st_redirect
{
	std::string id_texture;
	std::string file_image;
	g_point cg; //Center of gravity
	g_rect rc;
};

//button.jpg 1 --> ui.jpg (200,300,64,32)
const st_redirect* redirect_image_file(const char* file,int frame);
void regist_image_file(const char* file,int frame,st_redirect& rc);

//��Ϊ3d�Ĺ�ϵ��������һ�����飬����һ����ͼ��������һ�鶯����һ��ui��ƴ��ͬһ����ͼ�ϣ�����Ҳ��Ҫ<=1024*1024��
//����Ƕ����Ļ�����������һ���������
//�����2d��ԭ����ģʽ���ͱ��뿼�Ƕ�̬�Զ�ƴ��ԴͼΪһ��texture�Ĵ�ͼ���Ƚ��鷳�����ٶ�̬֧�֣�������������֧�֡�

#endif