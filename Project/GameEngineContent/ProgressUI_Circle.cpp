#include "PrecompileHeader.h"
#include "ProgressUI_Circle.h"
#include "ContentUIRender.h"

ProgressUI_Circle::ProgressUI_Circle()
{
}

ProgressUI_Circle::~ProgressUI_Circle()
{
}

void ProgressUI_Circle::Start()
{
	if (nullptr == GameEngineTexture::Find("FrameFill_Box.png"))
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Texture");
		Path.Move("0_Common");
		Path.Move("UI");

		GameEngineTexture::Load(Path.GetPlusFileName("FrameFill_Box.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("FrameFill_Circle.png").GetFullPath());
	}


	ProgressRender = CreateComponent<ContentUIRender>();
	ProgressRender->PipeSetting("2DTexture_ProgressUI_Circle");
	ProgressRender->GetShaderResHelper().SetConstantBufferLink("ProgressCircleBuffer", Buffer);
	ProgressRender->Off();
}

void ProgressUI_Circle::UpdateProgress(float _Progress)
{
	Buffer.Progress = _Progress;
}
