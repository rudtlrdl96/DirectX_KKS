#pragma once
#include "ContentLevel.h"

class ShaderDebugLevel : public ContentLevel
{
private:
	enum class TargetRotType
	{
		X,
		Y,
		Z,
	};
public:
	ShaderDebugLevel();
	~ShaderDebugLevel();

	ShaderDebugLevel(const ShaderDebugLevel& _Other) = delete;
	ShaderDebugLevel(ShaderDebugLevel&& _Other) noexcept = delete;
	ShaderDebugLevel& operator=(const ShaderDebugLevel& _Other) = delete;
	ShaderDebugLevel& operator=(ShaderDebugLevel&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	std::shared_ptr<GameEngineActor> ShaderTestActor = nullptr;

	float4 RotVector = float4::Zero;

	bool IsTargetRot = false;

	TargetRotType TargetRotState = TargetRotType::X;
	TransformData OldTrans;
};
