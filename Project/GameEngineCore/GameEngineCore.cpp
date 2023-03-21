#include "GameEngineCore.h"
#include <GameEngineBase\GameEngineDebug.h>
#include <GameEnginePlatform\GameEngineInput.h>
#include <GameEnginePlatform\GameEngineWindow.h>
#include <GameEnginePlatform\GameEngineSound.h>
#include <GameEnginePlatform\GameEngineInput.h>
#include <GameEngineBase\GameEngineTime.h>

std::map<std::string, std::shared_ptr<GameEngineLevel>> GameEngineCore::LevelMap;
std::shared_ptr<GameEngineLevel> GameEngineCore::MainLevel = nullptr;
std::shared_ptr<GameEngineLevel> GameEngineCore::NextLevel = nullptr;

GameEngineCore::GameEngineCore() 
{
}

GameEngineCore::~GameEngineCore() 
{
}

void GameEngineCore::EngineStart(std::function<void()> _ContentsStart)
{
	if (nullptr == _ContentsStart)
	{
		MsgAssert("시작 컨텐츠가 존재하지 않습니다.");
	}
	_ContentsStart();
}

void GameEngineCore::EngineUpdate() 
{
	if (nullptr != NextLevel)
	{
		MainLevel = NextLevel;
	}

	if (nullptr == MainLevel)
	{
		MsgAssert("아무런 레벨도 동작하지 않고 있습니다.");
		return;
	}

	GameEngineSound::SoundUpdate();
	float TimeDeltaTime = GameEngineTime::GlobalTime.TimeCheck();
	GameEngineInput::Update(TimeDeltaTime);

	MainLevel->TimeEvent.Update(TimeDeltaTime);
	MainLevel->Update(TimeDeltaTime);
	MainLevel->Render(TimeDeltaTime);
}

void GameEngineCore::EngineEnd(std::function<void()> _ContentsEnd)
{
	if (nullptr == _ContentsEnd)
	{
		MsgAssert("끝 컨텐츠가 존재하지 않습니다.");
	}

	_ContentsEnd();

	LevelMap.clear();
}

void GameEngineCore::Start(HINSTANCE _instance,  std::function<void()> _Start, std::function<void()> _End, float4 _Pos, float4 _Size)
{
	GameEngineDebug::LeakCheck();

	if (false == GameEngineInput::IsKey("EngineMouseLeft"))
	{
		GameEngineInput::CreateKey("EngineMouseLeft", VK_LBUTTON);
		GameEngineInput::CreateKey("EngineMouseRight", VK_RBUTTON);
	}

	GameEngineWindow::WindowCreate(_instance, "MainWindow", _Size, _Pos);
	GameEngineWindow::WindowLoop(std::bind(GameEngineCore::EngineStart, _Start), GameEngineCore::EngineUpdate, std::bind(GameEngineCore::EngineEnd, _End));
}

void GameEngineCore::ChangeLevel(const std::string_view& _Name) 
{
	std::string UpperName = GameEngineString::ToUpper(_Name);

	if (LevelMap.end() == LevelMap.find(UpperName))
	{
		MsgAssert("존재하지 않는 레벨로 체인지 하려고 했습니다.");
		return;
	}

	NextLevel = LevelMap[UpperName];
}

void GameEngineCore::LevelInit(std::shared_ptr<GameEngineLevel> _Level) 
{
	_Level->Loading();
}