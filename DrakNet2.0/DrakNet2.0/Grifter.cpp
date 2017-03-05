#include "Grifter.h"

Grifter::Grifter(char * name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	m_name = name;
	m_class = E_CCGrifter;
	m_currentTarget = -1;
	m_previousTarget = -1;

	m_maxHealth = 25 + healthBoost;
	m_defaultAttack = 15 + atkBoost;
	m_defaultDefence = 15 + defBoost;
	m_defaultSpeed = 8 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;
}

Grifter::~Grifter()
{
}

void Grifter::Update()
{
}

void Grifter::UseAttack()
{
}

void Grifter::UseHeal()
{
}

void Grifter::UseSpecial(RakNet::NetworkID playerIDs[], int size)
{
}
