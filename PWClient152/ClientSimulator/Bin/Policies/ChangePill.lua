---------------------------------------------------------------------
-- 搞一堆玩家在安全区吃变形丸，测自定义脸的问题
-- CREATED BY: Shizhenhua, 2012/12/26
---------------------------------------------------------------------


ChangePill = {}

local AI_STATE_IDLE = 0		-- 空闲
local AI_STATE_EATPILL = 1	-- 吃变形丸
local AI_STATE_DUMMY = 2	-- 妖精或妖兽变身

-- 惊涛城
local JINGTAO_REGION = { 2665.0,2679.0,-4258.0,-4240.0 }
-- 万流城
local WANLIU_REGION = { -2738.0,-2720.0,3089.0,3101.0 }

-- 变形丸TID
local FACEPILL_ITEM =
{
	6346, 6348, 6347
}


---------------------------------------------------------------------

-- 产生一份实例
function ChangePill:new(username)
	-- 这里定义实例的成员变量
	local member =
	{
		this = username,
		state = AI_STATE_IDLE,
		counter = 0,
		city = 0,			-- 0为惊涛城，1为万流城
		stateMap = {},		-- 状态处理函数映射

		have_facepill = 0,
	}

	-- 处理各个状态的函数
	member.stateMap[AI_STATE_IDLE] = self.OnState_Idle
	member.stateMap[AI_STATE_EATPILL] = self.OnState_EatPill

	return CreateObj(self, member)
end

-- 玩家进入该策略
function ChangePill:OnEnter()

	-- 随机飞一个地方，刷售卖凭证
	PlayerAPI.RandomMove(self.this)
	self.state = AI_STATE_EATPILL
end

-- 玩家离开该策略
function ChangePill:OnLeave()

end

-- 玩家状态更新
function ChangePill:OnTick()

	-- 处理响应的状态
	local stateHandler = self.stateMap[self.state]
	if stateHandler ~= nil then
		stateHandler(self)
	end
end

-- 响应AI命令
function ChangePill:OnCommand(cmd)

end

-- 空闲状态，啥也不做
function ChangePill:OnState_Idle()
	if self:CanEatFacePill() and not PlayerAPI.IsChangingFace(self.this) then
		if self.counter > 20 then
			-- 变形丸效果已结束，重新吃
			self.counter = 0
			self.state = AI_STATE_EATPILL
			PlayerAPI.RandomMove(self.this)
		else
			self.counter = self.counter + 1
		end
	end

	PlayerAPI.AddIdleTask(self.this, 3000)
end

-- 是否能吃变形丸(武侠，法师，羽灵，羽芒)
function ChangePill:CanEatFacePill()
	local prof = PlayerAPI.GetProfession(self.this)
	if prof == 0 or prof == 1 or prof == 6 or prof == 7 then
		return true
	else
		return false
	end
end

-- 是否能变身
function ChangePill:CanChangeShape()
	local prof = PlayerAPI.GetProfession(self.this)
	if prof == 3 or prof == 4 then
		return true
	else
		return false
	end
end

-- 吃变形丸
function ChangePill:OnState_EatPill()
	if self:CanEatFacePill() and not PlayerAPI.IsChangingFace(self.this) then
		if self.have_facepill == 0 then
			local rnd = math.random(1, table.getn(FACEPILL_ITEM))
			PlayerAPI.SendDebugCmd(self.this, 10800, FACEPILL_ITEM[rnd])
			self.have_facepill = FACEPILL_ITEM[rnd]
		else
			local idx = PlayerAPI.GetItemIndex(self.this, 0, self.have_facepill)
			if idx ~= -1 then
				PlayerAPI.UseItem(self.this, idx)
				self.have_facepill = 0
			end
		end

		PlayerAPI.AddIdleTask(self.this, 3000)
	else
		-- 飞到指定位置
		local x, z
		if self.city == 0 then
			x = JINGTAO_REGION[1] + math.random() * (JINGTAO_REGION[2] - JINGTAO_REGION[1])
			z = JINGTAO_REGION[3] + math.random() * (JINGTAO_REGION[4] - JINGTAO_REGION[3])
		else
			x = WANLIU_REGION[1] + math.random() * (WANLIU_REGION[2] - WANLIU_REGION[1])
			z = WANLIU_REGION[3] + math.random() * (WANLIU_REGION[4] - WANLIU_REGION[3])
		end

		PlayerAPI.Goto(self.this, x, z)
		self.state = AI_STATE_IDLE
	end
end

---------------------------------------------------------------------

-- 注册策略模板
AIManager:RegisterAI("ChangePill", ChangePill)
