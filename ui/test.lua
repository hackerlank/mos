--c++ 的初始化和消息入口

function test_timer1()
	g_timer:add_timer(0.9,function() print("test_timer_0.9") end)
	g_timer:add_timer(1.1,function() print("test_timer_1.1") end)
	g_timer:add_timer(1,function() print("test_timer_1") end)

	local i9 = 1
	local i10 = 1
	local i11 = 1
	g_timer:add_timer(0.9,function() print("test_timer_0.9",i9) i9 = i9+1 return i9<22 end)
	g_timer:add_timer(1.1,function() print("test_timer_1.1",i11) i11 = i11+1 return i11<20 end)
	g_timer:add_timer(1,function() print("test_timer_1",i10) i10 = i10+1 return i10<21 end)
end

function test_timer3()
	local start = os.time()
	for i = 1,100 do
		g_timer:add_timer_type(
			1.0,
			function() 
				local t = math.floor(os.time() - start) 
				print("test_timer3",i,t) 
				return t < i / 10
			end
			,nil,3)
	end
end

function test_timer2()
	local start = os.time()
	for i = 1,10 do
		g_timer:add_timer_type(
			1.0,
			function() 
				local t = math.floor(os.time() - start) 
				print("test_timer2",i,t) 
				return t < i 
			end
			,nil,2)
	end
end

function test_emitter()
	local e = emitter()
	e:on("test",function (w) print("hello",w) end)
	local rt = e:on("test",function (w) print("hello2",w) return true end)
	e:emit("test","world1")
	e:emit("test","world2")
	rt.invalid = true
	e:emit("test","world3")
end

function test_class()
	Animal = class()

	function Animal:__tostring()
	  return self.name..': '..self:speak()
	end
	function Animal:init(name)
		self.name = name
	end

	Dog = class(Animal)

	function Dog:speak()
	  return 'bark'
	end

	Cat = class(Animal)

	function Cat:init(name,breed)
		Animal.init(self,name)
		self.breed = breed
	end
	function Cat:speak()
	  return 'meow'
	end

	Lion = class(Cat)

	function Lion:speak()
	  return 'roar'
	end

	fido = Dog()
	fido:init('Fido')
	felix = Cat()
	felix:init('Felix','Tabby')
	leo = Lion()
	leo:init('Leo','African')
	print(fido,felix,leo)
end

function test_sort()
	local tb = {
		b = {name = 'b'},
		a = {name = 'a'},
		c = {name = 'c'},
	}
	local t = sort_pairs(tb,"name")
	table_print(t)
end

function test_stringex()
	print(split2("lala.haha","%."))
	split("a,b,c", ",") --> {"a", "b", "c"}
end

function test_tableex()
	table_print({44,{name = "www",sex = 1},func = {1,2,3}})
	local t = {1, {2, 3, 4}, default = {"a", "b", d = {12, "w"}, e = 14}}
	print(table.show(t, "t"))            -- most typical use
end

function regist_dog()
	cdriver.regist_image("dog",1,"dog.png",0,0,105,95,50,80)
	cdriver.regist_image("dog",2,"dog.png",105,0,210,95,50,80)
	cdriver.regist_image("dog",3,"dog.png",105,0,210,95,50,80)
	g_ani_data:regist_ani_data_simple(101,"stand","dog",0,1,1,false)
	g_ani_data:regist_ani_data_simple(101,"walk","dog",0.25,1,3,true)
end
--test
function test_win3()
	local r = g_root
	local l1 = r:find_child("top")
	local win3 = window()
	win3:create_window("win3",200,200,10,200,200)
	l1:add_window(win3)
	win3:set_bg("land.png")
	win3:get_bg().alpha = 200

	local img = image()
	img:create_image("ani_dog",40,-40,0,"dog",1)
	win3:add_child(img)
	img:set_ani(0.5,1,3,true)

	local mv = win3:get_move()
	mv:move_to(400,400,100)
end

function test_win4()
	local r = g_root
	local l1 = r:find_child("top")

	local win4 = window()
	win4:create_window("win4",10,10,20,200,100)
	win4:set_box(0x008080ff,180)

	l1:add_window(win4)
	local label1 = label()
	local s1 = "1.fps:100√\n2.hello world!HAPPY.\n3.你好，世界！\n4.long long ago."
	for i=5,20 do 
		s1 = s1 .."\n".. i.. ".test s"
	end
	label1:create_label("lable1",0,0,0,200,100,s1)
	label1._disable = false
	label1:get_text():set_scroll(4)
	--label1:create_label("lable1",0,0,0,100,60,"好a好b若c您d为e了！")
	--label1:set_color(0x00ff00)
	label1:get_text():set_color(0x00ff8080)
	win4:add_child(label1)
end

function test_win2()
	local r = g_root
	local l2 = r:find_child("normal")

	local win2 = window()
	--win2.is_chain = true
	win2:create_window("win2",200,200,10,200,200)
	l2:add_window(win2)
	win2:set_bg("menu2.png")
end

function test_png8()
	--use pal render do some pal swap 
	--or just use png for diskspace.
	local file = "girl8.png"

	--local file = "test-fs8.png" --4bit index and no alpha channel

	--local file = "test-fs8.png"

	cdriver.regist_image_palette(file,"")

	local r = g_root
	local l2 = r:find_child("normal")

	local win2 = window()
	--win2.is_chain = true
	win2:create_window("win2",100,100,10,300,300)
	l2:add_window(win2)
	win2:set_bg(file)
	win2:get_bg().alpha = 200
end

function test_png8_shader()
	local file = "girl8.png"
	cdriver.regist_image_palette(file,"")

	local r = g_root
	local l2 = r:find_child("normal")

	local win2 = window()
	--win2.is_chain = true
	win2:create_window("win2",100,100,10,300,300)
	l2:add_window(win2)
	win2:set_bg(file)
	win2:get_bg().alpha = 200
	win2:get_bg().shader = "pal2"
end

function test_win1()
	cdriver.regist_image("play",1,"play-button.png")
	cdriver.regist_image("play",2,"play-button.png")
	cdriver.regist_image("play",3,"play-button-down.png")

	local r = g_root
	local l2 = r:find_child("normal")

	local win = window()
	--win.is_chain = true
	win:create_window("win1",100,100,0,200,200)
	l2:add_window(win)
	win:set_bg("menu2.png")
	--win:set_bg("farm.jpg")
	--win:get_bg().alpha = 200
	win.alpha = 200
	--win:set_bg("dog.png")

	local btn = button()
	btn:create_button("btn1",20,20,0)
	btn:set_bg("play")
	btn:set_caption("游戏",0,50,50)
	local text = btn:get_caption_text()
	text.color = 0x000000ff
	win:add_child(btn)
	assert(btn == r:find_control("normal.win1.btn1"))

	local btn2 = button()
	btn2:create_button("dog",200,100,0)
	btn2:set_bg("dog")
	btn2:set_caption("狐狸")
	btn2:get_caption():set_align(ALIGN_CENTER)
	local text = btn2:get_caption_text()
	text.color = 0x0000ff00
	win:add_child(btn2)
end

function test_msgbox()
	local mb = msgbox()
	mb:create_msgbox("msgbox",100,100,100,100)
	mb:set_bg("dog.png")
	local btn = button()
	btn:create_button("msgbox_btn1",60,20,0)
	btn:set_bg("menu1.png")
	mb:add_child(btn)
	btn.on_lclick = function(self)
		self.father:close()
	end
end

function test_map()
	local r = g_root
	local m = r:get_map()
	m:set_bg("farm.jpg")
end

function register_sprite_png(shape,ani,speed,loop,inverse_dir)
	local file = string.format("char/%04d/%s.png",shape,ani)
	local rt,dir,frame 
	rt,dir,frame = cdriver.regist_image(file)
	--print(file,rt,dir,frame)
	g_ani_data:regist_ani_data(0120,ani,file,speed,dir,frame,0,frame,1,loop,inverse_dir)
end

function register_sprite_weapon_png(shape,weapon,ani)
	local file = string.format("char/%04d/%02d/%s.png",shape,weapon,ani)
	local rt,dir,frame 
	rt,dir,frame = cdriver.regist_image(file)
	print(file,rt,dir,frame)
	--g_ani_data:regist_ani_data(0120,ani,file,0.3,dir,frame,0,frame,1,false,true)
end


function print_test_sprite()
	
end

function test_sprite(png)
	if png then
		register_sprite_png(120,"walk",0.1,true,true)
		register_sprite_png(120,"stand",0.3,false,true)
		register_sprite_weapon_png(120,2,"walk")
		register_sprite_weapon_png(120,2,"stand")
	end

	local sp = sprite()
	sp:create_sprite(nil,100,100,0,0120,true)
	local r = g_root
	local m = r:get_map()
	m:add_child(sp)
	sp:stop()
	sp:set_name("猫猫",0x00ff00)
	sp:set_weapon(2)

	sp:set_room(0.7)

	r:set_play(sp)
end

function test_sprite2()
	local r = g_root

	local sp = sprite()
	sp:create_sprite(nil,100,100,0,101)
	local m = r:get_map()
	m:add_child(sp)
	sp:stop()
	sp:set_name("精灵",0x00ff00)

	r:set_play(sp)
end

function test_fps()
	local r = g_root
	local l1 = r:find_child("top")

	local label1 = label()
	label1:create_label("fps",0,0,0,400,20,"fps:100")
	label1._disable = false
	label1:get_text():set_color(-1)
	l1:add_child(label1)
end

function test_notice()
	local r = g_root
	local l1 = r:find_child("top")

	local label1 = label()
	label1:create_label("notice",100,20,0,250,20,"long long text long long text abcdefg")
	label1._disable = true
	local text = label1:get_text()
	text:set_color(0xffff00)
	text:set_pos(250,0)
	label1:set_box(0x808080,128)
	l1:add_child(label1)

	g_timer:add_timer(
		0.03,
		function() 
			local x,y = text:get_pos()
			if x < -text.text_w then
				text:set_pos(250,0)
				return true
			end
			text:set_pos(x-1,0)
			return true
		end)
end

function test()
	print("window:\ttest_win1(),test_win2(),test_win3(),test_win4()")
	print("game:\ttest_map(),test_sprite()")
	print("other:\ttest_fps(),test_msgbox(),test_notice(),test_png8")
end
