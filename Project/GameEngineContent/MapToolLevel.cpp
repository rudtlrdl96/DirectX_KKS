#include "PrecompileHeader.h"
#include "MapToolLevel.h"
#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEngineCore/GameEngineCamera.h>
#include <GameEngineCore/imgui.h>
#include <GameEngineCore/GameEngineGUI.h>
#include <GameEngineCore/GameEngineSpriteRenderer.h>


#include "Tilemap.h"
#include "ObjectManager.h"
#include "GameEventManager.h"
#include "ParticleManager.h"
#include "MonsterManager.h"
#include "NPCManager.h"

#include "TilemapPallet.h"
#include "TilemapHoverRenderActor.h"
#include "TilemapOutlineRenderActor.h"
#include "PlayerSpawnPointRenderer.h"

#include "GameEngineActorGUI.h"
#include "MapToolGUI.h"

MapToolLevel::MapToolLevel()
{
}

MapToolLevel::~MapToolLevel()
{
}

void MapToolLevel::Start()
{
	ContentLevel::Start();

	if (false == GameEngineInput::IsKey("ToolActive"))
	{
		GameEngineInput::CreateKey("ToolActive", VK_LBUTTON);
		GameEngineInput::CreateKey("ToolDisable", VK_RBUTTON);

		GameEngineInput::CreateKey("CameraMoveUp", 'W');
		GameEngineInput::CreateKey("CameraMoveDown", 'S');
		GameEngineInput::CreateKey("CameraMoveLeft", 'A');
		GameEngineInput::CreateKey("CameraMoveRight", 'D');

		GameEngineInput::CreateKey("CameraMoveBoost", VK_LSHIFT);
	}

	TilemapOutLinePtr = CreateActor<TilemapOutlineRenderActor>();
	TilemapPtr = CreateActor<Tilemap>();

	TilemapPtr->SetDepth(1);
	TilemapPtr->ResizeTilemap(20, 20);
	ObjectMgrPtr = CreateActor<ObjectManager>();
	ObjectMgrPtr->PlatformDebugOn();

	EventMgrPtr = CreateActor<GameEventManager>();

	ParticleMgrPtr = CreateActor<ParticleManager>();

	MonsterMgrPtr = CreateActor<MonsterManager>();
	MonsterMgrPtr->SetMapToolManager();

	NPCMgrPtr = CreateActor<NPCManager>();

	TilePalletPtr = CreateActor<TilemapPallet>();
	TilePalletPtr->SetPencleIndex(1000);

	MapToolGuiPtr = GameEngineGUI::FindGUIWindowConvert<MapToolGUI>("MapToolGUI");
	
	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - 맵툴 Gui가 생성되지 않았습니다.");
	}

	MapToolGuiPtr->Pushback_ObjectManagerCallback(std::bind(&ObjectManager::ShowGUI, ObjectMgrPtr));
	MapToolGuiPtr->Pushback_TilemapCallback(std::bind(&Tilemap::ShowGUI, TilemapPtr));
	MapToolGuiPtr->Pushback_EventManagerCallback(std::bind(&GameEventManager::ShowGUI, EventMgrPtr));
	MapToolGuiPtr->Pushback_ParticleManagerCallback(std::bind(& ParticleManager::ShowGUI, ParticleMgrPtr));
	MapToolGuiPtr->Pushback_MonsterManagerCallback(std::bind(& MonsterManager::ShowGUI, MonsterMgrPtr));
	MapToolGuiPtr->Pushback_NPCManagerCallback(std::bind(& NPCManager::ShowGUI, NPCMgrPtr));

	ActorGUIPtr = GameEngineGUI::FindGUIWindowConvert<GameEngineActorGUI>("GameEngineActorGUI");

	if (nullptr == ActorGUIPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui가 생성되지 않았습니다.");
	}

	TilemapHoverPtr = CreateActor<TilemapHoverRenderActor>();
	TilemapOutLinePtr->SetSize(TilemapPtr->GetSize() * ContentConst::TileSize);

	PlayerSpawnRender = CreateActor<PlayerSpawnPointRenderer>();

}

void MapToolLevel::Update(float _DeltaTime)
{
	ContentLevel::Update(_DeltaTime);

	MapToolType = MapToolGuiPtr->GetMapToolState();

	TilePalletPtr->SetActiveCursor(false);
	TilemapHoverPtr->HoverOff();

	float4 SpawnPos = EventMgrPtr->GetSpawnPoint();
	PlayerSpawnRender->GetTransform()->SetWorldPosition(SpawnPos);

	switch (MapToolType)
	{
	case MapToolState::Tilemap:
	{
		Update_Tilemap(_DeltaTime);
		break;
	}
	case MapToolState::Object:
	{
		Update_Object(_DeltaTime);		
		break;
	}

	case MapToolState::BObject:
	{
		Update_BObject(_DeltaTime);
		break;
	}
	case MapToolState::Platform:
	{
		Update_Platfrom(_DeltaTime);	
		break;
	}
	case MapToolState::Event:
		Update_Event(_DeltaTime);
		break;
	case MapToolState::Particle:
		Update_Particle(_DeltaTime);
		break;
	case MapToolState::Light:
		break;
	case MapToolState::Monster:
		Update_Monster(_DeltaTime);
		break;
	case MapToolState::NPC:
		Update_NPC(_DeltaTime);
		break;
	default:
		break;
	}

	if (true == MapToolGuiPtr->CheckSaveTrigger())
	{
		Save();
	}

	if (true == MapToolGuiPtr->CheckLoadTrigger())
	{
		Load();
	}


	CameraMoveFunction(_DeltaTime);
}

void MapToolLevel::LevelChangeStart()
{
	ContentLevel::LevelChangeStart();

	EffectManager::SetLevel(DynamicThis<GameEngineLevel>());

	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - MapTool Gui를 찾을 수 없습니다");
		return;
	}

	MapToolGuiPtr->On();

	if (nullptr == ActorGUIPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui를 찾을 수 없습니다");
		return;
	}

	ActorGUIPtr->On();
}

void MapToolLevel::LevelChangeEnd()
{
	ContentLevel::LevelChangeEnd();

	EffectManager::SetLevel(nullptr);

	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - MapTool Gui를 찾을 수 없습니다");
		return;
	}

	MapToolGuiPtr->Off();

	if (nullptr == ActorGUIPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui를 찾을 수 없습니다");
		return;
	}

	ActorGUIPtr->SetTarget(nullptr);
	ActorGUIPtr->Off();
}

void MapToolLevel::Save()
{
	std::string Path = ContentFunc::GetSaveFilePath();

	if ("" == Path)
	{
		return;
	}

	GameEngineSerializer SaveSerializer;
	SaveSerializer.BufferResize(131072);

	TilemapPtr->SaveBin(SaveSerializer);
	ObjectMgrPtr->SaveBin(SaveSerializer);
	EventMgrPtr->SaveBin(SaveSerializer);
	ParticleMgrPtr->SaveBin(SaveSerializer);
	MonsterMgrPtr->SaveBin(SaveSerializer);
	NPCMgrPtr->SaveBin(SaveSerializer);

	GameEngineFile SaveFile = GameEngineFile(Path);
	SaveFile.SaveBin(SaveSerializer);
}

void MapToolLevel::Load()
{
	std::string Path = ContentFunc::GetOpenFilePath();

	if ("" == Path)
	{
		return;
	}

	GameEngineFile LoadFile = GameEngineFile(Path);

	GameEngineSerializer LoadSerializer;
	LoadSerializer.BufferResize(131072);
	LoadFile.LoadBin(LoadSerializer);

	TilemapPtr->LoadBin(LoadSerializer);
	ObjectMgrPtr->LoadBin(LoadSerializer);
	EventMgrPtr->LoadBin(LoadSerializer);
	ParticleMgrPtr->LoadBin(LoadSerializer);
	MonsterMgrPtr->LoadBin(LoadSerializer);
	NPCMgrPtr->LoadBin(LoadSerializer);

	TilemapOutLinePtr->SetSize(TilemapPtr->GetSize() * ContentConst::TileSize);
}


void MapToolLevel::CameraMoveFunction(float _DeltaTime)
{

	float CurFrameCameraSpeed = CameraSpeed * _DeltaTime;

	if (GameEngineInput::IsPress("CameraMoveBoost"))
	{
		CurFrameCameraSpeed *= 2.0f;
	}

	if (GameEngineInput::IsPress("CameraMoveUp"))
	{
		GetCamCtrl().AddCameraPos(float4::Up * CurFrameCameraSpeed);
	}
	else if (GameEngineInput::IsPress("CameraMoveDown"))
	{
		GetCamCtrl().AddCameraPos(float4::Down * CurFrameCameraSpeed);
	}
	if (GameEngineInput::IsPress("CameraMoveLeft"))
	{
		GetCamCtrl().AddCameraPos(float4::Left * CurFrameCameraSpeed);
	}
	else if (GameEngineInput::IsPress("CameraMoveRight"))
	{
		GetCamCtrl().AddCameraPos(float4::Right * CurFrameCameraSpeed);
	}
}


void MapToolLevel::Update_Tilemap(float _DeltaTime)
{
	ActorGUIPtr->SetTarget(nullptr);
	TilePalletPtr->SetActiveCursor(true);
	TilePalletPtr->On();

	TilemapOutLinePtr->SetSize(TilemapPtr->GetSize() * ContentConst::TileSize);

	float4 WorldMousePos = GetMousePos();
	WorldMousePos.z = 0.0f;

	int2 MouseIndex = TilemapPtr->GetTileIndex(WorldMousePos);

	UINT CastingIndexX = static_cast<UINT>(MouseIndex.x);
	UINT CastingIndexY = static_cast<UINT>(MouseIndex.y);

	TilePalletPtr->GetTransform()->SetWorldPosition(WorldMousePos);

	if (MouseIndex.x >= 0 && MouseIndex.y >= 0 && false == TilemapPtr->IsOver(MouseIndex.x, MouseIndex.y))
	{
		float4 TileIndexPos = TilemapPtr->GetTilePos(CastingIndexX, CastingIndexY);
		TileIndexPos.z = -50;

		TilemapHoverPtr->HoverOn();
		TilemapHoverPtr->GetTransform()->SetWorldPosition(TileIndexPos);
	}
	else
	{
		return;
	}

	if (false == ImGui::GetIO().WantCaptureMouse &&
		(true == GameEngineInput::IsPress("ToolActive") ||
			true == GameEngineInput::IsPress("ToolDisable")))
	{
		UINT TilemapCurDepthCount = TilemapPtr->GetCurDepth();

		if (true == GameEngineInput::IsPress("ToolActive"))
		{
			TilemapPtr->ChangeData(TilemapCurDepthCount, MouseIndex.x, MouseIndex.y, TilePalletPtr->GetPencleIndex());
		}
		else if (true == GameEngineInput::IsPress("ToolDisable"))
		{
			TilemapPtr->ChangeData(TilemapCurDepthCount, MouseIndex.x, MouseIndex.y, 0);
		}
	}
}

void MapToolLevel::Update_Object(float _DeltaTime)
{
	ObjectMgrPtr->SetGuiType(ObjectManager::GuiType::Object);

	std::shared_ptr<BaseContentActor> GetActorPtr = ObjectMgrPtr->GetSelectObject();

	if (nullptr == GetActorPtr)
	{
		ActorGUIPtr->SetTarget(nullptr);
	}
	else
	{
		ActorGUIPtr->SetTarget(GetActorPtr->GetTransform(), { std::bind(&BaseContentActor::ShowGUI, GetActorPtr) });
	}


	if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsDown("ToolActive"))
	{
		float4 TestMousePos = GetMousePos();
		TestMousePos.z = 0;

		ObjectMetaData NewObjectMetaData = MapToolGuiPtr->GetSelectObject();
		NewObjectMetaData.Pos = TestMousePos;

		ObjectMgrPtr->CreateObject(NewObjectMetaData); 
		ObjectMgrPtr->SelectLastObject();
	}
}

void MapToolLevel::Update_BObject(float _DeltaTime)
{
	ObjectMgrPtr->SetGuiType(ObjectManager::GuiType::BehaviorObject);

	if (nullptr == GetFocus())
	{
		return;
	}

	if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsDown("ToolActive"))
	{
		float4 TestMousePos = GetMousePos();
		TestMousePos.z = 0;

		BehaviorObjectMetaData NewBehaviorObjectMetaData = MapToolGuiPtr->GetSelectBehaviorObject();
		NewBehaviorObjectMetaData.Position = TestMousePos;

		ObjectMgrPtr->CreateBehaviorObject(NewBehaviorObjectMetaData);
		ObjectMgrPtr->SelectLastBehaviorObject();
	}

}

void MapToolLevel::Update_Platfrom(float _DeltaTime)
{
	ObjectMgrPtr->SetGuiType(ObjectManager::GuiType::Platform);

	static float4 PlatformStartPos = float4::Zero;
	static float4 PlatformEndPos = float4::Zero;

	if (false == ImGui::GetIO().WantCaptureMouse)
	{
		if (true == GameEngineInput::IsDown("ToolActive"))
		{
			PlatformStartPos = GetMousePos();
		}
		else if (true == GameEngineInput::IsUp("ToolActive"))
		{
			PlatformEndPos = GetMousePos();
			MapPlatform::PlatformMetaData NewMetaData;

			NewMetaData.Type = MapToolGuiPtr->GetPlatformType();
			NewMetaData.Pos = (PlatformStartPos + PlatformEndPos) * 0.5f;
			NewMetaData.Pos.z = -100;
			NewMetaData.Rot = float4::Zero;
			NewMetaData.Scale = float4(
				std::abs(PlatformStartPos.x - PlatformEndPos.x),
				std::abs(PlatformStartPos.y - PlatformEndPos.y),
				1,
				1);

			ObjectMgrPtr->CreatePaltform(NewMetaData);
			ObjectMgrPtr->SelectLastPlatform();
		}	
	}
}

void MapToolLevel::Update_Event(float _DeltaTime)
{

}

void MapToolLevel::Update_Particle(float _DeltaTime)
{
	static float4 ParticleStartPos = float4::Zero;
	static float4 ParticleEndPos = float4::Zero;

	if (false == ImGui::GetIO().WantCaptureMouse)
	{
		if (true == GameEngineInput::IsDown("ToolActive"))
		{
			ParticleStartPos = GetMousePos();
		}
		else if (true == GameEngineInput::IsUp("ToolActive"))
		{
			ParticleEndPos = GetMousePos();

			ParticleAreaMetaData NewMetaData = MapToolGuiPtr->GetNewParticleMetaData();

			NewMetaData.Center = (ParticleStartPos + ParticleEndPos) * 0.5f;
			NewMetaData.Center.z = -100;
			NewMetaData.Size = float4(
				std::abs(ParticleStartPos.x - ParticleEndPos.x),
				std::abs(ParticleStartPos.y - ParticleEndPos.y),
				1,
				1);

			ParticleMgrPtr->CreateMapParticleArea(NewMetaData);
		}
	}
}

void MapToolLevel::Update_Monster(float _DeltaTime)
{		
	if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsDown("ToolActive"))
	{
		float4 WorldMousePos = GetMousePos();
		WorldMousePos.z = GameEngineRandom::MainRandom.RandomFloat(-20.0f, -10.0f);
	
		const MonsterData& CreateMonsterData = MapToolGuiPtr->GetMonsterData();
			
		if ("" == CreateMonsterData.Name)
		{
			return;
		}

		MonsterMgrPtr->AddMonster(CreateMonsterData.Index, WorldMousePos);
	}
}

void MapToolLevel::Update_NPC(float _DeltaTime)
{
	if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsDown("ToolActive"))
	{
		float4 WorldMousePos = GetMousePos();
		WorldMousePos.z = GameEngineRandom::MainRandom.RandomFloat(-20.0f, -10.0f);

		NPCMetaData CreateMonsterData = MapToolGuiPtr->GetNPCData();

		if ("" == CreateMonsterData.Name)
		{
			return;
		}

		CreateMonsterData.CenterPos = WorldMousePos;

		NPCMgrPtr->CreateNPC(CreateMonsterData);
	}
}
