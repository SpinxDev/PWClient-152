---------------------------------------------------------------------
-- 本文件定义了一个策略管理器，管理各个玩家的策略对象
-- CREATED BY: Shizhenhua, 2012/9/11
---------------------------------------------------------------------


AIManager =
{
	instMap = {},		-- 使用该策略的实体列表（用户名 -> 策略对象）
	instCreator = {}	-- AI名字到AI模板的映射（AI名字 -> AI模板）
}

---------------------------------------------------------------------

-- 拷贝表
function CopyTable(src)
	local inst = {}
	for k, v in pairs(src) do
		if type(v) == "table" then
			inst[k] = CopyTable(v)
		else
			inst[k] = v
		end
	end

	local mt = getmetatable(src)
	setmetatable(inst, mt)
	return inst
end

-- 从模板创建对象
function CreateObj(template, obj)
	local inst = obj or {}
	for k, v in pairs(template) do
		if not inst[k] and type(v) ~= "function" then
			if type(v) == "table" and v ~= template then
				inst[k] = CopyTable(v)
			end
		end
	end
	setmetatable(inst, template)
	template.__index = template
	return inst
end

-- 注册策略创建函数
function AIManager:RegisterAI(ai_name, ai_templ)
	if self.instCreator[ai_name] == nil then
		self.instCreator[ai_name] = ai_templ
	end
end

-- 响应玩家进入该策略
function AIManager:OnEnter(username, ai_name)

	if self.instMap[username] == nil then
		local creator = self.instCreator[ai_name]
		if creator == nil then return false end
		self.instMap[username] = creator:new(username)	-- 创建一个新的AI实体
	end

	-- 玩家进入该策略
	self.instMap[username]:OnEnter()
	return true
end

-- 响应玩家离开该策略
function AIManager:OnLeave(username)
	if self.instMap[username] ~= nil then
		self.instMap[username]:OnLeave()	-- 玩家离开该策略
		self.instMap[username] = nil
	end
end

-- 响应玩家Tick
function AIManager:OnTick(username)
	if self.instMap[username] ~= nil then
		self.instMap[username]:OnTick()
	end
end

-- 响应AI命令
function AIManager:OnCommand(username, cmd)
	if self.instMap[username] ~= nil then
		self.instMap[username]:OnCommand(cmd)
	end
end

-- 玩家离开游戏
function AIManager:OnExit(username)
	if self.instMap[username] ~= nil then
		self.instMap[username] = nil
	end
end

-- 获取指定AI对象
function AIManager:GetAIObject(username)
	return self.instMap[username]
end

-- 初始化随机数种子
math.randomseed(os.time())
