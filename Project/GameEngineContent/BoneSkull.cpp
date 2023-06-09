#include "PrecompileHeader.h"
#include "BoneSkull.h"

#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineCollision.h>

#include "Player.h"
#include "BoneHead.h"
#include "EffectManager.h"
#include "Inventory.h"

BoneSkull::BoneSkull()
{
}

BoneSkull::~BoneSkull()
{
}

void BoneSkull::SetBoneSkullState(BoneSkullState _State)
{
	State = _State;

	switch (State)
	{
	case BoneSkull::BoneSkullState::Normal:
		AnimNamePlusText = "";
		break;
	case BoneSkull::BoneSkullState::NoHead:
		AnimNamePlusText = "_NoHead";
		break;
	case BoneSkull::BoneSkullState::NoWeapon:
		AnimNamePlusText = "_NoWeapon";
		break;
	default:
		break;
	}
}

void BoneSkull::Death()
{
	PlayerBaseSkull::Death();

	if (nullptr != HeadActor)
	{
		HeadActor->Death();
		HeadActor = nullptr;
	}
}

void BoneSkull::Destroy()
{
	ContentLevel* LevelPtr = GetContentLevel();

	LevelPtr->RemoveEvent("SkeleTong_Script00", GetActorCode());
	LevelPtr->RemoveEvent("SkeleTongWalkEnd", GetActorCode());
	LevelPtr->RemoveEvent("SkeleTong_Script01_End", GetActorCode());
	LevelPtr->RemoveEvent("SkeleTong_Script02_End", GetActorCode());
	LevelPtr->RemoveEvent("SkeleTong_Script03_End", GetActorCode());
	LevelPtr->RemoveEvent("Suekeleton_Script01_End", GetActorCode());
	LevelPtr->RemoveEvent("Suekeleton_Script02_End", GetActorCode());
	LevelPtr->RemoveEvent("ChiefGuard_Script00_End", GetActorCode());
	LevelPtr->RemoveEvent("CastleReborn", GetActorCode());
}

void BoneSkull::Start()
{
	PlayerBaseSkull::Start();

	AttackEffectType = HitEffectType::Skull;

	HeadActor = GetLevel()->CreateActor<BoneHead>();
	HeadActor->SetParentSkull(this);
	HeadActor->Off();

	HeadPickupCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::Unknown);
	HeadPickupCol->GetTransform()->SetLocalPosition(float4(0.0f, 30.0f, 1.0f));
	HeadPickupCol->GetTransform()->SetWorldScale(float4(30.0f, 60.0f, 1.0f));
	HeadPickupCol->GetTransform()->SetWorldRotation(float4::Zero);

	Switch_DamageRatio = 0.5f;

	ContentLevel* LevelPtr = GetContentLevel();

	if (nullptr == LevelPtr)
	{
		return;
	}

	LevelPtr->AddEvent("SkeleTong_Script00", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_Idle";
			PlayerFSM.ChangeState("Behavior");
		});

	LevelPtr->AddEvent("SkeleTongWalkEnd", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_WakeUp";
			PlayerFSM.ChangeState("Behavior");
		});	

	LevelPtr->AddEvent("SkeleTong_Script01_End", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_GetHead";
			PlayerFSM.ChangeState("Behavior");

			IsBehaviorAnimPause = true;
			PauseFrame = 11;
			PauseTime = 0.5f;

			PauseEndCallback = [this]()
			{
				PauseFrame = 49;
				PauseTime = 0.25f;
				PauseEndCallback = nullptr;
				IsBehaviorAnimPause = true;
			};

			BehaviorEndCallback = [this]()
			{
				BehaviorAnimationName = "Idle_NoWeapon";
				PlayerFSM.ChangeState("Behavior");
				GetContentLevel()->CallEvent("SkeleTong_Script02");
			};

		});

	LevelPtr->AddEvent("SkeleTong_Script02_End", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_Awkward";
			PlayerFSM.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				BehaviorAnimationName = "Idle_NoWeapon";
				PlayerFSM.ChangeState("Behavior");
				GetContentLevel()->CallEvent("SkeleTong_Script03");

				BehaviorEndCallback = nullptr;
			};
		});

	LevelPtr->AddEvent("SkeleTong_Script03_End", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_Awkward";
			PlayerFSM.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				GetContentLevel()->CallEvent("StoryFadeOut");
				GetContentLevel()->CallEvent("PlayerInputUnlock");

				SetBoneSkullState(BoneSkullState::NoWeapon);
				PlayerFSM.ChangeState("Idle");

				IsLockAttack = true;

				IsLockSkillA = true;
				IsActiveSkillA_Value = false;

				IsLockSkillB = true;
				IsActiveSkillB_Value = false;

				ParentPlayer->ActivePlayerFrame();
			};
		});
		
	LevelPtr->AddEvent("Suekeleton_Script01_End", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_Getbone";
			PlayerFSM.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				SetBoneSkullState(BoneSkullState::Normal);
				PlayerFSM.ChangeState("Idle");
			};
		});

	LevelPtr->AddEvent("Suekeleton_Script02_End", GetActorCode(), [this]()
		{
			IsLockAttack = false;

			IsLockSkillA = false;
			IsActiveSkillA_Value = true;

			IsLockSkillB = false;
			IsActiveSkillB_Value = true;

			BehaviorEndCallback = [this]()
			{
				SetBoneSkullState(BoneSkullState::Normal);
				PlayerFSM.ChangeState("Idle");
			};
		});

	LevelPtr->AddEvent("ChiefGuard_Script00_End", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Intro_GetChiefHead";
			PlayerFSM.ChangeState("Behavior");

			std::function<void()> PauseCall02 = [this]()
			{
				GetContentLevel()->GetCamCtrl().CameraShake(20, 20, 12);
				float4 EffectPos = ParentPlayer->GetTransform()->GetWorldPosition();

				EffectManager::PlayEffect({
					.EffectName = "GetSkull_Thunder_Effect",
					.Position = EffectPos,
					.WaitTime = 0.0f });

				EffectManager::PlayEffect({
					.EffectName = "GetSkull_Legendary_Smoke",
					.Position = EffectPos,
					.WaitTime = 0.0f });
			
				EffectManager::PlayEffect({
					.EffectName = "GetSkull_Legendary_ElectricUp",
					.Position = EffectPos,
					.WaitTime = 0.95f });

				EffectManager::PlayEffect({
					.EffectName = "GetSkull_Legendary_ElectricSide",
					.Position = EffectPos,
					.WaitTime = 1.9f });
			};

			std::function<void()> PauseCall01 = [this, PauseCall02]()
			{
				IsBehaviorAnimPause = true;
				PauseFrame = 22;
				PauseTime = 0.0f;

				PauseEndCallback = PauseCall02;
			};

			IsBehaviorAnimPause = true;
			PauseFrame = 8;
			PauseTime = 0.1f;

			PauseEndCallback = [this, PauseCall01]()
			{
				IsBehaviorAnimPause = true;
				PauseFrame = 20;
				PauseTime = 0.1f;

				PauseEndCallback = PauseCall01;
			};

			BehaviorEndCallback = [this]()
			{				
				ParentPlayer->InsertNewSkull(203);
				ParentPlayer->ForceSwapSkull();
				PlayerFSM.ChangeState("Idle");

				GetContentLevel()->CallEvent("PlayerFrameActive");
				GetContentLevel()->CallEvent("StoryFadeOut");
				GetContentLevel()->CallEvent("PlayerInputUnlock");
				GetContentLevel()->CallEvent("UnlockMonsterMove");
			};
		});

	LevelPtr->AddEvent("CastleReborn", GetActorCode(), [this]()
		{
			BehaviorAnimationName = "Castle_Reborn";
			PlayerFSM.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				PlayerFSM.ChangeState("Idle");
			};
		});
}

void BoneSkull::Update(float _DeltaTime)
{
	PlayerBaseSkull::Update(_DeltaTime);

	if (nullptr == HeadActor)
	{
		return;
	}

	if (true == HeadActor->IsUpdate())
	{
		if (1.0f <= HeadActor->ShotLiveTime)
		{

			if (nullptr != HeadPickupCol->Collision((int)CollisionOrder::BoneHead, ColType::AABBBOX2D, ColType::AABBBOX2D))
			{
				HeadReturn();
				return;
			}
		}		

		if (CurSkillATime >= GetSkillAEndTime())
		{
			HeadReturn();
		}

		IsLockSkillB = false;
	}
	else
	{
		IsLockSkillB = true;
	}

}

void BoneSkull::ChangeSwitchEnd()
{
	HeadReturn();
}

void BoneSkull::Idle_Enter()
{
	PlayerBaseSkull::Idle_Enter();

	WaitTime = 0.0f;
}

void BoneSkull::Idle_Update(float _DeltaTime)
{
	if (false == ParentPlayer->IsInputLock())
	{
		WaitTime += _DeltaTime;
	}

	if (8.0f <= WaitTime && State == BoneSkullState::Normal)
	{
		Render->ChangeAnimation("Idle_Wait", false);

		if (true == Render->IsAnimationEnd())
		{
			Render->ChangeAnimation("Idle", false);
			WaitTime = 0.0f;
		}
	}

	PlayerBaseSkull::Idle_Update(_DeltaTime);
}

void BoneSkull::Skill_SlotA_Enter()
{
	PlayerBaseSkull::Skill_SlotA_Enter();
	SetBoneSkullState(BoneSkullState::NoHead);

	HeadActor->ShotLiveTime = 0.0f;
	HeadActor->Off();

	CurSkillATime = 0.0f;
}

void BoneSkull::Skill_SlotA_Update(float _DeltaTime)
{
	PlayerBaseSkull::Skill_SlotA_Update(_DeltaTime);

	if (false == HeadActor->IsUpdate() && 1 == Render->GetCurrentFrame())
	{
		HeadActor->ShotHead(GetViewDir());
		HeadActor->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + float4(8, 50));
		HeadActor->On();
	}
}

void BoneSkull::Skill_SlotB_Enter()
{
	HeadReturn();

	GameEngineTransform* HeadTrans = HeadActor->GetTransform();

	float4 PlayerPos = PlayerTrans->GetWorldPosition();
	float4 HeadPos = HeadTrans->GetWorldPosition();

	PlayerTrans->SetWorldPosition(HeadPos - float4(0, 15));

	EffectManager::PlayEffect({ "SkullAppearance", PlayerPos });
	EffectManager::PlayEffect({ "SkullAppearance", HeadPos - float4(0, 0) });
	EffectManager::PlayEffect({ "LandSmoke", HeadPos - float4(0, 50) });

	PlayerBaseSkull::Skill_SlotB_Enter();
}

void BoneSkull::Switch_Enter()
{
	PlayerBaseSkull::Switch_Enter();
	DashRigidbody.SetVelocity(float4::Zero);

	float4 Vel = BattleActorRigidbody.GetVelocity(); 
	Vel.y = 0;
	BattleActorRigidbody.SetVelocity(Vel);

	//JumpDir.y = 0;
}

void BoneSkull::Switch_Update(float _DeltaTime)
{
	BattleActorRigidbody.AddVelocity(float4(0, _DeltaTime * ContentConst::Gravity_f, 0));
	PlayerTrans->AddLocalPosition(BattleActorRigidbody.GetVelocity() * _DeltaTime);

	std::shared_ptr<GameEngineCollision> GroundColPtr = ContentFunc::PlatformColCheck(GroundCol, true);

	if (nullptr != GroundColPtr)
	{
		float4 CurPos = PlayerTrans->GetWorldPosition();

		GameEngineTransform* ColTrans = GroundColPtr->GetTransform();
		CurPos.y = ColTrans->GetWorldPosition().y + ColTrans->GetWorldScale().hy();

		PlayerTrans->SetWorldPosition(CurPos);
		PlayerTrans->SetLocalPosition(PlayerTrans->GetLocalPosition());

		float4 Vel = BattleActorRigidbody.GetVelocity();
		Vel.y = 0;
		BattleActorRigidbody.SetVelocity(Vel);

		//JumpDir.y = 0.0f;
	}

	switch (GetViewDir())
	{
	case ActorViewDir::Left:
		DashRigidbody.SetVelocity(float4::Left * 175.0f);
		break;
	case ActorViewDir::Right:
		DashRigidbody.SetVelocity(float4::Right * 175.0f);
		break;
	default:
		break;
	}

	PlayerBaseSkull::Switch_Update(_DeltaTime);
}

void BoneSkull::Switch_End()
{
	PlayerBaseSkull::Switch_End();
	DashRigidbody.SetVelocity(float4::Zero);
}

void BoneSkull::DataLoad()
{
	Data = ContentDatabase<SkullData, SkullGrade>::GetData(0); // 0 == ��Ʋ��
}

void BoneSkull::TextureLoad()
{
	DeadPartNames.reserve(6);
	DeadPartNames.push_back("BoneSkul_Head.png");
	DeadPartNames.push_back("Skul_DeatParts01.png");
	DeadPartNames.push_back("Skul_DeatParts02.png");
	DeadPartNames.push_back("Skul_DeatParts03.png");
	DeadPartNames.push_back("Skul_DeatParts04.png");
	DeadPartNames.push_back("Skul_DeatParts05.png");
}

void BoneSkull::CreateAnimation()
{
	//Idle Animation
	Render->CreateAnimation({ .AnimationName = "Idle", .SpriteName = "BoneSkull_Idle.png", .FrameInter = 0.15f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Idle_NoHead", .SpriteName = "BoneSkull_Idle_NoHead.png", .FrameInter = 0.15f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Idle_NoWeapon", .SpriteName = "BoneSkull_Idle_NoWeapon.png", .FrameInter = 0.15f, .ScaleToTexture = true });

	//Walk Animation
	Render->CreateAnimation({ .AnimationName = "Walk", .SpriteName = "BoneSkull_Walk.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Walk_NoHead", .SpriteName = "BoneSkull_Walk_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Walk_NoWeapon", .SpriteName = "BoneSkull_Walk_NoWeapon.png", .FrameInter = 0.08f, .ScaleToTexture = true });

	//Dash Animation
	Render->CreateAnimation({ .AnimationName = "Dash", .SpriteName = "BoneSkull_Dash.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Dash_NoHead", .SpriteName = "BoneSkull_Dash_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Dash_NoWeapon", .SpriteName = "BoneSkull_Dash_NoWeapon.png", .FrameInter = 0.08f, .ScaleToTexture = true });

	// Fall
	Render->CreateAnimation({ .AnimationName = "Fall", .SpriteName = "BoneSkull_Fall.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Fall_NoHead", .SpriteName = "BoneSkull_Fall_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Fall_NoWeapon", .SpriteName = "BoneSkull_Fall_NoWeapon.png", .FrameInter = 0.08f, .ScaleToTexture = true });

	// Fall Repeat
	Render->CreateAnimation({ .AnimationName = "FallRepeat", .SpriteName = "BoneSkull_FallRepeat.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "FallRepeat_NoHead", .SpriteName = "BoneSkull_FallRepeat_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "FallRepeat_NoWeapon", .SpriteName = "BoneSkull_FallRepeat_NoWeapon.png", .FrameInter = 0.08f, .ScaleToTexture = true });

	// Jump
	Render->CreateAnimation({ .AnimationName = "Jump", .SpriteName = "BoneSkull_Jump.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Jump_NoHead", .SpriteName = "BoneSkull_Jump_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Jump_NoWeapon", .SpriteName = "BoneSkull_Jump_NoWeapon.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	
	// AttackA NoHead
	Render->CreateAnimation({ .AnimationName = "AttackA_NoHead", .SpriteName = "BoneSkull_AttackA_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// AttackB NoHead
	Render->CreateAnimation({ .AnimationName = "AttackB_NoHead", .SpriteName = "BoneSkull_AttackB_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// JumpAttack NoHead
	Render->CreateAnimation({ .AnimationName = "JumpAttack_NoHead", .SpriteName = "BoneSkull_JumpAttack_NoHead.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	
	// Behavior Anim
	Render->CreateAnimation({ .AnimationName = "Intro_Idle", .SpriteName = "BoneSkull_Intro_Idle.png", .FrameInter = 0.1f, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_WakeUp", .SpriteName = "BoneSkull_Intro_WlakUp.png", .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_GetHead", .SpriteName = "BoneSkull_Intro_GetHead.png", .FrameInter = 0.08f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Getbone", .SpriteName = "BoneSkull_Intro_Getbone.png", .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Awkward", .SpriteName = "BoneSkull_Intro_Awkward.png",
		.Start = 0, .End = 18, .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_GetChiefHead", .SpriteName = "BoneSkull_Normal_Getskull.png",
		.Start = 0, .End = 44, .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Castle_Reborn", .SpriteName = "BoneSkull_Normal_RebornCastle.png",
		.Start = 0, .End = 26, .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Idle_Wait", .SpriteName = "BoneSkull_Wait.png", .FrameInter = 0.11f, .ScaleToTexture = true });
}

void BoneSkull::AnimationColLoad()
{
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Data");
		Path.Move("8_Player");
		Path.Move("BoneSkull"); 
		
		Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_AttackA").GetFullPath()), 0.08f);
		Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_AttackB").GetFullPath()), 0.08f);
		Pushback_JumpAttack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_JumpAttack").GetFullPath()), 0.08f);
		Pushback_SkillA(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_SkillA").GetFullPath()), 0.1f);
		Pushback_SkillB(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_SkillB").GetFullPath()), 0.05f);

		AnimationAttackMetaData SwitchMeta = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("BoneSukll_Switch").GetFullPath());

		Pushback_Switch(SwitchMeta, 0.06f);
		Pushback_Switch(SwitchMeta, 0.06f);
		Pushback_Switch(SwitchMeta, 0.06f);
	}
}

void BoneSkull::HeadReturn()
{
	HeadActor->Off();
	SetBoneSkullState(BoneSkullState::Normal);

	size_t Frame = Render->GetCurrentFrame();

	switch (FsmState)
	{
	case PlayerBaseSkull::PlayerFSM_State::Idle:
		Render->ChangeAnimation("Idle", Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::Walk:
		Render->ChangeAnimation("Walk", Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::Fall:
		Render->ChangeAnimation("Fall", Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::Jump:
		Render->ChangeAnimation("Jump", Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::JumpAttack:
		Render->ChangeAnimation(std::string(AnimColMeta_JumpAttack[JumpAttackCombo].GetAnimationName()), Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::Attack:
		Render->ChangeAnimation(std::string(AnimColMeta_Attack[AttackComboCount].GetAnimationName()), Frame);
		break;
	case PlayerBaseSkull::PlayerFSM_State::Dash:
		Render->ChangeAnimation("Dash", Frame);
		break;
	default:
		break;
	}

	CurSkillATime = 1000.0f;
}