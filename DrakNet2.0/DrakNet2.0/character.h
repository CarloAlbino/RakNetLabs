#pragma once
#include "NetworkIDManager.h"
#include <stdio.h>

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
	RakNet::NetworkID GetTarget();
	RakNet::NetworkID GetPreviousTarget();
	char* GetName();
	CharacterClasses GetClass();

	virtual void Update() = 0;	// Update updates the character at the begining of their turn
	virtual void UseAttack() = 0;
	virtual void UseHeal() = 0;
	virtual void UseSpecial(RakNet::NetworkID playerIDs[], int size) = 0;

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
	RakNet::NetworkID m_previousTarget;

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