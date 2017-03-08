#pragma once
#include "character.h"

class Grifter : public Character
{
public:
	Grifter(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Grifter();

	void Update() override;
	void UseAttack(char* enemy = "", int damage = 0) override;
	void UseHeal() override;
	void UseSpecial(std::vector<Character*> players) override;

private:
	int m_spTurnsRemaining;

};