#pragma once
#include "NetworkIDManager.h"

class Character : public RakNet::NetworkIDObject
{
public:
	void SetTarget(int ID);

	virtual void UseAttack() = 0;
	virtual void UseHeal() = 0;
	virtual void UseDodge() = 0;
	virtual void UseSpecial() = 0;

	int GetHealth();
	void SetDamage(int damage);
	int GetSpeed();
	void SetSpeed(int speed);
	int GetDefence();
	void SetDefence(int defence);
	int GetAttack();
	void SetAttack();

	void ResetStats();
	void DisplayStats();

	void SetIsMaster(bool isMaster);
	bool IsMaster() const;

protected:
	int m_maxHealth;
	int m_health;

	int m_maxDefence;
	int m_defence;

	int m_maxSpeed;
	int m_speed;

	int m_maxAttack;
	int m_attack;
	
	int m_currentTarget;

	bool m_isMaster;
};