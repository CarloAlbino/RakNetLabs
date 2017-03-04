#pragma once
#include "NetworkIDManager.h"

enum CharacterClasses
{
	E_CCSamurai,
	E_CCWrestler,
	E_CCGrifter,
	E_CCManager
};

class Character : public RakNet::NetworkIDObject
{
public:
	void SetTarget(RakNet::NetworkID ID);
	int GetTarget();
	char* GetName();
	CharacterClasses GetClass();

	virtual void UseAttack() = 0;
	virtual void UseHeal() = 0;
	virtual void UseDodge() = 0;
	virtual void UseSpecial() = 0;

	bool CanAttack();

	int GetHealth();
	void SetDamage(int damage);
	int GetSpeed();
	void SetSpeed(int speed);
	int GetDefence();
	void SetDefence(int defence);
	int GetAttack();
	void SetAttack(int attack);

	void ResetStats();
	void DisplayStats();

	void SetIsMaster(bool isMaster);
	bool IsMaster() const;

protected:
	char* GetCharClassName(CharacterClasses c);

protected:
	char* m_name;
	CharacterClasses m_class;
	RakNet::NetworkID m_currentTarget;

	int m_maxHealth;
	int m_health;
	int m_defaultDefence;
	int m_defence;
	int m_defaultSpeed;
	int m_speed;
	int m_defaultAttack;
	int m_attack;

	bool m_isMaster;
};