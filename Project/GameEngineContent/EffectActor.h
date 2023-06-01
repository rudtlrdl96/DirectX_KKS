#pragma once
#include "BaseContentActor.h"


enum class EffectDeathTrigger
{
	Time,
	AnimEnd,
};

class EffectMetaData
{
public:
	std::string SpriteName = "";

	float4 RenderPivot = float4::Zero;

	UINT AnimStart = 0;
	UINT AnimEnd = 0;

	float AnimIter = 0.1f;
	float ScaleRatio = 1.0f;


};

class EffectActor : public BaseContentActor
{
private:
	friend class EffectManager;
	
public:
	EffectActor();
	~EffectActor();

	EffectActor(const EffectActor& _Other) = delete;
	EffectActor(EffectActor&& _Other) noexcept = delete;
	EffectActor& operator=(const EffectActor& _Other) = delete;
	EffectActor& operator=(EffectActor&& _Other) noexcept = delete;

	void PauseOn();
	void PauseOff();

	inline size_t GetCurrentFrame() const
	{
		return EffectRender->GetCurrentFrame();
	}

protected:
	std::shared_ptr<ContentSpriteRenderer> EffectRender = nullptr;

	ColorBuffer Buffer;

	EffectDeathTrigger DeathTrigger = EffectDeathTrigger::Time;
	float DeathTime = 0.0f;

	void Init(const EffectMetaData& _MetaData, EffectDeathTrigger _DeathTrigger, float _DeathTime);

	void Start() override;
	void Update(float _DeltaTime) override;

private:

};

