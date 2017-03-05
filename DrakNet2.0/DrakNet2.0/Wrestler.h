#pragma once
#include "character.h"

class Wrestler : public Character
{
public:
	Wrestler(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Wrestler();

	void Update() override;
	void UseAttack() override;
	void UseHeal() override;
	void UseSpecial(RakNet::NetworkID playerIDs[], int size) override;

private:
	int m_spTurnsRemaining;

};