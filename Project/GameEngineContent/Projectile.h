#pragma once
#include "BaseContentActor.h"
#include "ColEnterCheck.h"

class ProjectileHitParameter
{
public:
	int Attack = 0;

	float4 ProjectilePos = float4::Zero;
};

class ProjectileParameter
{
public:
	const std::string_view& EffectName = "";
	
	ProjectileHitParameter HitParameter;

	float4 Pos = float4::Zero;
	float4 Dir = float4::Up;
	float4 ColScale = float4::Zero;

	int ColOrder = -1;

	bool IsColDeath = false;

	float Speed = 100.0f;
	float LiveTime = 1.0f;

	std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> EnterEvent = nullptr;
	std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> UpdateEvent = nullptr;
	std::function<void(const float4& _Pos)> DeathEvent = nullptr;
};

class Projectile : public BaseContentActor
{
public:
	Projectile();
	~Projectile();

	Projectile(const Projectile& _Other) = delete;
	Projectile(Projectile&& _Other) noexcept = delete;
	Projectile& operator=(const Projectile& _Other) = delete;
	Projectile& operator=(Projectile&& _Other) noexcept = delete;

	void ShotProjectile(const ProjectileParameter& _Parameter);

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	std::shared_ptr<class GameEngineCollision> ProjectileCol = nullptr;

	ProjectileHitParameter HitParameter;

	float4 Dir = float4::Zero;

	int ColOrder = -1;

	bool IsColDeath = false;

	float LiveTime = 1.0f;
	float Speed = 100.0f;

	std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> EnterEvent = nullptr;
	std::function<void(std::shared_ptr<class BaseContentActor>, ProjectileHitParameter _Parameter)> UpdateEvent = nullptr;
	std::function<void(const float4& _Pos)> DeathEvent = nullptr;

	std::vector<std::shared_ptr<GameEngineCollision>> ColDatas;
	std::map<UINT, std::shared_ptr<BaseContentActor>> ColBuffers;
};