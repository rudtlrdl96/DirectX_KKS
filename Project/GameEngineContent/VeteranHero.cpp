#include "PrecompileHeader.h"
#include "VeteranHero.h"
#include "HealthBar.h"
#include "VeteranHero_HealthBar.h"
#include "MonsterDeadBodyActor.h"
#include "Player.h"
#include "VeteranHeroMagicSword.h"

VeteranHero::VeteranHero()
{
}

VeteranHero::~VeteranHero()
{
}

void VeteranHero::Death()
{
	BossMonster::Death();
	GetContentLevel()->StopCustomBgm();
}

void VeteranHero::Destroy()
{
	ContentLevel* LevelPtr = GetContentLevel();

	LevelPtr->RemoveEvent("VeteranHero_Script02_End", GetActorCode());
	LevelPtr->RemoveEvent("VeteranHero_Script01_End", GetActorCode());
	LevelPtr->RemoveEvent("VeteranHero_Script00_End", GetActorCode());

	for (size_t i = 0; i < MagicSwordProjectiles.size(); i++)
	{
		MagicSwordProjectiles[i]->FadeDeath();
	}

	MagicSwordProjectiles.clear();
}

void VeteranHero::HitMonster(float _Damage, ActorViewDir _HitDir, bool _IsStiffen, bool _IsPush, bool _IsMagicAttack, HitEffectType _Type, std::function<void()> _KillEvent /*= nullptr*/)
{
	BossMonster::HitMonster(_Damage, _HitDir, _IsStiffen, _IsPush, _IsMagicAttack, _Type, _KillEvent);

	HealthBarActiveTime = 3.0f;
}

void VeteranHero::Start()
{
	BossMonster::Start();
	SetViewDir(ActorViewDir::Right);

	BubblePivot = float4(0, 150, -100);

	BossRigidbody.SetGravity(ContentConst::Gravity_f - 2000.0f);
	BossRigidbody.SetActiveGravity(true);
	BossRigidbody.SetFricCoeff(3000.0f);
	DashPower = 1450.0f;
	BackDashPower = 1050.0f;

	PatternWaitTime = 1.5f;

	PauseTimes["DeathIntro"][0] = 0.3f;

	HealthBarPtr = GetLevel()->CreateActor<HealthBar>();
	HealthBarPtr->GetTransform()->SetParent(GetTransform());
	HealthBarPtr->GetTransform()->SetLocalPosition(float4(0, -15, -10));
	HealthBarPtr->SetTexture("EnemyHpFrame.png", "EnemyHpBar.png", "EnemySubBar.png", HealthBarScale);
	HealthBarPtr->Off();

	HeroHealthBar = GetLevel()->CreateActor<VeteranHero_HealthBar>();
	HeroHealthBar->Off();

	BodyCol->GetTransform()->SetLocalPosition(float4(0, 60, 1));
	BodyCol->GetTransform()->SetLocalScale(float4(80, 120, 1));

	WalkCol->GetTransform()->SetLocalPosition(float4(47, 65, 1));
	WalkCol->GetTransform()->SetLocalScale(float4(7, 120, 1));

	BackCol->GetTransform()->SetLocalPosition(float4(-47, 65, 1));
	BackCol->GetTransform()->SetLocalScale(float4(7, 120, 1));

	GroundCol->GetTransform()->SetLocalPosition(float4(0, 3.0f, 1));
	GroundCol->GetTransform()->SetLocalScale(float4(20, 5.0f, 1));

	EventCol = CreateComponent<GameEngineCollision>();
	EventCol->GetTransform()->SetWorldPosition(float4(1248, 564, 1));
	EventCol->GetTransform()->SetLocalScale(float4(1220, 640, 1));
	EventCol->SetColType(ColType::AABBBOX2D);

	AttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	AttackCol->SetColType(ColType::AABBBOX2D);

	StingerAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	StingerAttackCol->SetColType(ColType::AABBBOX2D);
	StingerAttackCol->GetTransform()->SetLocalPosition(float4(0, 35, 1));
	StingerAttackCol->GetTransform()->SetWorldScale(float4(150, 50, 1));
	StingerAttackCol->Off();

	WaveSmokeCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	WaveSmokeCol->GetTransform()->SetWorldScale(float4(40, 100, 1));
	WaveSmokeCol->GetTransform()->SetWorldRotation(float4::Zero);
	WaveSmokeCol->SetColType(ColType::AABBBOX2D);
	WaveSmokeCol->Off();

	StingerSwordAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	StingerSwordAttackCol->GetTransform()->SetWorldScale(float4(330, 240, 1));
	StingerSwordAttackCol->GetTransform()->SetWorldRotation(float4::Zero);
	StingerSwordAttackCol->SetColType(ColType::AABBBOX2D);
	StingerSwordAttackCol->Off();
	
	ExplosionCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	ExplosionCol->GetTransform()->SetLocalPosition(float4(0, 5, 0));
	ExplosionCol->GetTransform()->SetLocalScale(float4(450, 450, 1));
	ExplosionCol->Off();

	ExplosionChargeCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	ExplosionChargeCol->GetTransform()->SetLocalPosition(float4(0, 0, 0));
	ExplosionChargeCol->GetTransform()->SetLocalScale(float4(1, 1, 1));
	ExplosionChargeCol->Off();

	JumpAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	JumpAttackCol->SetColType(ColType::AABBBOX2D);
	JumpAttackCol->Off();

	FindLandingCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::Unknown);
	FindLandingCol->GetTransform()->SetWorldScale(float4(20, 100000, 1));
	FindLandingCol->Off();

	LandingAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	LandingAttackCol->GetTransform()->SetLocalPosition(float4(0, 50, 1));
	LandingAttackCol->GetTransform()->SetLocalScale(float4(110, 100, 1));
	LandingAttackCol->Off();

	UltimateStingerAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	UltimateStingerAttackCol->SetColType(ColType::AABBBOX2D);
	UltimateStingerAttackCol->GetTransform()->SetLocalPosition(float4(0, 35, 1));
	UltimateStingerAttackCol->GetTransform()->SetWorldScale(float4(150, 50, 1));
	UltimateStingerAttackCol->Off();

	ExplosionChargeScaleStart = float4(200, 200, 1);
	ExplosionChargeScaleEnd = float4(450, 450, 1);

	UltimateFinishAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::MonsterAttack);
	UltimateFinishAttackCol->GetTransform()->SetLocalPosition(float4(-5, 0));
	UltimateFinishAttackCol->SetColType(ColType::AABBBOX2D);
	UltimateFinishAttackCol->Off();

	SecondFinishScaleStart = float4(1, 2000);
	SecondFinishScaleEnd = float4(290, 2000);

	SwordRender = CreateComponent<ContentSpriteRenderer>();
	SwordRender->PipeSetting("2DTexture_Color");
	SwordRender->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", SwordBuffer);
	SwordRender->SetScaleToTexture("AdventurerHero_Sword.png");
	float4 TextureSclae = SwordRender->GetTransform()->GetLocalScale();
	SwordRender->GetTransform()->SetLocalScale(TextureSclae * 2.0f);
	SwordRender->Off();


	Battle_Platform_Left = CreateComponent<GameEngineCollision>((int)CollisionOrder::Platform_Normal);
	Battle_Platform_Left->SetColType(ColType::AABBBOX2D);
	Battle_Platform_Left->GetTransform()->SetWorldPosition(float4(705, 565, 1));
	Battle_Platform_Left->GetTransform()->SetWorldScale(float4(64, 640, 1));
	Battle_Platform_Left->GetTransform()->SetWorldRotation(float4::Zero);
	Battle_Platform_Left->Off();

	Battle_Platform_Right = CreateComponent<GameEngineCollision>((int)CollisionOrder::Platform_Normal);
	Battle_Platform_Right->SetColType(ColType::AABBBOX2D);
	Battle_Platform_Right->GetTransform()->SetWorldPosition(float4(2050, 565, 1));
	Battle_Platform_Right->GetTransform()->SetWorldScale(float4(64, 640, 1));
	Battle_Platform_Right->GetTransform()->SetWorldRotation(float4::Zero);
	Battle_Platform_Right->Off();

	UltimateLight = CreateComponent<ContentSpriteRenderer>();
	UltimateLight->PipeSetting("2DTexture_Color");
	UltimateLight->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", UltimateLightBuffer);
	UltimateLight->SetTexture("FadeImage.png");
	UltimateLight->GetTransform()->SetWorldPosition(float4(0, 0, 1.0f));
	UltimateLight->GetTransform()->SetWorldRotation(float4::Zero);
	UltimateLight->GetTransform()->SetWorldScale(float4(100000, 100000, 1));
	UltimateLight->Off();

	UltimateFade = CreateComponent<ContentSpriteRenderer>();
	UltimateFade->PipeSetting("2DTexture_Color");
	UltimateFade->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", UltimateColorBuffer);
	UltimateFade->SetTexture("FadeImage.png");
	UltimateFade->GetTransform()->SetWorldPosition(float4(0, 0, -999.0f));
	UltimateFade->GetTransform()->SetWorldRotation(float4::Zero);
	UltimateFade->GetTransform()->SetWorldScale(float4(100000, 100000, 1));
	UltimateFade->Off();

	AttackCheck.SetCol(AttackCol, (UINT)CollisionOrder::Player);
	AttackCheck.SetRender(Render);
	


	SwordRigidbody.SetMaxSpeed(3000.0f);
	SwordRigidbody.SetFricCoeff(500.0f);

	ContentLevel* LevelPtr = GetContentLevel();
	//

	LevelPtr->AddEvent("VeteranHero_Script02_End", GetActorCode(), [this]()
		{
			IsIntro = false;
			HeroHealthBar->DropFrame();
			HeroHealthBar->On();
			GetContentLevel()->PlayCustomBgm("Chapter1_Boss.wav");
		});

	LevelPtr->AddEvent("VeteranHero_Script01_End", GetActorCode(), [this]()
		{
			IsBehaviorLoop = true;
			PlayBehaviorAnim = "Intro_Throwing";
			BossFsm.ChangeState("Behavior");

			SwordRigidbody.SetActiveGravity(true);
			SwordRigidbody.SetVelocity(float4(0, 700, 0));
			SwordRigidbody.SetGravity(-900.0f);

			SwordRender->GetTransform()->SetLocalRotation(float4(0, 0, 180));
			SwordRender->GetTransform()->SetLocalPosition(float4(20, 170, 0));
			SwordRender->On();
			IsSwordThrowing = true;
		});

	LevelPtr->AddEvent("VeteranHero_Script00_End", GetActorCode(), [this]()
		{
			IsIntroJump = true;
			PlayAnimation("Intro_LandReady");

			GetTransform()->SetWorldPosition(float4(1970, 800));
			BossRigidbody.SetVelocity(float4(-1000, -500, 0));
			BossRigidbody.SetGravity(ContentConst::Gravity_f - 1000.0f);
			IsGroundUp = false;

			SetViewDir(ActorViewDir::Left);
		});

	if (false == GameEngineInput::IsKey("Debug_VeteranHeroMove"))
	{
		GameEngineInput::CreateKey("Debug_VeteranHeroMove", 'L');
	}

	//IsIntro = false;

	//처음 패턴 강제설정
	Cur_Pattern_Enter = std::bind(&VeteranHero::Stinger_Enter, this);
	Cur_Pattern_Update = std::bind(&VeteranHero::Stinger_Update, this, std::placeholders::_1);
	Cur_Pattern_End = std::bind(&VeteranHero::Stinger_End, this);
	AttackDistance = 600.0f;
}

void VeteranHero::Update(float _DeltaTime)
{
	SpeechCoolTime += _DeltaTime;

	if (nullptr != FindPlayer && true == FindPlayer->IsDeath())
	{
		FindPlayer = nullptr;
	}

	if (nullptr != LandingSignEffect && true == LandingSignEffect->IsDeath())
	{
		LandingSignEffect = nullptr;
	}

	if (true == GameEngineInput::IsDown("Debug_VeteranHeroMove"))
	{
		GetTransform()->SetWorldPosition(float4(800, 250));
	}

	if (true == IsSwordThrowing)
	{
		SwordRigidbody.UpdateForce(_DeltaTime);

		float4 Vel = SwordRigidbody.GetVelocity();
		float4 CurFrameVel = Vel * _DeltaTime;

		SwordRender->GetTransform()->AddLocalPosition(CurFrameVel);
		SwordRender->GetTransform()->AddLocalRotation(float4(0, 0, _DeltaTime * 1200.0f));

		if (170.0f > SwordRender->GetTransform()->GetLocalPosition().y)
		{
			SwordRender->Off();
			SwordRigidbody.SetActiveGravity(false);
			SwordRigidbody.SetVelocity(float4::Zero);
			IsSwordThrowing = false;

			IsBehaviorLoop = false;
			PlayBehaviorAnim = "Intro_LandEnd";
			BossFsm.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				GetContentLevel()->CallEvent("VeteranHero_Script02");
				PlayAnimation("Intro_LandEndWait");
			};
		}
	}

	if (true == IsIntroJump)
	{
		BossRigidbody.UpdateForce(_DeltaTime);
		float4 IntroVel =  BossRigidbody.GetVelocity();

		if (true == IsGroundUp)
		{
			BossRigidbody.SetVelocity(float4::Zero);

			IsBehaviorLoop = false;
			PlayBehaviorAnim = "Intro_Land";
			BossFsm.ChangeState("Behavior");

			BehaviorEndCallback = [this]()
			{
				GetContentLevel()->CallEvent("VeteranHero_Script01");
				PlayAnimation("Intro_Land", true, 26);
			};

			IsGroundUp = false;
			IsIntroJump = false;
		}

		GetTransform()->AddLocalPosition(IntroVel * _DeltaTime);
		BossMonster::Update(_DeltaTime);
		return;
	}
	BossMonster::Update(_DeltaTime);
	if (true == IsUltimateLightOn)
	{
		UltimateLightProgress += _DeltaTime * 2.0f;

		UltimateLightBuffer.Color = float4::LerpClamp(
			float4(0, 0, 0, 0), float4(0, 0, 0, 0.6f), UltimateLightProgress);

		if (1.0f <= UltimateLightProgress)
		{
			IsUltimateLightOn = false;
		}
	}

	if (true == IsUltimateLightOff)
	{
		UltimateLightProgress += _DeltaTime * 2.0f;

		UltimateLightBuffer.Color = float4::LerpClamp(
			float4(0, 0, 0, 0.6f), float4(0, 0, 0, 0), UltimateLightProgress);

		if (1.0f <= UltimateLightProgress)
		{
			IsUltimateLightOff = false;
			UltimateLight->Off();
		}
	}

	if (true == IsUltimateFadeOn)
	{
		UltimateFadeProgress += _DeltaTime * 1.5f;

		float Progress = UltimateFadeProgress - 0.2f;

		UltimateColorBuffer.Color = float4::LerpClamp(
			float4(1, 1, 1, 0.6f), float4(1, 1, 1, 0), Progress);

		if (1.0f <= Progress)
		{
			IsUltimateFadeOn = false;
			UltimateFade->Off();
		}
	}

	HealthBarPtr->UpdateBar(HP / Data.HP, _DeltaTime);

	if (nullptr != HeroHealthBar)
	{
		HeroHealthBar->UpdateBar(HP / Data.HP, _DeltaTime);
	}

	if (0.0f >= HP)
	{
		if (nullptr != Bubble)
		{
			Bubble->Death();
			Bubble = nullptr;
		}

		if (false == IsDeathIntro)
		{
			if (nullptr != SecondUltimateStingerEffect)
			{
				SecondUltimateStingerEffect->IsFadeDeathOn(1.5f);
				SecondUltimateStingerEffect = nullptr;
			}

			if (true == UltimateLight->IsUpdate())
			{
				UltimateLightOff();
			}

			for (size_t i = 0; i < MagicSwordProjectiles.size(); i++)
			{
				MagicSwordProjectiles[i]->FadeDeath();
			}

			MagicSwordProjectiles.clear();

			IsDeathIntro = true;
			HealthBarPtr->Off();
			HeroHealthBar->Death();
			HeroHealthBar = nullptr;

			PlayAnimation("DeathIntro", false);
			GetContentLevel()->CallEvent("VeteranHero_Death");
			GetContentLevel()->GetCamCtrl().CameraShake(1, 1, 0);
		}

		if (true == Render->IsAnimationEnd())
		{
			Death();

			std::shared_ptr<MonsterDeadBodyActor> DeadBody = GetLevel()->CreateActor<MonsterDeadBodyActor>();

			DeadBody->SetTexture("RookieHero_DeadBody.png", 2.0f);
			DeadBody->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + float4(0, -2, -1.0f));

			if (ActorViewDir::Left == Dir)
			{
				DeadBody->GetTransform()->SetLocalNegativeScaleX();
			}
		}

		return;
	}

	UltimateTime += _DeltaTime;

	if (false == IsPlayerEnter)
	{
		std::shared_ptr<GameEngineCollision> PlayerCol =
			EventCol->Collision((int)CollisionOrder::Player, ColType::AABBBOX2D, ColType::AABBBOX2D);

		if (nullptr == PlayerCol)
		{
			return;
		}

		FindPlayer = PlayerCol->GetActor()->DynamicThis<Player>();

		if (nullptr == FindPlayer)
		{
			MsgAssert_Rtti<VeteranHero>(" - 플레이어 클래스만 Player ColOrder를 가질 수 있습니다");
		}

		if (nullptr != PlayerCol)
		{
			GetContentLevel()->CallEvent("GoodsUIOff");
			GetContentLevel()->CallEvent("MinimapOff");
			GetContentLevel()->CallEvent("VeteranHero_Intro");

			IsPlayerEnter = true;

			Battle_Platform_Left->On();
			Battle_Platform_Right->On();

			GetContentLevel()->StopBaseBGM();
		}
	}


	if (nullptr != ExplosionEffect && ExplosionEffect->IsDeath())
	{
		ExplosionEffect = nullptr;
	}

	if (nullptr != UltimateSmokeEffect && UltimateSmokeEffect->IsDeath())
	{
		UltimateSmokeEffect = nullptr;
	}

	if (nullptr != StingerEffect && StingerEffect->IsDeath())
	{
		StingerEffect = nullptr;
	}

	HealthBarActiveTime -= _DeltaTime;

	if (0.0f < HealthBarActiveTime)
	{
		HealthBarPtr->On();
	}
	else
	{
		HealthBarPtr->Off();
	}
}

void VeteranHero::DataLoad()
{
	Data = ContentDatabase<MonsterData, LevelArea>::GetData(190); // 180 = 각성 용사
}

void VeteranHero::CreateAnimation()
{
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Data");
		Path.Move("3_ForestOfHarmony");
		Path.Move("Boss");

		AttackA_Data = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("RookieHero_AttackA").GetFullPath());
		AttackB_Data = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("RookieHero_AttackB").GetFullPath());
		AttackD_Data = ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("RookieHero_AttackD").GetFullPath());

		Render->CreateAnimation({
			.AnimationName = AttackA_Data.GetAnimationName().data(),
			.SpriteName = AttackA_Data.GetSpriteName().data(),
			.Start = AttackA_Data.GetStartFrame(),
			.End = AttackA_Data.GetEndFrame(),
			.FrameInter = 0.11f,
			.Loop = false,
			.ScaleToTexture = true });

		Render->CreateAnimation({
			.AnimationName = AttackB_Data.GetAnimationName().data(),
			.SpriteName = AttackB_Data.GetSpriteName().data(),
			.Start = AttackB_Data.GetStartFrame(),
			.End = AttackB_Data.GetEndFrame(),
			.FrameInter = 0.11f,
			.Loop = false,
			.ScaleToTexture = true });

		Render->CreateAnimation({
			.AnimationName = AttackD_Data.GetAnimationName().data(),
			.SpriteName = AttackD_Data.GetSpriteName().data(),
			.Start = AttackD_Data.GetStartFrame(),
			.End = AttackD_Data.GetEndFrame(),
			.FrameInter = 0.11f,
			.Loop = false,
			.ScaleToTexture = true });
	}

	Render->CreateAnimation({ .AnimationName = "Idle", .SpriteName = "RookieHero_Idle.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_ComboA", .SpriteName = "RookieHero_Intro_ComboA.png",
		.Start = 0, .End = 43, .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_ComboB", .SpriteName = "RookieHero_Intro_ComboB.png",
		.Start = 0, .End = 25, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Yeah", .SpriteName = "RookieHero_Yeah.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_lol", .SpriteName = "RookieHero_lol.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_lol_End", .SpriteName = "RookieHero_lol_End.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Who", .SpriteName = "RookieHero_Who.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_LandReady", .SpriteName = "RookieHero_LandingReady.png", .FrameInter = 0.05f, .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Land", .SpriteName = "RookieHero_LandingIntro.png", .FrameInter = 0.035f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_LandEnd", .SpriteName = "RookieHero_LandingEnd.png", .FrameInter = 0.05f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_LandEndWait", .SpriteName = "RookieHero_LandingEndWait.png", .FrameInter = 0.05f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Intro_Throwing", .SpriteName = "RookieHero_Throwing.png", .FrameInter = 0.05f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Potion", .SpriteName = "RookieHero_Potion.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Hit", .SpriteName = "RookieHero_Hit.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Dash", .SpriteName = "RookieHero_Dash.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "BackDash", .SpriteName = "RookieHero_BackDash.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Groggy", .SpriteName = "RookieHero_Groggy.png",.FrameInter = 0.15f, .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "EnergyBallReady", .SpriteName = "RookieHero_EnergyBallReady.png", .FrameInter = 0.06666f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "EnergyBall", .SpriteName = "RookieHero_EnergyBall.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "ExplosionReady", .SpriteName = "RookieHero_ExplosionReady.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Explosion", .SpriteName = "RookieHero_ExplosionLoop.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "AttackA_Ready", .SpriteName = "RookieHero_AttackAReady.png", .FrameInter = 0.125f, .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "AttackC", .SpriteName = "RookieHero_AttackC.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "AttackE", .SpriteName = "RookieHero_AttackE.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "SwordEnergyReady", .SpriteName = "RookieHero_SwordEnergyReady.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "SwordEnergy", .SpriteName = "RookieHero_SwordEnergy.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "DeathIntro", .SpriteName = "RookieHero_DeadIntro.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "StingerReady", .SpriteName = "RookieHero_StingerReady.png", .Loop = false, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Stinger", .SpriteName = "RookieHero_Stinger.png", .Loop = true, .ScaleToTexture = true });
	Render->CreateAnimation({ .AnimationName = "Jump", .SpriteName = "RookieHero_Jump.png", .Loop = true, .ScaleToTexture = true });
}

void VeteranHero::SelectPattern()
{
	GameEngineRandom& Rand = GameEngineRandom::MainRandom;

	float CurHpRatio = HP / Data.HP;

	if(false == IsSecondUltimateShot && 0.333334f >= CurHpRatio)
	{
		IsSecondUltimateShot = true;

		Cur_Pattern_Enter = std::bind(&VeteranHero::SecondUltimate_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::SecondUltimate_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::SecondUltimate_End, this);

		AttackDistance = 2000.0f;
		return;
	}

	// Ultimate
	if (0.666667f >= CurHpRatio && 0.0f <= UltimateTime)
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::Ultimate_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::Ultimate_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::Ultimate_End, this);

		AttackDistance = 300.0f;
		UltimateTime = -30.0f;
		return;
	}

	switch (Rand.RandomInt(0, 6))
	{
	case 0: // ComboAttack
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::ComboAttack_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::ComboAttack_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::ComboAttack_End, this);
		AttackDistance = 400.0f;
	}
	break;
	case 1: // EnergyBall
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::EnergyBall_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::EnergyBall_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::EnergyBall_End, this);
		AttackDistance = 1000.0f;
	}
	break;
	case 2: // Explosion
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::Explosion_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::Explosion_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::Explosion_End, this);
		AttackDistance = 230.0f;
	}
		break;
	case 3: // Stinger
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::Stinger_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::Stinger_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::Stinger_End, this);
		AttackDistance = 600.0f;
	}
	break;
	case 4: // SwordWave
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::SwordWave_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::SwordWave_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::SwordWave_End, this);
		AttackDistance = 700.0f;
	}
	break;
	case 5: // JumpAttack
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::JumpAttack_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::JumpAttack_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::JumpAttack_End, this);
		AttackDistance = 300.0f;
	}
		break;
	case 6: // LandingAttack
	{
		Cur_Pattern_Enter = std::bind(&VeteranHero::LandingAttack_Enter, this);
		Cur_Pattern_Update = std::bind(&VeteranHero::LandingAttack_Update, this, std::placeholders::_1);
		Cur_Pattern_End = std::bind(&VeteranHero::LandingAttack_End, this);
		AttackDistance = 2000.0f;
	}
	break;
	default:
		MsgAssert_Rtti<VeteranHero>(" - 존재하지 않는 패턴으로 설정하려 했습니다");
		break;
	}
}


void VeteranHero::UltimateLightOn()
{
	IsUltimateLightOn = true;
	IsUltimateLightOff = false;
	UltimateLightProgress = 0.0f;

	UltimateLight->On();
	UltimateLightBuffer.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
}


void VeteranHero::UltimateLightOff()
{
	IsUltimateLightOn = false;
	IsUltimateLightOff = true;
	UltimateLightProgress = 0.0f;

	UltimateLight->On();
	UltimateLightBuffer.Color = float4(0.0f, 0.0f, 0.0f, 0.6f);
}


void VeteranHero::UltimateFadeOn()
{
	IsUltimateFadeOn = true;
	UltimateFadeProgress = 0.0f;

	UltimateFade->On();
	UltimateColorBuffer.Color = float4(1.0f, 1.0f, 1.0f, 0.6f);
}