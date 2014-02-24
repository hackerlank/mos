--cell base function

local cell = class("cell")

--���ⲻ��ͬһ��init
function cell:create_cell(name,x,y,z,w,h)
	assert(self.class_name)
	self.name = name
	self.x = x
	self.y = y
	self.z = z
	self.w = w
	self.h = h
	self.childs = {} 
	self.room = 1
end

--create and destroy
function cell:destroy()
	if self.father then
		self.father:remove_child(self)	
		self.father = nil
	end
	self._destroy = true
	for _,child in pairs(self.childs) do
		child:on_destroyed()
	end
	if self.move then
		self.move:stop_move()
	end
	self:on_destroyed()
end

function cell:on_destroyed()
end

function cell:is_valid()
	return not self._destroy
end

--name,pos,size,rect
function cell:get_name()
	return self.name
end

function cell:get_pos()
	return self.x,self.y
end

--�õ����Ե�pos
function cell:get_screen_pos_abs()	
	local f = self
	local t = {}
	while(f) do
		table.insert(t,1,f)
		f = f.father
	end
	local x = 0
	local y = 0
	local room = 1
	for i,cell in ipairs(t) do
		local x2,y2 = cell:get_pos()
		x = x + x2 * room 
		y = y + y2 * room
		room = room * cell.room
	end
	return x,y
end

function cell:in_rect_abs(x,y)
	local x2,y2 = self:abs_2_local(x,y)
	local rt = self:in_rect(x2,y2)
	--print("in_rect_abs",self.name,x,y,x2,y2,rt)
	--return self:in_rect(x2,y2)
	return rt
end

function cell:abs_2_local(x,y)
	local f = self
	local t = {}
	while(f) do
		table.insert(t,1,f)
		f = f.father
	end
	local room = 1
	for i,cell in ipairs(t) do
		local x2,y2 = cell:get_pos()
		x = (x - x2) / cell.room 
		y = (y - y2) / cell.room
	end
	return x,y	
end

function cell:set_pos(x,y)
	self.x,self.y = x,y
end

function cell:offset(x,y)
	self.x,self.y = self.x + x,self.y + y
end

function cell:get_size()
	return self.w,self.h
end

function cell:set_size(w,h)
	self.w,self.h = w,h
end

function cell:in_x(x)
	return x >= self.x and x < self.x + self.w 
end

function cell:in_y(y)
	return y >= self.y and y < self.y + self.h
end

function cell:in_size(x,y)
	return x >= 0 and x < self.w and y >= 0 and y < self.h
end

function cell:get_rect()
	return 0,0,self.w,self.h
	--return self.x,self.y,self.x + self.w,self.y + self.h
end

function cell:_in_rect(x,y)
	local l,t,r,b = self:get_rect()
	--return x >= self.x and x < self.x + self.w and y >= self.y and y < self.y + self.h
	return x >= l and x < r and y >= t and y < b
end

--ֻ��image��������������������û��д��in_rect_override����ʽ��
--ʵ���ϣ������˵�menuҲ�����ˣ�����menu�����ڲ�ʹ�õġ�
function cell:in_rect(x,y)
	local bg = self:get_bg() 
	if bg then 
		return bg:in_rect(x,y)
	end
	return self:_in_rect(x,y)
end

function cell:in_rect_pixel(x,y)
	local bg = self:get_bg() 
	if bg then 
		return bg:in_rect_pixel(x,y)
	end
	return self:_in_rect(x,y)
end

---------------------------------------------------------
--����
c_box_z = -2000
c_bg_z = -1000

--bg and box and caption
function cell:set_bg(image_file,frame)
	--z��������ֵ������������
	local bg = self:get_bg()
	if not bg then
		bg = image()
		bg:create_image("bg",0,0,c_bg_z,image_file,frame or 0)
		self:add_child(bg)
	else
		bg:set_image(image_file,frame)
	end
	self.w = bg.w
	self.h = bg.h
end

function cell:get_bg()
	return self:find_child("bg")
end

function cell:set_box(color,alpha)
	local b = self:get_box()
	if not b then
		b = box()
		--z��������ֵ������������
		b:create_box("box",0,0,c_box_z,self.w,self.h,color,alpha)
		self:add_child(b)
	else
		b:set_color(color)
		b:set_alpha(alpha)
	end
end

function cell:get_box()
	return self:find_child("box")
end

function cell:set_caption(text,font,offx,offy,w,h)
	assert(self.w >= 0 and self.h >= 0)
	assert(not self:get_caption())

	offx = offx or 0
	offy = offy or 0
	w = w or 100
	h = h or 32

	local lb = label()
	--lb:create_label("caption",offx,offy,1000,self.w-2*offx,self.h-2*offy,text,font,nil,ALIGN_CENTER)
	lb:create_label("caption",offx,offy,1000,w,h,text,font,nil,ALIGN_CENTER)
	self:add_child(lb)
end

function cell:get_caption()
	return self:find_child("caption")
end

function cell:get_caption_text()
	return self:get_caption():get_text()
end

function cell:get_caption_string()
	return self:get_caption():get_string()
end


--�����Ȳ�����
function cell:set_room(room)
	self.room = room
end

function cell:get_room()
	return self.room 
end

--color and alpha, ��ϧû��bitop��������Զ���Ϊһ
function cell:get_alpha()
	return self.alpha
end

function cell:set_alpha(a)
	self.alpha = a
end

function cell:get_color()
	return self.color
end

function cell:set_color(c)
	self.color = c
end

--hide and show
function cell:hide()
	self._hide = true
end

function cell:is_hide()
	return self._hide
end

function cell:is_hide_all()
	local c = self
	while(c) do
		if c:is_hide() then
			return true
		end
		c = c.father
	end
	return false
end

function cell:show()
	self._hide = false
end

function cell:is_show()
	return not self:is_hide()
end

--enable and disable
function cell:disable()
	self._disable = true
end

function cell:is_disable()
	return self._disable 
end

function cell:enable()
	self._disable = false
end

function cell:is_enable()
	return not self:is_disable()
end

function cell:is_disable_all()
	local c = self
	while(c) do
		if c:is_disable() then
			return true
		end
		c = c.father
	end
	return false
end

--�Ƿ����msg
function cell:is_msg()
	return self:is_show() and self:is_enable()
end

--�Ƿ�����϶�
function cell:can_drag()
	return self.drag
end

--z
function cell:get_z()
	return self.z
end

--window��z���򣬰�zֵ��������ƣ�zԽ��Խ��ǰ�浲ס�����ġ�
function cell:max_z()
	--print("cell:max_z()",self.name)
	local maxz = 0
	for name,w in pairs(self.childs) do
		--print("cell:max_z()",name,w.z)
		if w.z > maxz then
			maxz = w.z
		end
	end	
	return maxz
end

function cell:topest_z()
	return self:max_z()
end

function cell:topest()
	assert(self.father)
	self.z = self.father:topest_z() + 1
	--print("cell:topest()",self.name,self.z)
end

-------------------------------------------------------
--child and father
--ʹ��name������id����Ϊname�Ķ��Ը�ǿ��ע��name���ܸı䲢���ڸò��Ψһ��
function cell:add_child(child)
	--print("cell:add_child",child.name)
	self.childs = self.childs or {}
	local old = self.childs[child.name]
	if old == child then
		return
	end
	if old then
		old:destroy()
	end
	self.childs[child.name] = child
	child.father = self
end

function cell:get_childs_num()
	return table_kv_size(self.childs)
end

--���û��remove��ֱ��hide���ɣ��޷��ǵ��ڴ�
function cell:remove_child(child)
	self.childs[child.name] = nil
end

function cell:find_child(name)
	--print("cell:find_child",name)
	return self.childs[name]
end

function cell.get_offset(from,to)
	local tb = cell.get_father_chain(from,to)
	if not tb then
		return
	end
	local x = 0
	local y = 0
	for _,c in ipairs(tb) do
		x = x + c.x
		y = y + c.y
	end
	return x,y
end

function cell.get_father_chain(from,to)
	local tb = {}
	while(from) do
		table.insert(tb,from)
		from = from.father
		if from == to then
			return tb
		end
	end
end

function cell.get_child_chain(from,to)
	local tb = cell.get_father_chain(from,to)
	if not tb then
		return
	end
	local t = {}
	for i = #tb,1,-1 do
		table.insert(t,tb[i])
	end
	return t
end

--like find top.loginwindow.loginctrl.bg
function cell:find_control(full_name)
	local child_name,next_name = split2(full_name,"%.")
	child_name = child_name or full_name
	local child = self:find_child(child_name)
	if child then
		if next_name then
			return child:find_control(next_name)
		else
			return child
		end
	else
		--print("error! not find "..child_name .. " of "..self.name)
		--table_print_kv(self.childs)
	end
end

function cell:is_father_all(f)
	assert(f)
	local c = self.father
	while c do
		if c == f then
			return true
		end
		c = c.father
	end
	return false
end

function cell:is_child_all(c)
	for _,child in pairs(self.childs) do
		if child == c then
			return true
		elseif child:is_child_all(c) then
			return true
		end
	end
	return false
end

----------------------------------------------- draw
--Ϊ�˻����޳���hide��window
local function get_sortshow_tb(t,func,great)
	--print("get_sortshow_tb",table_kv_size(t))
	local tb = {}
	for name,l in pairs(t) do
		if func(l) then
			table.insert(tb,l)
		end
	end
	if great then
		table.sort(tb,function(l1,l2) return l1:get_z() > l2:get_z() end)
	else
		table.sort(tb,function(l1,l2) return l1:get_z() < l2:get_z() end)
	end
	return tb
end

--layer���5,6�㣬
--ÿ��layer���5,6��window��
--����sortһ��Ҳ����ν��
function cell:get_sortshow_childs()
	return get_sortshow_tb(self.childs,self.is_show)
end

function cell:get_sortmsg_childs()
	--great ���򷴹���
	return get_sortshow_tb(self.childs,self.is_msg,true)
end

--��������õ���Ⱦ����
function cell:get_render_override()
end

function table_copyvalue(t,tb)
	for k,v in pairs(tb) do
		if type(k) == "string" then
			if type(v) == "string" or type(v) == "number" or type(v) == "boolean" then
				t[k] = v
			end
		end
	end
	return t
end
--render
--�麯�� get_render_override,������lua��ȷ��offset����c++��ȷ�����ɡ�
function cell:get_render_childs()
	if self:get_childs_num() > 0 then
		--base is here!
		local tb = {}
		local shows = self:get_sortshow_childs()
		for _,child in pairs(shows) do
			local t = child:get_render_childs()
			if t then
				table.insert(tb,t)
			end
		end	
		if #tb == 0 then			
			return self:get_render_override()
		end
		table_copyvalue(tb,self)
		return tb
	else
		return self:get_render_override() --or cell
	end
end

--��ӡrender,�൱��static�ĺ�����
function cell:print_render(tb)
	if not tb then
		tb = self:get_render_childs()
	end
	local ignores = {father = true,childs = true}
	table_print(tb,nil,nil,ignores)
end

--��ӡcell
--move��ai�Լ��ؽ��������洢�ˡ�
local print_ignores = {father = true,move = true,ai = true,timer = true}
function cell:print()
	table_print(self,nil,nil,print_ignores)
end

function cell:to_string()
	return table.show(self,self.name,nil,print_ignores)
end

function cell:clone()
	local s = self:to_string()
	local str = "local "..s .."\nreturn "..self.name
	--print(str)
	local c = assert(loadstring(str))()
	if c then
		cell.loaded(c)
		return c
	end
end

--window��console��֧��utf8�����������Ҫ���¡�

--������Ϣ
function cell:on_mouse_msg(mouse_event,x,y,param)
end

function cell:on_key_down(key,alt,ctrl,shift)
end

function cell:recv_key_down(key,alt,ctrl,shift)
	if self:on_key_down(key,alt,ctrl,shift) then
		return true
	end
	if self.father then
		return self.father:recv_key_down(key,alt,ctrl,shift)
	end
end

function cell:on_child_mouse_msg(mouse_event,x,y,param,child)
end

--Ҳֻ��menu�����������
--menu���ڲ�ʹ�á�
function cell:recv_mouse_msg(mouse_event,x,y,param)
	if mouse_event == WM_MOUSEWHEEL then
		--print("recv_mouse_msg WM_MOUSEWHEEL",self.name,x,y,param,self:in_rect(x,y))
	end
	
	local x2 = (x - self.x) / self.room
	local y2 = (y - self.y) / self.room

	if not self:in_rect(x2,y2) then
		return
	end

	--local s = string.format("cell:recv_mouse_msg %s x %d y %d x2 %d y2 %d room %.1f ",self.name,x,y,x2,y2,self.room)
	--print(s) 

	local msgs = self:get_sortmsg_childs()
	for _,child in pairs(msgs) do
		local rt = child:recv_mouse_msg(mouse_event,x2,y2,param)
		if rt then
			self:on_child_mouse_msg(mouse_event,x,y,param,rt)
			return rt
		end
	end	

	if mouse_event == WM_MOUSEWHEEL then
		--print("recv_mouse_msg WM_MOUSEWHEEL",self.name,self:in_rect_pixel(x,y),self:in_rect(x,y))
	end
	if self:in_rect_pixel(x2,y2) then
		return self:on_mouse_msg(mouse_event,x,y,param)	
	end
end

--һЩ��root�Ķ�Ӧ
function cell:on_get_focus(is_focus,focus) 
	self.is_focus = is_focus
end

function cell:on_get_hover(is_hover,hover)
	self.is_hover = is_hover
end

function cell:on_set_drag(is_drag,drag)
	self.is_drag = is_drag
end

--û������
--function cell:on_set_modal(is_modal,modal)
--	self.is_modal = is_modal
--end

--function cell:on_set_popup(is_popup,popup)
--	self.is_popup = is_popup
--end

------------------------------------------------------
function cell.assign_class(c,class_name)
	local t = {
		box = box,
		button = button,
		cell = cell,
		image = image,
		label = label,
		layer = layer,
		menu = menu,
		menu_popup = menu_popup,
		mitem = mitem,
		msgbox = msgbox,
		root = root,
		text = text,
		window = window,
		sprite = sprite,
		sprite_body = sprite_body,
		map = map,
	}
	if t[class_name] then
		setmetatable(c,t[class_name])
	else 
		print("error! assign_class "..class_name.." not find")
	end
end

function cell:on_loaded_from_table()
end

function cell:load_from_table()
	for k,v in pairs(self.childs) do
		--because father is ignored. so re assign to self.
		v.father = self
		cell.assign_class(v,v.class_name)
		v:load_from_table()
	end
	self:on_loaded_from_table()
end

function cell.loaded(win)
	cell.assign_class(win,win.class_name)
	win:load_from_table()
end
------------------------------------------------------
--reload
function cell:on_reload_class(old,new)
end

function cell:reload_class(old,new)
	--�����������û������
	--ע����ڻ��࣬����cell�޷�����
	if getmetatable(self) == old then
		setmetatable(self,new)
	end
	for _,child in pairs(self.childs) do
		child:reload_class(old,new)
	end

	if getmetatable(self.move) == old then
		setmetatable(self.move,new)
	end

	self:on_reload_class(old,new)
end

function cell:on_mouse_scroll_room(param,min,max)
	local depth = param / 120
	local room = self.room + depth / 10
	if room < 0.5 then
		room = 0.5
	elseif room > 2 then
		room = 2
	end	
	--print("self:set_room",self.name,room)
	self:set_room(room)
end

return cell
 