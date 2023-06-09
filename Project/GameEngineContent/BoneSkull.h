#pragma once
#include "PlayerBaseSkull.h"

class BoneSkull : public PlayerBaseSkull
{
	friend class BoneHead;
private:
	enum class BoneSkullState
	{
		Normal,
		NoHead,
		NoWeapon
	};
public:
	BoneSkull();
	~BoneSkull();

	BoneSkull(const BoneSkull& _Other) = delete;
	BoneSkull(BoneSkull&& _Other) noexcept = delete;
	BoneSkull& operator=(const BoneSkull& _Other) = delete;
	BoneSkull& operator=(BoneSkull&& _Other) noexcept = delete;

	void SetBoneSkullState(BoneSkullState _State);

	void Death() override;
	void Destroy() override;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

	void ChangeSwitchEnd() override;

	void Idle_Enter() override;
	void Idle_Update(float _DeltaTime) override;

	void Skill_SlotA_Enter() override;
	void Skill_SlotA_Update(float _DeltaTime) override;
	void Skill_SlotB_Enter() override;

	void Switch_Enter() override;
	void Switch_Update(float _DeltaTime) override;
	void Switch_End() override;

private:
	BoneSkullState State = BoneSkullState::Normal;
	std::shared_ptr<class GameEngineCollision> HeadPickupCol = nullptr;
	std::shared_ptr<class BoneHead> HeadActor = nullptr;
	
	float WaitTime = 0.0f;

	void DataLoad() override;
	void TextureLoad() override;
	void CreateAnimation() override;
	void AnimationColLoad() override;

	void HeadReturn();

};

