#pragma once
#include "GameEngineRenderer.h"
#include "GameEngineSprite.h"
#include <map>
#include "EngineContentRenderingStruct.h"

class Tile 
{
public:
	GameEngineSprite* Sprite;
	int Index = 0;
};



// ���� :
class GameEngineTileMapRenderer : public GameEngineRenderer
{
public:
	// constrcuter destructer
	GameEngineTileMapRenderer();
	~GameEngineTileMapRenderer();

	// delete Function
	GameEngineTileMapRenderer(const GameEngineTileMapRenderer& _Other) = delete;
	GameEngineTileMapRenderer(GameEngineTileMapRenderer&& _Other) noexcept = delete;
	GameEngineTileMapRenderer& operator=(const GameEngineTileMapRenderer& _Other) = delete;
	GameEngineTileMapRenderer& operator=(GameEngineTileMapRenderer&& _Other) noexcept = delete;

	void CreateTileMap(int _X, int _Y, const float4& _TileSize);

	void Clear();

	void SetTile(int _X, int _Y, const std::string_view& _SpriteName, int _Index = 0);

protected:

	void Render(float _Delta) override;


private:
	std::vector<std::vector<Tile>> Tiles;
	float4 MapCount;
	ColorOption ColorOptionValue;
	float4 AtlasData;
	float4 TileSize;

	void Start() override;
};

