// Includes
#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "Kbhit.h"
#include <stdio.h>
#include <string.h>
#include "Gets.h"
#include <algorithm>

#include <time.h>

enum EDirection
{
	ED_North, 
	ED_East,
	ED_South, 
	ED_West,
	ED_Weast,
	ED_DirectionLength
};

// Stores user position to be passed into the bitstream
struct SPos
{
	float x, y, z;
	EDirection dir;
};

// States a user can be in
enum EState
{
	ES_Idle,
	ES_Walking, 
	ES_Lost,
	ES_StateLength
};

unsigned char GetPacketIdentifier(RakNet::Packet *p);
int CheckForCommands(char* message);
SPos GetRandomPos();
EState GetRandomState();

RakNet::RakPeerInterface *g_rakPeerInterface;	// Used to connect users together
bool g_isServer = false;
RakNet::SystemAddress g_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;	// Default server address

enum UserInputResult
{
	UIR_BREAK,
	UIR_CONTINUE, 
	UIR_POS, 
	UIR_STATE,
	UIR_COUNT,
};

enum {
   ID_GB3_CHAT = ID_USER_PACKET_ENUM,
   ID_GB3_POS,
   ID_GB3_STATE,
};

int main(void)
{
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	// Key Line -> Starts up the server
	g_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
	g_rakPeerInterface->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	// Holds user data
	char userInput[30];
	// Holds user name
	char userName[30] = "[Server] : ";

	puts("===============================================================\n");
	puts("This is an implementation of a text based chat server/client.\n");
	puts("What role would you like? Client[c] or Server[s]\n\n");

	Gets(userInput, sizeof(userInput));
	g_isServer = userInput[0] == 's' || userInput[0] == 'S';

	if (g_isServer)
	{
		puts("\nYou are the server.\n");
	}
	else
	{
		puts("\nYou are a client.\n");
	}

	// A server
	puts("\n===============================================================\n");
	puts("Enter the port to listen on:");
	Gets(userInput, sizeof(userInput));
	if (userInput[0] == 0)
		strcpy(userInput, "1234");

	puts("\nSetting up socket.\n");
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	// I am creating two socketDesciptors, to create two sockets. One using IPV6 and the other IPV4
	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port = atoi(userInput);
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
	socketDescriptors[1].port = atoi(userInput);
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6
	const unsigned int maxConnections = g_isServer ? 100 : 1;
	bool isSuccess = g_rakPeerInterface->Startup(maxConnections, socketDescriptors, 2) == RakNet::RAKNET_STARTED;

	if (g_isServer)
	{
		g_rakPeerInterface->SetMaximumIncomingConnections(maxConnections);
	}

	if (!isSuccess)
	{
		puts("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.\n");

		// Try again, but leave out IPV6
		isSuccess = g_rakPeerInterface->Startup(maxConnections, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
		if (!isSuccess)
		{
			puts("Server failed to start.  Terminating.\n");
			exit(1);
		}
	}
	g_rakPeerInterface->SetOccasionalPing(true);
	g_rakPeerInterface->SetUnreliableTimeout(1000);

	DataStructures::List< RakNet::RakNetSocket2* > sockets;
	g_rakPeerInterface->GetSockets(sockets);
	puts("Socket addresses used by RakNet:");
	for (unsigned int i = 0; i < sockets.Size(); i++)
	{
		printf("%i. %s\n", i + 1, sockets[i]->GetBoundAddress().ToString(true));
	}

	puts("\nMy IP addresses:");
	for (unsigned int i = 0; i < g_rakPeerInterface->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = g_rakPeerInterface->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("%i. %s (LAN=%i)\n", i + 1, sa.ToString(false), sa.IsLANAddress());
	}

	// If a client
	if (!g_isServer)
	{
		puts("\nEnter IP to connect to: ");
		Gets(userInput, sizeof(userInput));
		g_rakPeerInterface->AllowConnectionResponseIPMigration(false);
		if (userInput[0] == 0)
			strcpy(userInput, "127.0.0.1");

		char port[10];
		puts("\nEnter the port to connect to:");
		Gets(port, sizeof(port));
		if (port[0] == 0)
			strcpy(port, "1234");

		// Important
		RakNet::ConnectionAttemptResult car = g_rakPeerInterface->Connect(userInput, atoi(port), nullptr, 0);
		RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);
	}

	if (!g_isServer)
	{
		// Ask for user name
		puts("\nEnter a user name:");
		Gets(userName, sizeof(userName));
		char temp[30] = { '[' };
		strcat(temp, userName);
		strcat(temp, "] : ");
		strcpy(userName, temp);
	}

	printf("\nMy GUID is %s\n", g_rakPeerInterface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	puts("\n===============================================================\n");
	puts("\n\n[INSTRUCTIONS]\n");
	puts("'quit' to quit. \n'pos' to show your position. \n'state' to show your current state. \nType to talk.\n\n");
	puts("\n===============================================================\n");

	char message[2046];
	// Message now holds what we want to broadcast
	char msgWithIdentifier[2048];
	// Holds packets
	RakNet::Packet* packet;
	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Loop for input
	while (1)
	{
		// This sleep keeps RakNet responsive
		RakSleep(30);

		if (_kbhit())
		{
			Gets(message, sizeof(message));
			unsigned int result = CheckForCommands(message);
			if (result == UIR_BREAK)
			{
				// Quit program
				break;
			}
			else if (result == UIR_CONTINUE)
			{
				// Goes back to the top of the while loop
				continue;
			}
			else if (result == UIR_POS)
			{
				// Send your position to the server, also prints your position for you to see
				RakNet::BitStream bs;
				bs.Write((unsigned char)ID_GB3_POS);
				SPos randPos = GetRandomPos();
				bs.Write(randPos);
				bs.Write(userName);

				char direction[30] = " facing -> ";

				switch (randPos.dir)
				{
				case ED_North:
					strcat(direction, "North");
					break;
				case ED_East:
					strcat(direction, "East");
					break;
				case ED_South:
					strcat(direction, "South");
					break;
				case ED_West:
					strcat(direction, "West");
					break;
				case ED_Weast:
					strcat(direction, "Weast");
					break;
				default:
					strcat(direction, "Unknown");
					break;
				}

				printf("%s pos x -> %f - pos y -> %f - pos z -> %f - %s\n", userName, randPos.x, randPos.y, randPos.z, direction);
				g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

				continue;
			}
			else if (result == UIR_STATE)
			{
				// Send your state to the server, also print your state for you to see
				RakNet::BitStream bs;
				bs.Write((unsigned char)ID_GB3_STATE);
				EState randState = GetRandomState();
				bs.Write(randState);
				bs.Write(userName);

				char state[30] = " state -> ";

				switch (randState)
				{
				case ES_Idle:
					strcat(state, "Idle");
					break;
				case ES_Walking:
					strcat(state, "Walking");
					break;
				case ES_Lost:
					strcat(state, "Lost");
					break;
				default:
					strcat(state, "Unknown");
					break;
				}

				printf("%s %s\n", userName, state);

				g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

				continue;
			}
			else
			{
				// Send a normal message
				msgWithIdentifier[0] = ID_GB3_CHAT;
				msgWithIdentifier[1] = '\0';
				strncat(msgWithIdentifier, userName, sizeof(msgWithIdentifier));
				strncat(msgWithIdentifier, message, sizeof(message) - strlen(userName) - 1);

				// strlen(message2)+1 is to send the null terminator
				// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
				// RELIABLE_ORDERED means make sure the message arrives in the right order
				// We arbitrarily pick 0 for the ordering stream
				// RakNet::UNASSIGNED_SYSTEM_ADDRESS means don't exclude anyone from the broadcast
				// true means broadcast the message to everyone connected
				if (g_isServer)
				{
					g_rakPeerInterface->Send(msgWithIdentifier, (const int)strlen(msgWithIdentifier) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else
				{
					g_rakPeerInterface->Send(msgWithIdentifier, (const int)strlen(msgWithIdentifier) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
			}

		}// End of KBHIT

		// Get a packet from either the server or the client
		// Important
		for (packet = g_rakPeerInterface->Receive(); packet/*!=nullptr*/; g_rakPeerInterface->DeallocatePacket(packet), packet = g_rakPeerInterface->Receive())
		{
			// We got a packet, get the identifier with our handy function
			packetIdentifier = GetPacketIdentifier(packet);	// Packet identifier, lets you know what type of communication is being recieved

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n\n");
				if (!g_isServer)
				{
					g_serverAddress = packet->systemAddress;
				}
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION from %s\n\n", packet->systemAddress.ToString(true));;
				break;

			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				printf("Remote internal IDs:\n");
				for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
				{
					RakNet::SystemAddress internalId = g_rakPeerInterface->GetInternalID(packet->systemAddress, index);
					if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
					{
						printf("%i. %s\n", index + 1, internalId.ToString(true));
					}
				}
				break;

			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;

			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", packet->systemAddress.ToString(true));
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST from %s\n", packet->systemAddress.ToString(true));;
				break;

			case ID_GB3_CHAT:
			{
				unsigned char *pp = packet->data;
				++pp;
				printf("%s\n", pp);
			}
				break;
			case ID_GB3_POS:
			{
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				SPos newPos;
				bs.Read(newPos);
				char name[30];
				bs.Read(name);

				char direction[30] = " facing -> ";

				switch (newPos.dir)
				{
				case ED_North:
					strcat(direction, "North");
					break;
				case ED_East:
					strcat(direction, "East");
					break;
				case ED_South:
					strcat(direction, "South");
					break;
				case ED_West:
					strcat(direction, "West");
					break;
				case ED_Weast:
					strcat(direction, "Weast");
					break;
				default:
					strcat(direction, "Unknown");
					break;
				}

				printf("%s pos x -> %f - pos y -> %f - pos z -> %f - %s\n", name, newPos.x, newPos.y, newPos.z, direction);
			}
				
				break;
			case ID_GB3_STATE:
			{
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				EState newState;
				bs.Read(newState);
				char name[30];
				bs.Read(name);
				char state[30] = " state -> ";

				switch (newState)
				{
				case ES_Idle:
					strcat(state, "Idle");
					break;
				case ES_Walking:
					strcat(state, "Walking");
					break;
				case ES_Lost:
					strcat(state, "Lost");
					break;
				default:
					strcat(state, "Unknown");
					break;
				}

				printf("%s %s\n", name, state);
			}
				break;
			default:
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				printf("%i\n", packet->data[0]);

				break;
			}

		}
	}

	g_rakPeerInterface->Shutdown(300);
	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(g_rakPeerInterface);

	return 0;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}

// Add any commands needed in here
int CheckForCommands(char* message)
{
	if (strcmp(message, "quit") == 0)
	{
		puts("Quitting.");
		return UIR_BREAK;
	}

	if (strcmp(message, "pos") == 0)
	{
		return UIR_POS;
	}

	if (strcmp(message, "state") == 0)
	{
		return UIR_STATE;
	}

	return UIR_COUNT;
}

// Creates a random position for a user to be at
SPos GetRandomPos()
{
	SPos position;
	int randomNum;
	srand(time(NULL));

	randomNum = rand() % 100 + 1;
	position.x = randomNum;

	randomNum = rand() % 100 + 1;
	position.y = randomNum;

	randomNum = rand() % 100 + 1;
	position.z = randomNum;

	randomNum = rand() % ED_DirectionLength;
	position.dir = EDirection(randomNum);

	return position;
}

// Return a random state
EState GetRandomState()
{
	int randomNum;
	srand(time(NULL));

	randomNum = rand() % ES_StateLength;

	return EState(randomNum);
}