#include "PrecompileHeader.h"
#include "SpeechBubble.h"
#include "ContentUIRender.h"

SpeechBubble::SpeechBubble()
{
}

SpeechBubble::~SpeechBubble()
{
}

#include "GameEngineActorGUI.h"

void SpeechBubble::PlayBubble(const SpeechBubbleParameter& _BubbleParameter)
{
	Target = _BubbleParameter.Target;

	if (nullptr == Target.lock())
	{
		BubbleRender->Off();
		return;
	}

	if (0.0f != _BubbleParameter.FontSize)
	{
		FontRender->SetScale(_BubbleParameter.FontSize);
	}

	FontRender->SetText(_BubbleParameter.Text);
	FontRender->On();

	TalkText = _BubbleParameter.Text;
	Pivot = _BubbleParameter.Pivot;

	IsLoop = _BubbleParameter.IsLoop;

	if (true == _BubbleParameter.IsLarge)
	{
		BubbleRender->SetTextureAndSlice("SpeechBubble_Large.png", 0.1f, 0.1f, 0.1f, 0.35f);
		FontRender->GetTransform()->SetLocalPosition(float4(0, 5, -0.1f));
	}
	else
	{
		BubbleRender->SetTextureAndSlice("SpeechBubble.png", 0.1f, 0.1f, 0.1f, 0.35f);
		FontRender->GetTransform()->SetLocalPosition(float4(0, 5, -0.1f));
	}

	//std::shared_ptr<GameEngineActorGUI> Ptr = GameEngineGUI::FindGUIWindowConvert<GameEngineActorGUI>("GameEngineActorGUI");
	//Ptr->SetTarget(FontRender->GetTransform());
	//Ptr->On();



	LiveTime = _BubbleParameter.LiveTime;
	LoopInter = _BubbleParameter.LoopInter;

	BubbleRender->ResetScale();

	if (true == _BubbleParameter.IsAutoScale)
	{
		std::wstring BubbleText = GameEngineString::AnsiToUniCode(TalkText);

		float TextureBoardX = 0.0f;
		float TextureBoardY = 0.0f;
		
		if (true == _BubbleParameter.IsLarge)
		{
			TextureBoardX = 195.0f;
			TextureBoardY = 55.0f;
		}
		else
		{
			TextureBoardX = 130.0f;
			TextureBoardY = 30.0f;
		}

		float CurX = 0.0f;
		float CurY = _BubbleParameter.FontSize;

		float MaxX = 0.0f;
		float MaxY = 0.0f;

		for (size_t i = 0; i < BubbleText.size(); i++)
		{
			if (BubbleText[i] == L'\n')
			{
				CurX = 0;
				CurY += _BubbleParameter.FontSize;
			}
			else
			{
				CurX += _BubbleParameter.FontSize * 0.7f;
			}

			if (CurX > MaxX)
			{
				MaxX = CurX;
			}

			if (CurY > MaxY)
			{
				MaxY = CurY;
			}
		}

		if (TextureBoardX < MaxX)
		{
			BubbleRender->GetTransform()->AddWorldScale(float4(MaxX - TextureBoardX, 0));

		}

		if (TextureBoardX < MaxY)
		{
			BubbleRender->GetTransform()->AddWorldScale(float4(0, MaxY - TextureBoardX));
		}
	}

	BubbleRender->On();
}


void SpeechBubble::On()
{
	BaseContentActor::On();
}

void SpeechBubble::Start()
{
	if (nullptr == GameEngineTexture::Find("SpeechBubble.png"))
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Texture");
		Path.Move("0_Common");
		Path.Move("UI");

		GameEngineTexture::Load(Path.GetPlusFileName("SpeechBubble.png").GetFullPath());
		GameEngineTexture::Load(Path.GetPlusFileName("SpeechBubble_Large.png").GetFullPath());
	}

	BubbleRender = CreateComponent<ContentSlice9Renderer>();
	BubbleRender->Off();

	FontRender = CreateComponent<GameEngineFontRenderer>();
	FontRender->SetFont("�ؽ�Lv2����");
	FontRender->SetScale(15);
	FontRender->SetColor(float4(1, 1, 1, 1));
	FontRender->SetFontFlag(static_cast<FW1_TEXT_FLAG>(FW1_TEXT_FLAG::FW1_VCENTER | FW1_TEXT_FLAG::FW1_CENTER));

	FontRender->Off();
}

void SpeechBubble::Update(float _DeltaTime)
{
	if (nullptr != Target.lock() && true == Target.lock()->IsDeath())
	{
		Target.lock() = nullptr;
	}

	if (nullptr == Target.lock())
	{
		BubbleRender->Off();
		FontRender->Off();
		return;
	}
	else
	{
		BubbleRender->On();
		FontRender->On();
	}

	OffTime += _DeltaTime;

	if (0.0f > OffTime)
	{
		BubbleRender->Off();
		FontRender->Off();
		ResetLiveTime();
	}

	float4 TargetPos = Target.lock()->GetTransform()->GetWorldPosition();
	TargetPos.z = 0.0f;

	GetTransform()->SetWorldPosition(TargetPos + Pivot);
	
	if (GetLiveTime() >= LiveTime)
	{
		if (false == IsLoop)
		{
			Death();
		}
		else
		{
			ResetLiveTime();
			OffTime = -LoopInter;
		}
	}

}