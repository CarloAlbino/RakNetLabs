#include "Wrestler.h"

Wrestler::Wrestler(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCWrestler;
	m_currentTarget = 0;

	m_maxHealth = 25 + healthBoost;
	m_defaultAttack = 15 + atkBoost;
	m_defaultDefence = 15 + defBoost;
	m_defaultSpeed = 8 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;

	m_healthBoost = healthBoost;
	m_atkBoost = atkBoost;
	m_defBoost = defBoost;
	m_spdBoost = spdBoost;
}

Wrestler::~Wrestler()
{
}

void Wrestler::Update()
{
	if (m_spTurnsRemaining > 0)
	{
		m_spTurnsRemaining--;
		if (m_spTurnsRemaining == 0)
		{
			m_attack = m_defaultAttack;
			m_speed = m_defaultSpeed;
		}
	}
}

void Wrestler::UseAttack(char* enemy, int damage)
{
	if (m_isMaster)
		printf("You piledrived the enemy.\n");
	else
		printf("%s piledrived %s for %i damage.\n", m_name, enemy, damage);
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Wrestler::UseHeal()
{
	int healPercent = (float)m_maxHealth * 0.22f;
	printf("You pretended to be hurt while your enemies thought they were winning. \nYou healed yourself for %i health.\n", healPercent);
	SetDamage(healPercent);
}

void Wrestler::UseSpecial(RakNet::NetworkID playerIDs[], int size)
{
	printf("The crowd is cheering you on, your speed and attack doubled!\n");
	m_spTurnsRemaining = 3;
	m_attack *= 2;
	m_speed *= 1.5f;

	int healPercent = (float)m_maxHealth * 0.25f;
	printf("The crowd's cheers encouraged you. \n you heal for %i!\n", healPercent);
	SetDamage(healPercent);
}
