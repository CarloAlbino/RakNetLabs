#include "Wrestler.h"

Wrestler::Wrestler(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	strcpy(m_name, name);
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
			if (m_isMaster)
				printf("Your stat boost have worn off.\n");
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
	SetDamage(healPercent);

	if(m_isMaster)
		printf("You pretended to be hurt while your enemies thought they were winning. \nYou healed yourself for %i health.\n", healPercent);
	else
		printf("%s pretended to be hurt while you thought they were winning. \n%s healed themselves for %i health.\n", m_name, m_name, healPercent);

}

void Wrestler::UseSpecial(std::vector<Character*> players)
{
	if(m_isMaster)
		printf("The crowd is cheering you on, your speed and attack doubled!\n");
	else
		printf("The crowd is cheering %s on, their speed and attack is doubled!\n", m_name);
	m_spTurnsRemaining = 7;
	m_attack *= 2;
	m_speed *= 1.5f;

	int healPercent = (float)m_maxHealth * 0.25f;
	SetDamage(healPercent);
	if (m_isMaster)
		printf("The crowd's cheers encouraged you. \nYou heal for %i!\n", healPercent);
	else
		printf("The crowd's cheers encouraged %s. \nThey heal for %i!\n", m_name, healPercent);

}
