#include "PrecompileHeader.h"
#include "MapToolLevel.h"
#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEngineCore/GameEngineCamera.h>
#include <GameEngineCore/imgui.h>
#include <GameEngineCore/GameEngineGUI.h>
#include <GameEngineCore/GameEngineSpriteRenderer.h>


#include "Tilemap.h"
#include "ObjectManager.h"
#include "TilemapPallet.h"
#include "TilemapHoverRenderActor.h"

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

		GameEngineInput::CreateKey("TempSizeUp", VK_OEM_PLUS);
		GameEngineInput::CreateKey("TempSizeDown", VK_OEM_MINUS);

		GameEngineInput::CreateKey("CameraMoveUp", 'W');
		GameEngineInput::CreateKey("CameraMoveDown", 'S');
		GameEngineInput::CreateKey("CameraMoveLeft", 'A');
		GameEngineInput::CreateKey("CameraMoveRight", 'D');

		GameEngineInput::CreateKey("CameraMoveBoost", VK_LSHIFT);
	}

	TilemapPtr = CreateActor<Tilemap>();
	TilemapPtr->ResizeTilemap(20, 20, 0);

	ObjectMgr = CreateActor<ObjectManager>();

	TilemapPalletPtr = CreateActor<TilemapPallet>();
	TilemapPalletPtr->SetPencleIndex(1000);

	MapToolGuiPtr = GameEngineGUI::FindGUIWindowConvert<MapToolGUI>("MapToolGUI");
	
	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - 맵툴 Gui가 생성되지 않았습니다.");
	}

	ActorGUI = GameEngineGUI::FindGUIWindowConvert<GameEngineActorGUI>("GameEngineActorGUI");

	if (nullptr == ActorGUI)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui가 생성되지 않았습니다.");
	}

	TilemapHoverPtr = CreateActor<TilemapHoverRenderActor>();
}

void MapToolLevel::Update(float _DeltaTime)
{
	ContentLevel::Update(_DeltaTime);

	MapToolType = MapToolGuiPtr->GetMapToolState();

	TilemapPalletPtr->SetActiveCursor(false);
	TilemapHoverPtr->HoverOff();

	switch (MapToolType)
	{
	case MapToolLevel::MapToolState::Tilemap:
	{
		TilemapPalletPtr->SetActiveCursor(true);
		TilemapPalletPtr->On();

		float4 TestMousePos = GetMousePos();
		int2 MouseIndex = TilemapPtr->GetTileIndex(TestMousePos);

		UINT CastingIndexX = static_cast<UINT>(MouseIndex.x);
		UINT CastingIndexY = static_cast<UINT>(MouseIndex.y);


		TilemapPalletPtr->GetTransform()->SetWorldPosition(TestMousePos);

		if (MouseIndex.x >= 0 && MouseIndex.y >= 0)
		{
			TilemapHoverPtr->HoverOn();
			TilemapHoverPtr->GetTransform()->SetWorldPosition(TilemapPtr->GetTilePos(CastingIndexX, CastingIndexY));
		}
		else
		{
			break;
		}

		if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsPress("ToolActive"))
		{
			TilemapPtr->ChangeData(0, MouseIndex.x, MouseIndex.y, TilemapPalletPtr->GetPencleIndex());
		}
	}
		break;
	case MapToolLevel::MapToolState::Object:
	{
		
		if (false == ImGui::GetIO().WantCaptureMouse && true == GameEngineInput::IsDown("ToolActive"))
		{
			float4 TestMousePos = GetMousePos();
			SObject_DESC NewObjectDesc = MapToolGuiPtr->GetSelectSObject();
			NewObjectDesc.Pos = TestMousePos;
			ObjectMgr->CreateStaticObject(NewObjectDesc);
		}
	}
		break;
	case MapToolLevel::MapToolState::Light:
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

void MapToolLevel::Save()
{
	std::string Path = ContentFunc::GetSaveFilePath();

	if ("" == Path)
	{
		return;
	}

	GameEngineSerializer SaveSerializer;

	TilemapPtr->SaveBin(SaveSerializer);
	ObjectMgr->SaveBin(SaveSerializer);

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

	GameEngineSerializer SaveSerializer;
	SaveSerializer.BufferResize(30000);
	LoadFile.LoadBin(SaveSerializer);

	TilemapPtr->LoadBin(SaveSerializer);
	ObjectMgr->LoadBin(SaveSerializer);
}

void MapToolLevel::LevelChangeStart()
{
	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - MapTool Gui를 찾을 수 없습니다");
		return;
	}

	MapToolGuiPtr->On();

	if (nullptr == ActorGUI)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui를 찾을 수 없습니다");
		return;
	}

	ActorGUI->On();
}

void MapToolLevel::LevelChangeEnd()
{
	if (nullptr == MapToolGuiPtr)
	{
		MsgAssert_Rtti<MapToolLevel>(" - MapTool Gui를 찾을 수 없습니다");
		return;
	}

	MapToolGuiPtr->Off();

	if (nullptr == ActorGUI)
	{
		MsgAssert_Rtti<MapToolLevel>(" - GameEngineActor Gui를 찾을 수 없습니다");
		return;
	}

	ActorGUI->SetTarget(nullptr);
	ActorGUI->Off();
}

void MapToolLevel::CameraMoveFunction(float _DeltaTime)
{

	float CurFrameCameraSpeed = CameraSpeed * _DeltaTime;

	if (GameEngineInput::IsPress("CameraMoveBoost"))
	{
		CurFrameCameraSpeed *= 2.0f;
	}

	if (GameEngineInput::IsDown("TempSizeUp"))
	{
		TilemapPtr->ResizeTilemap(TilemapPtr->SizeX(0) + 1, TilemapPtr->SizeY(0) + 1, 0);
	}

	if (GameEngineInput::IsDown("TempSizeDown"))
	{
		TilemapPtr->ResizeTilemap(TilemapPtr->SizeX(0) - 1, TilemapPtr->SizeY(0) - 1, 0);
	}

	if (GameEngineInput::IsPress("CameraMoveUp"))
	{
		GetMainCamera()->GetTransform()->AddLocalPosition(float4::Up * CurFrameCameraSpeed);
	}
	else if (GameEngineInput::IsPress("CameraMoveDown"))
	{
		GetMainCamera()->GetTransform()->AddLocalPosition(float4::Down * CurFrameCameraSpeed);
	}
	if (GameEngineInput::IsPress("CameraMoveLeft"))
	{
		GetMainCamera()->GetTransform()->AddLocalPosition(float4::Left * CurFrameCameraSpeed);
	}
	else if (GameEngineInput::IsPress("CameraMoveRight"))
	{
		GetMainCamera()->GetTransform()->AddLocalPosition(float4::Right * CurFrameCameraSpeed);
	}
}
