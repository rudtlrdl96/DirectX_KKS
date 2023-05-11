#pragma once
#include <GameEngineCore/GameEngineComponent.h>

class Rigidbody2D : public GameEngineComponent
{
public:
	Rigidbody2D();
	~Rigidbody2D();

	Rigidbody2D(const Rigidbody2D& _Other) = delete;
	Rigidbody2D(Rigidbody2D&& _Other) noexcept = delete;
	Rigidbody2D& operator=(const Rigidbody2D& _Other) = delete;
	Rigidbody2D& operator=(Rigidbody2D&& _Other) noexcept = delete;
		
	inline void SetMass(float _Mass)
	{
		Mass = _Mass;
	}

	inline float GetMass() const
	{
		return Mass;
	}

	inline void AddVelocity(const float4& _Velocity)
	{
		Velocity += _Velocity;
	}

	inline void SetVelocity(const float4& _Velocity)
	{
		Velocity = _Velocity;
	}

	inline float4 GetVelocity()
	{
		return Velocity;
	}

	inline void AddForce(const float4& _Force)
	{
		ForceVector += _Force;
	}

	inline void SetMaxSpeed(float _MaxSpeed)
	{
		MaxSpeed = _MaxSpeed;
	}

	inline void SetActiveGravity(bool _IsGravity)
	{
		IsGravity = _IsGravity;
	}

	inline void SetFricCoeff(float _FricCoeff)
	{
		FricCoeff = _FricCoeff;
	}

	inline void SetGravity(float _Gravity)
	{
		Gravity = _Gravity;
	}

	void UpdateForce(float _DeltaTime);
protected:

private:
	float4 ForceVector = float4::Zero;
	float4 Accel = float4::Zero;
	float4 Velocity = float4::Zero;

	bool IsGravity = false;

	float MaxSpeed = 300.0f;
	float Mass = 1.0f;
	float FricCoeff = 1.0f;
	float Gravity = ContentConst::Gravity_f;
};

