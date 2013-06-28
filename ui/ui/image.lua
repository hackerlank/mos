--����ͼ���ļ����ؽ�image

local image = class(cell,"image")

function image:create_image(name,x,y,z,image_file,frame)
	cell.create_cell(self,name,x,y,z,0,0)
	self:set_image(image_file,frame)
	self._disable = true;
end

function image:set_image(image_file,frame)
	--print(debug.traceback())
	--print("image:set_image",self,image_file,frame)
	self.image_file = image_file
	self.frame = -1 --must update frame.
	self:_change_frame(frame)
	--self:update_frame_change()
end

function image:_change_frame(frame)
	if self.frame ~= frame then
		self.frame = frame
--		if frame >= 64 and string.find(self.image_file,"walk") then
--			print("image:change_frame",self.image_file,frame)
--			print(debug.traceback())
--		end
		local w,h,cx,cy = cdriver.get_image_sizecg(self.image_file,self.frame) --���ļ��õ�w,h,cx,cy
		self.w = w 
		self.h = h 
		self.cx = cx 
		self.cy = cy 
	end
end

function image:change_frame(frame)
	self:_change_frame(frame)
end

function image:get_render_override()
	--print("image:get_render_override()",self.name)
	if self.image_file then
		return self
	end
end

--�趨ani
function image:set_ani(ani_speed,frame_start,frame_to,loop)
	--print("image:get_render_override()",self.name)
	if not self.ani then
		self.ani = ani()
		self.ani:create_ani(self)
	end
	self.ani:set_ani(ani_speed,frame_start,frame_to,loop)
end


function image:on_loaded_from_table()
	if self.ani then
		setmetatable(self.ani,ani)
		self.ani:on_loaded_from_table()
	end
end

function image:on_destroyed()
	--cell.on_destroyed(self)
	if self.ani then
		self.ani:stop_ani()
	end
end

function image:is_jpg()
	--print(self.name,self.image_file)
	return string.sub(self.image_file,#self.image_file - 3) == ".jpg"
end

--����ֻ֧��image��room����������Ч�ʵ��ˣ���˵�ա�
function image:get_room_local()
	return self.room
end

function image:get_rect(room)
	--local room = self:get_room_local()
	local cx = self.cx * room
	local cy = self.cy * room

	return self.x - cx,self.y - cy,self.x - cx + self.w * room ,self.y - cy	+ self.h * room 
end

function image:in_rect_pixel(x,y,room)
	if not self:in_rect(x,y,room) then
		return false
	end
	if  self:is_jpg() then --jpgû��alphaͨ��
		return true
	else
		--local room = self:get_room_local()
		return cdriver.in_image(self.image_file,self.frame,(x - self.x)/room + self.cx , (y - self.y)/room + self.cy)
	end
end

function image:on_reload_class(old,new)
	--print("image:on_reload_class(old,new)")
	if getmetatable(self.ani) == old then
		setmetatable(self.ani,new)
	end
end

return image