
local window = class(cell,"window")

function window:create_window(name,x,y,z,w,h)
	local tb = cell.create_cell(self,name,x,y,z)
	self:set_size(w,h)
	self.drag = true
	self.is_window = true
	return tb
end

function window:show()
	cell.show(self)
	self:topest()
	g_root:set_focus(self)
	if self:is_normal() then
		g_root:add_chain(self)
	end
end

function window:on_closed()
end

function window:is_normal()
	return self.father and self.father.name == "normal"
end

function window:close()
	self:hide()
	--g_root:on_window_close(self)
	--һ��Ĺرվ������أ�ĳЩ��̬��window��destroy
	if self.is_dynamic then
		self:destroy()
	end
	if self:is_normal() then
		g_root:close_chain(self)
	end
	self:on_closed()
end

function window:on_key_down(key,alt,ctrl,shift)
	--print("window:on_key_down",key)
	--һ��������ͨ��esc���Դ�ͷ�ص�β��
	if self:is_normal() and key == VK_ESCAPE then
		self:close()
		return true
	end
end

function window:on_child_mouse_msg(mouse_event,x,y,param,child)
	if mouse_event == WM_LBUTTONDOWN or mouse_event == WM_RBUTTONDOWN then
		self:topest()
	end
end

function window:on_mouse_msg(mouse_event,x,y,param)
	--if mouse_event == WM_RBUTTONDOWN then
		--print("window:on_mouse_msg WM_RBUTTONDOWN",self.name,x,y,self:in_rect(x,y))
	--end
	if mouse_event ~= WM_MOUSEMOVE then
		--print("window:on_mouse_msg",mouse_event,x,y)
	end
	if mouse_event == WM_LBUTTONDOWN then
		self:topest()
	elseif mouse_event == WM_RBUTTONDOWN then
		--self:hide()
		ui:on_window_mouse_rd(self,x,y)
	end
	--cell.on_mouse_msg(self,mouse_event,x,y)
	return self
end

return window