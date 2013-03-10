--for ����һ������
local box = class(cell,"box")

function box:create_box(name,x,y,z,w,h,color,alpha)
	cell.create_cell(self,name,x,y,z,w,h)
	self:set_color(color)
	self:set_alpha(alpha)
	--print("box:create_box",self.color,self.alpha)
	self.is_box = true
	self._disable = true
end

function box:get_render_override()
	--print("image:get_render_override()",self.name)
	return self
end

return box