#pragma once
#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineCamera.h>

class BaseDebugLevel : public GameEngineLevel
{
public:
	BaseDebugLevel();
	~BaseDebugLevel();

	BaseDebugLevel(const BaseDebugLevel& _Other) = delete;
	BaseDebugLevel(BaseDebugLevel&& _Other) noexcept = delete;
	BaseDebugLevel& operator=(const BaseDebugLevel& _Other) = delete;
	BaseDebugLevel& operator=(BaseDebugLevel&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

	std::shared_ptr<GameEngineCamera> MainCam = nullptr;
private:


};

