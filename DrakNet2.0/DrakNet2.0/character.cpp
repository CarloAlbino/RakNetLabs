#include "character.h"
#include <stdio.h>

 void Character::SetTarget(RakNet::NetworkID ID) 
 {
	 m_currentTarget = ID;
 }

 int Character::GetTarget()
 {
	 return m_currentTarget;
 }

 char * Character::GetName()
 {
	 return m_name;
 }

 CharacterClasses Character::GetClass()
 {
	 return m_class;
 }

 bool Character::CanAttack()
 {
	 if (m_health <= 0)
		 return false;
	 else
		 return true;
 }

 int Character::GetHealth()
 {
	 return m_health;
 }

 void Character::SetDamage(int damage) 
 {
	 m_health += damage;
	 if (m_health < 0)
		 m_health = 0;
	 if (m_health > m_maxHealth)
		 m_health = m_maxHealth;
 }

 int Character::GetSpeed() 
 {
	 return m_speed;
 }

 void Character::SetSpeed(int speed)
 {
	 m_speed = speed;
	 if (m_speed < 0)
		 m_speed = 0;
 }

 int Character::GetDefence() 
 {
	 return m_defence; 
 }

 void Character::SetDefence(int defence)
 {
	 m_defence = defence;
	 if (m_defence < 0)
		 m_defence = 0;
 }

 int Character::GetAttack() 
 {
	 return m_attack;
 }

void Character::SetAttack(int attack)
{
	m_attack = attack;
	if (m_attack < 0)
		m_attack = 0;
}

void Character::ResetStats()
{
	m_health = m_maxHealth;
	m_attack = m_defaultAttack;
	m_defence = m_defaultDefence;
	m_speed = m_defaultSpeed;
}

void Character::DisplayStats()
{
	printf("************************************************\n");
	if (m_isMaster)
	{
		printf("******************YOUR STATS*******************\n");
	}
	else
	{
		printf("******************%s'S STATS*******************\n", m_name);
	}
	printf("NETWORKID: %i\n", this->GetNetworkID());
	printf("CLASS: %s\n", GetCharClassName(m_class));
	printf("HP: %i / %i\n", m_health, m_maxHealth);
	printf("ATTACK: %i\n", m_attack);
	printf("DEFENCE: %i\n", m_defence);
	printf("SPEED: %if\n", m_speed);
	printf("************************************************\n");
	printf("************************************************\n");
}

 void Character::SetIsMaster(bool isMaster)
 {
	 m_isMaster = isMaster;
 }

 bool Character::IsMaster() const
 {
	 return m_isMaster; 
 }

 char * Character::GetCharClassName(CharacterClasses c)
 {
	 switch (c)
	 {
	 case E_CCSamurai:
		 return "Samurai";
		 break;
	 case E_CCWrestler:
		 return "Wrestler";
		 break;
	 case E_CCGrifter:
		 return "Grifter";
		 break;
	 case E_CCManager:
		 return "Manager";
		 break;
	 default:
		 return "UNKNOWN CLASS!";
		 break;
	 }
 }
