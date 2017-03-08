#include "Manager.h"

Manager::Manager(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	strcpy(m_name, name);
	m_class = E_CCManager;
	m_currentTarget = 0;

	m_maxHealth = 10 + healthBoost;
	m_defaultAttack = 20 + atkBoost;
	m_defaultDefence = 35 + defBoost;
	m_defaultSpeed = 1 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;

	m_healthBoost = healthBoost;
	m_atkBoost = atkBoost;
	m_defBoost = defBoost;
	m_spdBoost = spdBoost;
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
			if (m_isMaster)
				printf("Your stat boost have worn off.\n");
		}
	}
}

void Manager::UseAttack(char* enemy, int damage)
{
	if (m_isMaster)
		printf("You made your employees attack the customer.\n");
	else
		printf("%s made their employees attack %s for %i damage.\n", m_name, enemy, damage);
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Manager::UseHeal()
{
	int healPercent = (float)m_maxHealth * 0.1f;
	SetDamage(healPercent);
	if (m_isMaster)
	{
		printf("You took a 2 hour lunch break, yet you're still complaining.  You healed yourself for %i health.\n", m_name, healPercent);
	}
	else
	{
		printf("%s took a 2 hour lunch break, yet is still complaining.  They healed for %i health.\n", m_name, healPercent);
	}
}

void Manager::UseSpecial(std::vector<Character*> players)
{
	if (m_isMaster)
		printf("You throw your employees at the customers.  You triple your defences!\n");
	else
		printf("%s throws their employees at the customers.  %s triple their defences!\n", m_name, m_name);

	m_spTurnsRemaining = 7;
	m_defence *= 3;
	// Attack target for a little damage
	for each(Character* p in players)
	{
		if (p->GetNetworkID() == GetTarget())
		{
			p->SetDamage(-2);
		}
	}
}
