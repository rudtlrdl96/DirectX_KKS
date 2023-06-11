#include "PrecompileHeader.h"
#include "FieldNoteActor.h"
#include "KeyActor.h"

FieldNoteActor::FieldNoteActor()
{
}

FieldNoteActor::~FieldNoteActor()
{
}

void FieldNoteActor::SetText(const std::string_view& _Text)
{
	NoteText = _Text;


}

void FieldNoteActor::Start()
{
	if (nullptr == GameEngineTexture::Find("Interaction_BackGround.png"))
	{
		GameEngineDirectory Path;
		Path.MoveParentToDirectory("Resources");
		Path.Move("Resources");
		Path.Move("Texture");
		Path.Move("0_Common");
		Path.Move("UI");

		GameEngineTexture::Load(Path.GetPlusFileName("Interaction_BackGround.png").GetFullPath());
	}

	BackRender = CreateComponent<ContentSpriteRenderer>();
	BackRender->PipeSetting("2DTexture_Color");
	BackRender->GetShaderResHelper().SetConstantBufferLink("ColorBuffer", Buffer);
	BackRender->SetScaleToTexture("Interaction_BackGround.png");

	float4 Sclae = BackRender->GetTransform()->GetLocalScale();

	Sclae.x *= 3.5f;
	Sclae.y *= 1.8f;

	BackRender->GetTransform()->SetLocalScale(Sclae);
}
