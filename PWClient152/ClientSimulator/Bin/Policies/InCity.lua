---------------------------------------------------------------------
-- 本文件定义了一个策略，使多个玩家待在指定的城里模拟随机行为
-- CREATED BY: Shizhenhua, 2012/9/26
---------------------------------------------------------------------

InCity = {}

local AI_STATE_BOOTH = 0	-- 摆摊
local AI_STATE_SPEAK = 1	-- 喊话
local AI_STATE_ACTION = 2	-- 做各种各样的动作
local AI_STATE_SITDOWN = 3	-- 打坐
local AI_STATE_PK = 4		-- 互相PK
local AI_STATE_DEAD = 5		-- 挂了
local AI_STATE_LEARN = 6	-- 玩家学习技能
local AI_STATE_PREPARE = 7	-- 准备摆摊
local AI_STATE_FASHION = 8	-- 刷时装
local AI_STATE_FOLLOW = 9	-- 跟随玩家

-- 惊涛城
local JINGTAO_REGION = { 2665.0,2679.0,-4258.0,-4240.0 }
--local JINGTAO_REGION = { 1061.0, 1081.0, 1139.0, 1159.0 }
-- 万流城
local WANLIU_REGION = { -2738.0,-2720.0,3089.0,3101.0 }

-- 随机说话
local SPEAK_TEXT =
{
	"测试LUA脚本策略~", "你今天吃的啥？", "老夫聊发少年狂！",
	"去玩儿请问阿斯顿努努嘴", "完美世界王牌工作室",
	"筝诉我心", "力量今年内", "简单策略模板", "问世间情为何物",
	"帝高阳之苗裔兮", "朕皇考曰伯庸"
}

-- 售卖凭证
local SELL_TICKET =
{
	30998, 27748, 35154, 24717, 27775, 28111, 27413, 31026
}

-- 教授技能的NPC信息(x, z, tid)
local SKILL_TEACHER =
{
	-2741, 3110, 7387,	-- 武侠
	-2741, 3077, 7388,	-- 法师
	-2768, 3079, 26671,	-- 巫师
	-2755, 3077, 7389,	-- 妖精
	-2755, 3110, 7390,	-- 妖兽
	-2768, 3107, 26670,	-- 刺客
	-2747, 3079, 7392,	-- 羽芒
	-2748, 3108, 7391,	-- 羽灵
	-2761, 3109, 29188,	-- 剑灵
	-2761, 3077, 29189,	-- 魅灵
}

-- 男时装
-- 武器，上衣，裤子，头饰，护腕，靴子
local FASHION_SUITE_MALE = {
	{ 38280, 35673, 36867, 27548, 34407, 13293 },
	{ 38688, 31298, 38762, 29930, 26351, 29928 },
	{ 37835, 26300, 27765, 28340, 37247, 31659 },
	{ 38694, 34012, 28210, 37657, 27694, 27992 },
	{ 38275, 26246, 28335, 34561, 31654, 32805 },
	{ 38278, 36695, 15803, 29604, 38197, 26546 },
	{ 38276, 32219, 22252, 38247, 31109, 26553 },
	{ 38279, 31389, 34077, 34023, 37235, 15776 },
	{ 38687, 26276, 31017, 28218, 29613, 34388 },
	{ 38691, 26282, 26495, 31376, 28493, 4399 },
	{ 38122, 28185, 38493, 34716, 24748, 31115 },
	{ 38689, 27904, 32307, 32226, 31108, 32899 },
	{ 38274, 29596, 36789, 27870, 26335, 27769 },
	{ 38693, 31097, 24354, 31691, 24357, 26506 },
	{ 38277, 30322, 25306, 27852, 30304, 34085 },
	{ 38273, 31028, 26472, 31308, 26594, 22253 },
	{ 37836, 32215, 28322, 27890, 27879, 31033 },
	{ 37832, 38243, 31015, 31388, 26354, 35676 },
	{ 37833, 32302, 14137, 34022, 38245, 36453 },
	{ 37830, 33963, 35654, 27335, 32300, 36721 },
}

-- 女时装
local FASHION_SUITE_FEMALE = {
	{ 38701, 26321, 31014, 27781, 24894, 33783 },
	{ 38287, 32208, 31870, 31306, 27975, 30335 },
	{ 37838, 32076, 8605, 35678, 36455, 30843 },
	{ 38288, 28192, 26457, 36877, 32885, 30336 },
	{ 38283, 30845, 26451, 31383, 26393, 25075 },
	{ 38699, 28065, 27320, 33809, 26400, 22257 },
	{ 37842, 28652, 24706, 27987, 24759, 38541 },
	{ 38290, 27176, 26487, 27169, 28407, 27868 },
	{ 38123, 24887, 35273, 30186, 37506, 25076 },
	{ 37841, 36821, 28193, 28345, 32209, 36745 },
	{ 37840, 27173, 32889, 27337, 24893, 24619 },
	{ 37837, 28102, 15804, 27336, 36810, 28668 },
	{ 37843, 23480, 26368, 27777, 34087, 29939 },
	{ 38286, 13676, 18298, 38546, 30306, 36732 },
	{ 38702, 26327, 28006, 28075, 27882, 21062 },
	{ 38282, 38192, 26707, 28097, 26391, 33975 },
	{ 37844, 27314, 27321, 26185, 24749, 21190 },
	{ 38705, 38520, 15771, 33823, 36874, 25313 },
	{ 38697, 38252, 26489, 31380, 38193, 24710 },
	{ 38289, 35714, 12288, 34563, 23593, 38730 },
}


---------------------------------------------------------------------

-- 产生一份实例
function InCity:new(username)
	-- 这里定义实例的成员变量
	local member =
	{
		this = username,
		state = AI_STATE_ACTION,
		counter = 0,
		city = 0,		-- 0为惊涛城，1为万流城
		stateMap = {},	-- 状态处理函数映射
		cmdMap = {},	-- 处理命令的函数

		pickup_err = 0,
		have_sellticket = 0,
		learning = false,
		fashion_ready = 0,
		fashion_suite = {},
		follow_id = 0,
	}

	-- 处理各个状态的函数
	member.stateMap[AI_STATE_SPEAK] = self.OnState_Speak
	member.stateMap[AI_STATE_BOOTH] = self.OnState_Booth
	member.stateMap[AI_STATE_ACTION] = self.OnState_Action
	member.stateMap[AI_STATE_SITDOWN] = self.OnState_Sitdown
	member.stateMap[AI_STATE_PK] = self.OnState_PK
	member.stateMap[AI_STATE_DEAD] = self.OnState_Dead
	member.stateMap[AI_STATE_LEARN] = self.OnState_Learn
	member.stateMap[AI_STATE_PREPARE] = self.OnState_Prepare
	member.stateMap[AI_STATE_FASHION] = self.OnState_Fashion
	member.stateMap[AI_STATE_FOLLOW] = self.OnState_Follow

	-- 处理命令的函数
	member.cmdMap["打坐"] = self.OnCmd_Sitdown			-- 打坐
	member.cmdMap["说话"] = self.OnCmd_Speak			-- 讲话
	member.cmdMap["退出"] = self.OnCmd_Logout			-- 退出
	member.cmdMap["时装"] = self.OnCmd_Fashion			-- 时装
	member.cmdMap["骑宠"] = self.OnCmd_Mount			-- 骑乘宠物
	member.cmdMap["拥抱"] = self.OnCmd_Buddy			-- 拥抱最近的女性
	member.cmdMap["摆摊"] = self.OnCmd_Booth			-- 使玩家摆摊
	member.cmdMap["变身"] = self.OnCmd_ChangeShape		-- 妖精妖兽变身
	member.cmdMap["跟随"] = self.OnCmd_Follow			-- 跟随指定玩家

	member.cmdMap["changecity"] = self.OnCmd_ChangeCity		-- 改变玩家聚集城市
	member.cmdMap["callback"] = self.OnCmd_TestCallback		-- 测试回调函数


	return CreateObj(self, member)
end

-- 玩家进入该策略
function InCity:OnEnter()


	-- 来点钱，摆摊要用到
	if PlayerAPI.GetMoney(self.this) <= 0 then
		PlayerAPI.SendDebugCmd(self.this, 1988)
	end

	-- 开启PK模式
	--PlayerAPI.SetForceAttack(self.this, true)

	-- 随机飞一个地方，刷售卖凭证
	local x = math.random(-2000, 2000)
	local z = math.random(-2000, 2000)
	PlayerAPI.Goto(self.this, x, z)

	-- 跳到NPC那里学技能
--	local prof = PlayerAPI.GetProfession(self.this)
--	local x = SKILL_TEACHER[prof*3+1]
--	local z = SKILL_TEACHER[prof*3+2]
--	PlayerAPI.Goto(self.this, x, z)
--	PlayerAPI.AddIdleTask(self.this, 3000)

	self.state = AI_STATE_FASHION
	self.counter = 10
end

-- 玩家离开该策略
function InCity:OnLeave()
	-- 取消摆摊状态
	if self.state == AI_STATE_BOOTH then
		PlayerAPI.Booth(self.this, false)
	end

	-- 取消打坐状态
	if self.state == AI_STATE_SITDOWN then
		PlayerAPI.Sitdown(self.this, false)
	end
end

-- 玩家状态更新
function InCity:OnTick()
	-- 如果玩家挂了强制进入死亡状态
	if PlayerAPI.IsDead(self.this) then
		self.state = AI_STATE_DEAD
		self.counter = 0
	end

	-- 处理跟随玩家的情况
	if self.follow_id ~= 0 and self.state ~= AI_STATE_FOLLOW then
		self.state = AI_STATE_FOLLOW
		self.counter = 0
	elseif self.follow_id == 0 and self.state == AI_STATE_FOLLOW then
		self.state = AI_STATE_IDLE
		self.counter = 0
	end

	-- 处理响应的状态
	local stateHandler = self.stateMap[self.state]
	if stateHandler ~= nil then
		stateHandler(self)
	end

	-- 每执行10次Tick，我们更换状态
	self.counter = self.counter - 1
	if self.counter <= 0 then
		self:ChangeState()
	end
end

-- 玩家自由表演动作
function InCity:OnState_Action()
	local pose = math.random(0, 26)
	--PlayerAPI.PlayAction(self.this, pose)
	PlayerAPI.AddIdleTask(self.this, 3000)
end

-- 玩家摆摊
function InCity:OnState_Booth()
	-- 等待3秒钟
	PlayerAPI.AddIdleTask(self.this, 3000)
end

-- 玩家讲话
function InCity:OnState_Speak()
	local rnd = math.random(1, table.getn(SPEAK_TEXT))
	local strText = SPEAK_TEXT[rnd]
	PlayerAPI.Speak(self.this, strText)
	PlayerAPI.AddIdleTask(self.this, 3000)
end

-- 比较两个对象的距离
function CalcDistanceSqr(pos1, pos2)
	local dist_x = pos2[1] - pos1[1]
	local dist_y = pos2[2] - pos1[2]
	local dist_z = pos2[3] - pos1[3]
	return dist_x * dist_x + dist_y * dist_y + dist_z * dist_z
end

function CalcDistance(pos1, pos2)
	return math.sqrt(CalcDistanceSqr(pos1, pos2))
end

-- 玩家互相PK状态
function InCity:OnState_PK()
	local force_attack = PlayerAPI.GetForceAttack(self.this)
	local cur_pos = PlayerAPI.GetCurPos(self.this)
	local run_speed = PlayerAPI.GetRunSpeed(self.this)
	local sel_target = PlayerAPI.GetSelectedTarget(self.this)

	if sel_target == -1 then
		-- 在50米范围内选取一个怪物
		local target = GameAPI.GatherTargetAll(self.this, 50.0)

		if target == -1 then
			-- 没有找到目标玩家，退出PK状态
			self:ChangeState()
		else
			self.pickup_err = 0

			-- 只攻击玩家
			if GameAPI.GetIDType(target) == 0 or GameAPI.GetIDType(target) == 1 then
				PlayerAPI.SelectTarget(self.this, target)
			elseif GameAPI.GetIDType(target) == 2 then
				PlayerAPI.SelectMatter(self.this, target)
			end
		end

		if PlayerAPI.IsMoving(self.this) then
			PlayerAPI.AddStopMoveTask(self.this, cur_pos, 100, run_speed, 0x21)
		end
	else
		local target_obj = GameAPI.GetEntity(self.this, sel_target)

		if target_obj then
			-- 选择技能或普通攻击
			local skill, min_dist = PlayerAPI.SelectAttackSkill(self.this)
			local obj_pos = { target_obj.pos_x, target_obj.pos_y, target_obj.pos_z }

			local obj_type = GameAPI.GetIDType(target_obj.id)
			if obj_type == 2 then min_dist = 1.5 * 1.5 end

			if CalcDistanceSqr(cur_pos, obj_pos) <= min_dist then
				if obj_type == 0 or obj_type == 1 then
					if skill ~= 0 then
						local targets = { target_obj.id }	-- 技能只攻击一个目标
						PlayerAPI.CastSkill(self.this, skill, force_attack, targets)
					else
						-- 普通攻击
						PlayerAPI.NormalAttack(self.this, force_attack)
					end
				elseif obj_type == 2 then
					PlayerAPI.Pickup(self.this, target_obj.id, target_obj.tid)

					-- 物品连续拣3次，如果都失败加入禁用列表，防止一直捡取
					self.pickup_err = self.pickup_err + 1
					if PlayerAPI.IsCurTaskEmpty(self.this) and self.pickup_err >= 3 then
						PlayerAPI.Unselect(self.this)
						GameAPI.AddInvalidMatter(self.this, target_obj.id)
					end
				end
			else
				-- 距离过远
				local bRet, next_pos = PlayerAPI.GetNextMovePos(self.this, cur_pos, obj_pos, run_speed)
				if bRet then
					local distance = CalcDistance(next_pos, cur_pos)
					local use_time = distance / run_speed * 1000
					if use_time < 100 then use_time = 100 end
					if use_time > 1000 then use_time = 1000 end

					if CalcDistanceSqr(next_pos, obj_pos) < min_dist then
						PlayerAPI.AddStopMoveTask(self.this, next_pos, use_time, run_speed, 0x21)
					else
						PlayerAPI.AddMoveTask(self.this, next_pos, use_time, run_speed, 0x21)
					end
				else
					if PlayerAPI.IsMoving(self.this) then
						PlayerAPI.AddStopMoveTask(self.this, cur_pos, 100, run_speed, 0x21)
					end
				end
			end
		else
			PlayerAPI.Unselect(self.this)

			if PlayerAPI.IsMoving(self.this) then
				PlayerAPI.AddStopMoveTask(self.this, cur_pos, 100, run_speed, 0x21)
			end
		end
	end

	if PlayerAPI.IsCurTaskEmpty(self.this) then
		PlayerAPI.AddIdleTask(self.this, 1000)
	end
end

-- 玩家被杀死
function InCity:OnState_Dead()
	if PlayerAPI.IsDead(self.this) then
		-- 如果包裹内有复活卷轴，则接复活，否则回城复活
		if PlayerAPI.GetItemCount(self.this, 0, 3043) > 0 then
			PlayerAPI.ResurrectByItem(self.this)
		else
			PlayerAPI.ResurrectInTown(self.this)
		end

		PlayerAPI.AddIdleTask(self.this, 3000)
	else
		-- 改变死亡状态
		self:ChangeState()
	end
end

-- 玩家打坐
function InCity:OnState_Sitdown()
	PlayerAPI.AddIdleTask(self.this, 3000)
end

-- 玩家学习技能
function InCity:OnState_Learn()
	local prof = PlayerAPI.GetProfession(self.this)

	if not self.learning then
		local tid = SKILL_TEACHER[prof*3+3]

		-- 与NPC对话
		local npc_id = GameAPI.SearchNPC(self.this, tid)
		if npc_id ~= -1 then
			PlayerAPI.TalkToNPC(self.this, npc_id)
			self.learning = true
		else
			-- 没有找到教授技能NPC, 退出学习状态
			self:ChangeState()
		end
	else
		-- 提高修真等级
		if PlayerAPI.GetLevel2(self.this) < 32 then
			PlayerAPI.SendDebugCmd(self.this, 1989, 32)
			return
		end

		-- 如果没钱了
		if PlayerAPI.GetMoney(self.this) <= 200000 then
			PlayerAPI.SendDebugCmd(self.this, 1988)
			return
		end

		-- 如果技能点不够
		if PlayerAPI.GetSP(self.this) <= 100000 then
			PlayerAPI.SendDebugCmd(self.this, 10889, 100000000)
			return
		end

		-- 学技能
		local skill_list = GameAPI.GetSkillList(prof)
		for i=1, table.getn(skill_list) do
			local skill = PlayerAPI.GetSkillByID(self.this, skill_list[i])
			if skill ~= nil and skill.level >= GameAPI.GetSkillMaxLevel(skill_list[i]) then
				table.remove(skill_list, i)
				i = i - 1
			else
				PlayerAPI.LearnSkill(self.this, skill_list[i])
			end
		end

		-- 检查技能是否学完
		if table.getn(skill_list) == 0 then
			self:ChangeState()
			self.learning = false
		end
	end
end

-- 准备摆摊
function InCity:OnState_Prepare()
	if not PlayerAPI.IsAnyEquipExist(self.this, 24) then
		if self.have_sellticket == 0 then
			local rnd = math.random(1, table.getn(SELL_TICKET))
			PlayerAPI.SendDebugCmd(self.this, 10800, SELL_TICKET[rnd])
			self.have_sellticket = SELL_TICKET[rnd]
		else
			local idx = PlayerAPI.GetItemIndex(self.this, 0, self.have_sellticket)
			if idx ~= -1 then
				PlayerAPI.EquipItem(self.this, idx, 24)
				self.have_sellticket = 0
			end
		end

		PlayerAPI.AddIdleTask(self.this, 3000)
	else
		self:ChangeState()
	end
end

-- 准备时装
function InCity:OnState_Fashion()
	local fashion_slots = { 29, 13, 14, 25, 16, 15 }
	for i=1, table.getn(fashion_slots) do
		if PlayerAPI.IsAnyEquipExist(self.this, fashion_slots[i]) then
			self.fashion_ready = 2
			self:ChangeState()
			return
		end
	end

	if self.fashion_ready == 0 then
		-- 随机刷一套时装
		local rnd = math.random(1, 20)
		if PlayerAPI.IsMale(self.this) then
			self.fashion_suite = FASHION_SUITE_MALE[rnd]
		else
			self.fashion_suite = FASHION_SUITE_FEMALE[rnd]
		end
		for i=1, 6 do
			PlayerAPI.SendDebugCmd(self.this, 10800, self.fashion_suite[i])
		end

		self.fashion_ready = 1
		PlayerAPI.AddIdleTask(self.this, 3000)
	elseif self.fashion_ready == 1 then
		for i=1, table.getn(self.fashion_suite) do
			local idx = PlayerAPI.GetItemIndex(self.this, 0, self.fashion_suite[i])
			if idx ~= -1 then
				PlayerAPI.EquipItem(self.this, idx, fashion_slots[i])
			end
		end

		PlayerAPI.AddIdleTask(self.this, 3000)
	end
end

-- 跟随他人
function InCity:OnState_Follow()
	local target_obj = GameAPI.GetEntity(self.this, self.follow_id)
	if target_obj == nil then
		self:ChangeState()
		return
	end

	local min_dist = 2.0 * 2.0
	local obj_pos = { target_obj.pos_x, target_obj.pos_y, target_obj.pos_z }
	local cur_pos = PlayerAPI.GetCurPos(self.this)
	local run_speed = PlayerAPI.GetRunSpeed(self.this)
	if CalcDistanceSqr(cur_pos, obj_pos) <= min_dist then
		return
	end

	local bRet, next_pos = PlayerAPI.GetNextMovePos(self.this, cur_pos, obj_pos, run_speed)
	if bRet then
		local distance = CalcDistance(next_pos, cur_pos)
		local use_time = distance / run_speed * 1000
		if use_time < 100 then use_time = 100 end
		if use_time > 1000 then use_time = 1000 end

		if CalcDistanceSqr(next_pos, obj_pos) < min_dist then
			PlayerAPI.AddStopMoveTask(self.this, next_pos, use_time, run_speed, 0x21)
		else
			PlayerAPI.AddMoveTask(self.this, next_pos, use_time, run_speed, 0x21)
		end
	else
		if PlayerAPI.IsMoving(self.this) then
			PlayerAPI.AddStopMoveTask(self.this, next_pos, 100, run_speed, 0x21)
		end
	end

	if PlayerAPI.IsCurTaskEmpty(self.this) then
		PlayerAPI.AddIdleTask(self.this, 1000)
	end
end

-- 改变当前状态
function InCity:ChangeState()
	local state = math.random(AI_STATE_BOOTH, AI_STATE_SITDOWN)
	if state == AI_STATE_SITDOWN or state == AI_STATE_SPEAK then return end
	local state = AI_STATE_ACTION
	if self.state == AI_STATE_BOOTH and state ~= self.state then
		PlayerAPI.Booth(self.this, false)
	elseif self.state ~= AI_STATE_BOOTH and state == AI_STATE_BOOTH then
		PlayerAPI.Booth(self.this, true)
	end

	if self.state == AI_STATE_SITDOWN and state ~= self.state then
		PlayerAPI.Sitdown(self.this, false)
	elseif self.state ~= AI_STATE_SITDOWN and state == AI_STATE_SITDOWN then
		PlayerAPI.Sitdown(self.this, true)
	end

	if self.state == AI_STATE_LEARN or self.state == AI_STATE_PREPARE or self.state == AI_STATE_FASHION then
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
	end

	self.state = state
	self.counter = 10
end

-- 响应AI命令
function InCity:OnCommand(cmd)
	if table.getn(cmd) >= 1 then
		local cmdHead = cmd[1]
		local cmdProc = self.cmdMap[cmdHead]
		if cmdProc then
			table.remove(cmd, 1)	-- 取掉命令本身
			cmdProc(self, cmd)
		else
			Util.LogOutput(self.this, string.format("InCity:OnCommand, 错误的命令(%s)！", cmdHead))
		end
	end
end

-- 响应Sitdown命令
function InCity:OnCmd_Sitdown(param)
	local bSit
	if tonumber(param[1]) == 1 then
		bSit = true
	else
		bSit = false
	end

	PlayerAPI.Sitdown(self.this, bSit)
end

-- 响应Speak命令
function InCity:OnCmd_Speak(param)
	PlayerAPI.Speak(self.this, param[1])
end

-- 响应Logout命令
function InCity:OnCmd_Logout(param)
	PlayerAPI.Logout(self.this)
end

-- 响应ChangeCity命令
function InCity:OnCmd_ChangeCity(param)
	local city_id = tonumber(param[1])
	if self.city ~= city_id then
		self:OnLeave()
		self.city = city_id
		self:OnEnter()
	end
end

-- 响应Fashion命令
function InCity:OnCmd_Fashion(param)
	local bFashion
	if tonumber(param[1]) == 1 then
		bFashion = true
	else
		bFashion = false
	end

	PlayerAPI.SwitchFashion(self.this, bFashion)
end

-- 响应Mount命令，骑乘宠物
function InCity:OnCmd_Mount(param)
	local pet_idx = tonumber(param[1])
	local pet_obj = PlayerAPI.GetPetData(self.this, pet_idx)
	if pet_obj ~= nil and pet_obj.class == PET_CLASS_MOUNT then
		PlayerAPI.SummonPet(self.this, pet_idx)
	end
end

-- 响应Buddy命令，拥抱最近的异性
-- Param: 指定的username
function InCity:OnCmd_Buddy(param)
	local id = PlayerAPI.GetRoleID(param[1])
	if id ~= nil then
		PlayerAPI.AttachBuddy(self.this, id)
	end
end

-- 使玩家摆摊
function InCity:OnCmd_Booth(param)
	local bBooth
	if tonumber(param[1]) == 1 then
		bBooth = true
	else
		bBooth = false
	end
	PlayerAPI.Booth(self.this, bBooth)
end

-- 妖精妖兽变身
function InCity:OnCmd_ChangeShape(param)
	local skillid = nil
	if PlayerAPI.GetProfession(self.this) == PROF_HAG and PlayerAPI.GetSkillByID(self.this, 312) then
		skillid = 312
	elseif PlayerAPI.GetProfession(self.this) == PROF_ORC and PlayerAPI.GetSkillByID(self.this, 112) then
		skillid = 112
	end

	if skillid ~= nil then
		local roleid = PlayerAPI.GetRoleID(self.this)
		PlayerAPI.CastSkill(self.this, skillid, 0, {roleid})
	end
end

-- 跟随指定玩家
function InCity:OnCmd_Follow(param)
	self.follow_id = tonumber(param[1])
end

-- 踢出不是妖兽的玩家
function CB_KickOutMale(username)
	if PlayerAPI.GetProfession(username) ~= PROF_ORC then
		PlayerAPI.Logout(username)
	end
end

function InCity:OnCmd_TestCallback(param)
	Util.EnumPlayers("CB_KickOutMale")
end

---------------------------------------------------------------------

-- 注册策略模板
AIManager:RegisterAI("InCity", InCity)
