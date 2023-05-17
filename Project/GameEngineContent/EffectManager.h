#pragma once
#include "EffectActor.h"

class EffectParameter
{
public:
	const std::string_view& EffectName = "";
	float4 Postion = float4::Zero;
	EffectDeathTrigger Triger = EffectDeathTrigger::AnimEnd;
	float Time = 0.0f;

	bool FlipX = false;
};

class EffectManager
{
public:
	static bool IsCreate(const std::string_view& _EffectName);
	static void CreateMetaData(const std::string_view& _EffectName, const EffectMetaData& _MetaData);
	static std::shared_ptr<EffectActor> PlayEffect(const EffectParameter& _Parameter);

	static void SetLevel(std::shared_ptr<class GameEngineLevel> _Level)
	{
		CurLevel = _Level;
	}

protected:
	
private:
	static std::map<std::string, EffectMetaData> EffectMetaDatas;
	static std::shared_ptr<class GameEngineLevel> CurLevel;

	EffectManager() {}
	~EffectManager() {}

	EffectManager(const EffectManager& _Other) = delete;
	EffectManager(EffectManager&& _Other) noexcept = delete;
	EffectManager& operator=(const EffectManager& _Other) = delete;
	EffectManager& operator=(EffectManager&& _Other) noexcept = delete;
};