#include "PrecompileHeader.h"
#include "ContentSpriteRenderer.h"

const SpriteInfo& ContentAnimationInfo::CurSpriteInfo()
{
	const SpriteInfo& Info = Sprite->GetSpriteInfo(FrameIndex[CurFrame]);
	return Info;
}

bool ContentAnimationInfo::IsEnd()
{
	return IsEndValue;
}

void ContentAnimationInfo::Reset()
{
	CurFrame = 0;
	CurTime = FrameTime[0];
	IsEndValue = false;
	IsPauseValue = false;
}

void ContentAnimationInfo::Update(float _DeltaTime)
{
	if (true == IsEndValue)
	{
		if (true == Loop)
		{
			CurFrame = 0;
		}
	}

	IsEndValue = false;

	// 1;
	// 

	if (true == IsPauseValue)
	{
		return;
	}


	size_t CurFrameIndex = 0;

	if (FrameIndex.size() <= CurFrame)
	{
		CurFrameIndex = FrameIndex.size() - 1;
	}
	else
	{
		CurFrameIndex = FrameIndex[CurFrame];
	}


	if (UpdateEventFunction.end() != UpdateEventFunction.find(CurFrameIndex))
	{
		UpdateEventFunction[CurFrameIndex]();
	}

	CurTime -= _DeltaTime;

	if (0.0f >= CurTime)
	{
		++CurFrame;

		if (FrameIndex.size() <= CurFrame)
		{
			IsEndValue = true;
			CurFrame = FrameIndex.size() - 1;
		}
		else
		{
			CurFrameIndex = FrameIndex[CurFrame];

			//Start콜백이 있다면 콜백을 호출
			if (StartEventFunction.end() != StartEventFunction.find(CurFrameIndex))
			{
				StartEventFunction[CurFrameIndex]();
			}
		}

		CurTime += FrameTime[CurFrame];

		// 0 ~ 9

		// 9
	}
}

// SpriteRenderer

ContentSpriteRenderer::ContentSpriteRenderer()
{
}

ContentSpriteRenderer::~ContentSpriteRenderer()
{
}


void ContentSpriteRenderer::Start()
{
	SetMesh("Rect");
	GameEngineRenderer::Start();
}

void ContentSpriteRenderer::SetTexture(const std::string_view& _Name)
{
	GetShaderResHelper().SetTexture("DiffuseTex", _Name);
}

void ContentSpriteRenderer::SetFlipX()
{
	float4 LocalScale = GetTransform()->GetLocalScale();
	LocalScale.x = -LocalScale.x;
	GetTransform()->SetLocalScale(LocalScale);
}

void ContentSpriteRenderer::SetFlipY()
{
	float4 LocalScale = GetTransform()->GetLocalScale();
	LocalScale.y = -LocalScale.y;
	GetTransform()->SetLocalScale(LocalScale);
}

void ContentSpriteRenderer::SetScaleToTexture(const std::string_view& _Name)
{
	GetShaderResHelper().SetTexture("DiffuseTex", _Name);
	std::shared_ptr<GameEngineTexture> FindTex = GameEngineTexture::Find(_Name);

	if (nullptr == FindTex)
	{
		MsgAssert("존재하지 않는 이미지 입니다.");
		return;
	}

	float4 Scale = float4(static_cast<float>(FindTex->GetWidth()), static_cast<float>(FindTex->GetHeight()), 1);
	GetTransform()->SetLocalScale(Scale);
}

void ContentSpriteRenderer::SetSprite(const std::string_view& _SpriteName, size_t _Frame/* = 0*/)
{
	Sprite = GameEngineSprite::Find(_SpriteName);
	Frame = _Frame;

	const SpriteInfo& Info = Sprite->GetSpriteInfo(Frame);
	GetShaderResHelper().SetTexture("DiffuseTex", Info.Texture);
	AtlasData = Info.CutData;
}

void ContentSpriteRenderer::SetFrame(size_t _Frame)
{
	Frame = _Frame;

	const SpriteInfo& Info = Sprite->GetSpriteInfo(Frame);
	GetShaderResHelper().SetTexture("DiffuseTex", Info.Texture);
	AtlasData = Info.CutData;
}

std::shared_ptr<ContentAnimationInfo> ContentSpriteRenderer::FindAnimation(const std::string_view& _Name)
{
	std::map<std::string, std::shared_ptr<ContentAnimationInfo>>::iterator FindIter = Animations.find(_Name.data());

	if (FindIter == Animations.end())
	{
		return nullptr;
	}

	return FindIter->second;
}

std::shared_ptr<ContentAnimationInfo> ContentSpriteRenderer::CreateAnimation(const ContentAnimationParameter& _Paramter)
{
	if (nullptr != FindAnimation(_Paramter.AnimationName))
	{
		MsgAssert("이미 존재하는 이름의 애니메이션을 또 만들려고 했습니다." + std::string(_Paramter.AnimationName));
		return nullptr;
	}

	std::shared_ptr<GameEngineSprite> Sprite = GameEngineSprite::Find(_Paramter.SpriteName);

	if (nullptr == Sprite)
	{
		MsgAssert("존재하지 않는 스프라이트로 애니메이션을 만들려고 했습니다." + std::string(_Paramter.AnimationName));
		return nullptr;
	}

	std::shared_ptr<ContentAnimationInfo> NewAnimation = std::make_shared<ContentAnimationInfo>();
	Animations[_Paramter.AnimationName.data()] = NewAnimation;

	if (0 != _Paramter.FrameIndex.size())
	{
		// 프레임 인덱스 입력시
		NewAnimation->FrameIndex = _Paramter.FrameIndex;

	}
	else
	{
		// 프레임 인덱스 미 입력시

		// 시작 프레임 지정
		if (-1 != _Paramter.Start)
		{
			if (_Paramter.Start < 0)
			{
				MsgAssert("스프라이트 범위를 초과하는 인덱스로 애니메이션을 마들려고 했습니다." + std::string(_Paramter.AnimationName));
				return nullptr;
			}

			NewAnimation->StartFrame = _Paramter.Start;
		}
		else
		{
			// -1 입력시 시작프레임 0
			NewAnimation->StartFrame = 0;
		}
		// 끝 프레임 지정
		if (-1 != _Paramter.End)
		{
			if (_Paramter.End >= Sprite->GetSpriteCount())
			{
				MsgAssert("스프라이트 범위를 초과하는 인덱스로 애니메이션을 생성하려 했습니다." + std::string(_Paramter.AnimationName));
				return nullptr;
			}

			NewAnimation->EndFrame = _Paramter.End;
		}
		else
		{
			// -1 입력시 끝프레임은 마지막
			NewAnimation->EndFrame = Sprite->GetSpriteCount() - 1;
		}

		if (NewAnimation->EndFrame < NewAnimation->StartFrame)
		{
			MsgAssert("애니메이션을 생성할때 End가 Start보다 클 수 없습니다");
			return nullptr;
		}
		NewAnimation->FrameIndex.reserve(NewAnimation->EndFrame - NewAnimation->StartFrame + 1);

		// StartFrame 부터 EndFrame까지 순서대로 FrameIndex에 푸시
		for (size_t i = NewAnimation->StartFrame; i <= NewAnimation->EndFrame; ++i)
		{
			NewAnimation->FrameIndex.push_back(i);
		}
	}

	// 타임 데이터가 있다면
	if (0 != _Paramter.FrameTime.size())
	{
		NewAnimation->FrameTime = _Paramter.FrameTime;
	}
	else
	{
		for (size_t i = 0; i < NewAnimation->FrameIndex.size(); ++i)
		{
			NewAnimation->FrameTime.push_back(_Paramter.FrameInter);
		}
	}



	NewAnimation->Sprite = Sprite;
	NewAnimation->Parent = this;
	NewAnimation->Loop = _Paramter.Loop;
	NewAnimation->ScaleToTexture = _Paramter.ScaleToTexture;

	return NewAnimation;
}


void ContentSpriteRenderer::ChangeAnimation(const std::string_view& _Name, size_t _Frame, bool _Force)
{
	std::shared_ptr<ContentAnimationInfo> Find = FindAnimation(_Name);

	if (nullptr == Find)
	{
		MsgAssert("이러한 이름의 애니메이션은 존재하지 않습니다" + std::string(_Name));
		return;
	}

	if (CurAnimation == Find && false == _Force)
	{
		return;
	}

	CurAnimation = FindAnimation(_Name);
	CurAnimation->Reset();

	if (_Frame != -1)
	{
		CurAnimation->CurFrame = _Frame;
	}

	if (CurAnimation->CurFrame >= CurAnimation->FrameIndex.size())
	{
		CurAnimation->CurFrame = 0;
	}
}

void ContentSpriteRenderer::Update(float _Delta)
{
	if (nullptr != CurAnimation)
	{
		CurAnimation->Update(_Delta);
		const SpriteInfo& Info = CurAnimation->CurSpriteInfo();

		GetShaderResHelper().SetTexture("DiffuseTex", Info.Texture);
		AtlasData = Info.CutData;

		if (true == CurAnimation->ScaleToTexture)
		{
			std::shared_ptr<GameEngineTexture> Texture = Info.Texture;

			float4 Scale = Texture->GetScale();

			Scale.x *= Info.CutData.SizeX;
			Scale.y *= Info.CutData.SizeY;
			Scale.z = 1.0f;

			Scale *= ScaleRatio;

			GetTransform()->SetLocalScale(Scale);
		}
	}
}

void ContentSpriteRenderer::Render(float _Delta)
{

	GameEngineRenderer::Render(_Delta);
}

void ContentSpriteRenderer::SetAnimationUpdateEvent(const std::string_view& _AnimationName, size_t _Frame, std::function<void()> _Event)
{
	std::shared_ptr<ContentAnimationInfo>  Info = FindAnimation(_AnimationName);

	if (nullptr == Info)
	{
		MsgAssert("존재하지 않는 애니메이션에 이벤트 세팅을 하려고 했습니다.");
	}

	Info->UpdateEventFunction[_Frame] = _Event;
}

void ContentSpriteRenderer::SetAnimationStartEvent(const std::string_view& _AnimationName, size_t _Frame, std::function<void()> _Event)
{
	std::shared_ptr<ContentAnimationInfo>  Info = FindAnimation(_AnimationName);

	if (nullptr == Info)
	{
		MsgAssert("존재하지 않는 애니메이션에 이벤트 세팅을 하려고 했습니다.");
	}

	Info->StartEventFunction[_Frame] = _Event;
}

std::string ContentSpriteRenderer::GetTexName()
{
	GameEngineTextureSetter* Tex = GetShaderResHelper().GetTextureSetter("DiffuseTex");
	std::string Name = Tex->Res->GetNameToString();
	return Name;
}

void ContentSpriteRenderer::PipeSetting(const std::string_view _PipeName, bool _IsAtlasData /*= true*/)
{
	SetPipeLine(_PipeName);

	AtlasData.x = 0.0f;
	AtlasData.y = 0.0f;
	AtlasData.z = 1.0f;
	AtlasData.w = 1.0f;

	if (true == _IsAtlasData)
	{
		GetShaderResHelper().SetConstantBufferLink("AtlasData", AtlasData);
	}
}
