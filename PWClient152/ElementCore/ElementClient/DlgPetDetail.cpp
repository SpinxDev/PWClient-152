// File		: DlgPetDetail.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#include "AFI.h"
#include "DlgPetDetail.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrPetItem.h"
#include "EC_Skill.h"
#include "A3DDevice.h"
#include "A3DFlatCollector.h"
#include "A2DSprite.h"
#include "EC_Global.h"
#include "AUIProgress.h"

#define new A_DEBUG_NEW

PetRadarMap CDlgPetDetail::m_mapPetRadar;
A2DSprite* CDlgPetDetail::m_pA2DSpritePetRadarMask = NULL;

CDlgPetDetail::CDlgPetDetail()
{
	m_nSlot = -1;
}

CDlgPetDetail::~CDlgPetDetail()
{
	A3DRELEASE(m_pA2DSpritePetRadarMask);
}

bool CDlgPetDetail::OnInitDialog()
{
	if( !m_pA2DSpritePetRadarMask )
	{
		m_pA2DSpritePetRadarMask = new A2DSprite;
		if( !m_pA2DSpritePetRadarMask ) return AUI_ReportError(__LINE__, __FILE__);

		bool bval = m_pA2DSpritePetRadarMask->Init(m_pA3DDevice, "InGame\\PetRadarMask.tga", 0);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}

	m_pTxt_Name = (PAUILABEL)GetDlgItem("Txt_Name");
	m_pTxt_Level = (PAUILABEL)GetDlgItem("Txt_Level");
	m_pTxt_Exp = (PAUILABEL)GetDlgItem("Txt_Exp");
	m_pTxt_Attack = (PAUILABEL)GetDlgItem("Txt_Attack");
	m_pTxt_PhyDefence = (PAUILABEL)GetDlgItem("Txt_PhyDefence");
	m_pTxt_MagDefence = (PAUILABEL)GetDlgItem("Txt_MagDefence");
	m_pTxt_MoveSpeed = (PAUILABEL)GetDlgItem("Txt_MoveSpeed");
	m_pTxt_Loyalty = (PAUILABEL)GetDlgItem("Txt_Loyalty");
	m_pTxt_Food = (PAUILABEL)GetDlgItem("Txt_Food");
	m_pTxt_Hunger = (PAUILABEL)GetDlgItem("Txt_Hunger");
	m_pTxt_RequireLevel = (PAUILABEL)GetDlgItem("Txt_RequireLevel");
	m_pTxt_HP = (PAUILABEL)GetDlgItem("Txt_HP");
	m_pTxt_Definition = (PAUILABEL)GetDlgItem("Txt_Definition");
	m_pTxt_Evade = (PAUILABEL)GetDlgItem("Txt_Evade");
	m_pTxt_AtkSpeed = (PAUILABEL)GetDlgItem("Txt_AtkSpeed");
	m_pTxt_Type = (PAUILABEL)GetDlgItem("Txt_Type");
	m_pTxt_Color = (PAUILABEL)GetDlgItem("Txt_Color");
	m_pTxt_Bind = (PAUILABEL)GetDlgItem("Txt_Bind");
	m_pImg_Icon = (PAUIIMAGEPICTURE)GetDlgItem("Img_Icon");
	m_pImg_PetRadar = (PAUIIMAGEPICTURE)GetDlgItem("Img_PetRadar");
	for(int i = 0; i < 4; i++ )
	{
		char szText[20];
		sprintf(szText, "Img_Skill%02d", i + 1);
		m_pImg_Skill[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
	}

	if( stricmp(m_szName, "Win_PetDetail1") == 0 )
	{
		AFileImage fList;

		if( fList.Open("surfaces\\ingame\\petradar.txt", AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		{
			DWORD dwLen;
			char szCurLine[100];
			do
			{
				if( !fList.ReadLine(szCurLine, 100, &dwLen) || strcmp(szCurLine, "") == 0)
					break;
				int id, p0, p1, p2, p3, p4, p5, p6, p7;
				sscanf(szCurLine, "%d%d%d%d%d%d%d%d%d",
					&id, &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
				m_mapPetRadar[id] = p0 + (p1 << 4) + (p2 << 8) + (p3 << 12) + (p4 << 16) + (p5 << 20) + (p6 << 24) + (p7 << 28);
			}while(true);
			fList.Close();
		}
	}

	return true;
}

void CDlgPetDetail::OnTick()
{
	UpdatePet();
}

void CDlgPetDetail::UpdatePet(int nSlot)
{
	if( nSlot != -1 )
	{
		m_nSlot = nSlot;
		int i;
		for(i = 0; i < GP_PET_CLASS_MAX; i++ )
		{
			char szText[20];
			sprintf(szText, "Win_PetDetail%d", i);
			CDlgPetDetail *pDlgDetail = (CDlgPetDetail *)GetGameUIMan()->GetDialog(szText);
			if( pDlgDetail && pDlgDetail != this)
				pDlgDetail->Show(false);
		}
		Show(true);
	}
	else
	{
		elementdataman* pDBMan = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		ACHAR szText[1000];
		ACString strText;
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetPetData(m_nSlot);
		if( pPet )
		{
			int nLevel, nLoyalty, nHunger;
			nLevel = pPet->GetLevel();
			m_pTxt_Name->SetText(pPet->GetName());

			if (m_pTxt_Bind && m_pTxt_Bind->IsShow() != pPet->IsBind())
			{
				m_pTxt_Bind->Show(!m_pTxt_Bind->IsShow());
				if (m_pTxt_Bind->IsShow())
					m_pTxt_Bind->SetText(GetStringFromTable(pPet->CanWebTrade() ? 895 : 894));
				else
					m_pTxt_Bind->SetText(_AL(""));
			}

			if( pPet->GetClass() == GP_PET_CLASS_MOUNT || pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
			{
				a_sprintf(szText, _AL("%d"), nLevel);
				m_pTxt_Level->SetText(szText);
				a_sprintf(szText, _AL("%d/%d"), pPet->GetExp(), pPet->GetMaxExp());
				m_pTxt_Exp->SetText(szText);
				if( pPet->GetIntimacy() <= 50 )
					nLoyalty = 0;
				else if( pPet->GetIntimacy() <= 150 )
					nLoyalty = 1;
				else if( pPet->GetIntimacy() <= 500 )
					nLoyalty = 2;
				else
					nLoyalty = 3;
				strText.Format(_AL("%s %d"), GetStringFromTable(3601 + nLoyalty), pPet->GetIntimacy());
				m_pTxt_Loyalty->SetText(strText);
				if( pPet->GetClass() == GP_PET_CLASS_MOUNT )
					strText.Format(GetStringFromTable(3622), GetStringFromTable(3601 + nLoyalty),
						hintDrop[nLoyalty], hintExp[nLoyalty]);
				else if( pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
					strText.Format(GetStringFromTable(3623), GetStringFromTable(3601 + nLoyalty),
						hintAttack[nLoyalty], hintExp[nLoyalty]);
				else
					strText = _AL("");
				m_pTxt_Loyalty->SetHint(strText);
				if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_0 )
					nHunger = 0;
				else if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_1 )
					nHunger = 1;
				else if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_2 )
					nHunger = 2;
				else if( pPet->GetHunger() <= CECPetData::HUNGER_LEVEL_4 )
					nHunger = 3;
				else if( pPet->GetHunger() <= CECPetData::HUNGER_LEVEL_7 )
					nHunger = 4;
				else
					nHunger = 5;
				m_pTxt_Hunger->SetText(GetStringFromTable(3611 + nHunger));
			}

			PET_ESSENCE *pDB = (PET_ESSENCE*)pDBMan->get_data_ptr(pPet->GetTemplateID(), 
				ID_SPACE_ESSENCE, DataType);
			if( pDB )
			{
				AString strFile;
				af_GetFileTitle(pDB->file_icon, strFile);
				strFile.MakeLower();
				m_pImg_Icon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

				float speed = pDB->speed_a + pDB->speed_b * (nLevel - 1);
				strText.Format(_AL("%3.1f %s"), speed, GetStringFromTable(280));
				m_pTxt_MoveSpeed->SetText(strText);
				
				if(m_pTxt_RequireLevel)
				{
					const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
					if( pDB->level_require > rbp.iLevel )
						a_sprintf(szText, _AL("^FF0000%d"), pDB->level_require);
					else
						a_sprintf(szText, _AL("%d"), pDB->level_require);
					m_pTxt_RequireLevel->SetText(szText);
				}

				if (pPet->GetClass() == GP_PET_CLASS_MOUNT && m_pTxt_Color)
				{
					//	骑宠增加颜色显示
					ACString strColor;
					if (pDB->require_dye_count > 0)
					{
						A3DCOLOR clr;
						if (!CECPlayer::RIDINGPET::GetColor(pPet->GetColor(), clr))
							clr = CECPlayer::RIDINGPET::GetDefaultColor();

						ACString strTemp;
						strTemp.Format(_AL("^%02x%02x%02x"), A3DCOLOR_GETRED(clr), A3DCOLOR_GETGREEN(clr), A3DCOLOR_GETBLUE(clr));
						strColor.Format(g_pGame->GetItemDesc()->GetWideString(ITEMDESC_COLORRECT), strTemp);
					}
					m_pTxt_Color->SetText(strColor);
				}

				if( pPet->GetClass() == GP_PET_CLASS_MOUNT || pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
				{
					CECStringTab* pDescTab = GetGame()->GetItemDesc();
					ACString szFood = _AL("");
					for (int i = 0; i < MAX_PET_FOOD; i++)
						if (pDB->food_mask & (1 << i))
						{
							if( szFood != _AL(""))
								szFood += _AL(",");
							szFood += pDescTab->GetWideString(ITEMDESC_FOOD_GRASS + i);
						}
					m_pTxt_Food->SetText(szFood);
				}
				if( pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
				{
					// 当前唤出的是否是进化宠
					bool bEvolutionActive = (pPet->GetClass() == GP_PET_CLASS_EVOLUTION) && (pPetCorral->GetActivePetIndex() == m_nSlot);

					const ROLEEXTPROP& petProp = pPet->GetExtendProps();

					int nMaxHP = bEvolutionActive ? petProp.bs.max_hp : int(pDB->hp_a * (nLevel - pDB->hp_b * pDB->level_require + pDB->hp_c));
					int nCurHP = bEvolutionActive ? pPet->GetHP() : int(nMaxHP * pPet->GetHPFactor());
					a_sprintf(szText, _AL("%d/%d"), nCurHP, nMaxHP);
					m_pTxt_HP->SetText(szText);

					int nCurVal = bEvolutionActive ? ((petProp.ak.damage_high - petProp.ak.damage_low) / 2 + petProp.ak.damage_low) : int((pDB->damage_a * (pDB->damage_b * nLevel * nLevel + pDB->damage_c * nLevel + pDB->damage_d))) * hintAttack[nLoyalty] / 100;
					a_sprintf(szText, _AL("%d"), nCurVal);
					m_pTxt_Attack->SetText(szText);

					nCurVal = bEvolutionActive ? petProp.ak.attack:int(pDB->attack_a * (nLevel - pDB->attack_b * pDB->level_require + pDB->attack_c));
					a_sprintf(szText, _AL("%d"), nCurVal);
					m_pTxt_Definition->SetText(szText);

					nCurVal = bEvolutionActive ? petProp.df.armor:int(pDB->armor_a * (nLevel - pDB->armor_b * pDB->level_require + pDB->armor_c));
					a_sprintf(szText, _AL("%d"), nCurVal);
					m_pTxt_Evade->SetText(szText);

					nCurVal = bEvolutionActive ? petProp.df.defense:int(pDB->physic_defence_a * (pDB->physic_defence_b * (nLevel - pDB->physic_defence_c * pDB->level_require) + pDB->physic_defence_d));
					a_sprintf(szText, _AL("%d"), nCurVal);
					m_pTxt_PhyDefence->SetText(szText);

					int magDef = 0;
					for (int cur=0;cur<NUM_MAGICCLASS;cur++)
					{
						magDef += petProp.df.resistance[cur];
					}
					magDef /= NUM_MAGICCLASS;
					
					nCurVal = bEvolutionActive ? magDef: int(pDB->magic_defence_a * (pDB->magic_defence_b * (nLevel - pDB->magic_defence_c * pDB->level_require) + pDB->magic_defence_d));
					a_sprintf(szText, _AL("%d"), nCurVal);
					m_pTxt_MagDefence->SetText(szText);

					if(bEvolutionActive)
						strText.Format(_AL("%4.2f%s"), petProp.ak.attack_speed ?  1.0f / (petProp.ak.attack_speed * 0.05f) : 0.0f,GetStringFromTable(279));
					else
						strText.Format(_AL("%4.2f%s"), 1.0f / pDB->attack_speed, GetStringFromTable(279));
					m_pTxt_AtkSpeed->SetText(strText);

					strText.Format(_AL("%s"), GetStringFromTable(1401 + pDB->inhabit_type));
					m_pTxt_Type->SetText(strText);

					PetRadarMap::iterator it = m_mapPetRadar.find(pPet->GetTemplateID());
					if( it != m_mapPetRadar.end() )
					{
						for(int j = 0; j < 8; j ++ )
							m_nPetRadar[j] = (it->second >> (j * 4)) & 15;
					}
					else
					{
						for(int j = 0; j < 8; j ++ )
							m_nPetRadar[j] = 0;
					}
					for(int i = 0; i < 4; i++ )
					{
						const CECPetData::PETSKILL *pSkill = pPet->GetSkill(CECPetData::EM_SKILL_NORMAL,i);
						if( pSkill && pSkill->idSkill != 0 )
						{
							const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pSkill->idSkill * 10);
							CECSkill::GetDesc(pSkill->idSkill, pSkill->iLevel, szText, 1000);
							m_pImg_Skill[i]->SetHint(szText);
							const char* szIcon = GNET::ElementSkill::GetIcon(pSkill->idSkill);
							AString strFile;
							af_GetFileTitle(szIcon, strFile);
							strFile.MakeLower();
							m_pImg_Skill[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
								GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
						}
						else
						{
							m_pImg_Skill[i]->ClearCover();
							m_pImg_Skill[i]->SetHint(_AL(""));
						}
					}

					if (pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
					{
						ACString str;
						const PET_ESSENCE* pE = pPet->GetPetEssence();

						// attack ration
						PAUIPROGRESS progress = ((PAUIPROGRESS)GetDlgItem("Pro_Ack"));
						progress->SetRangeMax(pE->attack_inherit_max_rate);
						progress->SetProgress(pPet->GetAtkRation());						
						str.Format(_AL("%d/%d"),pPet->GetAtkRation(),pE->attack_inherit_max_rate);
						GetDlgItem("Txt_Ack")->SetText(str);

						// defense ration
						progress = ((PAUIPROGRESS)GetDlgItem("Pro_Def"));
						progress->SetRangeMax(pE->defence_inherit_max_rate);
						progress->SetProgress(pPet->GetDefRation());
						str.Format(_AL("%d/%d"),pPet->GetDefRation(),pE->defence_inherit_max_rate);
						GetDlgItem("Txt_Def")->SetText(str);

						// hp ration
						progress = ((PAUIPROGRESS)GetDlgItem("Pro_Hp"));
						progress->SetRangeMax(pE->hp_inherit_max_rate);
						progress->SetProgress(pPet->GetHpRation());
						str.Format(_AL("%d/%d"),pPet->GetHpRation(),pE->hp_inherit_max_rate);
						GetDlgItem("Txt_Hp")->SetText(str);

						// attack level ration
						progress = ((PAUIPROGRESS)GetDlgItem("Pro_AckLv"));
						progress->SetRangeMax(pE->attack_level_inherit_max_rate);
						progress->SetProgress(pPet->GetAtkLvlRation());
						str.Format(_AL("%d/%d"),pPet->GetAtkLvlRation(),pE->attack_level_inherit_max_rate);
						GetDlgItem("Txt_AckLv")->SetText(str);

						// defense level ration
						progress = ((PAUIPROGRESS)GetDlgItem("Pro_DeLv"));
						progress->SetRangeMax(pE->defence_level_inherit_max_rate);
						progress->SetProgress(pPet->GetDefLvlRation());
						str.Format(_AL("%d/%d"),pPet->GetDefLvlRation(),pE->defence_level_inherit_max_rate);
						GetDlgItem("Txt_DeLv")->SetText(str);

						GetDlgItem("Txt_Nature")->SetText(pPet->GetNature());
					}
				}
				else if( pPet->GetClass() == GP_PET_CLASS_FOLLOW )
				{
					int nMaxHP = int(pDB->hp_a * (nLevel - pDB->hp_b * pDB->level_require + pDB->hp_c));
					a_sprintf(szText, _AL("%d/%d"), int(nMaxHP * pPet->GetHPFactor()), 
						nMaxHP);
					m_pTxt_HP->SetText(szText);
				}
				else if( pPet->GetClass() == GP_PET_CLASS_SUMMON )
				{
					const ROLEEXTPROP& petProp = pPet->GetExtendProps();

					// HP
					a_sprintf(szText, _AL("%d/%d"), pPet->GetHP(), petProp.bs.max_hp );
					m_pTxt_HP->SetText(szText);

					// MP
					PAUILABEL m_pTxt_MP = (PAUILABEL)GetDlgItem("Txt_MP");
					a_sprintf(szText, _AL("%d/%d"), pPet->GetMP(), petProp.bs.max_mp);
					m_pTxt_MP->SetText(szText);

					// level
					a_sprintf(szText, _AL("%d"), nLevel);
					m_pTxt_Level->SetText(szText);

					// 物理攻击
					a_sprintf(szText, _AL("%d"), (petProp.ak.damage_high - petProp.ak.damage_low) / 2 + petProp.ak.damage_low);
					m_pTxt_Attack->SetText(szText);

					// 法术攻击
					PAUILABEL m_pTxt_MagicAttack = (PAUILABEL)GetDlgItem("Txt_MagicAttack");
					a_sprintf(szText, _AL("%d"), (petProp.ak.damage_magic_high - petProp.ak.damage_magic_low) / 2 + petProp.ak.damage_magic_low);
					m_pTxt_MagicAttack->SetText(szText);

					// 物理防御
					a_sprintf(szText, _AL("%d"), petProp.df.defense);
					m_pTxt_PhyDefence->SetText(szText);
					
					// 金防
					PAUILABEL m_pTxt_Metal = (PAUILABEL)GetDlgItem("Txt_Metal");
					a_sprintf(szText, _AL("%d"), petProp.df.resistance[0]);
					m_pTxt_Metal->SetText(szText);
					
					// 木防
					PAUILABEL m_pTxt_Plant = (PAUILABEL)GetDlgItem("Txt_Plant");
					a_sprintf(szText, _AL("%d"), petProp.df.resistance[1]);
					m_pTxt_Plant->SetText(szText);
					
					// 水防
					PAUILABEL m_pTxt_Water = (PAUILABEL)GetDlgItem("Txt_Water");
					a_sprintf(szText, _AL("%d"), petProp.df.resistance[2]);
					m_pTxt_Water->SetText(szText);
					
					// 火防
					PAUILABEL m_pTxt_Fire = (PAUILABEL)GetDlgItem("Txt_Fire");
					a_sprintf(szText, _AL("%d"), petProp.df.resistance[3]);
					m_pTxt_Fire->SetText(szText);
					
					// 土防
					PAUILABEL m_pTxt_Crude = (PAUILABEL)GetDlgItem("Txt_Crude");
					a_sprintf(szText, _AL("%d"), petProp.df.resistance[4]);
					m_pTxt_Crude->SetText(szText);

					// 命中度
					a_sprintf(szText, _AL("%d"), petProp.ak.attack);
					m_pTxt_Definition->SetText(szText);

					// 攻击速度
					a_sprintf(szText, _AL("%4.2f%s"), petProp.ak.attack_speed ?  1.0f / (petProp.ak.attack_speed * 0.05f) : 0.0f, 
						m_pAUIManager->GetStringFromTable(279));
					m_pTxt_AtkSpeed->SetText(szText);

					// 闪避度
					a_sprintf(szText, _AL("%d"), petProp.df.armor);
					m_pTxt_Evade->SetText(szText);

					// 移动速度
					a_sprintf(szText, _AL("%3.1f %s"), petProp.mv.run_speed , GetStringFromTable(280));
					m_pTxt_MoveSpeed->SetText(szText);

				}
			}
			else
			{
				m_pImg_Icon->ClearCover();
				m_pImg_Icon->SetHint(_AL(""));
				m_pTxt_MoveSpeed->SetHint(_AL(""));
				m_pTxt_RequireLevel->SetHint(_AL(""));
				m_pTxt_Food->SetText(_AL(""));
			}
		}
	}
}

bool CDlgPetDetail::Render()
{
	if( !AUIDialog::Render() )
		return false;

	if( stricmp(m_szName, "Win_PetDetail1") == 0 )
	{
		A3DFlatCollector *pFC = GetGame()->GetA3DEngine()->GetA3DFlatCollector();
		DWORD i;
		A3DVECTOR3 *verts = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3)*9);
		WORD *indices = (WORD*)a_malloctemp(sizeof(WORD)*24);
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		A3DPOINT2 pt;
		POINT ptImg = m_pImg_PetRadar->GetPos();
		SIZE szImg = m_pImg_PetRadar->GetSize();
		pt.Set(p->X + ptImg.x + szImg.cx / 2, p->Y + ptImg.y + szImg.cy / 2);
		verts[8].Set(float(pt.x), float(pt.y), 0.0f);
		int nMax = szImg.cx * 3 / 7;
		
		for(i = 0; i < 8; i++ )
		{
			verts[i].x = float(pt.x + m_nPetRadar[i] * nMax * cos(i * 3.1415926 / 4) / 7);
			verts[i].y = float(pt.y + m_nPetRadar[i] * nMax * sin(i * 3.1415926 / 4) / 7);
			verts[i].z = 0.0f;
		}
		for(i = 0; i < 8; i++ )
		{
			indices[i * 3] = WORD(i);
			indices[i * 3 + 1] = WORD((i + 1) % 8);
			indices[i * 3 + 2] = 8;
		}

		A3DCULLTYPE oldtype = m_pA3DDevice->GetFaceCull();
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		pFC->AddRenderData_2D(verts, 9, 
			indices, 24, A3DCOLORRGB(0, 128, 0));
		pFC->Flush_2D();
		m_pA3DDevice->SetFaceCull(oldtype);
		a_freetemp(indices);
		a_freetemp(verts);
		m_pA2DSpritePetRadarMask->SetLinearFilter(true);
		m_pA2DSpritePetRadarMask->SetScaleX(GetGameUIMan()->GetWindowScale());
		m_pA2DSpritePetRadarMask->SetScaleY(GetGameUIMan()->GetWindowScale());
		m_pA2DSpritePetRadarMask->DrawToBack(int(p->X + ptImg.x), int(p->Y + ptImg.y));
	}
	return true;
}