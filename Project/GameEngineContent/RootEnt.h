#pragma once
#include "BaseMonster.h"

class RootEnt : public BaseMonster
{
public:
	RootEnt();
	~RootEnt();

	RootEnt(const RootEnt& _Other) = delete;
	RootEnt(RootEnt&& _Other) noexcept = delete;
	RootEnt& operator=(const RootEnt& _Other) = delete;
	RootEnt& operator=(RootEnt&& _Other) noexcept = delete;

protected:
	void Update(float _DeltaTime) override;

	void DataLoad() override;
	void TextureLoad() override;
	void LoadAnimation() override;
	void AnimationAttackMetaDataLoad() override;
	void SetColData() override;

	void Hit_Enter() override;
	void Hit_End() override;

	void Attack_Enter() override;
	void Attack_Update(float _DeltaTime) override;

private:
	bool IsAttackSign = false;
	bool IsAttackEffect = false;

	float4 AttackPos = float4::Zero;

	std::shared_ptr<EffectActor> SignEffectActor = nullptr;
};

