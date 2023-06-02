#include "PrecompileHeader.h"
#include "Player.h"
#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineCollision.h>
#include <GameEnginePlatform/GameEngineInput.h>

#include "CollisionDebugRender.h"

#include "PlayerHitFade.h"
#include "PlayerState.h"
#include "PlayerStateFrame.h"
#include "Inventory.h"
#include "BaseQuintessence.h"

#include "BoneSkull.h"
#include "ChiefGuard.h"

#include "ContentLevel.h"

Player::Player()
{
}

Player::~Player()
{
	if (nullptr != StateFrame)
	{
		StateFrame->Death();
		StateFrame = nullptr;
	}

	if (nullptr != HitFade)
	{
		HitFade->Death();
		HitFade = nullptr;
	}
}

void Player::SetInventoryData()
{
	const SkullData& MainData = Inventory::GetMainSkull();
	const SkullData& SubData = Inventory::GetSubSkull();

	MainSkull = CreateNewSkull(MainData.Index);

	if (static_cast<size_t>(-1) != SubData.Index)
	{
		SubSkull = CreateNewSkull(SubData.Index);
	}

	if (nullptr == MainSkull)
	{
		MsgAssert_Rtti<Player>(" - 메인 스컬 정보가 존재하지 않습니다");
		return;
	}

	MainSkull->On();
}

void Player::HitPlayer(float _Damage, const float4& _HitForce)
{
	if (nullptr == MainSkull)
	{
		MsgAssert_Rtti<Player>(" - 스컬 데이터가 존재하지 않는데 플레이어를 공격하려 했습니다");
		return;
	}

	if (0.0f < HitWaitTime)
	{
		return;
	}


	if (true == MainSkull->DashAvoidance)
	{
		return;
	}

	PlayerState::HP -= _Damage;

	HitWaitTime = 0.1f;
	HitFade->Active();
	MainSkull->HitRigidbody.AddVelocity(_HitForce);
	MainSkull->IsHit = true;
	MainSkull->IsHitEffectOn = true;
	MainSkull->HitEffect();
	MainSkull->HitStiffen();
}

void Player::PushPlayer(const float4& _HitForce)
{
	if (nullptr == MainSkull)
	{
		MsgAssert_Rtti<Player>(" - 스컬 데이터가 존재하지 않는데 플레이어를 밀어내려 했습니다");
		return;
	}

	MainSkull->JumpDir = _HitForce;
}

void Player::Start()
{
	PlayerBodyCol = CreateComponent<GameEngineCollision>((int)CollisionOrder::Player);
	PlayerBodyCol->GetTransform()->SetLocalPosition(float4(0.0f, 30.0f, 1.0f));
	PlayerBodyCol->GetTransform()->SetWorldScale(float4(30.0f, 60.0f, 1.0f));
	PlayerBodyCol->GetTransform()->SetWorldRotation(float4::Zero);

	StateFrame = GetLevel()->CreateActor<PlayerStateFrame>();
	StateFrame->SetParentPlayer(this);

	HitFade = GetLevel()->CreateActor<PlayerHitFade>();
}

void Player::Update(float _DeltaTime)
{
	HitWaitTime -= _DeltaTime;
	SwitchCoolTime += _DeltaTime;

	if (PlayerState::HP <= 0.0f)
	{
		std::shared_ptr<ContentLevel> CastingLevel = GetLevel()->DynamicThis<ContentLevel>();

		if (nullptr == CastingLevel) 
		{ 
			return;
		}

		CameraController& CamCtrl = CastingLevel->GetCamCtrl();

		CamCtrl.EffectScaleRatio(1.0f, 0.7f, 1.5f);

		GameEngineTime::GlobalTime.SetAllUpdateOrderTimeScale(0.1f);
		GameEngineTime::GlobalTime.SetAllRenderOrderTimeScale(0.1f);

		std::shared_ptr<GameEngineActor> HeadPart = MainSkull->SkullDeath();
		
		if (nullptr != HeadPart)
		{
			CamCtrl.SetLookatTarget(HeadPart);
		}
		
		StateFrame->Death();
		Death();
		return;
	}

	if (true == GameEngineInput::IsDown("PlayerCollisionDebugSwitch"))
	{
		if (true == IsDebug())
		{
			DebugOff();
		}
		else
		{
			DebugOn();
			CreateColDebugRender();
		}
	}

	if (nullptr == MainSkull)
	{
		MsgAssert_Rtti<Player>(" - 메인 스컬이 존재하지 않습니다.");
		return;
	}

	if (nullptr != SubSkull && true == MainSkull->IsSwitch() && SwitchCoolTime >= SwitchCoolEndTime)
	{
		SwitchCoolTime = 0.0f;
		DebugOff();

		ActorViewDir Dir = MainSkull->GetViewDir();

		EffectManager::PlayEffect({.EffectName = "SwitchEffect", 
			.Postion = GetTransform()->GetWorldPosition() + float4(0, 40, 0),
			.FlipX = Dir == ActorViewDir::Left});

 		SubSkull->PlayerFSM.ChangeState("Switch");
		SubSkull->SetViewDir(Dir);
		SubSkull->SwitchEnter();
		SubSkull->On();
		SubSkull->DebugOff();

		MainSkull->CaptureRenderTex(float4(0.85f, 0.2f, 0.92f, 1.0f), float4(0.85f, 0.2f, 0.92f, 0.0f), 1.5f);
		MainSkull->Off();
		MainSkull->SwitchEnd();
		MainSkull->DebugOff();
		MainSkull->IsSwitchValue = false;

		Inventory::SwapSkull();

		std::shared_ptr<PlayerBaseSkull> TempPtr = MainSkull;
		MainSkull = SubSkull;
		SubSkull = TempPtr;
	}

	if (nullptr != MainSkull)
	{
		MainSkull->CoolTimeCheck(_DeltaTime);
		MainSkull->IsSwitchValue = false;
	}

	if (nullptr != SubSkull)
	{
		SubSkull->CoolTimeCheck(_DeltaTime);
		SubSkull->IsSwitchValue = false;
	}
}

void Player::LevelChangeEnd()
{
	if (nullptr != StateFrame)
	{
		StateFrame->Death();
		StateFrame = nullptr;
	}

	if (nullptr != HitFade)
	{
		HitFade->Death();
		HitFade = nullptr;
	}
}

std::shared_ptr<PlayerBaseSkull> Player::CreateNewSkull(size_t _Index)
{
	std::shared_ptr<PlayerBaseSkull> NewSkull = nullptr;

	//LoadSkull({ .Name = "리틀본", .Index = 0, .Grade = SkullGrade::Normal });
	//LoadSkull({ .Name = "웨어울프", .Index = 1, .Grade = SkullGrade::Normal });
	//LoadSkull({ .Name = "나이 든 웨어울프", .Index = 100, .Grade = SkullGrade::Rare });
	//LoadSkull({ .Name = "미노타우로스", .Index = 101, .Grade = SkullGrade::Rare });
	//LoadSkull({ .Name = "연금술사", .Index = 102, .Grade = SkullGrade::Rare });
	//LoadSkull({ .Name = "우두머리 웨어울프", .Index = 200, .Grade = SkullGrade::Unique });
	//LoadSkull({ .Name = "미노타우로스 2세", .Index = 201, .Grade = SkullGrade::Unique });
	//LoadSkull({ .Name = "고위 연금술사", .Index = 202, .Grade = SkullGrade::Unique });
	//LoadSkull({ .Name = "경비대장", .Index = 203, .Grade = SkullGrade::Unique });
	//LoadSkull({ .Name = "영원의 웨어울프", .Index = 300, .Grade = SkullGrade::Legendary });
	//LoadSkull({ .Name = "미노타우로스 3세", .Index = 301, .Grade = SkullGrade::Legendary });
	//LoadSkull({ .Name = "검은 연금술사", .Index = 302, .Grade = SkullGrade::Legendary });

	switch (_Index)
	{
	case 0: // 리틀본
		NewSkull = GetLevel()->CreateActor<BoneSkull>();
		break;
	case 203: // 경비대장
		NewSkull = GetLevel()->CreateActor<ChiefGuard>();
		break;
	default:
		break;
	}

	if (nullptr != NewSkull)
	{
		NewSkull->GetTransform()->SetParent(GetTransform());
		NewSkull->SetPlayer(DynamicThis<Player>());
		NewSkull->Off();
	}

	NewSkull->GetTransform()->SetLocalPosition(float4(0, 0, 0));

	return NewSkull;
}

void Player::CreateColDebugRender()
{
	std::shared_ptr<CollisionDebugRender> BodyDebugRender = GetLevel()->CreateActor<CollisionDebugRender>();

	BodyDebugRender->SetColor(CollisionDebugRender::DebugColor::Magenta);
	BodyDebugRender->SetTargetCollision(PlayerBodyCol);
	BodyDebugRender->GetTransform()->SetParent(PlayerBodyCol->GetTransform(), false);
	BodyDebugRender->GetTransform()->AddLocalPosition(float4(0, 0, -10));
}
