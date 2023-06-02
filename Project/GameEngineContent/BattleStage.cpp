#include "PrecompileHeader.h"
#include "BattleStage.h"

#include <GameEngineCore/GameEngineLevel.h>

#include "MultiBackground.h"
#include "ObjectManager.h"
#include "Tilemap.h"
#include "GameEventManager.h"
#include "ParticleManager.h"
#include "MonsterManager.h"

#include "BattleLevel.h"

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
	MonsterManagerPtr->LoadBin(_LoadSerializer);
	
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
	ObjectManagerPtr->BehaviorObjectPlay();
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

	MonsterManagerPtr = LevelPtr->CreateActor<MonsterManager>();
	MonsterManagerPtr->GetTransform()->SetParent(GetTransform());
}

void BattleStage::Update(float _DeltaTime)
{
	if (MonsterManagerPtr->IsSpawnEnd())
	{
		EventManagerPtr->DoorActive();
	}
	else
	{
		EventManagerPtr->DoorDisable();
	}
}
