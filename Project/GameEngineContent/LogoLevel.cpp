#include "LogoLevel.h"
#include <GameEngineBase/GameEngineMath.h>
#include <GameEnginePlatform/GameEngineInput.h>
#include <GameEnginePlatform/GameEngineWindow.h>
#include <GameEngineCore/GameEngineRenderer.h>
#include <GameEngineCore/GameEngineCamera.h>
#include "BaseContentActor.h"

LogoLevel::LogoLevel()
{
}

LogoLevel::~LogoLevel()
{
}

void LogoLevel::Start()
{
	GetMainCamera()->GetTransform()->SetLocalPosition({ 0, 0, -1000.0f });

	GameEngineInput::CreateKey("ActorRotX+", 'W');
	GameEngineInput::CreateKey("ActorRotY-", 'A');
	GameEngineInput::CreateKey("ActorRotX-", 'S');
	GameEngineInput::CreateKey("ActorRotY+", 'D');
	GameEngineInput::CreateKey("ActorRotZ-", 'Q');
	GameEngineInput::CreateKey("ActorRotZ+", 'E');
	
	std::shared_ptr<BaseContentActor> NewActorPtr = CreateActor<BaseContentActor>();
	NewActorPtr->GetTransform()->SetWorldPosition(float4::Zero);

	//float4 veca = float4::Left;
	//float4 vecb = float4::Left;
	//
	//float4 vecc = float4::Cross3DReturn(veca, vecb);


	// 벡터의 내적
	// 벡터와 벡터의 곱셈으로 스칼라 값이 결과로 나온다
	// 두 벡터의 각 성분끼리의 곱의 합
	// 두 개의 벡터가 있을때 한 벡터의 방향으로 나머지 하나를 projection(투영) 시킨 것과 다른 한 벡터의 크기의 곱
	// 내적 연산은 벡터 두 개를 하나의 스칼라 값으로 변환시키는 연산
	// 
	// A * B = A의 길이 * B의 길이 cos 세타
	// = (Ax * Bx) + (Ay * By) + (Az * Bz) 
	// 
	// 위와 같은 성질을 이용해 두 벡터간의 각도를 손쉽게 구할 수 있다

	// 벡터의 외적
	// 벡터와 벡터의 곱셈으로 벡터가 결과로 나온다
	// 외적 공식을 이용해 곱한 결과는 두 벡터의 동시에 수직인 벡터가 나오게된다
	// 다만 주의할것은 두 벡터의 값이 완전히 같거나 반대인 벡터의 경우 값이 
	// 제대로 나오지 않는다


	// 크기 행렬
	//
	// [x][y][z][w] Local Pos *** w == 1
	// 
	// *
	// 
	// [xs] [0]  [0]  [0]
	// [0]  [ys] [0]  [0]
	// [0]  [0]  [zs] [0]
	// [0]  [0]  [0]  [1]   
	//
	// 해당 행렬의 곱셈을 할 경우
	//  
	// 
	// x = (x * xs) + (y * 0) + (z * 0) + (w * 0)
	// y = (x * 0) + (y * ys) + (z * 0) + (w * 0)
	// z = (x * 0) + (y * 0) + (z * zs) + (w * 0)
	// w = (x * 0) + (y * 0) + (z * 0) + (w * 1)
	//
	// [x * xs] [y * ys] [z * zs] [w * 1] 해당과 같은 결과가 나오기 때문에
	// 행렬의 곱셈으로 크기 변환이 가능하다

	// 자전 행렬
	//

	// 이동 행렬
	//
	// [x][y][z][w] Local Pos *** w == 1
	// 
	// *
	// 
	// [1]  [0]  [0]  [0]
	// [0]  [1]  [1]  [0]
	// [0]  [0]  [1]  [0]
	// [xm] [ym] [zm] [1]   
	//
	// 해당 행렬의 곱셈을 할 경우
	//  
	// 
	// x = (x * 1) + (y * 0) + (z * 0) + (w * xm)
	// y = (x * 0) + (y * 1) + (z * 0) + (w * ym)
	// z = (x * 0) + (y * 0) + (z * 1) + (w * zm)
	// w = (x * 0) + (y * 0) + (z * 0) + (w * 1)
	//
	// [x + xs] [y + ys] [z + zs] [w * 1] 해당과 같은 결과가 나오기 때문에
	// 행렬의 이동 표현이 가능하다

	// 여기서 주의깊게 봐야할 점은 크기행렬 곱하기 이동 행렬을 할 경우 해당 행렬을 한번에 표현할 수 있다 
	// 
	// S
	// [xs] [0]  [0]  [0]
	// [0]  [ys] [0]  [0] 
	// [0]  [0]  [zs] [0] 
	// [0]  [0]  [0]  [1] 
	// 
	// *
	// 
	// M
	// [1]  [0]  [0]  [0]
	// [0]  [1]  [0]  [0]
	// [0]  [0]  [1]  [0]
	// [xm] [ym] [zm] [1]  
	// 
	// 
	// 00 = (S00 * M00) + (S00 * M10) + (S00 * M20) + (S00 * M30)
	// 01 = (S01 * M01) + (S01 * M11) + (S01 * M21) + (S01 * M31)
	// 02 = (S02 * M02) + (S02 * M12) + (S02 * M22) + (S02 * M32)
	// 03 = (S03 * M03) + (S03 * M13) + (S03 * M23) + (S03 * M33)
	// 
	// 10 = (S10 * M00) + (S10 * M10) + (S10 * M20) + (S10 * M30)
	// 11 = (S11 * M01) + (S11 * M11) + (S11 * M21) + (S11 * M31)
	// 12 = (S12 * M02) + (S12 * M12) + (S12 * M22) + (S12 * M32)
	// 13 = (S13 * M03) + (S13 * M13) + (S13 * M23) + (S13 * M33)
	// 
	// 20 = (S20 * M00) + (S20 * M10) + (S20 * M20) + (S20 * M30)
	// 21 = (S21 * M01) + (S21 * M11) + (S21 * M21) + (S21 * M31)
	// 22 = (S22 * M02) + (S22 * M12) + (S22 * M22) + (S22 * M32)
	// 23 = (S23 * M03) + (S23 * M13) + (S23 * M23) + (S23 * M33)
	// 
	// 30 = (S30 * M00) + (S30 * M10) + (S30 * M20) + (S30 * M30)
	// 31 = (S31 * M01) + (S31 * M11) + (S31 * M21) + (S31 * M31)
	// 32 = (S32 * M02) + (S32 * M12) + (S32 * M22) + (S32 * M32)
	// 33 = (S33 * M03) + (S33 * M13) + (S33 * M23) + (S33 * M33)
	// 
	// [xs] [0]  [0]  [0]
	// [0]  [ys] [0]  [0]
	// [0]  [0]  [zs] [0]
	// [xm] [ym] [zm] [1]  
		
	// 월드 행렬
	// [x][y][z][w] Local Pos *** w == 1
	// 
	// *
	// 
	// [xs] [0]  [0]  [0]
	// [0]  [ys] [0]  [0]
	// [0]  [0]  [zs] [0]
	// [xm] [ym] [zm] [1]   
	// 
	// 해당 행렬의 곱셈을 할 경우
	// 
	// x = (x * xs) + (y * 0) + (z * 0) + (w * xm)
	// y = (x * 0) + (y * ys) + (z * 0) + (w * ym)
	// z = (x * 0) + (y * 0) + (z * zs) + (w * zm)
	// w = (x * 0) + (y * 0) + (z * 0) + (w * 1)
	//
	// 위와 같은 행렬 연산이 나온다 크기행렬과 이동행렬을 동시에 곱해줄 수 있다
	//	
	// 행렬은 교환법칙이 성사하지 않기 때문에 행렬곱셈의 순서에 매우 조심해야 한다
	//
	//
	// 회전 행렬
	//
	// 먼저 행렬식이 아닌 백터의 회전의 경우
	// 
	// x축 회전 
	// y = Y * cosf(Rad) - Z * sinf(Rad);
	// z = Y * sinf(Rad) + Z * cosf(Rad);
	//
	// y축 회전
	// x = Z * cosf(Rad) - X * sinf(Rad);
	// z = Z * sinf(Rad) + X * cosf(Rad);
	//
	// z축 회전
	// x = X * cosf(Rad) - Y * sinf(Rad);
	// y = X * sinf(Rad) + Y * cosf(Rad);
	//	
	// 다음과 같이 정의 된다
	// 해당 식은 행렬로도 표현이 가능하다
	//
	// x축 회전
	//
	// [x] [y] [z] [w]
	// 
	// *
	// 
	// [1] [0]        [0]         [0]
	// [0] [cos(Rad)] [-sin(Rad)] [0]
	// [0] [sin(Rad)] [cos(Rad)]  [0]
	// [0] [0]        [0]         [1]
	//
	// 위와 같이 x축 회전 행렬을 구할 수 있다
	//
	// y축 회전
	//
	// [x] [y] [z] [w]
	// 
	// *
	// 
	// [cos(rad)]  [0] [sin(rad)] [0]
	// [0]         [1] [0]        [0]
	// [-sin(rad)] [0] [cos(rad)] [0]
	// [0]         [0] [0]        [1]
	//
	// 위와 같이 y축 회전 행렬을 구할 수 있다
	// y축 회전
	//
	// [x] [y] [z] [w]
	// 
	// *
	// 
	// [cos(rad)] [-sin(rad)] [0] [0]
	// [sin(rad)] [cos(rad)]  [0] [0]
	// [0]        [0]         [1] [0]
	// [0]        [0]         [0] [1]
	// 
	// 위와 같이 z축 회전 행렬을 구할 수 있다
	//
	// 위 3가지 행렬을 곱해 하나의 행렬로 합칠 수 있다 
	// 위치 벡터와 합쳐진 행렬을 한번에 곱해 연산을 줄일 수 있다
}

void LogoLevel::Update(float _DeltaTime)
{
	GameEngineLevel::Update(_DeltaTime);
}
