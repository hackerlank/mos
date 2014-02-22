local table = require "table"

local socket = class("socket")
local msg = require "net.msg"
local net = require "socket.core"

function socket:send(s)
	assert(self:is_connected())
	--print("socket:send",s)
	local num,msg,param = self._tcp:send(s)
	if not num then
		self._tcp = nil
		self:on_error("send",msg)
		return nil,msg
	end
	return true,num
end

function socket:on_error(where,msg)
	print("on_error",where,msg)
	if self._callback_error then
		self._callback_error(where,msg)
	end
end

function socket:bind_handle(callbackdata,callbackerror)
	self._callback_data = callbackdata
	self._callback_error = callbackerror
end

function socket:connect(ip,port)
	print("socket:connect",ip,port)
	self._ip = ip
	self._port = port
	if not self._timer then
		self._timer = g_timer:add_timer_everyframe(self.ontimer_run,self)
	end
	return self:_connect()
end

function socket:_connect()
	--self._tcp = net.tcp()
	local ok,msg = net.connect(self._ip,self._port)	
	if not ok then
		print(msg)
	else
		self._tcp = ok
		self._tcp:settimeout(0)
	end
	return ok,msg
end

function socket:reconnect()
	return self:_connect()
end

function socket:is_connected()
	return self._tcp
end

function socket:close()
	if self._timer then
		self._timer:destroy()
		self._timer = nil
	end
	if not self:is_connected() then
		return
	end
	self._tcp:close()
	--self:on_error("close")
end

--�޷���Ϊ��Ϣ����socketû�а�len�ȷ����޷���ȷ�Ľ��
function socket:ontimer_run()
	if not self:is_connected() then
		return true
	end

	while true do
		-- if use "*l" pattern, some buffer will be discarded, why?
		local __body, __status, __partial = self._tcp:receive("*a")	-- read the package body
		--print("body:", __body, "__status:", __status, "__partial:", __partial)
    	if __status == STATUS_CLOSED or __status == STATUS_NOT_CONNECTED then
			self:on_error("receive")
		    self:close()
		   	return
	    end
		if not __body then
			__body = __partial
			__partial = nil
		elseif __partial then 
			__body = __body .. __partial 
		end

		if #__body == 0 then
			return true
		end
		--print("body:", __body, "__status:", __status, "__partial:", __partial)
		if self._callback_data then
			self._callback_data(self,__body)
		else
			msg.handle(self,__body)
		end
	end

	return true
end

return socket