#include "PrecompileHeader.h"
#include "EndingLogoLevel.h"
#include "BlackBackground.h"
#include "SkulLogo.h"

EndingLogoLevel::EndingLogoLevel()
{
}

EndingLogoLevel::~EndingLogoLevel()
{
}

void EndingLogoLevel::Start()
{
	UILevel::Start();

	GameEngineDirectory Path;
	Path.MoveParentToDirectory("Resources");
	Path.Move("Resources");
	Path.Move("Texture");
	Path.Move("UI");
	Path.Move("Ending");

	{
		std::vector<GameEngineFile> Files = Path.GetAllFile({ ".png" });

		for (size_t i = 0; i < Files.size(); i++)
		{
			GameEngineTexture::Load(Files[i].GetFullPath());
		}
	}

	BackPtr = CreateActor<BlackBackground>();
	LogoPtr = CreateActor<SkulLogo>();
}

void EndingLogoLevel::Update(float _DeltaTime)
{
	UILevel::Update(_DeltaTime);
}
