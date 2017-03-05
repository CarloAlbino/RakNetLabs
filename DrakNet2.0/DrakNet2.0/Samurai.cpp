#include "Samurai.h"
#include <stdio.h>

Samurai::Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCSamurai;
	m_currentTarget = -1;
	m_previousTarget = -1;

	m_maxHealth = 30 + healthBoost;
	m_defaultAttack = 15 + atkBoost;
	m_defaultDefence = 15 + defBoost;
	m_defaultSpeed = 3 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;
}

Samurai::~Samurai()
{
}

void Samurai::UseAttack()
{
	printf("You use your slashed at the enemy.\n");
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Samurai::UseHeal()
{
	int healPercent = (float)m_maxHealth * 1.7f;
	printf("You sat and rested.  You healed yourself for %i health.\n", healPercent);
	SetDamage(healPercent);
}

void Samurai::UseSpecial(RakNet::NetworkID playerIDs[], int size)
{
	printf("You unleash a flurry of attacks. \nYou hit all other fighters, doing 6 damage to all.\n");
	for (int i = 0; i < size; i++)
	{
		// Get class from ID and damage
		// compare with target's defence and speed.
	}
}
