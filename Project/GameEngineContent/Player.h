#pragma once
#include "BaseContentActor.h"
#include "PlayerBaseSkull.h"


class Player : public BaseContentActor
{
	friend class PlayerStateFrame;
public:
	Player();
	~Player();

	Player(const Player& _Other) = delete;
	Player(Player&& _Other) noexcept = delete;
	Player& operator=(const Player& _Other) = delete;
	Player& operator=(Player&& _Other) noexcept = delete;

	void Destroy() override;

	void SetInventoryData();
	void InsertNewSkull(size_t _SkullIndex);
	void ForceSwapSkull();

	inline float GetMeleeAttack() const
	{
		return MeleeAttack;
	}

	inline float GetMagicAttack() const
	{
		return MagicAttack;
	}

	void HitPlayer(float _Damage, const float4& _HitForce);
	void HealPlayer(float _Heal, const float4& _HealForce);

	void AddPushPlayer(const float4& _HitForce);
	void SetPushPlayer(const float4& _HitForce);

	float4 GetBattleRigidVelocity() const
	{
		return MainSkull->BattleActorRigidbody.GetVelocity();
	}

	bool CheckFall() const
	{
		return 0 > MainSkull->BattleActorRigidbody.GetVelocity().y;
	}

	inline float GetThornHitCoolTime() const
	{
		return MainSkull->ThornWaitTime;
	}

	inline void SetThornHitCoolTime(float _CoolTime)
	{
		MainSkull->ThornWaitTime = _CoolTime;
	}

	void InputLock();
	void InputUnlock();

	inline bool IsInputLock() const
	{
		return IsInputLockValue;
	}

	inline ActorViewDir GetViewDir() const
	{
		return MainSkull->GetViewDir();
	}

	void ActivePlayerFrame();
	void DisablePlayerFrame();

	void PlayStoryMove(const float4& _StoryMovePos, std::function<void()> _EndCallback);

protected:	
	void Start() override;
	void Update(float _DeltaTime);

	void LevelChangeEnd() override;

private:
	float SwitchCoolTime = 5.0f;
	float SwitchCoolEndTime = 5.0f;

	float MeleeAttack = 10.0f;
	float MagicAttack = 10.0f;
	float HP = 100.0f;
	
	float HitWaitTime = 0.0f;

	bool IsInputLockValue = false;
	bool IsUseKeyValue = true;
	
	std::shared_ptr<PointLightEffect> LightEffect = nullptr;

	std::shared_ptr<class PlayerHitFade> HitFade = nullptr;

	std::shared_ptr<class BaseQuintessence> Quintessence = nullptr;
	std::shared_ptr<class GameEngineCollision> PlayerBodyCol = nullptr;
	std::shared_ptr<class PlayerStateFrame> StateFrame = nullptr;

	std::shared_ptr<PlayerBaseSkull> MainSkull = nullptr;
	std::shared_ptr<PlayerBaseSkull> SubSkull = nullptr;

	std::shared_ptr<GameEngineComponent> CheatDebugComp_Attack = nullptr;
	std::shared_ptr<GameEngineComponent> CheatDebugComp_HP = nullptr;

	std::shared_ptr<GameEngineFontRenderer> CheatRender_Attack = nullptr;
	std::shared_ptr<GameEngineFontRenderer> CheatRender_HP = nullptr;

	std::vector<std::string> BoneEffectNames;

	bool IsInputUnlockWait = false;
	bool IsInputUnlockWaitEnd = false;

	bool Cheat_Attack = false;
	bool Cheat_HP = false;

	std::shared_ptr<PlayerBaseSkull> CreateNewSkull(size_t _Index);

	void CreateColDebugRender();
};

