#include "PrecompileHeader.h"
#include "BattleStage.h"

#include <GameEngineCore/GameEngineLevel.h>

#include "MultiBackground.h"
#include "ObjectManager.h"
#include "Tilemap.h"
#include "GameEventManager.h"
#include "ParticleManager.h"
#include "MonsterManager.h"
#include "NPCManager.h"
#include "StageRewardObject.h"

#include "BattleLevel.h"

RewardType BattleStage::NextRewardType = RewardType::None;

BattleStage::BattleStage()
{
}

BattleStage::~BattleStage()
{
}

void BattleStage::Init(GameEngineSerializer& _LoadSerializer)
{
	TilemapPtr->LoadBin(_LoadSerializer);
	ObjectManagerPtr->LoadBin(_LoadSerializer);
	EventManagerPtr->LoadBin(_LoadSerializer);
	ParticleMgrPtr->LoadBin(_LoadSerializer);
	MonsterMgrPtr->LoadBin(_LoadSerializer);
	NPCMgrPtr->LoadBin(_LoadSerializer);
	
	EventManagerPtr->SetDoorLevel(GetLevel()->DynamicThis<BattleLevel>());
}

TilemapMetaData BattleStage::GetTilemapMetaData() const
{
	return TilemapPtr->GetTilemapMetaData();
}

float4 BattleStage::GetSpawnPoint() const
{
	return EventManagerPtr->GetSpawnPoint();
}

void BattleStage::StageReset()
{
	LockCount = 0;
	ObjectManagerPtr->BehaviorObjectReset();
	NPCMgrPtr->ResetNPC();
	MonsterMgrPtr->ResetMonster();
	GetContentLevel()->CallEvent("UnlockMonsterMove");

	IsStageReward = false;
	IsRewardEnd = false;

	if (nullptr != RewardObject)
	{
		RewardObject->Death();
		RewardObject = nullptr;
	}
}

void BattleStage::StagePlay()
{
	LockCount = 0;
	NPCMgrPtr->PlayNPC();
	ObjectManagerPtr->BehaviorObjectPlay();

	IsStageReward = false;
	IsRewardEnd = false;

	if (nullptr != RewardObject)
	{
		RewardObject->Death();
		RewardObject = nullptr;
	}
}

void BattleStage::Destroy()
{
	GetContentLevel()->RemoveEvent("DoorLockPlus", GetActorCode());
	GetContentLevel()->RemoveEvent("DoorLockMinus", GetActorCode());
}

void BattleStage::Start()
{
	std::shared_ptr<GameEngineLevel> LevelPtr = GetLevel()->DynamicThis<GameEngineLevel>();

	TilemapPtr = LevelPtr->CreateActor<Tilemap>();
	TilemapPtr->GetTransform()->SetParent(GetTransform());

	ObjectManagerPtr = LevelPtr->CreateActor<ObjectManager>();
	ObjectManagerPtr->GetTransform()->SetParent(GetTransform());

	EventManagerPtr = LevelPtr->CreateActor<GameEventManager>();
	EventManagerPtr->GetTransform()->SetParent(GetTransform());

	ParticleMgrPtr = LevelPtr->CreateActor<ParticleManager>();
	ParticleMgrPtr->GetTransform()->SetParent(GetTransform());

	MonsterMgrPtr = LevelPtr->CreateActor<MonsterManager>();
	MonsterMgrPtr->GetTransform()->SetParent(GetTransform());

	NPCMgrPtr = LevelPtr->CreateActor<NPCManager>();
	NPCMgrPtr->GetTransform()->SetParent(GetTransform());

	GetContentLevel()->AddEvent("DoorLockPlus", GetActorCode(), [this]()
		{
			++LockCount;
		});

	GetContentLevel()->AddEvent("DoorLockMinus", GetActorCode(), [this]()
		{
			--LockCount;
		});

}

void BattleStage::Update(float _DeltaTime)
{
	if (false == IsNoneReward)
	{
		if (false == IsStageReward && MonsterMgrPtr->IsSpawnEnd())
		{
			IsStageReward = true;

			RewardObject = GetLevel()->CreateActor<StageRewardObject>();
			RewardObject->GetTransform()->SetParent(GetTransform());
			RewardObject->GetTransform()->SetLocalPosition(EventManagerPtr->GetDoorPoint());

			RewardObject->SetReward(NextRewardType);

		}

		if (true == IsStageReward && nullptr != RewardObject && RewardObject->IsRewardEnd())
		{
			IsRewardEnd = true;
		}
	}

	if ((true == IsNoneReward || true == IsRewardEnd) && MonsterMgrPtr->IsSpawnEnd() && 0 >= LockCount)
	{
		EventManagerPtr->DoorActive();
	}
	else
	{
		EventManagerPtr->DoorDisable();
	}
}
