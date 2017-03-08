#include "Grifter.h"

Grifter::Grifter(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCGrifter;
	m_currentTarget = 0;

	m_maxHealth = 20 + healthBoost;
	m_defaultAttack = 10 + atkBoost;
	m_defaultDefence = 5 + defBoost;
	m_defaultSpeed = 12 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;

	m_healthBoost = healthBoost;
	m_atkBoost = atkBoost;
	m_defBoost = defBoost;
	m_spdBoost = spdBoost;
}

Grifter::~Grifter()
{
}

void Grifter::Update()
{
	if (m_spTurnsRemaining > 0)
	{
		m_spTurnsRemaining--;
		if (m_spTurnsRemaining == 0)
		{
			m_speed = m_defaultSpeed;
		}
	}
}

void Grifter::UseAttack(char* enemy, int damage)
{
	if (m_isMaster)
		printf("You conned the enemy to punch themselves in the face.\n");
	else
		printf("%s conned %s to punch themselves in the face for %i damage.\n", m_name, enemy, damage);
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Grifter::UseHeal()
{
	int healPercent = (float)m_maxHealth * 0.5f;
	printf("You tricked your enemies into healing you. \nYou healed yourself for %i health.\n", healPercent);
	SetDamage(healPercent);
}

void Grifter::UseSpecial(RakNet::NetworkID playerIDs[], int size)
{
	printf("Your so tricky that your speed was tripled!\n");
	m_spTurnsRemaining = 3;
	m_speed *= 3;
	// Attack target for massive damage
}
