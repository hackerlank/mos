
--����ͬһ��ʱ�䣬��������1��2���timer���������
local timer_list = class("timer_list")

function on_every_frame_list(time,v,clock)
	for i = v.start,v.start + v.num - 1 do
		local timer = v[i]
		if clock < timer.clock_time then	
			break
		end

		v[i] = nil
		v.start = v.start + 1
		v.num = v.num - 1

		if timer:call() then
			--���뵽ĩβ
			timer.clock_time = clock + time
			v[v.start + v.num] = timer
			v.num = v.num + 1
		end
	end
	
	--�Ż�����Ҫ������λ��		
	if v.start >= 1000 then
	end
end

--ʵ����������ģ����һ��list��
function timer_list:on_every_frame()
	local clock = os.clock()
	for time,v in pairs(self.timers) do
		on_every_frame_list(time,v,clock)
	end
end

function timer_list:create_timer_list()
	self.timers = {}
end

function timer_list:add_timer(time,t)
	if not self.timers[time] then
		self.timers[time] = { start = 1,num = 0 }
	end
	local tb = self.timers[time]
	tb[tb.start + tb.num] = t
	tb.num = tb.num + 1
	return t
end


return timer_list

