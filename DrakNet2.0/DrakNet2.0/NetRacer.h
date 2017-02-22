#pragma once
#include "NetworkIDManager.h"

class CNetRacer : public RakNet::NetworkIDObject
{
public:
	CNetRacer();
	~CNetRacer() {}

	void Accelerate();
	void Brake();

	void Update();

	void DisplayStats();

	void SetIsMaster(bool isMaster) { m_isMaster = isMaster; }
	bool IsMaster() const { return m_isMaster; }

private:
	float m_pos;
	float m_currentSpeed;
	float m_topSpeed;
	float m_accelerationRate;

	bool m_isMaster;
};