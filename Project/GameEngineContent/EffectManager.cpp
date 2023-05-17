#include "PrecompileHeader.h"
#include "EffectManager.h"

#include <GameEngineCore/GameEngineLevel.h>

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
		MsgAssert_Rtti<EffectManager>(" - ���� �̸��� ����Ʈ�� �ߺ��ؼ� �����߽��ϴ�");
		return;
	}

	EffectMetaDatas[UpperName] = _MetaData;
}

std::shared_ptr<EffectActor> EffectManager::PlayEffect(const EffectParameter& _Parameter)
{
	if (nullptr == CurLevel)
	{
		MsgAssert_Rtti<EffectManager>(" - EffectManager�� ���� �����Ͱ� �������� �ʾҽ��ϴ�");
		return nullptr;
	}

	std::string UpperName = GameEngineString::ToUpper(_Parameter.EffectName);
	std::map<std::string, EffectMetaData>::iterator FindIter = EffectMetaDatas.find(UpperName);

	if (EffectMetaDatas.end() == FindIter)
	{
		MsgAssert_Rtti<EffectManager>(" - �������� ���� ����Ʈ�� ����Ϸ� �߽��ϴ�");
		return nullptr;
	}

	std::shared_ptr<EffectActor> NewEffectActor = CurLevel->CreateActor<EffectActor>();
	NewEffectActor->Init(FindIter->second, _Parameter.Triger, _Parameter.Time);
	NewEffectActor->GetTransform()->SetLocalPosition(_Parameter.Postion);

	if (true == _Parameter.FlipX)
	{
		NewEffectActor->GetTransform()->SetLocalNegativeScaleX();
	}

	return NewEffectActor;
}