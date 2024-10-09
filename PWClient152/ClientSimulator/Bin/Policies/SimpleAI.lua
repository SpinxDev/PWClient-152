---------------------------------------------------------------------
-- 本文件定义了一个极简单的策略作为参考框架
-- CREATED BY: Shizhenhua, 2012/9/11
---------------------------------------------------------------------


SimpleAI = {}		-- 简单策略模板

-- AI相关的状态
local AI_STATE_SPEAK = 0

-- 玩家随机讲话内容
local SPEAK_TEXT =
{
	"测试LUA脚本策略~", "你今天吃的啥？", "老夫聊发少年狂！",
	"去玩儿请问阿斯顿努努嘴", "完美世界王牌工作室",
	"筝诉我心", "力量今年内", "简单策略模板", "问世间情为何物",
	"帝高阳之苗裔兮", "朕皇考曰伯庸"
}

---------------------------------------------------------------------

-- 添加一个新的AI对象
function SimpleAI:new(username)
	-- 这里定义AI对象的成员变量，此AI对象和每个玩家相关联
	local member =
	{
		this = username,
		state = 0,
		counter = 0
	}

	-- 根据模板生成对应玩家的AI对象
	return CreateObj(self, member)
end

-- 响应玩家进入该策略
function SimpleAI:OnEnter()
	self.state = AI_STATE_SPEAK
	PlayerAPI.Speak(self.this, "进入SimpleAI策略！")
end

-- 响应玩家离开该策略
function SimpleAI:OnLeave()
	PlayerAPI.Speak(self.this, "退出SimpleAI策略！")
end

-- 响应玩家Tick
function SimpleAI:OnTick()

	if self.counter >= 50 then
		local rnd = math.random(1, table.getn(SPEAK_TEXT))
		PlayerAPI.Speak(self.this, SPEAK_TEXT[rnd])
		self.counter = 0
	end

	-- 间隔一秒
	self.counter = self.counter + 1
end

-- 响应AI命令
function SimpleAI:OnCommand(cmd)

	if table.getn(cmd) >= 1 then
		local strText = "SimpleAI执行命令:" .. cmd[1]
		for i=2, table.getn(cmd) do
			strText = strText .. string.format(" 参数%d: %s", i, cmd[i])
		end

		PlayerAPI.Speak(self.this, strText)
	else
		Util.LogOutput("错误命令~")
	end
end

---------------------------------------------------------------------

-- 注册AI模板
AIManager:RegisterAI("SimpleAI", SimpleAI)
