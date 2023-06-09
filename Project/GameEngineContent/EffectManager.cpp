#include "PrecompileHeader.h"
#include "EffectManager.h"

#include <GameEngineCore/GameEngineLevel.h>

#include "EffectActor.h"
#include "EffectUI.h"

std::map<std::string, EffectMetaData> EffectManager::EffectMetaDatas;
std::shared_ptr<GameEngineLevel> EffectManager::CurLevel = nullptr;

bool EffectManager::IsCreate(const std::string_view& _EffectName)
{
	std::string UpperName = GameEngineString::ToUpper(_EffectName);

	std::map<std::string, EffectMetaData>::iterator FindIter = EffectMetaDatas.find(UpperName);

	if (EffectMetaDatas.end() != FindIter)
	{
		return true;
	}

	return false;
}

void EffectManager::CreateMetaData(const std::string_view& _EffectName, const EffectMetaData& _MetaData)
{
	std::string UpperName = GameEngineString::ToUpper(_EffectName);

	std::map<std::string, EffectMetaData>::iterator FindIter = EffectMetaDatas.find(UpperName);

	if (EffectMetaDatas.end() != FindIter)
	{
		MsgAssert_Rtti<EffectManager>(" - 같은 이름의 이펙트를 중복해서 생성했습니다");
		return;
	}

	EffectMetaDatas[UpperName] = _MetaData;
}

std::shared_ptr<EffectActor> EffectManager::PlayEffect(const EffectParameter& _Parameter)
{
	if (nullptr == CurLevel)
	{
		MsgAssert_Rtti<EffectManager>(" - EffectManager에 레벨 포인터가 설정되지 않았습니다");
		return nullptr;
	}

	std::string UpperName = GameEngineString::ToUpper(_Parameter.EffectName);
	std::map<std::string, EffectMetaData>::iterator FindIter = EffectMetaDatas.find(UpperName);

	if (EffectMetaDatas.end() == FindIter)
	{
		MsgAssert_Rtti<EffectManager>(" - 생성하지 않은 이펙트를 사용하려 했습니다");
		return nullptr;
	}

	std::shared_ptr<EffectActor> NewEffectActor = nullptr;

	if (false == _Parameter.IsUI)
	{
		NewEffectActor = CurLevel->CreateActor<EffectActor>();
	}
	else
	{
		NewEffectActor = CurLevel->CreateActor<EffectUI>();
	}

	EffectMetaData Data = FindIter->second;

	if (false == _Parameter.Color.IsZero())
	{
		Data.Color = _Parameter.Color;
	}

	Data.ScaleRatio *= _Parameter.Scale;

	float4 SpawnPos = float4(_Parameter.Position.x, _Parameter.Position.y, _Parameter.AddSetZ + GameEngineRandom::MainRandom.RandomFloat(-40.0f, -31.0f));

	NewEffectActor->Init(Data, _Parameter.Triger, _Parameter.Time, _Parameter.WaitTime, _Parameter.IsForceLoopOff, _Parameter.IsFadeDeath, _Parameter.FadeSpeed);
	NewEffectActor->GetTransform()->SetLocalPosition(SpawnPos);

	if (true == _Parameter.FlipX)
	{
		NewEffectActor->GetTransform()->SetLocalNegativeScaleX();
	}

	return NewEffectActor;
}
