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

unsigned char GetPacketIdentifier(RakNet::Packet *p);
int CheckForCommands(char* message);
//bool IsNameTaken(char* name);

RakNet::RakPeerInterface *g_rakPeerInterface;	// Used to connect users together
bool g_isServer = false;
RakNet::SystemAddress g_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;	// Default server address

enum UserINputResult
{
	UIR_BREAK,
	UIR_CONTINUE, 
	UIR_POS, 
	UIR_COUNT,
};

enum {
   ID_GB3_CHAT = ID_USER_PACKET_ENUM,
   ID_GB3_POS,
};

struct SPos
{
	float x, y, z;
};

// Holds all the users' name
//DataStructures::List<char[]> userList;

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
	puts("\n\n[ISTRUCTIONS]\n");
	if (g_isServer)
	{
		puts("'quit' to quit. 'stat' to show stats. 'ping' to ping.\n'pingip' to ping an ip address\n'ban' to ban an IP from connecting.\n'kick to kick the first connected player.\nType to talk.\n\n");
	}
	else
	{
		puts("'quit' to quit. 'stat' to show stats. \nType to talk.\n\n");
	}
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
				RakNet::BitStream bs;
				bs.Write((unsigned char)ID_GB3_POS);
				bs.Write(7.0f);

				if (g_isServer)
				{
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else
				{
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
				continue;
			}
			else
			{
				// Notice what is not here: something to keep our network running.  It's
				// fine to block on gets or anything we want
				// Because the network engine was painstakingly written using threads.
				msgWithIdentifier[0] = ID_GB3_CHAT;
				msgWithIdentifier[1] = '\0';
				strncat(msgWithIdentifier, userName, sizeof(msgWithIdentifier));
				strncat(msgWithIdentifier, message, sizeof(message) - strlen(userName) - 1);
				// Append Server: to the message so clients know that it ORIGINATED from the server
				// All messages to all clients come from the server either directly or by being
				// relayed from other clients
				//msgWithIdentifier[0] = 0;
				//const static char prefix[] = ;
				//strncat(msgWithIdentifier, userName, sizeof(msgWithIdentifier));
				//strncat(msgWithIdentifier, message, sizeof(msgWithIdentifier) - strlen(userName) - 1);

				// message2 is the data to send
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
				printf("ID_GB3_POS\n");
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				float xPos;
				bs.Read(xPos);

				printf("pos x: %f \n", xPos);
			}
				
				break;
			default:
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				printf("%i\n", packet->data[0]);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				/*sprintf(message, "%s", packet->data);
				if (g_isServer)
				{
					g_rakPeerInterface->Send(message, (const int)strlen(message) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
				}*/

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

	return UIR_COUNT;
}