--����

local ani_data = class("ani_data")

function ani_data:create_ani_data()
	self.shapes = {}
end

function ani_data:regist_ani_data(shape,name,image_file,ani_speed,dir,frame_start,frame_end,loop)
	self.shapes[shape] = self.shapes[shape] or {}
	self.shapes[shape][name] = {
		shape = shape,
		name = name,
		image_file = image_file,
		ani_speed = ani_speed,
		dir = dir, --������Ŀ��
		frame_start = frame_start,
		frame_end = frame_end,
		loop = loop,
		--cx = cx,
		--cy = cy,
	}
end

function ani_data:find_ani_data(shape,name)
	if self.shapes[shape] then
		return self.shapes[shape][name]
	end
end

return ani_data
