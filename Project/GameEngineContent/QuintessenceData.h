#pragma once

enum class QuintessenceGrade
{
	Normal,
	Rare,
	Unique,
	Legendary
};

class QuintessenceData
{
public:
	std::string Name = "";
	std::string Note = "";
	size_t Index = 0;

	QuintessenceGrade Grade = QuintessenceGrade::Normal;

	float HP = 0;
	float DamageReduction = 0.0f;

	float MeleeAttack = 0.0f;
	float MagicAttack = 0.0f;

	float AttackSpeed = 0.0f;
	float MoveSpeed = 0.0f;
	float CastingSpeed = 0.0f;

	float SkillCoolDown = 0.0f;
	float SwitchCoolDown = 0.0f;
	float QuintessenceCoolDown = 0.0f;

	float CriticalPercent = 0.0f;
	float CriticalDamage = 0.0f;

	float GoldPercent = 0.0f;
};