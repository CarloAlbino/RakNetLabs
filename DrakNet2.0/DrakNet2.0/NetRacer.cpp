#include "NetRacer.h"
#include <stdio.h>

CNetRacer::CNetRacer() : m_pos(0.0f), m_currentSpeed(0.0f), m_topSpeed(10.0f), m_accelerationRate(1.0f), m_isMaster(false)
{
}

void CNetRacer::Accelerate()
{
	m_currentSpeed += m_accelerationRate;
	if (m_currentSpeed > m_topSpeed)
	{
		m_currentSpeed = m_topSpeed;
	}
}

void CNetRacer::Brake()
{
	m_currentSpeed = 0;
}

void CNetRacer::Update()
{
	static float fixedTime = 0.0000016f;
	if (m_currentSpeed > 0.000001f)
	{
		m_pos += m_currentSpeed * fixedTime;
	}
}

void CNetRacer::DisplayStats()
{
	printf("************************************************\n");
	if (m_isMaster)
	{
		printf("******************RACER STATS*******************\n");
	}
	else
	{
		printf("******************REPLICA STATS*******************\n");
	}
	printf("NETWORKID: %i\n", this->GetNetworkID());
	printf("POSITION: %.3f\n", m_pos);
	printf("SPEED: %.3f\n", m_currentSpeed);
	printf("************************************************\n");
	printf("************************************************\n");
}
