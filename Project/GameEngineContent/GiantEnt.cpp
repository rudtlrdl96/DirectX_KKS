#include "PrecompileHeader.h"
#include "GiantEnt.h"
#include <GameEngineCore/GameEngineCollision.h>
#include "Projectile.h"

GiantEnt::GiantEnt()
{
}

GiantEnt::~GiantEnt()
{
}

void GiantEnt::DataLoad()
{
	Data = ContentDatabase<MonsterData, MonsterArea>::GetData(103); // 103 = 숲지기(자이언트 엔트)

	IsSuperArmor = true;
	AttackWaitEndTime = 1.5f;
	TacklePauseTimes[0] = 0.5f;
	TacklePauseTimes[1] = 0.6f;
	TacklePauseTimes[2] = 0.4f;
}

void GiantEnt::TextureLoad()
{
	if (nullptr == GameEngineSprite::Find("ManAtArms_StempEffect.png"))
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Texture");
		Path.Move("3_ForestOfHarmony");
		Path.Move("Monster");

		{
			Path.Move("CarleonManAtArms");
			Path.Move("Effect");

			GameEngineSprite::LoadSheet(Path.GetPlusFileName("ManAtArms_StempEffect.png").GetFullPath(), 3, 4);
			GameEngineSprite::LoadSheet(Path.GetPlusFileName("ManAtArms_TackleFlash.png").GetFullPath(), 7, 2);
			GameEngineSprite::LoadSheet(Path.GetPlusFileName("ManAtArms_TackleSmoke.png").GetFullPath(), 4, 5);

			Path.MoveParent();
			Path.MoveParent();
		}

		EffectManager::CreateMetaData("ManAtArms_StempEffect", {
			.SpriteName = "ManAtArms_StempEffect.png",
			.AnimStart = 0,
			.AnimEnd = 11,
			.AnimIter = 0.025f,
			.ScaleRatio = 2.0f,
			});

		EffectManager::CreateMetaData("ManAtArms_TackleFlash", {
			.SpriteName = "ManAtArms_TackleFlash.png",
			.AnimStart = 0,
			.AnimEnd = 13,
			.AnimIter = 0.07f,
			.ScaleRatio = 3.0f,
			});

		EffectManager::CreateMetaData("ManAtArms_TackleSmoke", {
			.SpriteName = "ManAtArms_TackleSmoke.png",
			.AnimStart = 0,
			.AnimEnd = 16,
			.AnimIter = 0.05f,
			.ScaleRatio = 2.0f,
			});
	}
}

void GiantEnt::LoadAnimation()
{
	Render->CreateAnimation({ .AnimationName = "Idle",  .SpriteName = "CarleonManAtArms_Idle.png", .Start = 0, .End = 4, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Walk",  .SpriteName = "CarleonManAtArms_Walk.png", .Start = 0, .End = 7, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Hit1",  .SpriteName = "CarleonManAtArms_Idle.png", .Start = 0, .End = 0, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Hit2",  .SpriteName = "CarleonManAtArms_Idle.png", .Start = 0, .End = 0, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Death", .SpriteName = "CarleonManAtArms_Dead.png", .Start = 0, .End = 0, .ScaleToTexture = true });
}

void GiantEnt::AnimationAttackMetaDataLoad()
{
	AnimPauseTimes[2] = 0.5f;
	AnimPauseTimes[5] = 0.75f;

	AttackRigidbody.SetMaxSpeed(1000.0f);
	AttackRigidbody.SetFricCoeff(2000.0f);

	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Data");
		Path.Move("3_ForestOfHarmony");
		Path.Move("Monster");

		AnimColMeta_Attack = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("CarleonManAtArms_Attack").GetFullPath());

		Render->CreateAnimation({
			.AnimationName = AnimColMeta_Attack.GetAnimationName().data(),
			.SpriteName = AnimColMeta_Attack.GetSpriteName().data(),
			.Start = AnimColMeta_Attack.GetStartFrame(),
			.End = AnimColMeta_Attack.GetEndFrame(),
			.Loop = false,
			.ScaleToTexture = true });

		AnimColMeta_Tackle = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("CarleonManAtArms_Tackle").GetFullPath());

		Render->CreateAnimation({
			.AnimationName = AnimColMeta_Tackle.GetAnimationName().data(),
			.SpriteName = AnimColMeta_Tackle.GetSpriteName().data(),
			.Start = AnimColMeta_Tackle.GetStartFrame(),
			.End = AnimColMeta_Tackle.GetEndFrame(),
			.Loop = false,
			.ScaleToTexture = true });
	}
}

void GiantEnt::SetColData()
{
	{
		GameEngineTransform* ColTrans = GroundCol->GetTransform();

		ColTrans->SetLocalPosition(float4(0, -3, 0));
		ColTrans->SetWorldScale(float4(70, 6, 1));
	}

	{
		GameEngineTransform* ColTrans = BodyCol->GetTransform();

		ColTrans->SetLocalPosition(float4(0, 70, 0));
		ColTrans->SetWorldScale(float4(80, 140, 1));
	}

	{
		GameEngineTransform* ColTrans = WalkCol->GetTransform();

		LocalWalkColPos = float4(45, 72, 0);

		ColTrans->SetLocalPosition(LocalWalkColPos);
		ColTrans->SetWorldScale(float4(10, 140, 1));
	}
	{
		GameEngineTransform* ColTrans = BackCol->GetTransform();

		LocalBackColPos = float4(-45, 72, 0);

		ColTrans->SetLocalPosition(LocalBackColPos);
		ColTrans->SetWorldScale(float4(10, 140, 1));
	}
	{
		GameEngineTransform* ColTrans = WalkFallCol->GetTransform();

		LocalWalkFallColPos = float4(45, 55, 0);

		ColTrans->SetLocalPosition(LocalWalkFallColPos);
		ColTrans->SetWorldScale(float4(10, 170, 1));
	}

	{
		GameEngineTransform* ColTrans = FindCol->GetTransform();

		LocalFindColPos = float4(200, 50, 0);

		ColTrans->SetLocalPosition(LocalFindColPos);
		ColTrans->SetWorldScale(float4(400, 100, 1));
	}

	{
		GameEngineTransform* ColTrans = ChasingCol->GetTransform();

		LocalChasingColPos = float4(50, 50, 0);

		ColTrans->SetLocalPosition(LocalChasingColPos);
		ColTrans->SetWorldScale(float4(100, 100, 1));
	}

	LoadFindEffectPos = float4(0, 150, 0);
}

void GiantEnt::Attack_Enter()
{
	GameEngineTransform* PlayerTrans = PlayerActor->GetTransform();
	GameEngineTransform* MonsterTrans = GetTransform();

	float4 TargetDir = PlayerTrans->GetWorldPosition() - MonsterTrans->GetWorldPosition();
	float Distance = TargetDir.Size();


	// 거리가 가까우면 일반 공격
	// 
	// 
	// 거리가 멀면 원거리 공격

	//IsShotProjectile = false;
	//IsAttackStempEffect = false;
	//
	//if (0 == GameEngineRandom::MainRandom.RandomInt(0, 1))
	//{
	//	IsTackleAttack = true;
	//	IsTackleEffect = false;
	//
	//	if ("" == AnimColMeta_Tackle.GetAnimationName())
	//	{
	//		MsgAssert_Rtti<GiantEnt>(" - 칼레온중보병의 태클 공격 정보가 존재하지 않습니다");
	//	}
	//
	//	Render->ChangeAnimation(AnimColMeta_Tackle.GetAnimationName());
	//	AttackRigidbody.SetVelocity(float4::Zero);
	//
	//	AttackCheckFrame = 0;
	//	CurPauseTime = 0.0f;
	//}
	//else
	//{
	//	BaseMonster::Attack_Enter();
	//}
}

void GiantEnt::Attack_Update(float _DeltaTime)
{
	//if (true == IsTackleAttack)
	//{
	//	if (true == HitCheck())
	//	{
	//		return;
	//	}
	//
	//	if (1 == Render->GetCurrentFrame())
	//	{
	//		if (false == IsTackleEffect)
	//		{
	//			IsTackleEffect = true;
	//
	//			// ManAtArms_TackleFlash
	//
	//			float4 SmokeEffectPos = GetTransform()->GetWorldPosition();
	//
	//			switch (Dir)
	//			{
	//			case ActorViewDir::Left:
	//				SmokeEffectPos += float4(150, 0, -0.1f);
	//				break;
	//			case ActorViewDir::Right:
	//				SmokeEffectPos += float4(-150, 0, -0.1f);
	//				break;
	//			default:
	//				break;
	//			}
	//
	//			std::shared_ptr<EffectActor> SmokeEffect = EffectManager::PlayEffect({
	//				.EffectName = "ManAtArms_TackleSmoke",
	//				.Postion = SmokeEffectPos,
	//				.FlipX = Dir == ActorViewDir::Left,
	//				});
	//
	//			SmokeEffect->GetTransform()->SetParent(GetTransform());
	//
	//			float4 FlashEffectPos = GetTransform()->GetWorldPosition();
	//
	//			switch (Dir)
	//			{
	//			case ActorViewDir::Left:
	//				FlashEffectPos += float4(60, 80, -0.1f);
	//				break;
	//			case ActorViewDir::Right:
	//				FlashEffectPos += float4(-60, 80, -0.1f);
	//				break;
	//			default:
	//				break;
	//			}
	//
	//			std::shared_ptr<EffectActor> FlashEffect = EffectManager::PlayEffect({
	//				.EffectName = "ManAtArms_TackleFlash",
	//				.Postion = FlashEffectPos,
	//				.FlipX = Dir == ActorViewDir::Left,
	//				});
	//
	//			FlashEffect->GetTransform()->SetParent(GetTransform());
	//		}
	//
	//		switch (Dir)
	//		{
	//		case ActorViewDir::Left:
	//			AttackRigidbody.SetVelocity(float4::Left * 600.0f);
	//			break;
	//		case ActorViewDir::Right:
	//			AttackRigidbody.SetVelocity(float4::Right * 600.0f);
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//
	//	Fall(_DeltaTime);
	//	AttackRigidbody.UpdateForce(_DeltaTime);
	//
	//	size_t CurFrame = Render->GetCurrentFrame();
	//
	//	if (AttackCheckFrame != CurFrame)
	//	{
	//		AttackCheckFrame = CurFrame;
	//		CurPauseTime = 0.0f;
	//	}
	//
	//	CurPauseTime += _DeltaTime;
	//
	//	if (TacklePauseTimes[AttackCheckFrame] > CurPauseTime)
	//	{
	//		Render->SetAnimPauseOn();
	//	}
	//	else
	//	{
	//		Render->SetAnimPauseOff();
	//	}
	//
	//	if (nullptr == PlatformColCheck(WalkCol))
	//	{
	//		float4 Velocity = AttackRigidbody.GetVelocity() * _DeltaTime;
	//		GetTransform()->AddLocalPosition(Velocity);
	//	}
	//
	//	if (true == Render->IsAnimationEnd())
	//	{
	//		MonsterFsm.ChangeState("Idle");
	//		return;
	//	}
	//}
	//else
	//{
	//	BaseMonster::Attack_Update(_DeltaTime);
	//
	//	if (false == IsAttackStempEffect && 3 == Render->GetCurrentFrame())
	//	{
	//		IsAttackStempEffect = true;
	//
	//		float4 EffectPos = GetTransform()->GetWorldPosition();
	//
	//		switch (Dir)
	//		{
	//		case ActorViewDir::Left:
	//			EffectPos += float4(-40, 0, -0.1f);
	//			break;
	//		case ActorViewDir::Right:
	//			EffectPos += float4(40, 0, -0.1f);
	//			break;
	//		default:
	//			break;
	//		}
	//
	//		EffectManager::PlayEffect({
	//			.EffectName = "ManAtArms_StempEffect",
	//			.Postion = EffectPos });
	//
	//		// CameraShake
	//
	//		std::shared_ptr<ContentLevel> ContentLevelPtr = GetLevel()->DynamicThis<ContentLevel>();
	//
	//		if (nullptr != ContentLevelPtr)
	//		{
	//			ContentLevelPtr->GetCamCtrl().CameraShake(4, 40, 2);
	//		}
	//	}
	//}
}