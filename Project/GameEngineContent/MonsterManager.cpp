#include "PrecompileHeader.h"
#include "MonsterManager.h"

#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineGUI.h>

#include "CarleonRecruit.h"
#include "CarleonArcher.h"
#include "CarleonManAtArms.h"
#include "Ent.h"
#include "BlossomEnt.h"
#include "GiantEnt.h"
#include "RootEnt.h"
#include "FlameWizard.h"
#include "GlacialWizard.h"

std::shared_ptr<BaseMonster> MonsterSpawnMetaData::MonsterSpawn(GameEngineLevel* _SpawnLevel, GameEngineTransform* _Parent)
{
	std::shared_ptr<BaseMonster> NewMonster = nullptr;

	switch (Index)
	{
	case 0: // 칼레온 신병
	{
		NewMonster = _SpawnLevel->CreateActor<CarleonRecruit>();
		break;
	}
	case 1: // 칼레온 궁수
	{
		NewMonster = _SpawnLevel->CreateActor<CarleonArcher>();
		break;
	}
	case 106: // 칼레온 중보병
	{
		NewMonster = _SpawnLevel->CreateActor<CarleonManAtArms>();
		break;
	}
	case 100: // 엔트
	{
		NewMonster = _SpawnLevel->CreateActor<Ent>();
		break;
	}
	case 102: // 뿌리 엔트
	{
		NewMonster = _SpawnLevel->CreateActor<RootEnt>();
		break;
	}
	case 103: // 숲지기 (자이언트 엔트)
	{
		NewMonster = _SpawnLevel->CreateActor<GiantEnt>();
		break;
	}
	case 101: // 꽃 엔트
	{
		NewMonster = _SpawnLevel->CreateActor<BlossomEnt>();
		break;
	}
	case 108: // 화염 마법사
	{
		NewMonster = _SpawnLevel->CreateActor<FlameWizard>();
		break;
	}
	case 109: // 빙결 마법사
	{
		NewMonster = _SpawnLevel->CreateActor<GlacialWizard>();
		break;
	}
	default:
		break;
	}

	if (nullptr == NewMonster)
	{
		MsgAssert_Rtti<MonsterSpawnMetaData>("아직 존재하지 않은 몬스터를 소환하려 했습니다");
		return nullptr;
	}

	NewMonster->GetTransform()->SetParent(_Parent);
	NewMonster->GetTransform()->SetLocalPosition(SpawnPos);

	return NewMonster;
}

void MonsterSpawnMetaData::SaveBin(GameEngineSerializer& _SaveSerializer) const
{
	_SaveSerializer.Write(&Index, sizeof(size_t));
	_SaveSerializer.Write(&SpawnPos, sizeof(float4));
}

void MonsterSpawnMetaData::LoadBin(GameEngineSerializer& _LoadSerializer)
{
	_LoadSerializer.Read(&Index, sizeof(size_t));
	_LoadSerializer.Read(&SpawnPos, sizeof(float4));
}

void MonsterSpawnMetaData::ShowGUI()
{
	// Todo : 몬스터 인덱스 변경 기능
	ImGui::Text(("Monster Index : " + std::to_string(Index)).data());

	float InputSpawnPos[4] = { SpawnPos.x, SpawnPos.y, SpawnPos.z, SpawnPos.w };

	ImGui::DragFloat4("Spawn Postion", InputSpawnPos);

	SpawnPos.x = InputSpawnPos[0];
	SpawnPos.y = InputSpawnPos[1];
	SpawnPos.z = InputSpawnPos[2];
	SpawnPos.w = InputSpawnPos[3];
}

void MonsterGroupMetaData::WaveSpawn(GameEngineLevel* _SpawnLevel, GameEngineTransform* _Parent, std::vector<std::shared_ptr<BaseMonster>>& _WaveGroup)
{
	if (true == IsSpawnEnd)
	{
		MsgAssert_Rtti<MonsterGroupMetaData>(" - 몬스터 웨이브를 리셋하지 않고 소환하려 했습니다");
		return;
	}

	IsSpawnEnd = true;

	for (size_t i = 0; i < SpawnMonsters.size(); i++)
	{
		_WaveGroup.push_back(SpawnMonsters[i].MonsterSpawn(_SpawnLevel, _Parent));
	}
}

void MonsterGroupMetaData::SaveBin(GameEngineSerializer& _SaveSerializer) const
{
	_SaveSerializer.Write(&WaveNumber, sizeof(UINT));
	_SaveSerializer.Write(&IsCollision, sizeof(bool));
	_SaveSerializer.Write(&ColCenter, sizeof(float4));
	_SaveSerializer.Write(&ColScale, sizeof(float4));

	int SpawnMonsterCount = static_cast<int>(SpawnMonsters.size());

	_SaveSerializer.Write(&SpawnMonsterCount, sizeof(int));

	for (size_t i = 0; i < SpawnMonsters.size(); i++)
	{
		SpawnMonsters[i].SaveBin(_SaveSerializer);
	}
}

void MonsterGroupMetaData::LoadBin(GameEngineSerializer& _LoadSerializer)
{
	IsSpawnEnd = false;

	_LoadSerializer.Read(&WaveNumber, sizeof(UINT));
	_LoadSerializer.Read(&IsCollision, sizeof(bool));
	_LoadSerializer.Read(&ColCenter, sizeof(float4));
	_LoadSerializer.Read(&ColScale, sizeof(float4));

	int SpawnMonsterCount = 0;

	_LoadSerializer.Read(&SpawnMonsterCount, sizeof(int));

	SpawnMonsters.resize(SpawnMonsterCount);

	for (size_t i = 0; i < SpawnMonsters.size(); i++)
	{
		SpawnMonsters[i].LoadBin(_LoadSerializer);
	}
}

void MonsterGroupMetaData::ShowGUI()
{
	ImGui::Checkbox("Is Collision", &IsCollision);

	if (true == IsCollision)
	{
		float InputColCenter[4] = { ColCenter.x, ColCenter.y, ColCenter.z, ColCenter.w };

		ImGui::DragFloat4("Collision Center", InputColCenter);

		ColCenter.x = InputColCenter[0];
		ColCenter.y = InputColCenter[1];
		ColCenter.z = InputColCenter[2];
		ColCenter.w = InputColCenter[3];

		float InputColScale[4] = { ColScale.x, ColScale.y, ColScale.z, ColScale.w };

		ImGui::DragFloat4("Collision Scale", InputColScale);

		ColScale.x = InputColScale[0];
		ColScale.y = InputColScale[1];
		ColScale.z = InputColScale[2];
		ColScale.w = InputColScale[3];
	}

	if (ImGui::BeginListBox("Monster ListBox"))
	{
		for (int n = 0; n < SpawnMonsters.size(); n++)
		{
			const bool is_selected = (GUI_SelectMonster == n);

			if (ImGui::Selectable((std::string("Group : ") + std::to_string(n)).data(), is_selected))
			{
				GUI_SelectMonster = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	if (GUI_SelectMonster >= SpawnMonsters.size())
	{
		return;
	}

	SpawnMonsters[GUI_SelectMonster].ShowGUI();
}

MonsterManager::MonsterManager()
{
}

MonsterManager::~MonsterManager()
{
}

void MonsterManager::SaveBin(GameEngineSerializer& _SaveSerializer) const
{
	int WaveSize = static_cast<int>(WaveDatas.size());

	_SaveSerializer.Write(&WaveSize, sizeof(int));

	for (size_t i = 0; i < WaveDatas.size(); i++)
	{
		int GroupSize = static_cast<int>(WaveDatas[i].size());

		_SaveSerializer.Write(&GroupSize, sizeof(int));

		for (size_t j = 0; j < WaveDatas[i].size(); j++)
		{
			WaveDatas[i][j].SaveBin(_SaveSerializer);
		}
	}
}

void MonsterManager::LoadBin(GameEngineSerializer& _LoadSerializer)
{
	int WaveSize = 0;
	_LoadSerializer.Read(&WaveSize, sizeof(int));

	MonsterActors.resize(WaveSize);
	WaveDatas.resize(WaveSize);

	for (size_t i = 0; i < WaveDatas.size(); i++)
	{
		int GroupSize = 0;
		_LoadSerializer.Read(&GroupSize, sizeof(int));

		WaveDatas[i].resize(GroupSize);

		for (size_t j = 0; j < WaveDatas[i].size(); j++)
		{
			WaveDatas[i][j].LoadBin(_LoadSerializer);
		}
	}
}

void MonsterManager::ShowGUI()
{
	ImGui::Text("Wave");

	ImGui::Spacing();
	ImGui::Text(("WaveCount : " + std::to_string(WaveDatas.size())).data());
	ImGui::InputInt("WaveNumber", &GUI_SelectWave);

	static int InsertWaveIndex = 0;

	if (true == ImGui::Button("Insert Wave", ImVec2(100, 24)))
	{
		InsertWave(InsertWaveIndex);
	}

	ImGui::SameLine();

	ImGui::InputInt("Insert Wave Index", &InsertWaveIndex);

	static int RemoveWaveIndex = 0;

	if (true == ImGui::Button("Remove Wave", ImVec2(100, 24)))
	{
		RemoveWave(RemoveWaveIndex);
	}

	ImGui::SameLine();

	ImGui::InputInt("Remove Wave Index", &RemoveWaveIndex);

	if (0 > GUI_SelectWave)
	{
		GUI_SelectWave = 0;
		return;
	}

	if (WaveDatas.size() <= GUI_SelectWave)
	{
		if (0 != WaveDatas.size())
		{
			GUI_SelectWave = static_cast<int>(WaveDatas.size() - 1);
		}
		else
		{
			GUI_SelectWave = 0;
		}

		return;
	}

	ImGui::Spacing();
	ImGui::Text("Group");
	ImGui::Spacing();
	ImGui::Text(("GroupCount : " + std::to_string(WaveDatas[GUI_SelectWave].size())).data());

	static int InsertGroupIndex = 0;

	if (true == ImGui::Button("Insert Group", ImVec2(100, 24)))
	{
		InsertGroup(InsertGroupIndex);
	}

	ImGui::SameLine();

	ImGui::InputInt("Insert Group Index", &InsertGroupIndex);

	static int RemoveGroupIndex = 0;

	if (true == ImGui::Button("Remove Group", ImVec2(100, 24)))
	{
		RemoveGroup(RemoveGroupIndex);
	}

	ImGui::SameLine();

	ImGui::InputInt("Remove Group Index", &RemoveGroupIndex);

	if (0 == WaveDatas[GUI_SelectWave].size())
	{
		return;
	}


	if (ImGui::BeginListBox("Group ListBox"))
	{
		for (int n = 0; n < WaveDatas[GUI_SelectWave].size(); n++)
		{
			const bool is_selected = (GUI_SelectGroup == n);

			if (ImGui::Selectable((std::string("Group : ") + std::to_string(n)).data(), is_selected))
			{
				GUI_SelectGroup = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	WaveDatas[GUI_SelectWave][GUI_SelectGroup].ShowGUI();
}

void MonsterManager::Start()
{
	WaveCollision = CreateComponent<GameEngineCollision>((int)CollisionOrder::Unknown);
	WaveCollision->GetTransform()->SetWorldRotation(float4::Zero);
}

void MonsterManager::Update(float _DeltaTime)
{
	if (true == IsMapTool)
	{
		return;
	}

	std::vector<std::vector<std::shared_ptr<BaseMonster>>>::iterator WaveLoopIter = MonsterActors.begin();
	std::vector<std::vector<std::shared_ptr<BaseMonster>>>::iterator WaveEndIter = MonsterActors.end();

	// 죽은 몬스터 릴리즈 체크
	while (WaveLoopIter != WaveEndIter)
	{
		std::vector<std::shared_ptr<BaseMonster>>::iterator MonsterLoopIter = WaveLoopIter->begin();
		std::vector<std::shared_ptr<BaseMonster>>::iterator MonsterEndIter = WaveLoopIter->end();

		while (MonsterLoopIter != MonsterEndIter)
		{
			std::shared_ptr<BaseMonster> MonsterPtr = (*MonsterLoopIter);

			if (true == MonsterPtr->IsDeath())
			{
				(*MonsterLoopIter) = nullptr;
				MonsterLoopIter = WaveLoopIter->erase(MonsterLoopIter);
			}
			else
			{
				++MonsterLoopIter;
			}
		}

		++MonsterLoopIter;
	}

	// 다음 웨이브 체크
	if (CurWave < WaveDatas.size())
	{
		std::vector<MonsterGroupMetaData>& SpawnRef = WaveDatas[CurWave];

		bool WaveEndCheck = true;

		for (size_t i = 0; i < SpawnRef.size(); i++)
		{
			if (false == SpawnRef[i].IsSpawnEnd)
			{
				WaveEndCheck = false;
			}
		}

		if (0 != MonsterActors[CurWave].size())
		{
			WaveEndCheck = false;
		}

		if (true == WaveEndCheck)
		{
			++CurWave;
		}
	}

	// 몬스터 스폰조건 체크
	if (CurWave < WaveDatas.size())
	{
		std::vector<MonsterGroupMetaData>& SpawnRef = WaveDatas[CurWave];

		for (size_t i = 0; i < SpawnRef.size(); i++)
		{
			if (true == SpawnRef[CurWave].IsSpawnEnd)
			{
				continue;
			}

			if (true == SpawnRef[CurWave].IsCollision)
			{
				GameEngineTransform* ColTrans = WaveCollision->GetTransform();

				ColTrans->SetWorldPosition(SpawnRef[CurWave].ColCenter);
				ColTrans->SetWorldScale(SpawnRef[CurWave].ColScale);

				if (nullptr != WaveCollision->Collision((int)CollisionOrder::Player, ColType::AABBBOX2D, ColType::AABBBOX2D))
				{
					SpawnRef[CurWave].WaveSpawn(GetLevel(), GetTransform(), MonsterActors[CurWave]);
				}
			}
			else
			{
				SpawnRef[CurWave].WaveSpawn(GetLevel(), GetTransform(), MonsterActors[CurWave]);
			}
		}
	}
}

void MonsterManager::InsertWave(int _Index)
{
	if (0 > _Index)
	{
		_Index = 0;
	}

	if (WaveDatas.size() < _Index)
	{
		WaveDatas.push_back(std::vector<MonsterGroupMetaData>());
	}
	else
	{
		WaveDatas.insert(WaveDatas.begin() + _Index, std::vector<MonsterGroupMetaData>());
	}
}

void MonsterManager::RemoveWave(int _Index)
{
	if (0 == WaveDatas.size())
	{
		return;
	}

	if (0 > _Index)
	{
		_Index = 0;
	}

	if (WaveDatas.size() <= _Index)
	{
		_Index = static_cast<int>(WaveDatas.size() - 1);
	}

	WaveDatas.erase(WaveDatas.begin() + _Index);
}

void MonsterManager::InsertGroup(int _Index)
{
	if (0 > _Index)
	{
		_Index = 0;
	}

	if (WaveDatas[GUI_SelectWave].size() < _Index)
	{
		WaveDatas[GUI_SelectWave].push_back(MonsterGroupMetaData());
	}
	else
	{
		WaveDatas[GUI_SelectWave].insert(WaveDatas[GUI_SelectWave].begin() + _Index, MonsterGroupMetaData());
	}

}

void MonsterManager::RemoveGroup(int _Index)
{
	if (0 == WaveDatas[GUI_SelectWave].size())
	{
		return;
	}

	if (0 > _Index)
	{
		_Index = 0;
	}

	if (WaveDatas[GUI_SelectWave].size() <= _Index)
	{
		_Index = static_cast<int>(WaveDatas[GUI_SelectWave].size() - 1);
	}

	WaveDatas[GUI_SelectWave].erase(WaveDatas[GUI_SelectWave].begin() + _Index);

}
