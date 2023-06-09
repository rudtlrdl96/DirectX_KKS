#pragma once
#include "BaseContentActor.h"

class DebugSpriteActor : public BaseContentActor
{
public:
	DebugSpriteActor();
	~DebugSpriteActor();

	DebugSpriteActor(const DebugSpriteActor& _Other) = delete;
	DebugSpriteActor(DebugSpriteActor&& _Other) noexcept = delete;
	DebugSpriteActor& operator=(const DebugSpriteActor& _Other) = delete;
	DebugSpriteActor& operator=(DebugSpriteActor&& _Other) noexcept = delete;

	bool UnMove = false;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	std::shared_ptr<GameEngineSpriteRenderer> SpriteRender = nullptr;
	float MoveSpeed = 400.0f;
	float RotSpeed = 360.0f;
};