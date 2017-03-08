#pragma once
#include "character.h"

class Samurai : public Character
{
public:
	Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Samurai();

	void Update() override;
	void UseAttack(char* enemy = "", int damage = 0) override;
	void UseHeal() override;
	void UseSpecial(std::vector<Character*> players) override;

};