#include "PrecompileHeader.h"
#include "TalkBox.h"
#include "TalkArrow.h"

TalkBox::TalkBox()
{
}

TalkBox::~TalkBox()
{
}

void TalkBox::ActiveTalkBox(const std::string_view& _Name, const float4& _NamePivot /*= float4::Zero*/, const float4& _ForceSize /*= float4::Zero*/)
{ 
	NameText = _Name;
	NameTagPtr->SetNameTag(NameText, _NamePivot, _ForceSize);
	On();
}

void TalkBox::SetMainText(const std::wstring_view& _MainText, std::function<void()> _Callback, std::vector<TalkboxKeyImageData> _KeyImageData)
{
	for (size_t i = 0; i < KeyImages.size(); i++)
	{
		KeyImages[i]->Death(); 
		KeyImages[i] = nullptr;
	}

	FontTextRender->SetText("");

	KeyImages.clear();

	MainText = _MainText;
	IsReadEnd = false;
	IsBoost = false;
	BoostSpeed = 1.0f;
	Progress = 0.0f;
	ReadSoundCount = 0;
	ReadEndCallback = _Callback;
	KeyImageDatas = _KeyImageData;

	float TextureSclaeX = 600.0f;
	float CurX = 0.0f;
	float FontX = FontSize * 0.7f;


	for (size_t i = 0; i < MainText.size(); i++)
	{
		CurX += FontX;

		if (MainText[i] == L'\n')
		{
			CurX = 0.0f;
			continue;
		}

		if (CurX + FontX >= TextureSclaeX)
		{
			CurX = 0.0f;

			MainText.insert(MainText.begin() + i, L'\n');
		}
	}
}

void TalkBox::On()
{
	BaseContentActor::On();
}

void TalkBox::Off()
{
	BaseContentActor::Off();
	FontTextRender->Off();
}

void TalkBox::Start()
{
	if (false == GameEngineInput::IsKey("TalkKey"))
	{
		GameEngineInput::CreateKey("TalkKey", 'X');
	}

	if (nullptr == GameEngineTexture::Find("TalkBox.png"))
	{
		GameEngineDirectory Path;

		Path.MoveParentToDirectory("Resources");
		Path.Move("Resrouces");
		Path.Move("Texture");
		Path.Move("0_Common");
		Path.Move("UI");

		GameEngineTexture::Load(Path.GetPlusFileName("TalkBox.png").GetFullPath());
	}

	ArrowPtr = GetLevel()->CreateActor<TalkArrow>();
	ArrowPtr->GetTransform()->SetParent(GetTransform());
	ArrowPtr->GetTransform()->SetLocalPosition(float4(285.0f, -35.0f, -0.1f));
	ArrowPtr->SetSpeed(1.5f);
	ArrowPtr->Off();

	Render = CreateComponent<ContentUIRender>();
	Render->PipeSetting("2DTexture_Color");
	Render->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", Buffer);
	Render->SetScaleToTexture("TalkBox.png");

	NameTagPtr = GetLevel()->CreateActor<TalkNameTag>();
	NameTagPtr->GetTransform()->SetParent(GetTransform());
	NameTagPtr->GetTransform()->SetLocalPosition(float4(-310, 50, -0.1f));
	NameTagPtr->SetFontInterval(21);

	FontTextRender = CreateComponent<ContentUIFontRenderer>();
	FontTextRender->SetFont("�ؽ�Lv2����");
	FontTextRender->GetTransform()->SetLocalPosition(float4(-300, 25, -1));
	FontTextRender->SetScale(FontSize);
	FontTextRender->SetColor(float4(0.58431f, 0.48627f, 0.3647f, 1));
	FontTextRender->Off();
}

void TalkBox::Update(float _DeltaTime)
{
	if (true == IsReadEnd)
	{
		if (true == GameEngineInput::IsDown("TalkKey"))
		{
			if (nullptr != ReadEndCallback)
			{
				ReadEndCallback();
			}
		}

		return;
	}

	if (true == GameEngineInput::IsDown("TalkKey"))
	{
		BoostSpeed = 30.0f;
		IsBoost = true;

		if (1 <= ReadSoundCount)
		{
			GameEngineSound::Play("UI_Talk.wav");
			ReadSoundCount = 0;
		}
	}

	Progress += _DeltaTime * ReadSpeed * BoostSpeed / static_cast<float>(MainText.size());

	if (1.0f <= Progress)
	{
		Progress = 1.0f;
		IsReadEnd = true;
		ArrowPtr->On();
	}
	else
	{
		ArrowPtr->Off();
	}

	size_t ReadIndex = static_cast<size_t>(MainText.size() * Progress);

	for (size_t i = 0; i < ReadIndex; i++)
	{
		if (MainText[i] == L'%')
		{
			std::shared_ptr<GameEngineUIRenderer> NewImageRender = CreateComponent<GameEngineUIRenderer>();
			NewImageRender->SetScaleToTexture(KeyImageDatas[KeyImages.size()].KeyImageName);
			NewImageRender->GetTransform()->SetLocalPosition(KeyImageDatas[KeyImages.size()].LocaPos);

			float4 Scale = NewImageRender->GetTransform()->GetLocalScale();
			NewImageRender->GetTransform()->SetLocalScale(Scale * 1.5f);

			KeyImages.push_back(NewImageRender);

			MainText[i] = L'��';
			break;
		}
	}

	if (0 == ReadIndex)
	{
		return;
	}

	if (PrevAddIndex != ReadIndex)
	{
		if (MainText[ReadIndex] != L'\n')
		{
			++ReadSoundCount;
			PrevAddIndex = static_cast<int>(ReadIndex);
		}
		else
		{
			ReadSoundCount = 0;
		}
	}

	if (false == IsBoost)
	{
		if (2 <= ReadSoundCount)
		{
			GameEngineSound::Play("UI_Talk.wav");
			ReadSoundCount = 0;
		}
	}

	if (false == FontTextRender->IsUpdate())
	{
		FontTextRender->On();
	}

	std::wstring ReadText;
	ReadText.resize(ReadIndex + 1);

	memcpy_s(ReadText.data(), ReadIndex * 2, MainText.data(), ReadIndex * 2);

	ReadText[ReadIndex] = L'\0';

	FontTextRender->SetText(GameEngineString::UniCodeToAnsi(ReadText));
}