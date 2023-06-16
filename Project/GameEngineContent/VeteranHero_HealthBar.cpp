#include "PrecompileHeader.h"
#include "VeteranHero_HealthBar.h"
#include "ContentUIRender.h"

VeteranHero_HealthBar::VeteranHero_HealthBar()
{
}

VeteranHero_HealthBar::~VeteranHero_HealthBar()
{
}

void VeteranHero_HealthBar::UpdateBar(float _Progress, float _DeltaTime)
{
	GameEngineTransform* GetTrans = GetTransform();

	BarBuffer.ColorProgress.SizeX = _Progress;

	if (false == IsSubUpdate && BarBuffer.ColorProgress.SizeX != SubBarBuffer.ColorProgress.SizeX)
	{
		StartProgress = SubBarBuffer.ColorProgress;
		EndProgress = BarBuffer.ColorProgress;

		IsSubUpdate = true;
		ProgressTime = 0.0f;
	}

	if (true == IsSubUpdate)
	{
		ProgressTime += _DeltaTime * 6.0f;

		if (BarBuffer.ColorProgress != EndProgress)
		{
			ProgressTime = 0.0f;

			StartProgress = SubBarBuffer.ColorProgress;
			EndProgress = BarBuffer.ColorProgress;
		}

		SubBarBuffer.ColorProgress = float4::Lerp(StartProgress, BarBuffer.ColorProgress, ProgressTime);

		if (1.0f <= ProgressTime)
		{
			IsSubUpdate = false;
		}
	}
}

void VeteranHero_HealthBar::Start()
{
	if (nullptr == GameEngineTexture::Find("VeteranHero_TopFrame.png"))
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Texture");
		Path.Move("0_Common");
		Path.Move("UI");

		GameEngineTexture::Load(Path.GetPlusFileName("VeteranHero_TopFrame.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("VeteranHero_BottomFrame.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("VeteranHero_BarFrame.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("VeteranHero_HpBar.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("VeteranHero_SubBar.png").GetFullPath());
	}

	BarRedner = CreateComponent<ContentUIRender>();
	BarRedner->PipeSetting("2DTexture_ProgressUI");
	BarRedner->GetShaderResHelper().SetConstantBufferLink("ProgressBuffer", BarBuffer);
	BarRedner->GetTransform()->SetLocalPosition(float4(0.0f, 0.0f, 0.0f));
	BarRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	BarRedner->SetScaleToTexture("VeteranHero_HpBar.png");

	float4 MainBarFrameScale = BarRedner->GetTransform()->GetLocalScale();
	BarRedner->GetTransform()->SetLocalScale(MainBarFrameScale * 2.0f);

	SubBarRedner = CreateComponent<ContentUIRender>();
	SubBarRedner->PipeSetting("2DTexture_ProgressUI");
	SubBarRedner->GetShaderResHelper().SetConstantBufferLink("ProgressBuffer", SubBarBuffer);
	SubBarRedner->GetTransform()->SetLocalPosition(float4(0.0f, 0.0f, 0.01f));
	SubBarRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	SubBarRedner->SetScaleToTexture("VeteranHero_SubBar.png");

	float4 SubBarFrameScale = SubBarRedner->GetTransform()->GetLocalScale();
	SubBarRedner->GetTransform()->SetLocalScale(SubBarFrameScale * 2.0f);

	BarFrameRedner = CreateComponent<ContentUIRender>();
	BarFrameRedner->PipeSetting("2DTexture_Color");
	BarFrameRedner->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", FrameBuffer);
	BarFrameRedner->GetTransform()->SetLocalPosition(float4(0, 0, 0.02f));
	BarFrameRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	BarFrameRedner->SetScaleToTexture("VeteranHero_BarFrame.png");

	float4 BarFrameScale = BarFrameRedner->GetTransform()->GetLocalScale();
	BarFrameRedner->GetTransform()->SetLocalScale(BarFrameScale * 2.0f);

	TopFrameRedner = CreateComponent<ContentUIRender>();
	TopFrameRedner->PipeSetting("2DTexture_Color");
	TopFrameRedner->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", FrameBuffer);
	TopFrameRedner->GetTransform()->SetLocalPosition(float4(0, 13, 0.03f));
	TopFrameRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	TopFrameRedner->SetScaleToTexture("VeteranHero_TopFrame.png");

	float4 TopFrameScale = TopFrameRedner->GetTransform()->GetLocalScale();
	TopFrameRedner->GetTransform()->SetLocalScale(TopFrameScale * 2.0f);

	BottomFrameRedner = CreateComponent<ContentUIRender>();
	BottomFrameRedner->PipeSetting("2DTexture_Color");
	BottomFrameRedner->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", FrameBuffer);
	BottomFrameRedner->GetTransform()->SetLocalPosition(float4(0, 10, 0.04f));
	BottomFrameRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	BottomFrameRedner->SetScaleToTexture("VeteranHero_BottomFrame.png");

	float4 BottomFrameScale = BottomFrameRedner->GetTransform()->GetLocalScale();
	BottomFrameRedner->GetTransform()->SetLocalScale(BottomFrameScale * 2.0f);
	
	GetTransform()->SetWorldRotation(float4::Zero);
}