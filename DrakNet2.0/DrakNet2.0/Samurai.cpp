#include "Samurai.h"

Samurai::Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost)
{
	m_isMaster = false;
	strcpy(m_name, name);
	m_class = E_CCSamurai;
	m_currentTarget = 0;

	m_maxHealth = 30 + healthBoost;
	m_defaultAttack = 20 + atkBoost;
	m_defaultDefence = 15 + defBoost;
	m_defaultSpeed = 3 + spdBoost;

	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;

	m_healthBoost = healthBoost;
	m_atkBoost = atkBoost;
	m_defBoost = defBoost;
	m_spdBoost = spdBoost;
}

Samurai::~Samurai()
{
}

void Samurai::Update()
{
}

void Samurai::UseAttack(char* enemy, int damage)
{
	if (m_isMaster)
		printf("You slashed at the enemy with your katana.\n");
	else
		printf("%s slashed at %s with their katana for %i damage.\n", m_name, enemy, damage);
	// find class of targeted player with targetedID and attack
	// compare with target's defence and speed.
}

void Samurai::UseHeal()
{
	int healPercent = (float)m_maxHealth * 0.17f;
	SetDamage(healPercent);
	if(m_isMaster)
		printf("You sat and rested.  You healed yourself for %i health.\n", healPercent);
	else
		printf("%s sat and rested.  They healed for %i health.\n", m_name, healPercent);

}

void Samurai::UseSpecial(std::vector<Character*> players)
{
	if (m_isMaster)
		printf("You unleash a flurry of attacks. \nYou hit all other fighters, doing 3 damage to all.\n");
	else
		printf("%s unleashes a flurry of attacks. \nThey hit all other fighters, doing 3 damage to all.\n", m_name);
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i]->GetNetworkID() != this->GetNetworkID())
		{
			players[i]->SetDamage(-3);
		}
	}
}
