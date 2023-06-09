#include "PrecompileHeader.h"
#include "StoryFontActor.h"
#include "ContentUIFontRenderer.h"
#include "TalkArrow.h"

StoryFontActor::StoryFontActor()
{
}

StoryFontActor::~StoryFontActor()
{
}

void StoryFontActor::WriteText(std::vector<StoryFontParameter> _TextParameter)
{
	Reset();

	TextRender.resize(_TextParameter.size());

	for (size_t i = 0; i < TextRender.size(); i++)
	{
		float PosX = _TextParameter[i].PosX;
		float PosY = FontSize * 1.4f * -_TextParameter[i].Line;

		TextRender[i] = CreateComponent<ContentUIFontRenderer>();
		TextRender[i]->GetTransform()->SetParent(GetTransform());	
		TextRender[i]->SetFont("�ؽ�Lv2����");
		TextRender[i]->GetTransform()->SetLocalPosition({ PosX , PosY });
		TextRender[i]->SetScale(FontSize);
		TextRender[i]->SetFontFlag(static_cast<FW1_TEXT_FLAG>(FW1_TEXT_FLAG::FW1_VCENTER | FW1_TEXT_FLAG::FW1_CENTER));
		TextRender[i]->SetText(_TextParameter[i].StoryText);
		TextRender[i]->Off();
	}

	Index = 0;
	ReadProgress = 0.0f;
	IsRead = true;

}

void StoryFontActor::ReadText()
{
	++Index;
	for (size_t i = 0; i < Index; i++)
	{
		TextRender[i]->SetColor(FontColorFadeEnd);
	}

	IsRead = true;
	ReadProgress = 0.0f;
}

void StoryFontActor::Reset()
{
	Index = 0;

	for (size_t i = 0; i < TextRender.size(); i++)
	{
		if (nullptr != TextRender[i])
		{
			TextRender[i]->Death();
			TextRender[i] = nullptr;
		}
	}

	ArrowPtr->Off();
	TextRender.clear();
}

void StoryFontActor::SkipText()
{
	for (size_t i = 0; i < TextRender.size(); i++)
	{
		TextRender[i]->SetColor(FontColorFadeEnd);
		TextRender[i]->On();
	}

	Index = TextRender.size();
}

void StoryFontActor::Start()
{
	ArrowPtr = GetLevel()->CreateActor<TalkArrow>();
	ArrowPtr->GetTransform()->SetParent(GetTransform());
	ArrowPtr->GetTransform()->SetLocalPosition(float4(450.0f, -30.0f, -0.1f));
	ArrowPtr->SetSpeed(1.5f);
	ArrowPtr->Off();
}

void StoryFontActor::Update(float _DeltaTime)
{
	if (false == IsRead)
	{
		return;
	}

	ReadProgress += _DeltaTime * FadeSpeed;

	if (Index >= TextRender.size())
	{
		return;
	}

	TextRender[Index]->On();
	TextRender[Index]->SetColor(
		float4::LerpClamp(FontColorFadeStart, FontColorFadeEnd, ReadProgress));

	if (1.0f <= ReadProgress)
	{
		IsRead = false;

		if (Index + 1 >= TextRender.size())
		{
			ArrowPtr->On();
		}
	}
}