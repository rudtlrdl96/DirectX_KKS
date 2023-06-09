#include "PrecompileHeader.h"
#include "PlayerBaseSkull.h"

#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEngineCore/GameEngineCollision.h>

#include "CaptureTrail.h"
#include "Player.h"
#include "BaseMonster.h"
#include "CaptureAnimation.h"

void PlayerBaseSkull::Idle_Enter()
{
	Render->ChangeAnimation(std::string("Idle" + AnimNamePlusText));

	CanJump = true;
	DoubleJump = true;

	FsmState = PlayerFSM_State::Idle;
}

void PlayerBaseSkull::Idle_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	std::shared_ptr<GameEngineCollision> Ground = ContentFunc::PlatformColCheck(GroundCol, true);

	if (nullptr == Ground)
	{
		PlayerFSM.ChangeState("Fall");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
	}
	else if (false == ParentPlayer->IsInputLock() && false == IsLockAttack &&
			 true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("Attack");
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Down"))
		{
			if (nullptr == ContentFunc::PlatformColCheck(GroundCol))
			{
				FallCooldown = 0.2f;
				PlayerFSM.ChangeState("Fall");
				CanJump = false;
				return;
			}
		}

		Jump();
		PlayerFSM.ChangeState("Jump");
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");		
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		ViewDir = ActorViewDir::Left;
		PlayerFSM.ChangeState("Walk");
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		ViewDir = ActorViewDir::Right;
		PlayerFSM.ChangeState("Walk");
	}
}

void PlayerBaseSkull::Idle_End()
{
}

void PlayerBaseSkull::Jump_Enter()
{
	CanJump = false;
	Render->ChangeAnimation(std::string("Jump" + AnimNamePlusText));
	FsmState = PlayerFSM_State::Jump;
}

void PlayerBaseSkull::Jump_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (nullptr == ContentFunc::PlatformColCheck(JumpCol))
	{
		BattleActorRigidbody.AddVelocity(float4(0, -2800.0f * _DeltaTime, 0));
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}


	if (false == ParentPlayer->IsInputLock() && true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({.EffectName = "PlayerJumpEffect", .Position = PlayerTrans->GetWorldPosition()});

		Jump();
		DoubleJump = false;
	}

	if (false == ParentPlayer->IsInputLock() && false == IsLockAttack &&
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("JumpAttack");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}

	if (BattleActorRigidbody.GetVelocity().y <= 0)
	{
		PlayerFSM.ChangeState("Fall");
	}
}

void PlayerBaseSkull::Walk_Enter() 
{
	Render->ChangeAnimation(std::string("Walk" + AnimNamePlusText));
	//JumpDir = float4::Zero;
	CanJump = true;
	DoubleJump = true;
	FsmState = PlayerFSM_State::Walk;
}

void PlayerBaseSkull::Walk_Update(float _DeltaTime) 
{

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	std::shared_ptr<GameEngineCollision> Ground = ContentFunc::PlatformColCheck(GroundCol, true);

	if (nullptr == Ground)
	{
		PlayerFSM.ChangeState("Fall");
		return;
	}
	else
	{
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = Ground->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());
	}
	
	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Down"))
		{
			if (nullptr == ContentFunc::PlatformColCheck(GroundCol))
			{
				CanJump = false;
				FallCooldown = 0.2f;
				PlayerFSM.ChangeState("Fall");
				return;
			}
		}

		Jump();
		PlayerFSM.ChangeState("Jump");
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}

	if (false == ParentPlayer->IsInputLock() && false == IsLockAttack && 
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("Attack");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else
	{
		PlayerFSM.ChangeState("Idle");
		return;
	}
}

void PlayerBaseSkull::Walk_End() 
{

}

void PlayerBaseSkull::Dash_Enter()
{
	DashTrail->PlayTrail(Render->GetTexName(),
		Render->GetAtlasData(),
		(ActorViewDir::Left == ViewDir),
		Render->GetScaleRatio());

	Render->ChangeAnimation("Dash" + AnimNamePlusText);

	EffectManager::PlayEffect({ 
		.EffectName = "PlayerDashEffect",
		.Position = PlayerTrans->GetWorldPosition(),
		.FlipX = ViewDir == ActorViewDir::Left});

	FsmState = PlayerFSM_State::Dash;
	Dash();
	DashAvoidance = true;

	CanDash = false;
	DashCombo = false;

	BattleActorRigidbody.SetVelocity(float4::Zero);
	//JumpDir = float4::Zero;

	DashTrailCoolTime = 0.05f;
	FsmState = PlayerFSM_State::Dash;
}

void PlayerBaseSkull::Dash_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (false == ParentPlayer->IsInputLock() && true == CanJump && GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
		{
			EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Position = PlayerTrans->GetWorldPosition() });

			Jump();
			PlayerFSM.ChangeState("Jump");

			CanJump = false;
			DoubleJump = false;
			return;
		}

		Jump();
		PlayerFSM.ChangeState("Jump");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Position = PlayerTrans->GetWorldPosition() });

		Jump();
		PlayerFSM.ChangeState("Jump");
		DoubleJump = false;
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B") && false == IsLockSkillB)
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && false == IsLockAttack &&
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		if (nullptr == ContentFunc::PlatformColCheck(GroundCol))
		{
			PlayerFSM.ChangeState("JumpAttack");
		}
		else
		{
			PlayerFSM.ChangeState("Attack");
		}
		return;
	}

	DashTrailCoolTime -= _DeltaTime;

	if (0.0f >= DashTrailCoolTime)
	{
		DashTrailCoolTime += 0.05f;
		DashTrail->PlayTrail(Render->GetTexName(),
			Render->GetAtlasData(),
			(ActorViewDir::Left == ViewDir),
			Render->GetScaleRatio());
	}


	if (false == ParentPlayer->IsInputLock() && Type != SkullType::Power && false == DashCombo && GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			SetViewDir(ActorViewDir::Left);
		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			SetViewDir(ActorViewDir::Right);
		}

		EffectManager::PlayEffect({
			.EffectName = "PlayerDashEffect",
			.Position = PlayerTrans->GetWorldPosition(),
			.FlipX = ViewDir == ActorViewDir::Left });

		DashCombo = true;

		Render->ChangeAnimation("Dash" + AnimNamePlusText);
		Dash();
	}

	if (150.0f > std::fabsf(DashRigidbody.GetVelocity().x))
	{
		if (nullptr == ContentFunc::PlatformColCheck(GroundCol))
		{
			PlayerFSM.ChangeState("Fall");
			return;
		}

		if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else 
		{
			PlayerFSM.ChangeState("Idle");
		}
	}
}

void PlayerBaseSkull::Dash_End()
{
	DashAvoidance = false;
}


void PlayerBaseSkull::Fall_Enter() 
{
	Render->ChangeAnimation("Fall" + AnimNamePlusText);
	FsmState = PlayerFSM_State::Fall;
}

void PlayerBaseSkull::Fall_Update(float _DeltaTime) 
{
	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (true == Render->IsAnimationEnd())
	{
		Render->ChangeAnimation("FallRepeat" + AnimNamePlusText);
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && false == IsLockAttack && 
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("JumpAttack");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Position = PlayerTrans->GetWorldPosition() });

		Jump();
		PlayerFSM.ChangeState("Jump");
		DoubleJump = false;
		return;
	}

	BattleActorRigidbody.AddVelocity(float4(0, -2800.0f * _DeltaTime));

	if (0 < BattleActorRigidbody.GetVelocity().y)
	{
		PlayerFSM.ChangeState("Jump");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}

	bool IsHalfCheck = 0.0f >= FallCooldown;
	
	std::shared_ptr<GameEngineCollision> GroundColPtr = ContentFunc::PlatformColCheck(GroundCol, IsHalfCheck);

	if (nullptr != GroundColPtr)
	{
		IsFallEnd = true;

		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());

		if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
		}
	}	
}

void PlayerBaseSkull::Fall_End() 
{

}

void PlayerBaseSkull::Attack_Enter() 
{
	if (0 == AnimColMeta_Attack.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 공격 정보가 없습니다");
	}

	AttackRigidbody.SetVelocity(float4::Zero);

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);
		AttackRigidbody.AddVelocity(float4::Left * AttackMoveDis);
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);
		AttackRigidbody.AddVelocity(float4::Right * AttackMoveDis);
	}

	IsAttackCombo = false;
	AttackComboCount = 0;
	Render->ChangeAnimation(std::string(AnimColMeta_Attack[AttackComboCount].GetAnimationName() + AnimNamePlusText));

	AttackEnterCheck.SetColData(AnimColMeta_Attack[AttackComboCount]);
	FsmState = PlayerFSM_State::Attack;

	CurDamageRatio = Attack_DamageRatio;
}

void PlayerBaseSkull::Attack_Update(float _DeltaTime) 
{
	AttackEnterCheck.Update();

	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	BattleActorRigidbody.AddVelocity(float4(0, -2800.0f * _DeltaTime, 0));

	AttackRigidbody.UpdateForce(_DeltaTime);

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);

		float4 AttackVelocity = AttackRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(AttackVelocity);
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && false == IsLockAttack && 
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		IsAttackCombo = true;
	}

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == CanJump && GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		Jump();
		PlayerFSM.ChangeState("Jump");
		return;
	}

	if (true == Render->IsAnimationEnd())
	{
		AttackRigidbody.SetVelocity(float4::Zero);

		if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
		{
			PlayerFSM.ChangeState("Fall");
			return;
		}

		if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			SetViewDir(ActorViewDir::Left);
			AttackRigidbody.AddVelocity(float4::Left * AttackMoveDis);
		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			SetViewDir(ActorViewDir::Right);
			AttackRigidbody.AddVelocity(float4::Right * AttackMoveDis);
		}

		if (true == IsAttackCombo)
		{
			++AttackComboCount;

			if (AttackComboCount >= AnimColMeta_Attack.size())
			{
				AttackComboCount = 0;
			}

			IsAttackCombo = false;
			Render->ChangeAnimation(AnimColMeta_Attack[AttackComboCount].GetAnimationName() + AnimNamePlusText);
			AttackEnterCheck.SetColData(AnimColMeta_Attack[AttackComboCount]);
		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
		}
	}

}

void PlayerBaseSkull::Attack_End() 
{
	AttackEnterCheck.Reset();
}


void PlayerBaseSkull::JumpAttack_Enter()
{
	if (0 == AnimColMeta_JumpAttack.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 점프 공격 정보가 없습니다");
	}

	IsJumpAttackCombo = false;
	JumpAttackCombo = 0;
	Render->ChangeAnimation(AnimColMeta_JumpAttack[JumpAttackCombo].GetAnimationName() + AnimNamePlusText);
	AttackEnterCheck.SetColData(AnimColMeta_JumpAttack[JumpAttackCombo]);
	FsmState = PlayerFSM_State::JumpAttack;

	CurDamageRatio = Attack_DamageRatio;
}

void PlayerBaseSkull::JumpAttack_Update(float _DeltaTime)
{
	AttackEnterCheck.Update();

	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			SetViewDir(ActorViewDir::Left);
			PlayerTrans->AddLocalPosition(float4::Left * WalkSpeed * _DeltaTime);
		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			SetViewDir(ActorViewDir::Right);
			PlayerTrans->AddLocalPosition(float4::Right * WalkSpeed * _DeltaTime);
		}
	}


	BattleActorRigidbody.AddVelocity(float4(0, -2800.0f * _DeltaTime, 0));
	
	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
	{
		if (CurSkillATime > GetSkillAEndTime() && false == IsLockSkillA && true == IsActiveSkillA())
		{
			PlayerFSM.ChangeState("Skill_A");
			return;
		}
	}
	else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Skill_B"))
	{
		if (CurSkillBTime > GetSkillBEndTime() && false == IsLockSkillB && true == IsActiveSkillB())
		{
			PlayerFSM.ChangeState("Skill_B");
			return;
		}
	}

	std::shared_ptr<GameEngineCollision> GroundColPtr = ContentFunc::PlatformColCheck(GroundCol, true);

	if (BattleActorRigidbody.GetVelocity().y <= 0.0f && nullptr != GroundColPtr)
	{
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());

		if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
		}
	}

	if (false == ParentPlayer->IsInputLock() && true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Position = PlayerTrans->GetWorldPosition() });

		Jump();
		DoubleJump = false;
	}

	if (false == ParentPlayer->IsInputLock() && true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (false == ParentPlayer->IsInputLock() && false == IsLockAttack && 
		true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		IsJumpAttackCombo = true;
	}

	if (true == Render->IsAnimationEnd())
	{	
		if (true == IsJumpAttackCombo)
		{
			++JumpAttackCombo;

			if (JumpAttackCombo >= AnimColMeta_JumpAttack.size())
			{
				JumpAttackCombo = 0;
			}

			IsJumpAttackCombo = false;
			Render->ChangeAnimation(AnimColMeta_JumpAttack[JumpAttackCombo].GetAnimationName() + AnimNamePlusText);
			AttackEnterCheck.SetColData(AnimColMeta_JumpAttack[JumpAttackCombo]);
		}
		else if (BattleActorRigidbody.GetVelocity().y <= 0.0f)
		{
			PlayerFSM.ChangeState("Jump");

		}
		else
		{
			PlayerFSM.ChangeState("Fall");
		}
	}
}

void PlayerBaseSkull::JumpAttack_End()
{
	AttackEnterCheck.Reset();
}

void PlayerBaseSkull::Skill_SlotA_Enter()
{
	if (0 == AnimColMeta_SkillA.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스킬A 정보가 없습니다");
	}

	SkillACombo = 0;
	Render->ChangeAnimation(AnimColMeta_SkillA[SkillACombo].GetAnimationName() + AnimNamePlusText);
	AttackEnterCheck.SetColData(AnimColMeta_SkillA[SkillACombo]);

	CurSkillATime = 0.0f;
	FsmState = PlayerFSM_State::SkillA;

	CurDamageRatio = SkillA_DamageRatio;
}

void PlayerBaseSkull::Skill_SlotA_Update(float _DeltaTime)
{
	BattleActorRigidbody.SetVelocity(float4::Zero);
	AttackEnterCheck.Update();

	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == Render->IsAnimationEnd()) // + Skill CooltimeCheck
	{	

		++SkillACombo;

		if (AnimColMeta_SkillA.size() > SkillACombo)
		{
			Render->ChangeAnimation(AnimColMeta_SkillA[SkillACombo].GetAnimationName() + AnimNamePlusText);
			AttackEnterCheck.SetColData(AnimColMeta_SkillA[SkillACombo]);
		}
		else
		{
			if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
			{
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
			{
				PlayerFSM.ChangeState("Walk");
			}
			else
			{
				PlayerFSM.ChangeState("Idle");
			}
		}

	}
}

void PlayerBaseSkull::Skill_SlotA_End()
{
	AttackEnterCheck.Reset();
}

void PlayerBaseSkull::Skill_SlotB_Enter()
{
	if (0 == AnimColMeta_SkillB.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스킬B 정보가 없습니다");
	}

	SkillBCombo = 0;
	Render->ChangeAnimation(AnimColMeta_SkillB[SkillBCombo].GetAnimationName() + AnimNamePlusText);
	AttackEnterCheck.SetColData(AnimColMeta_SkillB[SkillBCombo]);

	CurSkillBTime = 0.0f;
	FsmState = PlayerFSM_State::SkillB;
	CurDamageRatio = SkillB_DamageRatio;
}

void PlayerBaseSkull::Skill_SlotB_Update(float _DeltaTime)
{
	BattleActorRigidbody.SetVelocity(float4::Zero);
	IsSwitchValue = false;

	if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == Render->IsAnimationEnd()) // + Skill CooltimeCheck
	{
		++SkillBCombo;

		if (AnimColMeta_SkillB.size() > SkillBCombo)
		{
			Render->ChangeAnimation(AnimColMeta_SkillB[SkillBCombo].GetAnimationName() + AnimNamePlusText);
			AttackEnterCheck.SetColData(AnimColMeta_SkillB[SkillBCombo]);
		}
		else
		{
			if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
			{
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
			{
				PlayerFSM.ChangeState("Walk");
			}
			else
			{
				PlayerFSM.ChangeState("Idle");
			}
		}
	}
}

void PlayerBaseSkull::Skill_SlotB_End()
{
	AttackEnterCheck.Reset();
}

void PlayerBaseSkull::Switch_Enter()
{
	if (0 == AnimColMeta_Switch.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스위치 정보가 없습니다");
	}

	SwitchCombo = 0;
	Render->ChangeAnimation(AnimColMeta_Switch[SwitchCombo].GetAnimationName());
	AttackEnterCheck.SetColData(AnimColMeta_Switch[SwitchCombo]);
	FsmState = PlayerFSM_State::Switch;

	IsLockSkillA = true;
	IsLockSkillB = true;

	CurDamageRatio = Switch_DamageRatio;


	EffectCaptureAnimation({
		.SpriteRender = Render,
		.StartColor = float4(0.59215f, 0.0f, 0.97647f, 1.0f), 
		.EndColor = float4(0.59215f, 0.0f, 0.97647f, 0.0f),
		.Speed = 2.0f,
		.WaitTime = 0.0f});
}

void PlayerBaseSkull::Switch_Update(float _DeltaTime)
{
	AttackEnterCheck.Update();

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == Render->IsAnimationEnd()) // + Skill CooltimeCheck
	{
		++SwitchCombo;

		if (AnimColMeta_Switch.size() > SwitchCombo)
		{
			Render->ChangeAnimation(AnimColMeta_Switch[SwitchCombo].GetAnimationName());
		}
		else
		{
			if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
			{
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (false == ParentPlayer->IsInputLock() && true == GameEngineInput::IsPress("PlayerMove_Right"))
			{
				PlayerFSM.ChangeState("Walk");
			}
			else
			{
				PlayerFSM.ChangeState("Idle");
			}		
		}
	}
}

void PlayerBaseSkull::Switch_End()
{
	AttackEnterCheck.Reset();

	IsLockSkillA = false;
	IsLockSkillB = false;
}

void PlayerBaseSkull::Behavior_Enter()
{
	if ("" != BehaviorAnimationName)
	{
		Render->ChangeAnimation(BehaviorAnimationName);
	}

	CurPauseTime = 0.0f;
}

void PlayerBaseSkull::Behavior_Update(float _DeltaTime)
{
	if (true == IsBehaviorAnimPause && PauseFrame == Render->GetCurrentFrame())
	{
		CurPauseTime += _DeltaTime;

		if (PauseTime <= CurPauseTime)
		{
			CurPauseTime = 0.0f;
			IsBehaviorAnimPause = false;
			Render->SetAnimPauseOff();

			if (nullptr != PauseEndCallback)
			{
				PauseEndCallback();
			}
		}
		else
		{
			Render->SetAnimPauseOn();
		}
	}

	if (true == Render->IsAnimationEnd())
	{
		if (nullptr != BehaviorEndCallback)
		{
			BehaviorEndCallback();
			BehaviorEndCallback = nullptr;
		}
	}
}

void PlayerBaseSkull::Behavior_End()
{
	IsBehaviorAnimPause = false;
	PauseEndCallback = nullptr;
}

void PlayerBaseSkull::StoryMove_Enter()
{
}


void PlayerBaseSkull::StoryMove_Update(float _DeltaTime)
{
	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (nullptr == ContentFunc::PlatformColCheck(GroundCol, true))
	{
		BattleActorRigidbody.AddVelocity(float4(0, -2800.0f * _DeltaTime, 0));
		Render->ChangeAnimation("FallRepeat" + AnimNamePlusText, 0, false);
	}
	else
	{
		Render->ChangeAnimation("Walk" + AnimNamePlusText, 0, false);
	}

	std::shared_ptr<GameEngineCollision> GroundColPtr = ContentFunc::PlatformColCheck(GroundCol, true);

	if (nullptr != GroundColPtr)
	{
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());
	}

	float4 MoveDir = StoryMovePos - GetTransform()->GetWorldPosition();
	MoveDir.z = 0;

	float DiX = fabsf(MoveDir.x);

	if (5.0f > DiX)
	{
		StoryMovePos.z = GetTransform()->GetWorldPosition().z;

		if (nullptr != StoryMoveEndCallback)
		{
			StoryMoveEndCallback();
			StoryMoveEndCallback = nullptr;
		}

		PlayerFSM.ChangeState("Idle");
		return;
	}

	if (MoveDir.x < 0.0f)
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}

}

void PlayerBaseSkull::StoryMove_End()
{

}