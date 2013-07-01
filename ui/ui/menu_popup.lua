--for menu
local menu_popup = class(menu,"menu_popup")

function menu_popup:create_menu_texts(name,x,y,z,w,htext,texts,c)
	x = x - 5
	y = y - 5
	menu.create_menu_texts(self,name,x,y,z,w,htext,texts)
	self.cell = c
	self.popup_x = x
	self.popup_y = y
	self.is_window = true
end

--Ҫ��ʵ�֣����cell hide�ˣ�menuҲhide
function menu_popup:is_hide()
	return menu.is_hide(self) or self.cell and self.cell:is_hide()
end


function menu_popup:recv_mouse_msg(mouse_event,x,y,param)
	--print("menu_popup:recv_mouse_msg",mouse_event,x,y)
	local x2 = x - self.x
	local y2 = y - self.y
	if not self:in_rect(x2,y2) then
		--print("not menu_popup:in_rect(x,y)")
		if mouse_event == WM_MOUSEMOVE and WM_MOUSEWHEEL then
			if not self.cell then--or not self.cell:in_rect_all(x,y,room) then
				--����ע��lostfocus���¼���
				self:hide()
			end
		else
			self:hide()
		end
		return
	end
	return cell.recv_mouse_msg(self,mouse_event,x,y,param)
end

return menu_popup