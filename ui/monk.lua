--lua�Լ�Ҳ���Բ����߼���������c++��ͨ��monk����
--��Ŀ����ʹ�ã������Ѿ�����

function init_monk_libs()
	local clib_files = {
		"cdriver",
	}
	for _,file in ipairs(clib_files) do
		_G[file] = require ("clib."..file)
	end
end

function monk()
	require ("main")
	init_main()
	init_monk_libs()
	g_root = test_ui()
	ui:on_every_frame()
end

function auto_test()
	for k,v in pairs(_G) do
		if type(k) == "string" and type(v) == "function" and string.sub(k,1,5) == "test_" then
			print(k,"start")
			v()
			print(k,"end\n")
		end
	end
end


monk()