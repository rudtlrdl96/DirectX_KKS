#include "PrecompileHeader.h"
#include "MinotaurusSkull_Unique.h"
#include "BaseMonster.h"
#include "RigidProjectile.h"

MinotaurusSkull_Unique::MinotaurusSkull_Unique()
{
}

MinotaurusSkull_Unique::~MinotaurusSkull_Unique()
{
	AttackDoubleCheck.clear();
	PassiveDoubleCheck.clear();

	if (nullptr != SkillBPlowUpEffect)
	{
		SkillBPlowUpEffect->IsFadeDeathOn(2.0f);
		SkillBPlowUpEffect = nullptr;
	}
}

void MinotaurusSkull_Unique::Start()
{
	Type = SkullType::Power;

	PlayerBaseSkull::Start();

	IsActiveSkillA_Value = true;
	IsActiveSkillB_Value = true;

	AttackEffectType = HitEffectType::MinoTaurus;

	JumpAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::PlayerAttack);
	JumpAttackCol->GetTransform()->SetLocalPosition(float4(10, 45, 0));
	JumpAttackCol->GetTransform()->SetLocalScale(float4(80, 90, 1));
	JumpAttackCol->SetColType(ColType::AABBBOX2D);
	JumpAttackCol->Off();

	DashAttackCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::PlayerAttack);
	DashAttackCol->GetTransform()->SetLocalPosition(float4(5, 43, 0));
	DashAttackCol->GetTransform()->SetLocalScale(float4(116, 85, 1));
	DashAttackCol->SetColType(ColType::AABBBOX2D);
	DashAttackCol->Off();

	SkillACol = CreateComponent<GameEngineCollision>((int)CollisionOrder::PlayerAttack);
	SkillACol->GetTransform()->SetLocalPosition(float4(0, 40, 0));
	SkillACol->GetTransform()->SetLocalScale(float4(350, 80, 1));
	SkillACol->SetColType(ColType::AABBBOX2D);
	SkillACol->Off();

	SkillBCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::PlayerAttack);
	SkillBCol->GetTransform()->SetLocalPosition(float4(110, 50, 0));
	SkillBCol->GetTransform()->SetLocalScale(float4(80, 150, 1));
	SkillBCol->GetTransform()->SetLocalRotation(float4(0, 0, -30));
	SkillBCol->SetColType(ColType::AABBBOX2D);
	SkillBCol->Off();

	PassiveCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::PlayerAttack);
	PassiveCol->GetTransform()->SetLocalPosition(float4(0, 15, 0));
	PassiveCol->GetTransform()->SetLocalScale(float4(110, 30, 1));
	PassiveCol->SetColType(ColType::AABBBOX2D);
	PassiveCol->Off();

	SkillARigidbody.SetMaxSpeed(3000.0f);
	SkillARigidbody.SetGravity(-3500.0f);
	SkillARigidbody.SetActiveGravity(true);
	SkillARigidbody.SetFricCoeff(1000.0f);

	SkillA_DamageRatio = 2.0f;

	ProjectileEffectNames.resize(4);

	ProjectileEffectNames[0] = "Minotaurus_Rare_PlowUp_Projectile1";
	ProjectileEffectNames[1] = "Minotaurus_Rare_PlowUp_Projectile2";
	ProjectileEffectNames[2] = "Minotaurus_Rare_PlowUp_Projectile3";
	ProjectileEffectNames[3] = "Minotaurus_Rare_PlowUp_Projectile4";
}

void MinotaurusSkull_Unique::Update(float _DeltaTime)
{
	PlayerBaseSkull::Update(_DeltaTime);

	if (true == IsPassive)
	{
		PassiveTime += _DeltaTime;

		for (std::pair<const UINT, float>& Pair : PassiveDoubleCheck)
		{
			Pair.second += _DeltaTime;
		}

		std::vector<std::shared_ptr<GameEngineCollision>> AllCol;
		AllCol.reserve(8);

		if (true == PassiveCol->CollisionAll((int)CollisionOrder::Monster, AllCol, ColType::AABBBOX2D, ColType::AABBBOX2D))
		{
			for (size_t i = 0; i < AllCol.size(); i++)
			{
				std::shared_ptr<BaseMonster> CastingCol = AllCol[i]->GetActor()->DynamicThis<BaseMonster>();

				if (nullptr == CastingCol)
				{
					MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
					return;
				}

				if (0.0f > PassiveDoubleCheck[CastingCol->GetActorCode()])
				{
					continue;
				}

				CastingCol->HitMonster(GetMeleeAttackDamage() * 0.5f, GetViewDir(), true, false, false, HitEffectType::Normal);
				PassiveDoubleCheck[CastingCol->GetActorCode()] = -0.5f;
			}
		}

		if (3.0f <= PassiveTime)
		{
			if (nullptr != PassiveEffect)
			{
				PassiveEffect->Death();
			}

			PassiveEffect = nullptr;
			IsPassive = false;
			PassiveTime = 0.0f;
			PassiveCol->Off();
			PassiveDoubleCheck.clear();
		}
	}
}

void MinotaurusSkull_Unique::Attack_Enter()
{
	PlayerBaseSkull::Attack_Enter();

	HitEvent = [this]()
	{
		GetContentLevel()->GetCamCtrl().CameraShake(5, 30, 5);
	};
}

void MinotaurusSkull_Unique::Attack_End()
{
	PlayerBaseSkull::Attack_End();

	HitEvent = nullptr;
}

void MinotaurusSkull_Unique::JumpAttack_Enter()
{
	Render->ChangeAnimation("JumpAttack");

	AttackRigidbody.SetActiveGravity(true);
	AttackRigidbody.SetGravity(-13000.0f);
	AttackRigidbody.SetMaxSpeed(5000.0f);
	AttackRigidbody.SetVelocity(float4(0, 1300));

	JumpAttackTime = 0.0f;
	JumpAttackLandTime = 0.0f;

	BattleActorRigidbody.SetVelocity(float4::Zero);
	DashRigidbody.SetVelocity(float4::Zero);
	IsJumpAttackLand = false;

	AttackDoubleCheck.clear();
	IsDownPlatformCheckOff = true;
}

void MinotaurusSkull_Unique::JumpAttack_Update(float _DeltaTime)
{
	if (false == IsJumpAttackLand)
	{
		if (0.25f >= JumpAttackTime)
		{
			JumpAttackTime += _DeltaTime;
			AttackRigidbody.AddForce(float4(0, 7000));
		}

		AttackRigidbody.UpdateForce(_DeltaTime);

		float4 AttackVel = AttackRigidbody.GetVelocity();
		PlayerTrans->AddLocalPosition(AttackVel * _DeltaTime);

		if (-200.0f > AttackVel.y)
		{
			JumpAttackCol->On();
			IsDownPlatformCheckOff = false;
		}

		std::vector<std::shared_ptr<GameEngineCollision>> AllCol;
		AllCol.reserve(8);

		if (true == JumpAttackCol->CollisionAll((int)CollisionOrder::Monster, AllCol, ColType::AABBBOX2D, ColType::AABBBOX2D))
		{
			for (size_t i = 0; i < AllCol.size(); i++)
			{
				std::shared_ptr<BaseMonster> CastingCol = AllCol[i]->GetActor()->DynamicThis<BaseMonster>();

				if (nullptr != AttackDoubleCheck[CastingCol->GetActorCode()])
				{
					continue;
				}

				if (nullptr == CastingCol)
				{
					MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
					return;
				}

				CastingCol->HitMonster(GetMeleeAttackDamage(), GetViewDir(), true, true, false, HitEffectType::MinoTaurus);
				AttackDoubleCheck[CastingCol->GetActorCode()] = CastingCol;
			}
		}
	}

	if (false == IsDownPlatformCheckOff && false == IsJumpAttackLand && nullptr != ContentFunc::PlatformColCheck(GroundCol, true))
	{
		Render->ChangeAnimation("JumpAttack_Land");
		IsJumpAttackLand = true;

		float4 AttackVel = AttackRigidbody.GetVelocity();
		AttackRigidbody.SetVelocity(float4::Zero);
		JumpAttackCol->Off();

		if (-200.0f > AttackVel.y)
		{
			GetContentLevel()->GetCamCtrl().CameraShake(6, 60, 15);
		}
		else
		{
			PlayerFSM.ChangeState("Idle");
			return;
		}
	}

	if (true == IsJumpAttackLand)
	{
		JumpAttackLandTime += _DeltaTime;

		if (0.3f <= JumpAttackLandTime)
		{
			PlayerFSM.ChangeState("Idle");
		}
	}
}

void MinotaurusSkull_Unique::JumpAttack_End()
{
	AttackRigidbody.SetActiveGravity(false);
	AttackRigidbody.SetMaxSpeed(1000.0f);

	AttackDoubleCheck.clear();
	IsDownPlatformCheckOff = false;
}

void MinotaurusSkull_Unique::Switch_Enter()
{
	PlayerBaseSkull::Switch_Enter();

	IsSwitchMove = false;
	IsSwitchProjectileShot = false;
	SwitchProjectileTime = 0.0f;
	SwitchShotCount = 0;
}

void MinotaurusSkull_Unique::Switch_Update(float _DeltaTime)
{
	PlayerBaseSkull::Switch_Update(_DeltaTime);

	if (false == IsSwitchMove && 3 == Render->GetCurrentFrame())
	{
		IsSwitchMove = true;

		switch (GetViewDir())
		{
		case ActorViewDir::Left:
			AttackRigidbody.SetVelocity(float4::Left * 800.0f);

			EffectManager::PlayEffect({
				.EffectName = "Minotaurus_PlowUp_Smoke",
				.Position = GetTransform()->GetWorldPosition() + float4(-130, 50),
				.AddSetZ = -20,
				.FlipX = true,
				});
			break;
		case ActorViewDir::Right:
			AttackRigidbody.SetVelocity(float4::Right * 800.0f);

			EffectManager::PlayEffect({
				.EffectName = "Minotaurus_PlowUp_Smoke",
				.Position = GetTransform()->GetWorldPosition() + float4(130, 50),
				.AddSetZ = -20 });
			break;
		default:
			break;
		}

		IsSwitchProjectileShot = true;
	}

	if (true == IsSwitchProjectileShot)
	{
		SwitchProjectileTime += _DeltaTime * 20.0f;

		if (1.0f <= SwitchProjectileTime)
		{
			SwitchProjectileTime -= 1.0f;
			ShotProjectile(SwitchShotCount);
			++SwitchShotCount;
		}

		if (4 <= SwitchShotCount)
		{
			IsSwitchProjectileShot = false;
		}
	}

	AttackRigidbody.UpdateForce(_DeltaTime);

	if (nullptr == ContentFunc::PlatformColCheck(WalkCol))
	{
		float4 AttackVelocity = AttackRigidbody.GetVelocity() * _DeltaTime;
		PlayerTrans->AddLocalPosition(AttackVelocity);
	}

}

void MinotaurusSkull_Unique::Dash_Enter()
{
	PlayerBaseSkull::Dash_Enter();

	std::shared_ptr<EffectActor> DashEffect = EffectManager::PlayEffect({
		.EffectName = "DashTackleEffect",
		.FlipX = ActorViewDir::Left == GetViewDir() });

	DashEffect->GetTransform()->SetParent(GetTransform());
	DashEffect->GetTransform()->SetLocalPosition(float4(30, 45, -35));

	AttackDoubleCheck.clear();
	DashAttackCol->On();
}

void MinotaurusSkull_Unique::Dash_Update(float _DeltaTime)
{
	PlayerBaseSkull::Dash_Update(_DeltaTime);

	std::vector<std::shared_ptr<GameEngineCollision>> AllCol;
	AllCol.reserve(8);

	if (true == DashAttackCol->CollisionAll((int)CollisionOrder::Monster, AllCol, ColType::AABBBOX2D, ColType::AABBBOX2D))
	{
		for (size_t i = 0; i < AllCol.size(); i++)
		{
			std::shared_ptr<BaseMonster> CastingCol = AllCol[i]->GetActor()->DynamicThis<BaseMonster>();

			if (nullptr != AttackDoubleCheck[CastingCol->GetActorCode()])
			{
				continue;
			}

			if (nullptr == CastingCol)
			{
				MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
				return;
			}

			CastingCol->HitMonster(GetMeleeAttackDamage(), GetViewDir(), true, true, false, HitEffectType::Normal);
			AttackDoubleCheck[CastingCol->GetActorCode()] = CastingCol;
		}
	}
}

void MinotaurusSkull_Unique::Dash_End()
{
	PlayerBaseSkull::Dash_End();
	AttackDoubleCheck.clear();
	DashAttackCol->Off();
}

void MinotaurusSkull_Unique::Skill_SlotA_Enter()
{
	PlayerBaseSkull::Skill_SlotA_Enter();
	AttackDoubleCheck.clear();

	switch (GetViewDir())
	{
	case ActorViewDir::Left:
		SkillARigidbody.SetVelocity(float4(-900, 1000));
		break;
	case ActorViewDir::Right:
		SkillARigidbody.SetVelocity(float4(900, 1000));
		break;
	}

	DashRigidbody.SetVelocity(float4::Zero);
	BattleActorRigidbody.SetVelocity(float4::Zero);

	IsSkillALand = false;
	IsSkillADoubleAttack = false;

	SkillALandTime = 0.0f;
	SkillACol->Off();

	PassiveCheck();
	IsDownPlatformCheckOff = true;
}

void MinotaurusSkull_Unique::Skill_SlotA_Update(float _DeltaTime)
{
	if (false == IsSkillALand)
	{
		SkillARigidbody.UpdateForce(_DeltaTime);
		float4 SkillVel = SkillARigidbody.GetVelocity();

		if (0.0f < SkillVel.y)
		{
			if (nullptr != ContentFunc::PlatformColCheck(JumpCol))
			{
				SkillVel.y = 0.0f;
				SkillARigidbody.SetVelocity(SkillVel);
			}
		}

		if (nullptr != ContentFunc::PlatformColCheck(WalkCol))
		{
			SkillVel.x = 0.0f;
			SkillARigidbody.SetVelocity(SkillVel);
		}

		if (-200.0f > SkillVel.y)
		{
			IsDownPlatformCheckOff = false;
		}

		PlayerTrans->AddLocalPosition(SkillVel * _DeltaTime);
	}

	if (false == IsDownPlatformCheckOff && false == IsSkillALand && nullptr != ContentFunc::PlatformColCheck(GroundCol, true))
	{
		SkillALandTime = 0.0f;
		IsSkillALand = true;
		Render->ChangeAnimation("JumpAttack_Land");
		GetContentLevel()->GetCamCtrl().CameraShake(15, 120, 20);

		std::shared_ptr<EffectActor> StampEffect = EffectManager::PlayEffect({
			.EffectName = "StampEffect",
			.Position = GetTransform()->GetWorldPosition(),
			.Scale = 0.65f });

		if (true == IsSkillADoubleAttack)
		{
			EffectManager::PlayEffect({
			.EffectName = "MinoSkillA_Smoke",
			.Position = GetTransform()->GetWorldPosition() + float4(20, 40),
			.Scale = 0.5f });
		}

		SkillACol->On();

		std::vector<std::shared_ptr<GameEngineCollision>> AllCol;
		AllCol.reserve(8);

		if (true == SkillACol->CollisionAll((int)CollisionOrder::Monster, AllCol, ColType::AABBBOX2D, ColType::AABBBOX2D))
		{
			for (size_t i = 0; i < AllCol.size(); i++)
			{
				std::shared_ptr<BaseMonster> CastingCol = AllCol[i]->GetActor()->DynamicThis<BaseMonster>();

				if (nullptr == CastingCol)
				{
					MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
					return;
				}

				CastingCol->HitMonster(GetMeleeAttackDamage() * SkillA_DamageRatio, GetViewDir(), true, true, false, HitEffectType::MinoTaurus);
			}
		}

		SkillACol->Off();
	}

	if (true == IsSkillALand)
	{
		SkillALandTime += _DeltaTime;

		if (false == IsSkillADoubleAttack && true == GameEngineInput::IsDown("PlayerMove_Skill_A"))
		{
			IsSkillALand = false;
			IsSkillADoubleAttack = true;

			SkillARigidbody.SetVelocity(float4(0, 700));
		}
		else if (0.3f <= SkillALandTime)
		{
			PlayerFSM.ChangeState("Idle");
		}
	}
}

void MinotaurusSkull_Unique::Skill_SlotA_End()
{
	PlayerBaseSkull::Skill_SlotA_End();
	AttackDoubleCheck.clear();
	AttackRigidbody.SetActiveGravity(false);
	AttackRigidbody.SetMaxSpeed(1000.0f);
	SkillACol->Off();
	IsDownPlatformCheckOff = false;
}

void MinotaurusSkull_Unique::Skill_SlotB_Enter()
{
	PlayerBaseSkull::Skill_SlotB_Enter();

	IsSkillBWait = false;
	IsSkillBEffect = false;
	SkillBWaitTime = 0.0f;

	SkillBShotCount = 0;
	IsSKillBProjectileShot = false;
	SkillBProjectileTime = 0.0f;
}

void MinotaurusSkull_Unique::Skill_SlotB_Update(float _DeltaTime)
{
	if (false == IsSkillBEffect && 3 == Render->GetCurrentFrame())
	{
		IsSkillBEffect = true;
		IsSKillBProjectileShot = true;

		switch (GetViewDir())
		{
		case ActorViewDir::Left:
		{
			EffectManager::PlayEffect({
			.EffectName = "Minotaurus_PlowUp_Smoke",
			.Position = GetTransform()->GetWorldPosition() + float4(-130, 50),
			.AddSetZ = -20,
			.FlipX = true,
				});

			SkillBPlowUpEffect = EffectManager::PlayEffect({
			.EffectName = "Minotaurus_Rare_PlowUp",
			.Position = GetTransform()->GetWorldPosition() + float4(-110, 100),
			.Triger = EffectDeathTrigger::Time,
			.Time = 5.0f,
			.FlipX = true,
			.IsForceLoopOff = true, });
		}
		break;
		case ActorViewDir::Right:
		{
			EffectManager::PlayEffect({
			.EffectName = "Minotaurus_PlowUp_Smoke",
			.Position = GetTransform()->GetWorldPosition() + float4(130, 50),
			.AddSetZ = -20});

			SkillBPlowUpEffect = EffectManager::PlayEffect({
			.EffectName = "Minotaurus_Rare_PlowUp",
			.Position = GetTransform()->GetWorldPosition() + float4(110, 100),
			.Triger = EffectDeathTrigger::Time,
			.Time = 5.0f,
			.IsForceLoopOff = true, });
		}
		break;
		}

		SkillBCol->On();

		std::vector<std::shared_ptr<GameEngineCollision>> AllCol;
		AllCol.reserve(8);

		GetContentLevel()->GetCamCtrl().CameraShake(5, 30, 5);

		if (true == SkillBCol->CollisionAll((int)CollisionOrder::Monster, AllCol, ColType::OBBBOX2D, ColType::AABBBOX2D))
		{
			for (size_t i = 0; i < AllCol.size(); i++)
			{

				std::shared_ptr<BaseMonster> CastingCol = AllCol[i]->GetActor()->DynamicThis<BaseMonster>();

				if (nullptr == CastingCol)
				{
					MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
					return;
				}

				CastingCol->HitMonster(GetMeleeAttackDamage() * SkillB_DamageRatio * 2.0f, GetViewDir(), true, true, false, HitEffectType::MinoTaurus);
			}
		}

		SkillBCol->Off();
	}

	if (true == IsSKillBProjectileShot)
	{
		SkillBProjectileTime += _DeltaTime * 20.0f;

		if (1.0f <= SkillBProjectileTime)
		{
			SkillBProjectileTime -= 1.0f;
			ShotProjectile(SkillBShotCount);
			++SkillBShotCount;
		}

		if (4 <= SkillBShotCount)
		{
			IsSKillBProjectileShot = false;
		}
	}

	if (false == IsSkillBWait && true == Render->IsAnimationEnd())
	{
		IsSkillBWait = true;		
	}

	if (true == IsSkillBWait)
	{
		SkillBWaitTime += _DeltaTime;

		if (0.2f <= SkillBWaitTime)
		{
			PlayerFSM.ChangeState("Idle");
		}
	}
}

void MinotaurusSkull_Unique::Skill_SlotB_End()
{
	if (nullptr != SkillBPlowUpEffect)
	{
		SkillBPlowUpEffect->IsFadeDeathOn(2.0f);
		SkillBPlowUpEffect = nullptr;
	}
}


void MinotaurusSkull_Unique::DataLoad()
{
	Data = ContentDatabase<SkullData, SkullGrade>::GetData(201); // 201 == 미노타우로스 2세
}

void MinotaurusSkull_Unique::TextureLoad()
{
	DeadPartNames.reserve(6);
	DeadPartNames.push_back("Minotaurus_Head2.png");
	DeadPartNames.push_back("Skul_DeatParts01.png");
	DeadPartNames.push_back("Skul_DeatParts02.png");
	DeadPartNames.push_back("Skul_DeatParts03.png");
	DeadPartNames.push_back("Skul_DeatParts04.png");
	DeadPartNames.push_back("Skul_DeatParts05.png");

	TexName_MainSkullUI = "Unique_Minotaurus_UI_MainSkull.png";
	TexName_SubSkullUI = "Unique_Minotaurus_UI_SubSkull.png";
	TexName_InventoryUI = "Unique_Minotaurus_UI_Inventory.png";

	TexName_SkillA = "Minotaurus_Stamp2.png";
	TexName_SkillB = "Minotaurus_PlowUp2.png";
}

void MinotaurusSkull_Unique::CreateAnimation()
{
	//Idle Animation
	Render->CreateAnimation({ .AnimationName = "Idle", .SpriteName = "Minotaurus_Unique_Idle.png", .FrameInter = 0.1f, .ScaleToTexture = true });
	//Walk Animation
	Render->CreateAnimation({ .AnimationName = "Walk", .SpriteName = "Minotaurus_Unique_Walk.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	//Dash Animation
	Render->CreateAnimation({ .AnimationName = "Dash", .SpriteName = "Minotaurus_Unique_Dash.png", .FrameInter = 0.1f, .Loop = false, .ScaleToTexture = true });
	// Fall
	Render->CreateAnimation({ .AnimationName = "Fall", .SpriteName = "Minotaurus_Unique_Fall.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// Fall Repeat
	Render->CreateAnimation({ .AnimationName = "FallRepeat", .SpriteName = "Minotaurus_Unique_FallRepeat.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// Jump
	Render->CreateAnimation({ .AnimationName = "Jump", .SpriteName = "Minotaurus_Unique_Jump.png", .FrameInter = 0.08f, .ScaleToTexture = true });

	Render->CreateAnimation({ .AnimationName = "JumpAttack_Land", .SpriteName = "Minotaurus_Unique_JumpAttackLand.png", .FrameInter = 0.1f, .ScaleToTexture = true });
}

void MinotaurusSkull_Unique::AnimationColLoad()
{
	GameEngineDirectory Path;

	Path.MoveParentToDirectory("Resources");
	Path.Move("Resources");
	Path.Move("Data");
	Path.Move("8_Player");
	Path.Move("Minotaurus");
	Path.Move("Unique");

	Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_AttackA").GetFullPath()), 0.08f);
	Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_AttackB").GetFullPath()), 0.08f);
	Pushback_JumpAttack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_JumpAttack").GetFullPath()), 0.08f);
	Pushback_SkillA(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_SkillA").GetFullPath()), 0.08f);
	Pushback_SkillB(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_SkillB").GetFullPath()), 0.1f);
	Pushback_Switch(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("Minotaurus_Unique_Switch").GetFullPath()), 0.1f);
}

void MinotaurusSkull_Unique::PassiveCheck()
{
	if (false == IsPassive)
	{
		PassiveEffect = EffectManager::PlayEffect({
			.EffectName = "Minotaurus_Rare_Passive",
			.Position = GetTransform()->GetWorldPosition() + float4(0, 10),
			.Scale = 0.6f,
			.Triger = EffectDeathTrigger::Time
			,.Time = 100.0f,
			});

		PassiveEffect->GetTransform()->SetParent(GetTransform());
		IsPassive = true;
		PassiveTime = 0.0f;
		PassiveDoubleCheck.clear();
		PassiveCol->On();
	}
}

void MinotaurusSkull_Unique::ShotProjectile(size_t _TextureIndex)
{
	_TextureIndex %= 4;

	std::shared_ptr<RigidProjectile> Projectile = GetLevel()->CreateActor<RigidProjectile>();

	GameEngineRandom& Rand = GameEngineRandom::MainRandom;

	float4 ProjectilePos = GetTransform()->GetWorldPosition() + float4(Rand.RandomFloat(-25, 25), Rand.RandomFloat(-25, 25));
	ActorViewDir LookDir = GetViewDir();

	switch (LookDir)
	{
	case ActorViewDir::Left:
		ProjectilePos += float4(-100, 100);
		break;
	case ActorViewDir::Right:
		ProjectilePos += float4(100, 100);
		break;
	}

	Projectile->InitRigd(2000.0f, 0.0f, 1.0f);

	Rigidbody2D& ProjectileRigid = Projectile->GetRigid();

	ContentLevel* Level = GetContentLevel();

	Projectile->ShotProjectile({
		.EffectName = ProjectileEffectNames[_TextureIndex],
		.Pos = ProjectilePos,
		.Dir = float4::Up,
		.ColScale = float4(30, 30, 1),
		.ColOrder = (int)CollisionOrder::Monster,
		.IsPlatformCol = true,
		.IsColDeath = true,
		.IsFlipX = ActorViewDir::Left == GetViewDir(),
		.Damage = GetMeleeAttackDamage() * 0.7f,
		.Speed = 0.0f,
		.LiveTime = 10.0f,
		.EnterEvent = [LookDir, Level](std::shared_ptr<BaseContentActor> _HitActor, ProjectileHitParameter _HitData)
		{
			std::shared_ptr<BaseMonster> CastingActor = _HitActor->DynamicThis<BaseMonster>();

			if (nullptr == CastingActor)
			{
				MsgAssert_Rtti<MinotaurusSkull_Unique>(" - BaseMonster를 상속 받은 클래스만 Monster ColOrder를 가질 수 있습니다.");
				return;
			}

			CastingActor->HitMonster(_HitData.Attack, LookDir, true, true, false, HitEffectType::None);

			EffectManager::PlayEffect({
				.EffectName = "Minotaurus_Projectile_Hit",
				.Position = _HitData.ProjectilePos});
		
			Level->GetCamCtrl().CameraShake(5, 30, 5);
		},
		.DeathEvent = [Level](const float4& _DeathPos)
		{
			EffectManager::PlayEffect({
				.EffectName = "Minotaurus_Projectile_Hit",
				.Position = _DeathPos});

			Level->GetCamCtrl().CameraShake(5, 30, 5);
		}
		});


	ProjectileRigid.SetActiveGravity(true);
	ProjectileRigid.SetGravity(-1300.0f);

	switch (LookDir)
	{
	case ActorViewDir::Left:
		ProjectileRigid.SetVelocity(float4(-500, 800) * Rand.RandomFloat(0.8f, 1.1f));
		break;
	case ActorViewDir::Right:
		ProjectileRigid.SetVelocity(float4(500, 800) * Rand.RandomFloat(0.8f, 1.1f));
		break;
	}

	//const std::string_view& EffectName = "";
	//
	//std::shared_ptr<GameEngineActor> TrackingTarget = nullptr;
	//
	//float4 Pos = float4::Zero;
	//float4 Dir = float4::Up;
	//float4 ColScale = float4::Zero;
	//float4 TrackingPivot = float4::Zero;
	//
	//int ColOrder = -1;
	//ColType ProjectileColType = ColType::SPHERE2D;
	//
	//bool IsPlatformCol = false;
	//bool IsColDeath = false;
	//bool IsRot = true;
	//bool IsFlipX = false;
	//bool IsForceLoopOff = false;
	//bool IsEffectEndDeath = false;
	//
	//float Damage = 1.0f;
	//
	//float Speed = 100.0f;
	//float LiveTime = 1.0f;
	//float WaitTime = 0.0f;
	//float TrackingSpeed = 100.0f;
	//
	//std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> EnterEvent = nullptr;
	//std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> UpdateEvent = nullptr;
	//std::function<void(const float4& _Pos)> DeathEvent = nullptr;

}
