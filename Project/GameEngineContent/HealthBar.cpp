#include "PrecompileHeader.h"
#include "HealthBar.h"

HealthBar::HealthBar()
{
}

HealthBar::~HealthBar()
{
}

void HealthBar::SetTexture(const std::string_view& _FrameName, const std::string_view& _BarName, const std::string_view& _SubBarName, const float4& _ScaleRatio /*= float4::One*/)
{
	BarRedner->SetScaleToTexture(_BarName);
	BarRedner->GetTransform()->SetWorldScale(BarRedner->GetTransform()->GetWorldScale() * _ScaleRatio);

	SubBarRedner->SetScaleToTexture(_SubBarName);
	SubBarRedner->GetTransform()->SetWorldScale(SubBarRedner->GetTransform()->GetWorldScale() * _ScaleRatio);

	FrameRedner->SetScaleToTexture(_FrameName);
	FrameRedner->GetTransform()->SetWorldScale(FrameRedner->GetTransform()->GetWorldScale() * _ScaleRatio);

	BarRedner->On();
	SubBarRedner->On();
	FrameRedner->On();
}

void HealthBar::Start()
{
	BarRedner = CreateComponent<ContentSpriteRenderer>();
	BarRedner->PipeSetting("2DTexture_ProgressUI");
	BarRedner->GetShaderResHelper().SetConstantBufferLink("ProgressBuffer", BarBuffer);
	BarRedner->GetTransform()->SetLocalPosition(float4(0, 0, 0.0f));
	BarRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	BarRedner->Off();

	SubBarRedner = CreateComponent<ContentSpriteRenderer>();
	SubBarRedner->PipeSetting("2DTexture_ProgressUI");
	SubBarRedner->GetShaderResHelper().SetConstantBufferLink("ProgressBuffer", SubBarBuffer);
	SubBarRedner->GetTransform()->SetLocalPosition(float4(0, 0, 0.01f));
	SubBarRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	SubBarRedner->Off();

	FrameRedner = CreateComponent<ContentSpriteRenderer>();
	FrameRedner->PipeSetting("2DTexture_Color");
	FrameRedner->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", FrameBuffer);
	FrameRedner->GetTransform()->SetLocalPosition(float4(0, 0, 0.02f));
	FrameRedner->GetTransform()->SetWorldRotation(float4(0, 0, 0));
	FrameRedner->Off();

	GetTransform()->SetWorldRotation(float4::Zero);
}

void HealthBar::UpdateBar(float _Progress, float _DeltaTime)
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
