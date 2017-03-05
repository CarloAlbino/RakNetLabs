#pragma once
#include "character.h"

class Samurai : public Character
{
public:
	Samurai(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Samurai();

	void Update() override;
	void UseAttack() override;
	void UseHeal() override;
	void UseSpecial(RakNet::NetworkID playerIDs[], int size) override;

};