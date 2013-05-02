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

//因为3d的关系，不再用一个数组，而是一个大图。尽量把一组动作，一套ui，拼在同一个大图上，但是也需要<=1024*1024。
//如果是动作的话，还必须有一个概念：重心
//想兼容2d的原本的模式，就必须考虑动态自动拼资源图为一个texture的大图，比较麻烦，不再动态支持，而是做工具来支持。

#endif