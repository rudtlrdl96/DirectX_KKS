#pragma once
#include "BaseMonster.h"

class CarleonArcher : public BaseMonster
{
public:
	CarleonArcher();
	~CarleonArcher();

	CarleonArcher(const CarleonArcher& _Other) = delete;
	CarleonArcher(CarleonArcher&& _Other) noexcept = delete;
	CarleonArcher& operator=(const CarleonArcher& _Other) = delete;
	CarleonArcher& operator=(CarleonArcher&& _Other) noexcept = delete;

protected:
	void Update(float _DeltaTime) override;

	void DataLoad() override;
	void TextureLoad() override;
	void LoadAnimation() override;
	void AnimationAttackMetaDataLoad() override;
	void SetColData() override;

	void Hit_Enter() override;

	void Attack_Enter() override;
	void Attack_Update(float _DeltaTime) override;

private:
	std::shared_ptr<EffectActor> SignEffectActor = nullptr;

	bool IsAttackSign = false;
	bool IsAttackShot = false;
};

