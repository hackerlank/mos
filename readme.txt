��Ǹֻ�����ĵ�˵��

��ɫ
1. c++�����ϵͳ��صĲ��֣�����ʵ������Ⱦ����gdi��opengl��
2. ������Ϸ������߼�����lua
	a. lua����Ϸ������c++��û�ж�Ӧ�����Ҳ��ͨ��get set�໥����
	b. ÿһ֡�����е���Ⱦ�������������һ��table����c++��c++��������table����Ⱦ
3. ������Ƶĺô�
	�ǳ��ɵ����
	lua��ƽ̨
	����ʹ������˵�����˽�c++
	c++ֻ����ϵͳ�Ļ�����Ժã�luaֻ�����߼���չ�Ժ�
	c++д�߼���Ҫ���ģʽ����lua�Ķ�̬��gc���հ���д�߼�����ʲô���ģʽ��ֻ��һ��observe�õ��ˣ�
	c++�޸Ĵ�����Ҫ���ϱ��룬ʱ���ѣ���lua���Զ�̬���أ����Է���
	��ȡֱ�Ӵ�ȡlua���󼴿�
	lua��corroutineдai��ħ�����֣���ͷ�����ȷǳ�����ֱ����c++�������Ƶ�fiber�����ǲ���һ���ȼ��Ķ���
	һ������Ⱦ�ķ�ʽ��c++���������ϲ����ε��Ż�������δʵ��
4. ������Ƶľ���
	������̫�࣬���������û�в��Թ����ر��ʺ��ֻ�������100�����ң�

Ŀ¼�ṹ
mos c++ 
ui lua

��������
mos/mos.sln ��� ui/mos.exe
vs2010�����ж�lua��֧����
https://github.com/downloads/yujiang/vsLua/LuaLanguage.vsix

lua�����ݽṹ
g_root
	һ�����������е���Ϸ���󣨰���ui����ͼ�����飩		
g_camera
	�����������ֻ�ܶ�׼g_root:get_play()
g_timer
	��ʱ����֧��4�ֶ�ʱ����һ��ֻ��Ҫ�˽�2��
	t = add_timer_everyframe(func,param)
	t = add_timer(time,func,param)
	del_timer�ķ���:
		1. func����false��timer����ִ��
		2. t:destroy()
g_db
	���еı����Դ������action�Ķ���

c++�����ݽṹ
graph
	���е���Դcache
image_db
	����һ��image+frame������һ��ͼƬ��ĳ��rect�����������ǰ�����ui��Դ����һ����ͼ�С�
device
	ƽ̨��صĲ��֣�����gdi��opengl����ʵ��

���볣������(��main.lua)
r reload �������һ��button��r button�������root��������button�����metatable�滻���µġ�
l load window	
s show window
df dofile ����df main.lua 
dump cdriver.dump_resource ��c++��cache
����ִ��dostring: test()��ó�������