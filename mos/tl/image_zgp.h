#ifndef IMAGE_ZGP_H
#define IMAGE_ZGP_H

//image_zgpʹ��zgp��ʽ��256ɫÿ��ͼƬ�ֿ飨ͷ������������װ�Σ��洢��ÿ���鶼��һ����ɫ��
//���ļ���ʽ��ÿ��λ�ô�������Ϣ������ɫ�����+��ɫֵ0-255
//zgpҲ�ж�����Ϣ��ĳ��������8������������֡�����һ��

//��Ⱦ��ʱ����ʱ������ͼ�����һ�������

//get zgp info...
bool regist_zgp(const char* file,int& dir,int& frame);

class file_source;
file_source* get_file_source_zgp();
#endif