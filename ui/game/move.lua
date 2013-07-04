--�˶�
local move= class("move")

function move:create_move(cell)
	self.cell = cell
end

function move:stop_move()
	if self.timer then
		self.timer.invalid = true
		self.timer = nil
		self.on_reached(false)
	end
end

function move:on_reached(is_reached)
	
end

function move:on_change_dir(dir)
	
end

function move:on_timer_updatepos(pass)
	local x,y = self.cell:get_pos()
	local dx = x + self.x_speed * pass 
	local dy = y + self.y_speed * pass 
	--print ("move:on_timer_updatepos",pass,dx,dy)

	if dx == self.x_des and dy == self.y_des or
		(dx - self.x_des) * (x - self.x_des) < 0 or 
		(dy - self.y_des) * (y - self.y_des) < 0 then
		self.cell:set_pos(self.x_des,self.y_des)	
		--return nil ���ˣ�����ִ��timer!
		self.timer = nil

		if #self.pos == 0 then
			self.on_reached(true)
		else
			local dir = self:continue_move()
			self.on_change_dir(dir)
		end
	else
		self.cell:set_pos(dx,dy)	
		return true
	end
end

function move:add_timer()
	self:stop_move()
	self.timer = g_timer:add_timer_everyframe(move.on_timer_updatepos,self)		
end


--speed ÿ��������أ�
function move:move_to_straight(x,y,speed)
	assert(speed > 0)
	local x0,y0 = self.cell:get_pos()

	local distance = math.sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))
	if distance <= 0.0001 then
		return
	end

	self.x_start = x0
	self.y_start = y0
	self.x_des = x
	self.y_des = y
	self.x_speed = (x-x0) / distance * speed
	self.y_speed = (y-y0) / distance * speed

	--print("move:move_to",x,y,speed,self.x_speed,self.y_speed)

	self:add_timer()
	return cdriver.get_dir(x-x0,y-y0)
end

function move:move_to(x,y,speed)
	self:stop_move()

	local x0,y0 = self.cell:get_pos()
	local pos = cdriver.find_path(x0,y0,x,y)
	if not pos then
		return 
	end

	print("move:move_to",x,y,pos[1],pos[2])

	self.speed = speed
	self.pos = pos
	assert(#pos >= 2)
	return self:continue_move()
end

function move:continue_move()
	local pos = self.pos
	local nx,ny = pos[#pos-1],pos[#pos]
	pos[#pos-1] = nil
	pos[#pos] = nil
	return self:move_to_straight(nx,ny,self.speed)
end

--֧��һ��coroutine�ķ�����
return move