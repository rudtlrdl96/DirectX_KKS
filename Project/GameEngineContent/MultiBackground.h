#pragma once
#include "BaseContentActor.h"
#include "Background.h"

class MultiBackground : public BaseContentActor
{
public:
	MultiBackground();
	~MultiBackground();

	MultiBackground(const MultiBackground& _Other) = delete;
	MultiBackground(MultiBackground&& _Other) noexcept = delete;
	MultiBackground& operator=(const MultiBackground& _Other) = delete;
	MultiBackground& operator=(MultiBackground&& _Other) noexcept = delete;

	void CreateBackground(Background::BG_DESC& _Desc, int _Depth);
	void UpdateTargetPos(float _DeltaTime, const float4& _TargetPos);

	void SaveBin(GameEngineSerializer& _SaveSerializer) const;
	void LoadBin(GameEngineSerializer& _SaveSerializer);

protected:
	
private:
	std::map<int, std::shared_ptr<Background>> BackgroundBuffer;
};

