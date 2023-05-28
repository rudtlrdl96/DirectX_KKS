#include "PrecompileHeader.h"
#include "ObjectManager.h"

#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/imgui.h>

ObjectManager::ObjectManager()
{
	StaticObjectActors.reserve(32);
	BrokenObjectActors.reserve(32);
	MapPlatformActors.reserve(32);
}

ObjectManager::~ObjectManager()
{
}

std::shared_ptr<StaticObject> ObjectManager::CreateStaticObject(const SObjectMetaData& _Meta)
{
	if ("" == _Meta.Name)
	{
		return nullptr;
	}

	std::shared_ptr<StaticObject> CreatePtr = GetLevel()->CreateActor<StaticObject>();
	CreatePtr->SetName(_Meta.Name + " - " + std::to_string(CreatePtr->GetActorCode()));
	CreatePtr->GetTransform()->SetParent(this->GetTransform());
	CreatePtr->Init(_Meta);
	StaticObjectActors.push_back(CreatePtr);

	return CreatePtr;
}

std::shared_ptr<MapPlatform> ObjectManager::CreatePaltform(const MapPlatform::PlatformMetaData& _MetaData)
{
	std::shared_ptr<MapPlatform> CreatePtr = GetLevel()->CreateActor<MapPlatform>();
	CreatePtr->SetName("Platform - " + std::to_string(CreatePtr->GetActorCode()));
	CreatePtr->GetTransform()->SetParent(this->GetTransform());
	CreatePtr->Init(_MetaData);
	MapPlatformActors.push_back(CreatePtr);

	if (true == IsPlatformDebug)
	{
		CreatePtr->PlatformDebugOn();
	}

	return CreatePtr;
}

void ObjectManager::SaveBin(GameEngineSerializer& _SaveSerializer) const
{
	_SaveSerializer.Write(static_cast<int>(StaticObjectActors.size()));

	for (const std::shared_ptr<StaticObject>& LoopRef : StaticObjectActors)
	{ 
		LoopRef->SaveBin(_SaveSerializer);
	}	

	_SaveSerializer.Write(static_cast<int>(BrokenObjectActors.size()));

	for (const std::shared_ptr<BrokenObject>& LoopRef : BrokenObjectActors)
	{
		//LoopRef->SaveBin(_SaveSerializer);
	}

	_SaveSerializer.Write(static_cast<int>(MapPlatformActors.size()));

	for (const std::shared_ptr<MapPlatform>& LoopRef : MapPlatformActors)
	{
		LoopRef->SaveBin(_SaveSerializer);
	}
}

void ObjectManager::LoadBin(GameEngineSerializer& _LoadSerializer)
{
	// 정적 오브젝트 불러오기
	{
		for (size_t i = 0; i < StaticObjectActors.size(); i++)
		{
			if (nullptr == StaticObjectActors[i])
			{
				continue;
			}

			StaticObjectActors[i]->Death();
			StaticObjectActors[i] = nullptr;
		}

		StaticObjectActors.clear();

		int StaticObjectCount = 0;
		_LoadSerializer.Read(StaticObjectCount);

		StaticObjectActors.reserve(StaticObjectCount);

		for (int i = 0; i < StaticObjectCount; i++)
		{
			SObjectMetaData LoadMetaData = StaticObject::LoadBin(_LoadSerializer);
			CreateStaticObject(LoadMetaData);
		}
	}

	// Broken Object 불러오기
	{
		for (size_t i = 0; i < BrokenObjectActors.size(); i++)
		{
			if (nullptr == BrokenObjectActors[i])
			{
				continue;
			}

			BrokenObjectActors[i]->Death();
			BrokenObjectActors[i] = nullptr;
		}

		BrokenObjectActors.clear();

		int BrokenObjectCount = 0;
		_LoadSerializer.Read(BrokenObjectCount);

		for (int i = 0; i < BrokenObjectCount; i++)
		{
			//BrokenObjectActors
		}
	}

	// 플랫폼 불러오기
	{
		for (size_t i = 0; i < MapPlatformActors.size(); i++)
		{
			if (nullptr == MapPlatformActors[i])
			{
				continue;
			}

			MapPlatformActors[i]->Death();
			MapPlatformActors[i] = nullptr;
		}

		MapPlatformActors.clear();

		int PlatformCount = 0;
		_LoadSerializer.Read(PlatformCount);

		for (size_t i = 0; i < PlatformCount; i++)
		{
			MapPlatform::PlatformMetaData LoadMetaData = MapPlatform::LoadBin(_LoadSerializer);
			CreatePaltform(LoadMetaData);
		}
	}
}


void ObjectManager::ShowGUI()
{
	switch (ShowGuiType)
	{
	case ObjectManager::GuiType::SObject:
		Draw_SObject_GUI();
		break;
	case ObjectManager::GuiType::BObject:
		Draw_BObject_GUI();
		break;
	case ObjectManager::GuiType::Platform:
		Draw_Platform_GUI();
		break;
	default:
		break;
	}

}

void ObjectManager::PlatformDebugOn()
{
	IsPlatformDebug = true;

	for (size_t i = 0; i < MapPlatformActors.size(); i++)
	{
		MapPlatformActors[i]->PlatformDebugOn();
	}
}

void ObjectManager::PlatformDebugOff()
{
	IsPlatformDebug = false;

	for (size_t i = 0; i < MapPlatformActors.size(); i++)
	{
		MapPlatformActors[i]->PlatformDebugOff();
	}
}

void ObjectManager::SelectLastStaticObject()
{
	CurrentStaticObjectIndex = StaticObjectActors.size() - 1;
}

void ObjectManager::SelectLastPlatform()
{
	CurrentPlatformIndex = MapPlatformActors.size() - 1;
}

void ObjectManager::Start()
{
	if (false == GameEngineInput::IsKey("PlatformDebugSwtich"))
	{
		GameEngineInput::CreateKey("PlatformDebugSwtich", VK_F1);
	}

	OutlineRender = CreateComponent<ContentSpriteRenderer>();
	OutlineRender->PipeSetting("2DTexture_Capture");
	OutlineRender->GetShaderResHelper().SetConstantBufferLink("CaptureBuffer", Buffer);
	OutlineRender->Off();
	OutlineRender->GetTransform()->SetLocalPosition(float4::Zero);

	Buffer.Color = float4::Black;
}

void ObjectManager::Update(float _DeltaTime)
{
	if (true == GameEngineInput::IsDown("PlatformDebugSwtich"))
	{
		if (true == IsPlatformDebug)
		{
			PlatformDebugOff();
		}
		else
		{
			PlatformDebugOn();
		}
	}

	EmphasizeCurrentPlatform();
}

void ObjectManager::PushAllStaticObject(const float4& _AddPos)
{
	for (size_t i = 0; i < StaticObjectActors.size(); i++)
	{
		StaticObjectActors[i]->GetTransform()->AddLocalPosition(_AddPos);
	}

}

void ObjectManager::PushAllPlatform(const float4& _AddPos)
{
	for (size_t i = 0; i < MapPlatformActors.size(); i++)
	{
		MapPlatformActors[i]->GetTransform()->AddLocalPosition(_AddPos);
		MapPlatformActors[i]->MetaData.Pos = MapPlatformActors[i]->GetTransform()->GetLocalPosition();
	}
}

void ObjectManager::Draw_SObject_GUI()
{
	if (ImGui::BeginListBox("Static Object ListBox"))
	{
		for (int n = 0; n < StaticObjectActors.size(); n++)
		{
			const bool is_selected = (CurrentStaticObjectIndex == n);

			if (ImGui::Selectable(StaticObjectActors[n]->GetName().data(), is_selected))
			{
				CurrentStaticObjectIndex = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	if (true == ImGui::Button("Copy", ImVec2(70, 25)))
	{
		if (CurrentStaticObjectIndex < 0)
		{
			return;
		}

		CreateStaticObject(StaticObjectActors[CurrentStaticObjectIndex]->GetMetaData());

		SelectLastStaticObject();
	}

	if (true == ImGui::Button("Remove", ImVec2(70, 25)))
	{
		if (CurrentStaticObjectIndex < 0)
		{
			return;
		}

		std::vector<std::shared_ptr<StaticObject>>::iterator EraseIter = StaticObjectActors.begin();

		EraseIter += CurrentStaticObjectIndex;

		(*EraseIter)->Death();
		(*EraseIter) = nullptr;
		EraseIter = StaticObjectActors.erase(EraseIter);

		if (CurrentStaticObjectIndex >= StaticObjectActors.size())
		{
			CurrentStaticObjectIndex = static_cast<int>(StaticObjectActors.size() - 1);
		}

		if (StaticObjectActors.size() <= 0)
		{
			CurrentStaticObjectIndex = -1;
		}
	}

	float ArrowButtonSize = ImGui::GetFrameHeight();

	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Up ArrowButton", ImGuiDir_Up))
	{
		PushAllStaticObject(float4(0, ContentConst::TileSize.y, 0));
	}

	if (true == ImGui::ArrowButton("StaticObject Push Left ArrowButton", ImGuiDir_Left))
	{
		PushAllStaticObject(float4(-ContentConst::TileSize.x, 0, 0));
	}

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Right ArrowButton", ImGuiDir_Right))
	{
		PushAllStaticObject(float4(ContentConst::TileSize.x, 0, 0));
	}

	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Down ArrowButton", ImGuiDir_Down))
	{
		PushAllStaticObject(float4(0, -ContentConst::TileSize.y, 0));
	}

	if (0 <= CurrentStaticObjectIndex && StaticObjectActors.size() > CurrentStaticObjectIndex)
	{
		std::shared_ptr<StaticObject> SelectObject = StaticObjectActors[CurrentStaticObjectIndex];

		std::string TexName = SelectObject->GetTexName();
		OutlineRender->SetTexture(TexName);
		OutlineRender->On();

		GameEngineTransform* OutlineTrans = OutlineRender->GetTransform();
		OutlineTrans->SetWorldPosition(SelectObject->GetTransform()->GetWorldPosition() + float4(0, 0, 0.1f));
		OutlineTrans->SetWorldScale(SelectObject->GetTexWorldScale() * 1.1f);
	}
	else
	{
		OutlineRender->Off();
	}
}

void ObjectManager::Draw_BObject_GUI()
{
}

void ObjectManager::Draw_Platform_GUI()
{
	if (CurrentPlatformIndex >= 0 && CurrentPlatformIndex < MapPlatformActors.size())
	{
		MapPlatformActors[CurrentPlatformIndex]->ShowGUI();
	}

	if (ImGui::BeginListBox("Paltform ListBox"))
	{
		for (int n = 0; n < MapPlatformActors.size(); n++)
		{
			const bool is_selected = (CurrentPlatformIndex == n);

			if (ImGui::Selectable(MapPlatformActors[n]->GetName().data(), is_selected))
			{
				CurrentPlatformIndex = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	if (true == ImGui::Button("Copy", ImVec2(70, 25)))
	{
		if (CurrentPlatformIndex < 0)
		{
			return;
		}

		CreatePaltform(MapPlatformActors[CurrentPlatformIndex]->GetMetaData());
		SelectLastPlatform();
	}

	if (true == ImGui::Button("Remove", ImVec2(70, 25)))
	{
		if (CurrentPlatformIndex < 0)
		{
			return;
		}

		std::vector<std::shared_ptr<MapPlatform>>::iterator EraseIter = MapPlatformActors.begin();

		EraseIter += CurrentPlatformIndex;

		(*EraseIter)->Death();
		(*EraseIter) = nullptr;
		EraseIter = MapPlatformActors.erase(EraseIter);

		if (CurrentPlatformIndex >= MapPlatformActors.size())
		{
			CurrentPlatformIndex = static_cast<int>(MapPlatformActors.size() - 1);
		}

		if (MapPlatformActors.size() <= 0)
		{
			CurrentPlatformIndex = -1;
		}
	}

	float ArrowButtonSize = ImGui::GetFrameHeight();

	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Up ArrowButton", ImGuiDir_Up))
	{
		PushAllPlatform(float4(0, ContentConst::TileSize.y, 0));
	}

	if (true == ImGui::ArrowButton("StaticObject Push Left ArrowButton", ImGuiDir_Left))
	{
		PushAllPlatform(float4(-ContentConst::TileSize.x, 0, 0));
	}

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Right ArrowButton", ImGuiDir_Right))
	{
		PushAllPlatform(float4(ContentConst::TileSize.x, 0, 0));
	}

	ImGui::Dummy(ImVec2(ArrowButtonSize, ArrowButtonSize));
	ImGui::SameLine();
	if (true == ImGui::ArrowButton("StaticObject Push Down ArrowButton", ImGuiDir_Down))
	{
		PushAllPlatform(float4(0, -ContentConst::TileSize.y, 0));
	}
}

void ObjectManager::EmphasizeCurrentPlatform()
{
	if (nullptr != CurrentEmphasizePlatform)
	{
		CurrentEmphasizePlatform->EmphasizeOff();
	}

	if (0 <= CurrentPlatformIndex && MapPlatformActors.size() >CurrentPlatformIndex)
	{
		CurrentEmphasizePlatform = MapPlatformActors[CurrentPlatformIndex];
		CurrentEmphasizePlatform->EmphasizeOn();
	}
}
