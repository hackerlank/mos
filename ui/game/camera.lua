--��ͼ

local camera = class("camera")

function camera:create_camera(map,type)
	self.map = map
end

--������timer������������˶�֮����watch_player()
function camera:watch_player() 
	local player = g_root:get_play()
	if player then
		local x,y = player:get_pos()
		self.map:center_play(x,y,g_root.w,g_root.h)
	end
	return true
end


return camera