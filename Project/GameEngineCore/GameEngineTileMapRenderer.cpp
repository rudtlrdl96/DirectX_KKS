#include "PrecompileHeader.h"
#include "GameEngineTileMapRenderer.h"
#include "GameEngineSprite.h"
#include "GameEngineLevel.h"

GameEngineTileMapRenderer::GameEngineTileMapRenderer()
{
}

GameEngineTileMapRenderer::~GameEngineTileMapRenderer()
{
}

void GameEngineTileMapRenderer::Start()
{
	GameEngineRenderer::Start();

	SetPipeLine("2DTexture");

	AtlasData.x = 0.0f;
	AtlasData.y = 0.0f;
	AtlasData.z = 1.0f;
	AtlasData.w = 1.0f;

	ColorOptionValue.MulColor = float4::One;
	ColorOptionValue.PlusColor = float4::Null;

	GetShaderResHelper().SetConstantBufferLink("AtlasData", AtlasData);
	GetShaderResHelper().SetConstantBufferLink("ColorOption", ColorOptionValue);
}

void GameEngineTileMapRenderer::CreateTileMap(int _X, int _Y, const float4& _TileSize)
{
	TileSize = _TileSize;
	TileSize.z = 1.0f;

	MapCount.x = static_cast<float>(_X);
	MapCount.y = static_cast<float>(_Y);

	Tiles.resize(_Y);

	for (size_t y = 0; y < Tiles.size(); y++)
	{
		Tiles[y].resize(_X);
	}
}

void GameEngineTileMapRenderer::Clear()
{
	Tiles.clear();
}

void GameEngineTileMapRenderer::SetTile(int _X, int _Y, const std::string_view& _SpriteName, int _Index)
{
	if (true == Tiles.empty())
	{
		MsgAssert("CreateTileMap을 먼저 호출해주셔야 합니다.");
	}


	// 인덱스 오버는 만들어주세요
	if (true == IsOver(_X, _Y))
	{
		return;
	}


	std::shared_ptr<GameEngineSprite> Sprite = GameEngineSprite::Find(_SpriteName);

	Tiles[_X][_Y].Sprite = Sprite.get();
	Tiles[_X][_Y].Index = _Index;
}

bool GameEngineTileMapRenderer::IsOver(int _X, int _Y) const
{
	if (MapCount.ix() <= _X)
	{
		return true;
	}

	if (MapCount.iy() <= _Y)
	{
		return true;
	}

	return false;
}

void GameEngineTileMapRenderer::Render(float _Delta)
{
	const TransformData& TransData = GetTransform()->GetTransDataRef();
	// 

	TransformData TileTransData = TransData;

	float4x4 Scale;
	float4x4 Pos;
	float4x4 Rot;
	float4 vPos;

	for (size_t y = 0; y < Tiles.size(); y++)
	{
		for (size_t x = 0; x < Tiles[y].size(); x++)
		{
			GameEngineSprite* Sprite = Tiles[y][x].Sprite;
			const SpriteInfo& SpriteInfo = Sprite->GetSpriteInfo(Tiles[y][x].Index);

			// 트랜스폼 세팅
			{
				vPos = { TileSize.x * x, TileSize.y * y, 1.0f };

				Scale.Scale(TileSize);
				Pos.Pos(vPos);
				TileTransData.WorldViewProjectionMatrix = Scale * Pos * TransData.WorldMatrix;

				float4 WorldPos = TileTransData.WorldViewProjectionMatrix.ArrVector[3];

				//if (true == GetLevel()->IsCameraOver(WorldPos))
				//{
				//	continue;
				//}

				TileTransData.WorldViewProjectionMatrix = TileTransData.WorldViewProjectionMatrix * TransData.View * TransData.Projection;

				GetShaderResHelper().SetConstantBufferLink("TransformData", TileTransData);
			}

			// 텍스처 세팅
			{
				GetShaderResHelper().SetTexture("DiffuseTex", SpriteInfo.Texture);
				AtlasData = SpriteInfo.CutData;
			}

			GameEngineRenderer::Render(_Delta);
		}
	}

	GetShaderResHelper().SetConstantBufferLink("TransformData", GetTransform()->GetTransDataRef());

}
