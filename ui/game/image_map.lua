--��ͼ

local image_map = class(cell,"image_map")

function image_map:create_image_map(name,x,y,z,image_file,frame)
	cell.create_cell(self,name,x,y,z,0,0)
	self:set_image_map(image_file,frame)
	self._disable = true;	
end

function image_map:set_image_map(image_file,frame)
	self.map_file = image_file
	self.frame = frame
	local w,h = cdriver.load_map(self.map_file,self.frame) --���ļ��õ�w,h
	self.w = w 
	self.h = h 
end

function image_map:get_render_override()
	--print("image:get_render_override()",self.name)
	if self.map_file then
		return self
	end
end

return image_map