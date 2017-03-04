#include "Samurai.h"

Samurai::Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCSamurai;
	m_currentTarget = -1;

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
}

void Samurai::UseHeal()
{
}

void Samurai::UseDodge()
{
}

void Samurai::UseSpecial()
{
}
