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

����
c++ call lua at main.lua
on_mouse_event �����Ϣ
on_key_down ������Ϣ
on_every_frame ÿ��ѭ������һ��

lua call c++ 
1 ��Ⱦ���
render ��Ⱦ
load_map ��Ϊmap�ֿ���Ⱦ�ķ����ǳ����⣬������c++ʵ����ȫ��
2 image���
get_image_size get_image_sizecg in_image
regist_image regist_zgp regist_image_palette
3 font���
regist_font 
get_text_line get_text_size
4 Ѱ·
find_path
5 os��أ�
create_window ��������
get_time ���ʱ��
exit �˳�

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

����ui
	��ײ�� box,image,text,mitem
	��װ�˼������ö���
	window-->box
	label-->text
	menu-->mitem
	cell-->bg(image) caption(lable) box(box)
	button-->handle event
	����ȥ����е�Ƕ�ף����������㼴�ɡ�

c++�����ݽṹ
graph
	���е���Դcache
image_db
	����һ��image+frame������һ��ͼƬ��ĳ��rect�����������ǰ�����ui��Դ����һ����ͼ�С�
device
	ƽ̨��صĲ��֣�����gdi��opengl����ʵ�֣�gdi����֧���ˡ�

���볣������(��main.lua)
r reload �������һ��button��r button�������root��������button�����metatable�滻���µġ�
l load window	
s show window
df dofile ����df main.lua 
dump cdriver.dump_resource ��c++��cache
����ִ��dostring: test()��ó�������

