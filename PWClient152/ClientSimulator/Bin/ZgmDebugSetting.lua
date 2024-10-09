function ToString(t,dept)
	if dept < 0 then return "" end
	local s = "{"
	local sep = ""
	for k,v in pairs(t) do
		local value = v
		if(type(v) ~= "function") then
			if(type(v) == "table") then
				value = ToString(v,dept-1)
			end
			s = s .. sep .. k .. " = " .. value
			sep = ", "
		end
	end
	return s .. "}"
end

function sleep(n)

   if n > 0 then os.execute("ping -n " .. tonumber(n) .. " localhost > NUL") end

end

function zgm_trace (event, line)
	local file = debug.getinfo(2, "S").source
	if string.find(file, "@") == 1 then
		file = string.sub(file, 2)
	end
	CHook(file , line, event)
	while true do
		if CCanContinue() > 0 then break end
		sleep(1)
	end
end

function zgm_lua_debug()
	debug.sethook(zgm_trace,'lcr')
end

function zgm_getvarstring(v)
	local value = tostring(v)
	if(type(v) == "table") then
		value = ToString(v,100)
	end
	return value
end

function zgm_getvarvalue (name)
	local value, found

	-- try local variables
	local i = 1
	while true do
		local n, v = debug.getlocal(4, i)
		if not n then break end
		if n == name then
			value = v
			found = true
		end
	i = i + 1
	end
	if found then return zgm_getvarstring(value) end

	-- try upvalues
	local func = debug.getinfo(4).func
	i = 1
	while true do
		local n, v = debug.getupvalue(func, i)
		if not n then break end
		if n == name then return zgm_getvarstring(v) end
		i = i + 1
	end

	-- not found; get global
	value = getfenv(func)[name]
	return zgm_getvarstring(value)
end

zgm_lua_debug()