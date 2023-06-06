#include "PrecompileHeader.h"
#include "OpeningLevel.h"

#include <GameEnginePlatform/GameEngineInput.h>

#include "MultiBackground.h"
#include "FadeActor.h"
#include "TalkBox.h"
#include "Player.h"

#include "StoryHero_FirstHero_Opening.h"

OpeningLevel::OpeningLevel()
{
}

OpeningLevel::~OpeningLevel()
{
}

void OpeningLevel::Start()
{
	BattleLevel::Start();

	GameEngineDirectory DirectoryPath;
	DirectoryPath.MoveParentToDirectory("Resources");
	DirectoryPath.Move("Resources");
	DirectoryPath.Move("Data");
	DirectoryPath.Move("1_Opening");

	{
		DirectoryPath.Move("Background");
		BattleAreaPtr->LoadBackground(DirectoryPath, "DB_Opening_Background");
		DirectoryPath.MoveParent();
	}

	{
		DirectoryPath.Move("Map");
		BattleAreaPtr->LoadMap(DirectoryPath, "DB_Opening_01_Map");
		BattleAreaPtr->LoadMap(DirectoryPath, "DB_Opening_02_Map");
		BattleAreaPtr->LoadMap(DirectoryPath, "DB_Opening_03_Map");
		BattleAreaPtr->LoadMap(DirectoryPath, "DB_Opening_04_Map");
		DirectoryPath.MoveParent();
	}

	StageNameInfos.reserve(4);

	StageNameInfos.push_back({"DB_Opening_01_Map", "DB_Opening_Background"});
	StageNameInfos.push_back({"DB_Opening_02_Map", "DB_Opening_Background"});
	StageNameInfos.push_back({"DB_Opening_03_Map", "DB_Opening_Background"});
	StageNameInfos.push_back({"DB_Opening_04_Map", "DB_Opening_Background"});

	CurStageIndex = 0;
	MainStageName = StageNameInfos[CurStageIndex].LoadMapName;
	MainBackgroundName = StageNameInfos[CurStageIndex].LoadBackgroundName;

	CreateOpeningEvent();

	if (false == GameEngineInput::IsKey("MagicDebugKey"))
	{
		GameEngineInput::CreateKey("MagicDebugKey", '5');
	}
}

void OpeningLevel::Update(float _DeltaTime)
{
	BattleLevel::Update(_DeltaTime);

	if (true == GameEngineInput::IsDown("MagicDebugKey"))
	{
		std::shared_ptr<StoryHero_FirstHero_Opening> Ptr = CreateActor<StoryHero_FirstHero_Opening>();

		Ptr->GetTransform()->SetLocalPosition(GameEngineWindow::GetScreenSize().half());
		Ptr->PlayLandingEffect();
	}
}

void OpeningLevel::LevelChangeStart()
{
	BattleLevel::LevelChangeStart();
	IsMongalEncounter = false;
}

void OpeningLevel::AreaClear()
{
	MoveLevel("Castle");
}
