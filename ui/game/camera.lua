--��ͼ

local camera = class("camera")

function camera:create_camera(type)
end

--������timer������������˶�֮����watch_player()
function camera:watch_player(map,player) 
	local x,y = player:get_pos()
	map:center_play(x,y,g_root.w,g_root.h)
	return true
end


return camera