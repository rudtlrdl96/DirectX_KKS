#pragma once
#include "GameEngineConstantBuffer.h"
#include "GameEngineTexture.h"

// 모든 텍스처건 상수버퍼건 
class GameEngineShaderResources
{
public:
	std::string Name;
	class GameEngineShader* ParentShader;
	int BindPoint = -1; // b0 t0 같은 몇번째 슬롯에 세팅되어야 하는지에 대한 정보.
};

class GameEngineConstantBufferSetter : public GameEngineShaderResources
{
public:
	std::shared_ptr<GameEngineConstantBuffer> Res;
};

class GameEngineTextureSetter : public GameEngineShaderResources
{
public:
	std::shared_ptr<GameEngineTexture> Res;
};

class GameEngineShaderResHelper
{
private:
	std::multimap<std::string, GameEngineConstantBufferSetter> ConstantBuffer;

public:
	void CreateConstantBufferSetter(const GameEngineConstantBufferSetter& _Buffer) 
	{
		ConstantBuffer.insert(std::make_pair(_Buffer.Name, _Buffer));
	}

	void Copy(const GameEngineShaderResHelper& _ResHelper);
};
