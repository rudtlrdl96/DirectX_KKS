#pragma once

enum class TileArea
{
	None,			// ������ None
	Opening,		// ������
	Castle,			// ĳ��
	ForestOfHamory,	// ��ȭ�� ��
	GrandHall,		// �׷��� Ȧ
	SacredGrounds,	// ����
};

class TilemapData
{
public:
	std::string Name = "";
	UINT Index = 0;
	TileArea Grade = TileArea::None;
};