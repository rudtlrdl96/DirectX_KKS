#include "PrecompileHeader.h"
#include "Tilemap.h"
#include <GameEngineCore/GameEngineLevel.h>
#include "TileActor.h"

Tilemap::Tilemap() :
	TileScale(ContentConst::TileSize)
{
}

Tilemap::~Tilemap()
{
}


void Tilemap::SetDepth(UINT _DepthCount)
{
	if (0 == _DepthCount)
	{		
		return;
	}

	TilemapDatas.reserve(_DepthCount);

	for (UINT i = _DepthCount; i < TilemapDatas.size(); i++)
	{
		std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[i];

		for (UINT y	 = 0; y < TilemapRef.size(); y++)
		{
			for (UINT x = 0; x < TilemapRef[y].size(); x++)
			{
				if (nullptr != TilemapRef[y][x])
				{
					TilemapRef[y][x]->Death();
					TilemapRef[y][x] = nullptr;
				}
			}

			TilemapRef[y].clear();
		}

		TilemapRef.clear();
	}

	TilemapDatas.resize(_DepthCount);

	ResizeTilemap(static_cast<UINT>(TilemapSize.x), static_cast<UINT>(TilemapSize.y));
}

void Tilemap::ResizeTilemap(UINT _SizeX, UINT _SizeY)
{
	int2 PrevTileSize = TilemapSize;
	TilemapSize = int2(_SizeX, _SizeY); 

	for (UINT i = 0; i < TilemapDatas.size(); i++)
	{
		std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[i];

		for (UINT y = 0; y < TilemapRef.size(); y++)
		{
			for (UINT x = 0; x < TilemapRef[y].size(); x++)
			{
				if (nullptr != TilemapRef[y][x])
				{
					TilemapRef[y][x]->TileOff();
				}
			}
		}
	}

	for (UINT i = 0; i < TilemapDatas.size(); i++)
	{
		std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[i];

		if (TilemapRef.size() < TilemapSize.y)
		{
			TilemapRef.resize(TilemapSize.y);
		}

		for (UINT y = 0; y < static_cast<UINT>(TilemapSize.y); y++)
		{
			if (TilemapRef[y].size() < TilemapSize.x)
			{
				TilemapRef[y].resize(TilemapSize.x);
			}

			for (UINT x = 0; x < static_cast<UINT>(TilemapSize.x); x++)
			{
				if (nullptr != TilemapRef[y][x])
				{
					if (0 != TilemapRef[y][x]->GetTileIndex())
					{
						TilemapRef[y][x]->TileOn();
					}
				}
				else
				{
					CreateTile(i, x, y);
				}
			}
		}
	}
}

void Tilemap::ChangeData(int _Depth, const float4& _WorldPos, UINT Index)
{
	int2 InputIndex = GetTileIndex(_WorldPos);

	if (InputIndex.x < 0)
	{
		return;
	}

	if (InputIndex.y < 0)
	{
		return;
	}

	ChangeData(_Depth, InputIndex.x, InputIndex.y, Index);
}

void Tilemap::ChangeData(int _Depth, UINT _X, UINT _Y, UINT Index)
{
	std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[_Depth];
	
	if (true == IsOver(_X, _Y))
	{
		return;
	}

	TilemapRef[_Y][_X]->SetTileData(Index);
}

void Tilemap::ChangeData(int _Depth, UINT _StartX, UINT _EndX, UINT _StartY, UINT _EndY, size_t Index)
{
	if (true == IsOver(_StartX, _StartY))
	{
		return;
	}

	if (true == IsOver(_EndX, _EndY))
	{
		return;
	}

	std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[_Depth];

	for (UINT y = _StartY; y <= _EndY; y++)
	{
		for (UINT x = _StartX; x <= _EndX; x++)
		{
			TilemapRef[y][x]->SetTileData(Index);
		}
	}
}

void Tilemap::ChangeData(int _Depth, UINT _StartX, UINT _StartY, const std::vector<std::vector<size_t>>& _Indexs)
{ 
	if (_Indexs.size() == 0)
	{
		return;
	}

	if (_Indexs[0].size() == 0)
	{
		return;
	}

	if (true == IsOver(_StartX, _StartY))
	{
		return;
	}

	std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[_Depth];

	for (UINT y = 0; y < _Indexs.size(); y++)
	{
		for (UINT x = 0; x < _Indexs[y].size(); x++)
		{
			if (true == IsOver(_StartX + x, _StartY + y))
			{
				continue;
			}

			TilemapRef[_StartY + y][_StartX + x]->SetTileData(_Indexs[y][x]);
		}
	}
}

void Tilemap::ClearTileMap()
{
	for (UINT i = 0; i < TilemapDatas.size(); i++)
	{
		std::vector<std::vector<std::shared_ptr<TileActor>>>& TilemapRef = TilemapDatas[i];

		for (UINT y = 0; y < TilemapRef.size(); y++)
		{
			for (UINT x = 0; x < TilemapRef[y].size(); x++)
			{
				if (nullptr != TilemapRef[y][x])
				{
					TilemapRef[y][x]->Death();
					TilemapRef[y][x] = nullptr;
				}
			}

			TilemapRef[y].clear();
		}

		TilemapRef.clear();
	}

	TilemapDatas.clear();
	TilemapSize = int2::Zero;
}

bool Tilemap::IsOver(UINT _X, UINT _Y)
{
	if (TilemapSize.x <= static_cast<int>(_X) || TilemapSize.y <= static_cast<int>(_Y))
	{
		return true;
	}

	return false;
}

int2 Tilemap::GetTileIndex(const float4& _WorldPos)
{
	float4 LocalPos = _WorldPos - GetTransform()->GetWorldPosition();

	LocalPos.x += TileScale.hx();
	LocalPos.y += TileScale.y;

	int2 ResultIndex = int2::Zero;

	float IndexFloatX = LocalPos.x / TileScale.x;
	float IndexFloatY = LocalPos.y / TileScale.y;

	if (IndexFloatX < 0)
	{
		ResultIndex.x = static_cast<int>(floorf(IndexFloatX));
	}
	else
	{
		ResultIndex.x = static_cast<int>(IndexFloatX);
	}

	if (IndexFloatY < 0)
	{
		ResultIndex.y = static_cast<int>(floorf(IndexFloatY));
	}
	else
	{
		ResultIndex.y = static_cast<int>(IndexFloatY);
	}

	return ResultIndex;
}

float4 Tilemap::GetTilePos(UINT _X, UINT _Y) const
{
	return float4(static_cast<float>(_X), static_cast<float>(_Y), 0, 0) * TileScale;
}

Tilemap_Meta Tilemap::GetTilemap_DESC(int _Depth)
{
	Tilemap_Meta Result = Tilemap_Meta();

	float4 TilemapPos = GetTransform()->GetWorldPosition();

	TilemapPos.x -= ContentConst::TileSize.hx();
	TilemapPos.y -= ContentConst::TileSize.y;

	Result.Bottom = TilemapPos.y;
	Result.Top	  = TilemapPos.y + (TilemapSize.y * TileScale.y);
	Result.Left   = TilemapPos.x;
	Result.Right  = TilemapPos.x + (TilemapSize.x * TileScale.x);

	return Result;
}

void Tilemap::SaveBin(GameEngineSerializer& _SaveSerializer)
{
	if (0 >= TilemapSize.x)
	{
		MsgAssert_Rtti<Tilemap>(" - X 사이즈가 0인 타일맵은 저장할 수 없습니다");
		return;
	}

	if (0 >= TilemapSize.y)
	{
		MsgAssert_Rtti<Tilemap>(" - Y 사이즈가 0인 타일맵은 저장할 수 없습니다");
		return;
	}

	_SaveSerializer.Write(static_cast<UINT>(TilemapDatas.size()));
	_SaveSerializer.Write(static_cast<int>(TilemapSize.x));
	_SaveSerializer.Write(static_cast<int>(TilemapSize.y));

	using namespace std;

	for (UINT i = 0; i < TilemapDatas.size(); i++)
	{
		const vector<vector<shared_ptr<TileActor>>>& TileRef = TilemapDatas[i];

		for (UINT y = 0; y < static_cast<UINT>(TilemapSize.y); y++)
		{
			for (UINT x = 0; x < static_cast<UINT>(TilemapSize.x); x++)
			{
				TileRef[y][x]->SaveBin(_SaveSerializer);
			}
		}
	}
}

void Tilemap::LoadBin(GameEngineSerializer& _LoadSerializer)
{
	ClearTileMap();

	int DepthCount = 0;
	_LoadSerializer.Read(DepthCount);
	SetDepth(static_cast<UINT>(DepthCount));

	_LoadSerializer.Read(TilemapSize.x);
	_LoadSerializer.Read(TilemapSize.y);

	ResizeTilemap(static_cast<UINT>(TilemapSize.x), static_cast<UINT>(TilemapSize.y));

	for (UINT i = 0; i < static_cast<UINT>(DepthCount); i++)
	{
		for (UINT y = 0; y < static_cast<UINT>(TilemapSize.y); y++)
		{
			for (UINT x = 0; x < static_cast<UINT>(TilemapSize.x); x++)
			{
				UINT LoadIndex = TileActor::LoadBin(_LoadSerializer);
				ChangeData(i, x, y, LoadIndex);
			}
		}
	}
}

void Tilemap::CreateTile(UINT _Depth, UINT _X, UINT _Y)
{
	std::shared_ptr<TileActor> NewTile = GetLevel()->CreateActor<TileActor>();

	NewTile->GetTransform()->SetParent(GetTransform());

	float4 TilePos = GetTilePos(_X, _Y);
	TilePos.z = _Depth * 10.0f;

	NewTile->GetTransform()->SetLocalPosition(TilePos);
	NewTile->SetTileData(0);

	TilemapDatas[_Depth][_Y][_X] = NewTile;
}
