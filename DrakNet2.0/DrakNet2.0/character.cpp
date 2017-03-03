#include "character.h"

 void Character::SetTarget(int ID) 
 {
	 m_currentTarget = ID;
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
		 m_health = m_maxAttack;
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
	 if (m_speed > m_maxSpeed)
		 m_speed = m_maxSpeed;
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
	 if (m_defence > m_maxDefence)
		 m_defence = m_maxDefence;
 }

 int Character::GetAttack() 
 {
	 return m_attack;
 }

void Character::SetAttack()
{
}

void Character::ResetStats()
{
}

void Character::DisplayStats()
{
}

 void Character::SetIsMaster(bool isMaster)
 {
	 m_isMaster = isMaster;
 }

 bool Character::IsMaster() const
 {
	 return m_isMaster; 
 }
