#pragma once
#include "character.h"

class Manager : public Character
{
public:
	Manager(char* name, int healthBoost, int atkBoost, int defBoost, int spdBoost);
	~Manager();

	void Update() override;
	void UseAttack() override;
	void UseHeal() override;
	void UseSpecial(RakNet::NetworkID playerIDs[], int size) override;

private:
	int m_spTurnsRemaining;

};