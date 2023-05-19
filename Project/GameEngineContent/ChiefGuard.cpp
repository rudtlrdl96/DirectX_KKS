#include "PrecompileHeader.h"
#include "ChiefGuard.h"

#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineCollision.h>

#include  "Projectile.h"

ChiefGuard::ChiefGuard()
{
}

ChiefGuard::~ChiefGuard()
{
}

void ChiefGuard::Start()
{
	PlayerBaseSkull::Start();

	FlashCol = CreateComponent<GameEngineCollision>();
}

void ChiefGuard::Skill_SlotA_Enter()
{
	PlayerBaseSkull::Skill_SlotA_Enter();
	IsFlash = false;
}

void ChiefGuard::Skill_SlotA_Update(float _DeltaTime)
{
	PlayerBaseSkull::Skill_SlotA_Update(_DeltaTime);

	if (true == IsFlash)
	{
		FlashProgress += _DeltaTime * FlashSpeed;
		PlayerTrans->SetWorldPosition(float4::LerpClamp(FlashStart, FlashEnd, FlashProgress));

		if (1.0f <= FlashProgress)
		{
			IsFlash = false;
			FlashProgress = 0.0f;
		}
	}

	if (false == IsFlash && 19 == SkullRenderer->GetCurrentFrame())
	{
		IsFlash = true;
		FlashProgress = 0.0f;

		FlashStart = PlayerTrans->GetWorldPosition();
		float4 FlashPos = FlashStart + float4(0, 30);

		switch (GetViewDir())
		{
		case ActorViewDir::Left: 
			FlashPos += float4(-225, 0);
			FlashEnd = FlashStart + float4(-450, 0);
			break;
		case ActorViewDir::Right:
			FlashPos += float4(225, 0);
			FlashEnd = FlashStart + float4(450, 0);
			break;
		default:
			break;
		}

		EffectManager::PlayEffect({
			.EffectName = "FlashCut",
			.Postion = FlashPos,
			.FlipX = GetViewDir() == ActorViewDir::Left});
	}
}

void ChiefGuard::Skill_SlotA_End()
{
}

void ChiefGuard::Skill_SlotB_Enter()
{
	PlayerBaseSkull::Skill_SlotB_Enter();

	IsProjecTileShot = false;
}

void ChiefGuard::Skill_SlotB_Update(float _DeltaTime)
{
	PlayerBaseSkull::Skill_SlotB_Update(_DeltaTime);

	if (false == IsProjecTileShot && 6 == SkullRenderer->GetCurrentFrame())
	{
		IsProjecTileShot = true;

		std::shared_ptr<Projectile> NewProjectile = GetLevel()->CreateActor<Projectile>();

		float4 Dir = float4::Right;

		switch (GetViewDir())
		{
		case ActorViewDir::Left:
			Dir = float4::Left;
			break;
		case ActorViewDir::Right:
			Dir = float4::Right;
			break;
		default:
			break;
		}

		EffectManager::PlayEffect({ .EffectName = "FireSlash",
			.Postion = PlayerTrans->GetWorldPosition() + float4(15 * Dir.x, 5),
			.FlipX = GetViewDir() == ActorViewDir::Left });

		NewProjectile->ShotProjectile({
			.EffectName = "FireProjectile",
			.Pos = PlayerTrans->GetWorldPosition() + float4(0, 30, 0),
			.Dir = Dir,
			.Speed = 1200.0f,
			.LiveTime = 2.0f
			});
	}
}

void ChiefGuard::Switch_Enter()
{
	PlayerBaseSkull::Switch_Enter();

	IsFlash = false;

	IsFlash_0 = false;
	IsFlash_1 = false;
	IsFlash_2 = false;
}

void ChiefGuard::Switch_Update(float _DeltaTime)
{
	if (true == IsFlash)
	{
		FlashProgress += _DeltaTime * FlashSpeed;
		PlayerTrans->SetWorldPosition(float4::LerpClamp(FlashStart, FlashEnd, FlashProgress));

		if (1.0f <= FlashProgress)
		{
			IsFlash = false;
			FlashProgress = 0.0f;
		}
	}

	if (false == IsFlash && 
		((false == IsFlash_0 && 0 == SkullRenderer->GetCurrentFrame()) ||
		 (false == IsFlash_1 && 3 == SkullRenderer->GetCurrentFrame()) || 
		 (false == IsFlash_2 && 6 == SkullRenderer->GetCurrentFrame())))
	{
		IsFlash = true;
		FlashProgress = 0.0f;

		FlashStart = PlayerTrans->GetWorldPosition();
		float4 FlashPos = FlashStart + float4(0, 30);

		float Flip = 1.0f;

		if (0 == SkullRenderer->GetCurrentFrame())
		{
			IsFlash_0 = true;
		}

		if (3 == SkullRenderer->GetCurrentFrame())
		{
			IsFlash_1 = true;
			Flip = -1.0f;
		}

		if (6 == SkullRenderer->GetCurrentFrame())
		{
			IsFlash_2 = true;
		}

		switch (GetViewDir())
		{
		case ActorViewDir::Left:
			FlashPos += float4(-225 * Flip, 0);
			FlashEnd = FlashStart + float4(-450 * Flip, 0);
			break;
		case ActorViewDir::Right:
			FlashPos += float4(225 * Flip, 0);
			FlashEnd = FlashStart + float4(450 * Flip, 0);
			break;
		default:
			break;
		}

		EffectManager::PlayEffect({
			.EffectName = "FlashCut",
			.Postion = FlashPos,
			.FlipX = GetViewDir() == ActorViewDir::Left });
	}

	PlayerBaseSkull::Switch_Update(_DeltaTime);
}

void ChiefGuard::DataLoad()
{
	Data = ContentDatabase<SkullData, SkullGrade>::GetData(203); // 203 == ������
}

void ChiefGuard::TextureLoad()
{
}

void ChiefGuard::CreateAnimation()
{
	//Idle Animation
	SkullRenderer->CreateAnimation({ .AnimationName = "Idle", .SpriteName = "ChiefGuard_Unique_Idle.png", .FrameInter = 0.15f, .ScaleToTexture = true });
	//Walk Animation
	SkullRenderer->CreateAnimation({ .AnimationName = "Walk", .SpriteName = "ChiefGuard_Unique_Walk.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	//Dash Animation
	SkullRenderer->CreateAnimation({ .AnimationName = "Dash", .SpriteName = "ChiefGuard_Unique_Dash.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// Fall
	SkullRenderer->CreateAnimation({ .AnimationName = "Fall", .SpriteName = "ChiefGuard_Unique_Fall.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// Fall Repeat
	SkullRenderer->CreateAnimation({ .AnimationName = "FallRepeat", .SpriteName = "ChiefGuard_Unique_FallRepeat.png", .FrameInter = 0.08f, .ScaleToTexture = true });
	// Jump
	SkullRenderer->CreateAnimation({ .AnimationName = "Jump", .SpriteName = "ChiefGuard_Unique_Jump.png", .FrameInter = 0.08f, .ScaleToTexture = true });
}

void ChiefGuard::AnimationColLoad()
{
	GameEngineDirectory Path;

	Path.MoveParentToDirectory("Resources");
	Path.Move("Resources");
	Path.Move("Data");
	Path.Move("8_Player");
	Path.Move("ChiefGuard");

	Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_AttackA").GetFullPath()), 0.08f);
	Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_AttackB").GetFullPath()), 0.08f);
	Pushback_Attack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_AttackC").GetFullPath()), 0.1f);
	Pushback_JumpAttack(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_JumpAttack").GetFullPath()), 0.1f);
	Pushback_SkillA(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_SkillA").GetFullPath()), 0.045f);
	Pushback_SkillB(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_SkillB").GetFullPath()), 0.09f);
	Pushback_Switch(ContentFunc::LoadAnimAttackMetaData(Path.GetPlusFileName("ChiefGuard_Unique_Switch").GetFullPath()), 0.1f);
}
