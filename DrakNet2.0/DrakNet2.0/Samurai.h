#pragma once
#include "character.h"

class Samurai : public Character
{
public:
	Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Samurai();

	void UseAttack() override;
	void UseHeal() override;
	void UseDodge() override;
	void UseSpecial() override;

private:


};