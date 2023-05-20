#include "PrecompileHeader.h"
#include "PlayerBaseSkull.h"

#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEngineCore/GameEngineCollision.h>

#include "CaptureTrail.h"
#include "Player.h"
#include "BaseMonster.h"

void PlayerBaseSkull::Idle_Enter()
{
	SkullRenderer->ChangeAnimation(std::string("Idle" + AnimNamePlusText));
	JumpDir = float4::Zero;
	CanJump = true;
	DoubleJump = true;
}

void PlayerBaseSkull::Idle_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(GroundCol, true))
	{
		PlayerFSM.ChangeState("Fall");
		return;
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("Attack");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Down"))
		{
			if (nullptr == PlatformColCheck(GroundCol))
			{
				FallCooldown = 0.2f;
				PlayerFSM.ChangeState("Fall");
				CanJump = false;
				return;
			}
		}

		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Left"))
	{
		ViewDir = ActorViewDir::Left;
		PlayerFSM.ChangeState("Walk");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Right"))
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
	SkullRenderer->ChangeAnimation(std::string("Jump" + AnimNamePlusText));
}

void PlayerBaseSkull::Jump_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (nullptr == PlatformColCheck(JumpCol))
	{
		JumpDir.y += ContentConst::Gravity_f * _DeltaTime;
		PlayerTrans->AddLocalPosition(JumpDir * _DeltaTime);
	}
	else 
	{
		JumpDir.y = -1.0f;
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}


	if (true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({.EffectName = "PlayerJumpEffect", .Postion = PlayerTrans->GetWorldPosition()});

		JumpDir = float4::Up * JumpPower;
		DoubleJump = false;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("JumpAttack");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}

	if (JumpDir.y < 0)
	{
		PlayerFSM.ChangeState("Fall");
	}
}

void PlayerBaseSkull::Walk_Enter() 
{
	SkullRenderer->ChangeAnimation(std::string("Walk" + AnimNamePlusText));
	JumpDir = float4::Zero;
	CanJump = true;
	DoubleJump = true;
}

void PlayerBaseSkull::Walk_Update(float _DeltaTime) 
{

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (nullptr == PlatformColCheck(GroundCol, true))
	{
		PlayerFSM.ChangeState("Fall");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Down"))
		{
			if (nullptr == PlatformColCheck(GroundCol))
			{
				CanJump = false;
				FallCooldown = 0.2f;
				PlayerFSM.ChangeState("Fall");
				return;
			}
		}

		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("Attack");
		return;
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == PlatformColCheck(WalkCol))
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
	DashTrail->PlayTrail(SkullRenderer->GetTexName(),
		SkullRenderer->GetAtlasData(),
		(ActorViewDir::Left == ViewDir),
		SkullRenderer->GetScaleRatio());

	SkullRenderer->ChangeAnimation("Dash" + AnimNamePlusText);

	EffectManager::PlayEffect({ 
		.EffectName = "PlayerDashEffect",
		.Postion = PlayerTrans->GetWorldPosition(),
		.FlipX = ViewDir == ActorViewDir::Left});

	switch (ViewDir)
	{
	case ActorViewDir::Left:
		DashRigidbody.SetVelocity(float4::Left * DashVelocity);
		break;
	case ActorViewDir::Right:
		DashRigidbody.SetVelocity(float4::Right * DashVelocity);
		break;
	default:
		break;
	}

	CanDash = false;
	DashCombo = false;

	JumpDir = float4::Zero;

	DashTrailCoolTime = 0.05f;
}

void PlayerBaseSkull::Dash_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (true == CanJump && GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		if (nullptr == PlatformColCheck(GroundCol, true))
		{
			EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Postion = PlayerTrans->GetWorldPosition() });

			JumpDir = float4::Up * JumpPower;
			PlayerFSM.ChangeState("Jump");

			CanJump = false;
			DoubleJump = false;
			return;
		}

		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
		return;
	}

	if (true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Postion = PlayerTrans->GetWorldPosition() });

		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
		DoubleJump = false;
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		if (nullptr == PlatformColCheck(GroundCol))
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
		DashTrail->PlayTrail(SkullRenderer->GetTexName(),
			SkullRenderer->GetAtlasData(),
			(ActorViewDir::Left == ViewDir),
			SkullRenderer->GetScaleRatio());
	}


	if (false == DashCombo && GameEngineInput::IsDown("PlayerMove_Dash"))
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
			.Postion = PlayerTrans->GetWorldPosition(),
			.FlipX = ViewDir == ActorViewDir::Left });

		DashCombo = true;

		switch (ViewDir)
		{
		case ActorViewDir::Left:
			DashRigidbody.SetVelocity(float4::Left * DashVelocity);
			break;
		case ActorViewDir::Right:
			DashRigidbody.SetVelocity(float4::Right * DashVelocity);
			break;
		default:
			break;
		}
	}

	if (5.0f > DashRigidbody.GetVelocity().Size())
	{
		if (nullptr == PlatformColCheck(GroundCol))
		{
			PlayerFSM.ChangeState("Fall");
			return;
		}

		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
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
}


void PlayerBaseSkull::Fall_Enter() 
{
	SkullRenderer->ChangeAnimation("Fall" + AnimNamePlusText);
}

void PlayerBaseSkull::Fall_Update(float _DeltaTime) 
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (true == SkullRenderer->FindAnimation("Fall")->IsEnd())
	{
		SkullRenderer->ChangeAnimation("FallRepeat" + AnimNamePlusText);
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		PlayerFSM.ChangeState("JumpAttack");
		return;
	}

	if (true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Postion = PlayerTrans->GetWorldPosition() });

		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
		DoubleJump = false;
		return;
	}

	JumpDir.y += _DeltaTime * ContentConst::Gravity_f;
	PlayerTrans->AddLocalPosition(JumpDir * _DeltaTime);

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);

		if (nullptr == PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Left * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);

		if (nullptr == PlatformColCheck(WalkCol))
		{
			float4 MoveDir = float4::Right * WalkSpeed * _DeltaTime;
			PlayerTrans->AddLocalPosition(MoveDir);
		}
	}

	bool IsHalfCheck = 0.0f >= FallCooldown;
	
	std::shared_ptr<GameEngineCollision> GroundColPtr = PlatformColCheck(GroundCol, IsHalfCheck);

	if (nullptr != GroundColPtr)
	{	
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());

		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
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

	if (true == GameEngineInput::IsPress("PlayerMove_Left"))
	{
		SetViewDir(ActorViewDir::Left);
		AttackRigidbody.AddVelocity(float4::Left * 230.0f);
	}
	else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
	{
		SetViewDir(ActorViewDir::Right);
		AttackRigidbody.AddVelocity(float4::Right * 230.0f);
	}

	IsAttackCombo = false;
	AttackComboCount = 0;
	SkullRenderer->ChangeAnimation(std::string(AnimColMeta_Attack[AttackComboCount].GetAnimationName() + AnimNamePlusText));

	AttackAnimIndex = static_cast<size_t>(-1);
}

void PlayerBaseSkull::Attack_Update(float _DeltaTime) 
{
	if(AttackAnimIndex != SkullRenderer->GetCurrentFrame())
	{
		AttackAnimIndex = SkullRenderer->GetCurrentFrame();
		AttackColBuffers.clear();
	}

	const AttackColMetaData& Data = AnimColMeta_Attack[AttackComboCount].GetAttackMetaData(static_cast<UINT>(AttackAnimIndex));

	if (0 < Data.ColMetaDatas.size())
	{
		GameEngineTransform* ColTrans = AttackCol->GetTransform();

		std::map<UINT, BaseMonster*> CurFrameColDatas;

		for (size_t i = 0; i < Data.ColMetaDatas.size(); i++)
		{
			ColTrans->SetLocalPosition(Data.ColMetaDatas[i].LocalCenter);
			ColTrans->SetWorldScale(Data.ColMetaDatas[i].LocalSize);

			std::vector<std::shared_ptr<GameEngineCollision>> Cols;

			AttackCol->CollisionAll(
				(int)CollisionOrder::Monster,
				ColType::AABBBOX2D,
				ColType::AABBBOX2D,
				Cols);

			for (size_t n = 0; n < Cols.size(); n++)
			{
				BaseMonster* CastPtr = reinterpret_cast<BaseMonster*>(Cols[n]->GetActor());

				if (nullptr == CastPtr)
				{
					MsgAssert_Rtti<PlayerBaseSkull>(" - 베이스 몬스터가 아닌 다른 클래스가 Monster Collision에 들어갔습니다");
				}
				else
				{					
					CurFrameColDatas[CastPtr->GetActorCode()] = CastPtr;
				}
			}

		}

		for (const std::pair<UINT, BaseMonster*>& MonsterRef : CurFrameColDatas)
		{
			if (nullptr == AttackColBuffers[MonsterRef.second->GetActorCode()])
			{
				MonsterRef.second->HitMonster(ViewDir);
			}

			AttackColBuffers[MonsterRef.second->GetActorCode()] = MonsterRef.second;
		}
	}

	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(GroundCol, true))
	{
		PlayerFSM.ChangeState("Fall");
		return;
	}

	AttackRigidbody.UpdateForce(_DeltaTime);

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);

		float4 AttackVelocity = AttackRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(AttackVelocity);
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		IsAttackCombo = true;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}
	else if (true == CanJump && GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		JumpDir = float4::Up * JumpPower;
		PlayerFSM.ChangeState("Jump");
		return;
	}

	if (true == SkullRenderer->IsAnimationEnd())
	{
		AttackRigidbody.SetVelocity(float4::Zero);

		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			SetViewDir(ActorViewDir::Left);
			AttackRigidbody.AddVelocity(float4::Left * 230.0f);
		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			SetViewDir(ActorViewDir::Right);
			AttackRigidbody.AddVelocity(float4::Right * 230.0f);
		}

		if (true == IsAttackCombo)
		{
			++AttackComboCount;

			if (AttackComboCount >= AnimColMeta_Attack.size())
			{
				AttackComboCount = 0;
			}

			IsAttackCombo = false;
			SkullRenderer->ChangeAnimation(AnimColMeta_Attack[AttackComboCount].GetAnimationName() + AnimNamePlusText);
		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
		}
	}

}

void PlayerBaseSkull::Attack_End() {}


void PlayerBaseSkull::JumpAttack_Enter()
{
	if (0 == AnimColMeta_JumpAttack.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 점프 공격 정보가 없습니다");
	}

	IsJumpAttackCombo = false;
	JumpAttackCombo = 0;
	SkullRenderer->ChangeAnimation(AnimColMeta_JumpAttack[JumpAttackCombo].GetAnimationName() + AnimNamePlusText);
}

void PlayerBaseSkull::JumpAttack_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 Velocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(Velocity);
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			SetViewDir(ActorViewDir::Left);
			PlayerTrans->AddLocalPosition(float4::Left * WalkSpeed * _DeltaTime);
		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			SetViewDir(ActorViewDir::Right);
			PlayerTrans->AddLocalPosition(float4::Right * WalkSpeed * _DeltaTime);
		}
	}

	if (nullptr == PlatformColCheck(JumpCol))
	{
		JumpDir.y += ContentConst::Gravity_f * _DeltaTime;
		PlayerTrans->AddLocalPosition(JumpDir * _DeltaTime);
	}
	else
	{
		JumpDir.y = -1.0f;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Skill_A"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_A");
		return;
	}
	else if (true == GameEngineInput::IsDown("PlayerMove_Skill_B"))  // + Skill CooltimeCheck
	{
		// CoolTime Check
		PlayerFSM.ChangeState("Skill_B");
		return;
	}

	std::shared_ptr<GameEngineCollision> GroundColPtr = PlatformColCheck(GroundCol, true);

	if (JumpDir.y <= 0.0f && nullptr != GroundColPtr)
	{
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());

		if (true == GameEngineInput::IsPress("PlayerMove_Left"))
		{
			PlayerFSM.ChangeState("Walk");

		}
		else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
		{
			PlayerFSM.ChangeState("Walk");
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
		}
	}

	if (true == DoubleJump && true == GameEngineInput::IsDown("PlayerMove_Jump"))
	{
		EffectManager::PlayEffect({ .EffectName = "PlayerJumpEffect", .Postion = PlayerTrans->GetWorldPosition() });

		JumpDir = float4::Up * JumpPower;
		DoubleJump = false;
	}

	if (true == CanDash && true == GameEngineInput::IsDown("PlayerMove_Dash"))
	{
		PlayerFSM.ChangeState("Dash");
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerMove_Attack"))
	{
		IsJumpAttackCombo = true;
	}

	if (true == SkullRenderer->IsAnimationEnd())
	{	
		if (true == IsJumpAttackCombo)
		{
			++JumpAttackCombo;

			if (JumpAttackCombo >= AnimColMeta_JumpAttack.size())
			{
				JumpAttackCombo = 0;
			}

			IsJumpAttackCombo = false;
			SkullRenderer->ChangeAnimation(AnimColMeta_JumpAttack[JumpAttackCombo].GetAnimationName() + AnimNamePlusText);
		}
		else if (JumpDir.y <= 0.0f)
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
	
}

void PlayerBaseSkull::Skill_SlotA_Enter()
{
	if (0 == AnimColMeta_SkillA.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스킬A 정보가 없습니다");
	}

	SkillACombo = 0;
	SkullRenderer->ChangeAnimation(AnimColMeta_SkillA[SkillACombo].GetAnimationName() + AnimNamePlusText);
}

void PlayerBaseSkull::Skill_SlotA_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == SkullRenderer->IsAnimationEnd()) // + Skill CooltimeCheck
	{	

		++SkillACombo;

		if (AnimColMeta_SkillA.size() > SkillACombo)
		{
			SkullRenderer->ChangeAnimation(AnimColMeta_SkillA[SkillACombo].GetAnimationName() + AnimNamePlusText);
		}
		else
		{
			if (nullptr == PlatformColCheck(GroundCol, true))
			{
				JumpDir.y = 0.0f;
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
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

}

void PlayerBaseSkull::Skill_SlotB_Enter()
{
	if (0 == AnimColMeta_SkillB.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스킬B 정보가 없습니다");
	}

	SkillBCombo = 0;
	SkullRenderer->ChangeAnimation(AnimColMeta_SkillB[SkillBCombo].GetAnimationName() + AnimNamePlusText);
}

void PlayerBaseSkull::Skill_SlotB_Update(float _DeltaTime)
{
	IsSwitchValue = false;

	if (true == GameEngineInput::IsDown("PlayerMove_Switch"))
	{
		IsSwitchValue = true;
	}

	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == SkullRenderer->IsAnimationEnd()) // + Skill CooltimeCheck
	{
		++SkillBCombo;

		if (AnimColMeta_SkillB.size() > SkillBCombo)
		{
			SkullRenderer->ChangeAnimation(AnimColMeta_SkillB[SkillBCombo].GetAnimationName() + AnimNamePlusText);
		}
		else
		{
			if (nullptr == PlatformColCheck(GroundCol, true))
			{
				JumpDir.y = 0.0f;
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
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

}


void PlayerBaseSkull::Switch_Enter()
{
	if (0 == AnimColMeta_Switch.size())
	{
		MsgAssert_Rtti<PlayerBaseSkull>(" - 스컬의 스위치 정보가 없습니다");
	}

	SwitchCombo = 0;
	SkullRenderer->ChangeAnimation(AnimColMeta_Switch[SwitchCombo].GetAnimationName());
}

void PlayerBaseSkull::Switch_Update(float _DeltaTime)
{
	if (nullptr == PlatformColCheck(WalkCol))
	{
		float4 DashVelocity = DashRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(DashVelocity);
	}

	if (true == SkullRenderer->IsAnimationEnd()) // + Skill CooltimeCheck
	{
		++SwitchCombo;

		if (AnimColMeta_Switch.size() > SwitchCombo)
		{
			SkullRenderer->ChangeAnimation(AnimColMeta_Switch[SwitchCombo].GetAnimationName());
		}
		else
		{
			if (nullptr == PlatformColCheck(GroundCol, true))
			{
				JumpDir.y = 0.0f;
				PlayerFSM.ChangeState("Fall");
				return;
			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Left"))
			{
				PlayerFSM.ChangeState("Walk");

			}
			else if (true == GameEngineInput::IsPress("PlayerMove_Right"))
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

}