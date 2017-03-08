#include "Grifter.h"

Grifter::Grifter(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	strcpy(m_name, name);
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
			if (m_isMaster)
				printf("Your stat boost have worn off.\n");
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
	SetDamage(healPercent);
	if(m_isMaster)
		printf("You tricked your enemies into healing you. \nYou are healed for %i health.\n", healPercent);
	else
		printf("%s tricked you into healing them. \n%s is healed for %i health.\n", m_name, m_name, healPercent);

}

void Grifter::UseSpecial(std::vector<Character*> players)
{
	if(m_isMaster)
		printf("Your so tricky that your speed was tripled!\n");
	else
		printf("%s is so tricky that their speed was tripled!\n", m_name);

	m_spTurnsRemaining = 7;
	m_speed *= 3;
	// Attack target for massive damage
	for each(Character* p in players)
	{
		if (p->GetNetworkID() == GetTarget())
		{
			p->SetDamage(-5);
		}
	}
}
