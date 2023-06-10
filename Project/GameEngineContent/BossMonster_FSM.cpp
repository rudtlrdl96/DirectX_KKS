#include "PrecompileHeader.h"
#include "BossMonster.h"
#include "Player.h"


void BossMonster::Idle_Enter() 
{
	PlayAnimation("Idle");
}

void BossMonster::Idle_Update(float _DeltaTime) 
{
	if (true == HitCheck())
	{
		BossFsm.ChangeState("Hit");
	}

	std::shared_ptr<Player> PlayerPtr = FindPlayer.lock();

	if (nullptr == PlayerPtr)
	{
		std::shared_ptr<GameEngineCollision> Col = PlayerFindCol->Collision((int)CollisionOrder::Player, ColType::AABBBOX2D, ColType::AABBBOX2D);

		if (nullptr == Col)
		{
			return;
		}

		std::shared_ptr<Player> CastingPtr = Col->GetActor()->DynamicThis<Player>();

		if (nullptr == CastingPtr)
		{
			MsgAssert_Rtti<BossMonster>(" - Player Ŭ������ Player ColOrder�� ���� �� �ֽ��ϴ�");
			return;
		}

		FindPlayer = CastingPtr;
		PlayerPtr = FindPlayer.lock();
	}
	
	if (0.0f < CurWaitTime)
	{
		GameEngineTransform* PlayerTrans = PlayerPtr->GetTransform();

		float4 BossPos = GetTransform()->GetWorldPosition();
		float4 PlayerPos = PlayerTrans->GetWorldPosition();
		float4 Dir = PlayerPos - BossPos;

		float DistanceX = fabsf(Dir.x);

		if (400 <= DistanceX)
		{
			if (0.0f > Dir.x)
			{
				SetViewDir(ActorViewDir::Left);
			}
			else
			{
				SetViewDir(ActorViewDir::Right);
			}

			BossFsm.ChangeState("Dash");
			return;
		}
		else
		{
			SelectPattern();
			BossFsm.ChangeState("Pattern");
			return;
		}
	}
}

void BossMonster::Idle_End() 
{

}


void BossMonster::Dash_Enter() 
{
	PlayAnimation("Dash");

	switch (Dir)
	{
	case ActorViewDir::Left:
		BossRigidbody.SetVelocity(float4::Left * DashPower);
		break;
	case ActorViewDir::Right:
		BossRigidbody.SetVelocity(float4::Right * DashPower);
		break;
	}
}

void BossMonster::Dash_Update(float _DeltaTime) 
{
	BossRigidbody.UpdateForce(_DeltaTime);

	float4 DashVel = BossRigidbody.GetVelocity() * _DeltaTime;

	if (true == MoveableCheck(DashVel, false))
	{
		GetTransform()->AddLocalPosition(DashVel);
	}

	if (1.0f >= DashVel.Size())
	{
		BossFsm.ChangeState("Idle");
	}
}

void BossMonster::Dash_End() 
{

}

void BossMonster::Hit_Enter()
{
	Render->ChangeAnimation("Hit");
	HitWaitTime = 0.0f;
}

void BossMonster::Hit_Update(float _DeltaTime)
{
	HitWaitTime += _DeltaTime;

	if (false == IsSuperArmor && true == IsHit)
	{
		HitEffect();

		if (false == IsSuperArmor && true == IsStiffen)
		{
			Render->ChangeAnimation("Hit");
			HitWaitTime = 0.0f;
		}

		HitPush();
	}

	if (0.5f <= HitWaitTime)
	{
		BossFsm.ChangeState("Idle");
	}

	if (0.0f < CurWaitTime)
	{
		std::shared_ptr<Player> PlayerPtr = FindPlayer.lock();
		GameEngineTransform* PlayerTrans = PlayerPtr->GetTransform();

		float4 BossPos = GetTransform()->GetWorldPosition();
		float4 PlayerPos = PlayerTrans->GetWorldPosition();
		float4 Dir = PlayerPos - BossPos;

		float DistanceX = fabsf(Dir.x);

		if (400 <= DistanceX)
		{
			if (0.0f > Dir.x)
			{
				SetViewDir(ActorViewDir::Left);
			}
			else
			{
				SetViewDir(ActorViewDir::Right);
			}

			BossFsm.ChangeState("Dash");
			return;
		}
		else
		{
			SelectPattern();
			BossFsm.ChangeState("Pattern");
			return;
		}
	}
}

void BossMonster::Hit_End()
{

}

void BossMonster::Pattern_Enter() 
{
	CurWaitTime = -PatternWaitTime;

	if (nullptr != Cur_Pattern_Enter)
	{
		Cur_Pattern_Enter();
	}
}

void BossMonster::Pattern_Update(float _DeltaTime) 
{
	if (nullptr != Cur_Pattern_Update)
	{
		Cur_Pattern_Update(_DeltaTime);
	}
}

void BossMonster::Pattern_End() 
{
	CurWaitTime = -PatternWaitTime;

	if (nullptr != Cur_Pattern_End)
	{
		Cur_Pattern_End();
	}

	Cur_Pattern_Enter = nullptr;
	Cur_Pattern_Update = nullptr;
	Cur_Pattern_End = nullptr;
}

void BossMonster::Behavior_Enter() 
{
	PlayAnimation(PlayBehaviorAnim);
}

void BossMonster::Behavior_Update(float _DeltaTime) 
{
	if (false == IsBehaviorLoop && true == Render->IsAnimationEnd())
	{
		BossFsm.ChangeState("Idle");
	}
}

void BossMonster::Behavior_End() 
{
	if (nullptr != BehaviorEndCallback)
	{
		std::function<void()> TempCallback = BehaviorEndCallback;
		BehaviorEndCallback = nullptr;
		TempCallback();
	}
}