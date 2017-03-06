#pragma once
#include "NetworkIDManager.h"
#include <stdio.h>
#include <vector>

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
	void SetPlayerAtkOrder(RakNet::NetworkID id, bool isSelf, int num);
	void PrintAtkOrder(std::vector<Character*> characters);
	RakNet::NetworkID IsTurn(int attackTurn);// , RakNet::NetworkID id);
	RakNet::NetworkID GetTarget();
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

	int GetHPBoost();
	int GetAtkBoost();
	int GetDefBoost();
	int GetSpdBoost();

	void ResetStats();
	void DisplayStats();

	void SetIsMaster(bool isMaster);
	bool IsMaster() const;

protected:
	char* GetCharClassName(CharacterClasses c);

protected:
	char* m_name;
	CharacterClasses m_class;
	std::vector<RakNet::NetworkID> m_playerAtkOrder;
	int m_turnNum;
	RakNet::NetworkID m_currentTarget;

	int m_maxHealth;
	int m_health;
	int m_defaultDefence;
	int m_defence;
	int m_defaultSpeed;
	int m_speed;
	int m_defaultAttack;
	int m_attack;

	// For making raplicas
	int m_healthBoost;
	int m_atkBoost;
	int m_defBoost;
	int m_spdBoost;

	bool m_isMaster;
};