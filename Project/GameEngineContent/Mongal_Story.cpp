#include "PrecompileHeader.h"
#include "Mongal.h"
#include "HealthBar.h"
#include "BattleLevel.h"

void Mongal::StoryUpdate(float _DeltaTime)
{

	Buffer.Color = float4::Zero;

	switch (State)
	{
	case Mongal::DeathState::BattleDeath:
	{
		HealthBarPtr->Off();

		if (13 == Render->GetCurrentFrame())
		{
			FadeTimeCheck += _DeltaTime;

			if (false == IsFade && 1.0f < FadeTimeCheck)
			{
				IsFade = true;
				GetContentLevel()->CallEvent("FadeIn");
			}

			if (4.0f < FadeTimeCheck)
			{
				GetTransform()->SetLocalPosition(float4(1710, 250, 0));

				GetContentLevel()->CallEvent("PlayerMove_MongalWalkup");
				GetContentLevel()->CallEvent("FadeOut");
				State = DeathState::WakeUp;
				Render->ChangeAnimation("Wakeup");
			}
		}
		else if (6 == Render->GetCurrentFrame())
		{
			DeathTimeCheck += _DeltaTime;

			if (1.0f < DeathTimeCheck)
			{
				Render->SetAnimPauseOff();
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		else if (7 == Render->GetCurrentFrame())
		{
			if (false == DeathCameraShake)
			{
				float4 DeathEffectPlusPos;

				if (Dir == ActorViewDir::Left)
				{
					DeathEffectPlusPos = float4(-30, 100);
				}
				else
				{
					DeathEffectPlusPos = float4(30, 100);
				}

				GetContentLevel()->CallEvent("StoryFadeIn");

				EffectManager::PlayEffect({
					.EffectName = "MongalDeathEffect",
					.Position = GetTransform()->GetWorldPosition() + DeathEffectPlusPos,
					.FlipX = (Dir == ActorViewDir::Left) });

				DeathCameraShake = true;

				std::shared_ptr<BattleLevel> CastPtr = GetLevel()->DynamicThis<BattleLevel>();

				if (nullptr == CastPtr)
				{
					return;
				}

				CastPtr->GetCamCtrl().CameraShake(5, 30, 7);
			}

			DeathTimeCheck += _DeltaTime;

			if (2.5f < DeathTimeCheck)
			{
				Render->SetAnimPauseOff();
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		else if (2 == Render->GetCurrentFrame())
		{
			DeathTimeCheck += _DeltaTime;

			if (1.0f < DeathTimeCheck)
			{
				Render->SetAnimPauseOff();
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		else
		{
			DeathTimeCheck = 0.0f;
		}
	}
	break;
	case Mongal::DeathState::WakeUp:
		if (0 == Render->GetCurrentFrame())
		{
			float& Time = WakeupWaitTime[0];

			Time += _DeltaTime;

			if (0.5f < Time)
			{
				Render->SetAnimPauseOff();
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		else if (9 == Render->GetCurrentFrame())
		{
			float& Time = WakeupWaitTime[9];

			Time += _DeltaTime;

			if (1.0f < Time)
			{
				Render->SetAnimPauseOff();
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		else if (20 == Render->GetCurrentFrame())
		{
			float& Time = WakeupWaitTime[20];

			Time += _DeltaTime;

			if (0.5f < Time)
			{
				Render->SetAnimPauseOff();
				State = DeathState::NormalIdle;
				Render->ChangeAnimation("Idle_NoBattle");
				GetContentLevel()->CallEvent("MongalWalkUp_Talk");
			}
			else
			{
				Render->SetAnimPauseOn();
			}
		}
		break;
	case Mongal::DeathState::NormalIdle:
		break;
	case Mongal::DeathState::Laugh:
		LaughTime += _DeltaTime;

		if (3.0f < LaughTime)
		{
			State = DeathState::DoorWalk;
			SetViewDir(ActorViewDir::Right, true);
			Render->ChangeAnimation("Walk_NoBattle");

			WalkStart = GetTransform()->GetLocalPosition();
			WalkEnd = float4(1980, 250, WalkStart.z);
			WalkProgress = 0.0f;
		}

		break;
	case Mongal::DeathState::DoorWalk:
	{
		WalkProgress += _DeltaTime * WalkSpeed;

		float4 LerpPos = float4::LerpClamp(WalkStart, WalkEnd, WalkProgress);

		if (1.0f <= WalkProgress)
		{
			State = DeathState::NormalDeath;
			Render->ChangeAnimation("Death_NoBattle");

			// Effect
			// CameraShake
			// 용사 출현 이벤트
		}

		GetTransform()->SetLocalPosition(LerpPos);
	}
		break;
	case Mongal::DeathState::NormalDeath:
		break;
	default:
		break;
	}
}