#include "Manager.h"

Manager::Manager(char * name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCManager;
	m_currentTarget = -1;
	m_previousTarget = -1;

	m_maxHealth = 10 + healthBoost;
	m_defaultAttack = 20 + atkBoost;
	m_defaultDefence = 35 + defBoost;
	m_defaultSpeed = 1 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;
}

Manager::~Manager()
{
}

void Manager::Update()
{
	if (m_spTurnsRemaining > 0)
	{
		m_spTurnsRemaining--;
		if (m_spTurnsRemaining == 0)
		{
			m_defence = m_defaultDefence;
		}
	}
}

void Manager::UseAttack()
{
	printf("You made your employees attack the customer.\n");
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Manager::UseHeal()
{
	int healPercent = (float)m_maxHealth * 0.1f;
	printf("You took a 2 hour lunch break, yet you're still complaining.  You healed yourself for %i health.\n", healPercent);
	SetDamage(healPercent);
}

void Manager::UseSpecial(RakNet::NetworkID playerIDs[], int size)
{
	printf("You throw your employees at the customers.  You triple your defences!\n");
	m_spTurnsRemaining = 3;
	m_defence *= 3;
	// Attack target for a little damage
}
