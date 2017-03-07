#include "MessageIdentifiers.h"
#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "ReadyEvent.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"
#include "Gets.h"
#include <thread>
#include <vector>
#include "Wrestler.h"
#include "Samurai.h"
#include "Grifter.h"
#include "Manager.h"
#include <mutex>

using namespace RakNet;

enum {
	ID_CHATFIGHT_CREATE_CHARACTER = ID_USER_PACKET_ENUM,
	ID_CHATFIGHT_ATTACK,
	ID_CHATFIGHT_HEAL,
	ID_CHATFIGHT_SPECIAL,
	ID_CHATFIGHT_TURN_OVER,
};

enum ReadyEventIDs
{
	ID_RE_PLAYER_JOIN = 0,
	ID_RE_CHARACTERS_SELECTED,
	//ID_RE_TURN_OVER,
	ID_RE_GAME_OVER,
};

void PacketListener();
void InputListener();
void DisplayHelp();
void DisplayClassInformation();
void SetPlayerOrder();
RakNet::NetworkID GetFastestPlayer();

RakPeerInterface* g_rakPeerInterface = nullptr;
int g_startingPort = 6500;
bool g_isRunning = true;
bool g_isGameRunning = false;

std::mutex g_playerMutex;

std::vector<Character*> g_characters;
std::vector<RakNet::NetworkID> g_playerIDs;

int g_maxPlayers = 4;

// Allows for ready events
ReadyEvent g_readyEventPlugin;
// These two plugins are just to automatically create a fully connected mesh so I don't have to call connect more than once
FullyConnectedMesh2 g_fcm2;
ConnectionGraph2 g_cg2;
RakNet::NetworkIDManager g_networkIDManager;

int main()
{
	// Set up rak peer interface
	g_rakPeerInterface = RakPeerInterface::GetInstance();

	// Set up ready event and fully connected mesh
	g_rakPeerInterface->AttachPlugin(&g_readyEventPlugin);
	g_rakPeerInterface->AttachPlugin(&g_fcm2);
	g_rakPeerInterface->AttachPlugin(&g_cg2);

	g_fcm2.SetAutoparticipateConnections(true);
	g_fcm2.SetConnectOnNewRemoteConnection(true, "");
	g_cg2.SetAutoProcessNewConnections(true);

	// Start up raknet
	const unsigned int maxConnections = g_maxPlayers;
	while (IRNS2_Berkley::IsPortInUse(g_startingPort, "127.0.0.1", AF_INET, SOCK_DGRAM) == true)
	{
		g_startingPort++;
	}
	SocketDescriptor sd(g_startingPort, "127.0.0.1");

	StartupResult result = g_rakPeerInterface->Startup(maxConnections, &sd, 1);
	if (result != RAKNET_STARTED)
	{
		printf("It just wasn't meant to be, raknet won't start %i", result);
		system("pause");
		exit(0);
	}
	g_rakPeerInterface->SetMaximumIncomingConnections(maxConnections);

	printf("We done it, raknet has started!! Started Port: %i\n", g_startingPort);

	printf("Start listening for packets.. \n");
	std::thread packetListenerThread(PacketListener);

	// Start interacting with user
	printf("Press y to connect to a game that has been started.  Press anything else to start a new instance of the game.\n");
	char userInput[32];
	Gets(userInput, sizeof(userInput));
	if (userInput[0] == 'y' || userInput[0] == 'Y')
	{
		printf("Time to connect.....four\n");
		printf("please enter the port to connect to\n");
		char port[32];
		Gets(port, sizeof(port));
		int portNum = atoi(port);

		ConnectionAttemptResult caResult = g_rakPeerInterface->Connect("127.0.0.1", portNum, nullptr, 0);
		if (caResult != CONNECTION_ATTEMPT_STARTED)
		{
			printf("It just wasn't meant to be, we can't connect %i\n", result);
			system("pause");
			exit(0);
		}

		printf("CONNECTED!!\n");
	}

	// Ask for confirmation to start the game
	g_readyEventPlugin.AddToWaitList(ID_RE_PLAYER_JOIN, g_rakPeerInterface->GetMyGUID());

	printf("ARE YOU READY TO CHAT FIGHT!?\n");
	printf("Type in y followed by enter if you are ready...\n\n");
	Gets(userInput, sizeof(userInput));
	if (userInput[0] == 'y' || userInput[0] == 'Y')
	{
		static char* wrestler = "wrestler";
		static char* samurai = "samurai";
		static char* grifter = "grifter";
		static char* manager = "manager";
		static char* help = "help";

		Character*  character;
		char* characterName = "Buddy";
		CharacterClasses chosenClass;
		int statBoostPoints = 10;
		int hpBoost;
		int atkBoost;
		int defBoost;
		int spdBoost;

		bool answered = false;
		do {
			printf("Choose your class:\n");
			printf("Type 'wrestler' for wrestler\n");
			printf("Type 'samurai' for samurai\n");
			printf("Type 'grifter' for grifter\n");
			printf("Type 'manager' for manager\n");
			printf("Type 'help' for character class descriptions\n");

			Gets(userInput, sizeof(userInput));

			system("cls");

			if (strcmp(userInput, wrestler) == 0)
			{
				printf("You chose the wrestler.\n");
				chosenClass = E_CCWrestler;
				answered = true;
			}
			else if (strcmp(userInput, samurai) == 0)
			{
				printf("You chose the samurai.\n");
				chosenClass = E_CCSamurai;
				answered = true;
			}
			else if (strcmp(userInput, grifter) == 0)
			{
				printf("You chose the grifer.\n");
				chosenClass = E_CCGrifter;
				answered = true;
			}
			else if (strcmp(userInput, manager) == 0)
			{
				printf("You chose the manager.\n");
				chosenClass = E_CCManager;
				answered = true;
			}
			else if (strcmp(userInput, help) == 0)
			{
				DisplayClassInformation();
			}
			else
			{
				printf("Unrecognized command please try again...\n\n");
			}

			if (answered)
			{
				int answer;
				printf("You have 10 bonus stat points. You can assign them however you want.\n");

				// Assign to HP
				printf("How many points do you want to add to your HP stat?\n");

				Gets(userInput, sizeof(userInput));
				answer = atoi(userInput);
				if (answer > statBoostPoints)
					answer = statBoostPoints;

				hpBoost = answer;
				statBoostPoints -= answer;

				// Assign to Attack
				printf("How many points do you want to add to your attack stat?\n");

				Gets(userInput, sizeof(userInput));
				answer = atoi(userInput);
				if (answer > statBoostPoints)
					answer = statBoostPoints;

				atkBoost = answer;
				statBoostPoints -= answer;

				// Assign to defence
				printf("How many points do you want to add to your defence stat?\n");

				Gets(userInput, sizeof(userInput));
				answer = atoi(userInput);
				if (answer > statBoostPoints)
					answer = statBoostPoints;

				defBoost = answer;
				statBoostPoints -= answer;

				// Assign to speed
				printf("How many points do you want to add to your speed stat?\n");

				Gets(userInput, sizeof(userInput));
				answer = atoi(userInput);
				if (answer > statBoostPoints)
					answer = statBoostPoints;

				spdBoost = answer;
				statBoostPoints -= answer;
			}

		} while (!answered);

		printf("Choose your character's name:\n");
		Gets(userInput, sizeof(userInput));
		characterName = userInput;

		switch (chosenClass)
		{
		case E_CCWrestler:
			character = new Wrestler(characterName, hpBoost, atkBoost, defBoost, spdBoost);
			break;
		case E_CCSamurai:
			character = new Samurai(characterName, hpBoost, atkBoost, defBoost, spdBoost);
			break;
		case E_CCGrifter:
			character = new Grifter(characterName, hpBoost, atkBoost, defBoost, spdBoost);
			break;
		case E_CCManager:
			character = new Manager(characterName, hpBoost, atkBoost, defBoost, spdBoost);
			break;
		default:
			character = new Wrestler(characterName, hpBoost, atkBoost, defBoost, spdBoost);
			break;
		}

		character->SetIsMaster(true);
		character->SetNetworkIDManager(&g_networkIDManager);
		// inserting our character at the beginning of the vector in order to have easy access later on
		// g_characters[0] is our racer
		g_playerMutex.lock();
		g_characters.insert(g_characters.begin(), character);
		g_playerMutex.unlock();

		// Set ready event to start the game
		g_readyEventPlugin.SetEvent(ID_RE_PLAYER_JOIN, true);
		std::thread inputListenerThread(InputListener);

		// this will make the program wait until the thread below is done executing
		packetListenerThread.join();
	}
	else
	{
		g_isRunning = false;
		printf("we understand..not everybody..is....ready\n");
	}

	return 0;
}

void InputListener()
{
	static char* attack = "attack";
	static char* stats = "stats";
	static char* help = "help";
	static char* quit = "quit";

	DisplayHelp();

	while (g_isRunning)
	{
		while (g_isGameRunning)
		{
			char input[32];
			Gets(input, sizeof(input));
			system("cls");

			if (strcmp(input, attack) == 0)
			{
				printf("Attacking..\n");

				// Send packet telling the world we are accelerating
				//BitStream bs;
				//bs.Write((unsigned char)ID_GB3_ACCELERATE);
				//bs.Write(g_racers[0]->GetNetworkID());
				//g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			else if (strcmp(input, stats) == 0)
			{
				g_playerMutex.lock();
				for each(Character* character in g_characters)
				{
					character->DisplayStats();
				}
				g_playerMutex.unlock();
			}
			else if (strcmp(input, help) == 0)
			{
				//DisplayHelp();
				DisplayClassInformation();
			}
			else if (strcmp(input, quit) == 0)
			{
				printf("Quitting..\n");
				g_isGameRunning = false;
				g_isRunning = false;
			}
		}
		Sleep(100);
	}
}

void DisplayHelp()
{
	printf("************************************************\n");
	printf("******************RACER STATS*******************\n");
	printf("'go' = Accelerate\n");
	printf("'stop' = Brake\n");
	printf("'stats'  = Racer Stats\n");
	printf("'help' = This\n");
	printf("'quit'  = Quit game\n");
	printf("************************************************\n");
	printf("************************************************\n");
}

void PacketListener()
{
	printf("Listening for packets..\n");
	Packet* packet;

	while (g_isRunning)
	{
		for (packet = g_rakPeerInterface->Receive();
			packet != nullptr;
			g_rakPeerInterface->DeallocatePacket(packet),
			packet = g_rakPeerInterface->Receive())
		{
			unsigned short packetIdentifier = packet->data[0];
			switch (packetIdentifier)
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				g_readyEventPlugin.AddToWaitList(ID_RE_PLAYER_JOIN, packet->guid);
				g_readyEventPlugin.AddToWaitList(ID_RE_CHARACTERS_SELECTED, packet->guid);
				g_readyEventPlugin.AddToWaitList(ID_RE_GAME_OVER, packet->guid);
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("ID_CONNECTION_ATTEMPT_FAILED\n");
				break;
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("** New player connected. **\n");
				g_readyEventPlugin.AddToWaitList(ID_RE_PLAYER_JOIN, packet->guid);
				g_readyEventPlugin.AddToWaitList(ID_RE_CHARACTERS_SELECTED, packet->guid);
				g_readyEventPlugin.AddToWaitList(ID_RE_GAME_OVER, packet->guid);
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_READY_EVENT_SET:
				printf("ID_READY_EVENT_SET\n");
				break;
			case ID_READY_EVENT_ALL_SET:
				printf("ID_READY_EVENT_ALL_SET\n");
				{
					BitStream bs(packet->data, packet->length, false);
					bs.IgnoreBytes(sizeof(MessageID));
					int readyEventId;
					bs.Read(readyEventId);

					// If player join is set send created characters to all connected players
					if (readyEventId == ID_RE_PLAYER_JOIN && !g_isGameRunning)
					{
						g_readyEventPlugin.AddToWaitList(ID_RE_CHARACTERS_SELECTED, packet->guid);
						//g_isGameRunning = true;

						BitStream bs;
						bs.Write((unsigned char)ID_CHATFIGHT_CREATE_CHARACTER);
						bs.Write(g_characters[0]->GetNetworkID());
						bs.Write(g_characters[0]->GetClass());
						char nameInput[32];
						strcpy(nameInput, g_characters[0]->GetName());
						bs.Write(nameInput);
						bs.Write(g_characters[0]->GetHPBoost());
						bs.Write(g_characters[0]->GetAtkBoost());
						bs.Write(g_characters[0]->GetDefBoost());
						bs.Write(g_characters[0]->GetSpdBoost());
						g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

						g_readyEventPlugin.SetEvent(ID_RE_CHARACTERS_SELECTED, true);
					}

					if (readyEventId == ID_RE_CHARACTERS_SELECTED)
					{
						printf("All players have selected their characters.\n");
						//g_playerMutex.lock();
						//for each(Character* character in g_characters)
						//{
						//	character->DisplayStats();
						//}
						//g_playerMutex.unlock();

						SetPlayerOrder();

						g_isGameRunning = true;
					}
				}
				break;
			case ID_READY_EVENT_UNSET:
				printf("ID_READY_EVENT_UNSET\n");
				break;
			case ID_CHATFIGHT_CREATE_CHARACTER:
			{
				printf("creating replica...\n");
				BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(MessageID));
				NetworkID netID;
				bs.Read(netID);
				CharacterClasses charClass;
				bs.Read(charClass);

				// Check first to see if the replica already exists
				bool canContinue = true;
				for each (Character* c in g_characters)
				{
					if (c->GetNetworkID() == netID)
					{
						// Replica already exists
						canContinue = false;
						break;
					}
				}

				if (canContinue)
				{
					Character* newCharacter = new Wrestler("name", 0, 0, 0, 0);
					char nameInput[32];
					int hpBoost;
					int atkBoost;
					int defBoost;
					int spdBoost;

					bs.Read(nameInput);
					bs.Read(hpBoost);
					bs.Read(atkBoost);
					bs.Read(defBoost);
					bs.Read(spdBoost);

					switch (charClass)
					{
					case E_CCWrestler:
						newCharacter = new Wrestler(nameInput, hpBoost, atkBoost, defBoost, spdBoost);
						break;
					case E_CCSamurai:
						newCharacter = new Samurai(nameInput, hpBoost, atkBoost, defBoost, spdBoost);
						break;
					case E_CCGrifter:
						newCharacter = new Grifter(nameInput, hpBoost, atkBoost, defBoost, spdBoost);
						break;
					case E_CCManager:
						newCharacter = new Manager(nameInput, hpBoost, atkBoost, defBoost, spdBoost);
						break;
					default:
						printf("UNKNOWN CLASS PASSED IN!\n");
						break;
					}

					if (newCharacter != nullptr)
					{
						newCharacter->SetIsMaster(false);
						newCharacter->SetNetworkIDManager(&g_networkIDManager);
						newCharacter->SetNetworkID(netID);
						g_playerMutex.lock();
						g_characters.push_back(newCharacter);
						g_playerMutex.unlock();
					}
					printf("Replica created.\n");
				}
				else
				{
					printf("Replica already created.\n");
				}
			}
			break;
			default:
				printf("Packet received %i\n", packetIdentifier);
				break;
			}
		}
		Sleep(100);
	}
}

void DisplayClassInformation()
{
	// Wrestler Description
	printf("-----------------------------------------------\n");
	printf("## Wrestler ##\n");
	printf("A good all around fighter.  They love to play to the crowd.\n");
	printf("-----------------------------------------------\n");
	printf("BASE HP: 25\n");
	printf("BASE ATTACK: 15\n");
	printf("BASE DEFENCE: 15\n");
	printf("BASE SPEED: 8\n");
	printf("-----------------------------------------------\n");
	printf("SPECIAL ATTACK:\n");
	printf("Attack x2, Speed x1.5 for 3 turns.\n");
	printf("Heal 25%s on the turn you use your special.\n", "%");
	printf("-----------------------------------------------\n");

	// Samurai Description
	printf("-----------------------------------------------\n");
	printf("## Samurai ##\n");
	printf("A powerful attacker.  Always stoic in the face of danger.\n");
	printf("-----------------------------------------------\n");
	printf("BASE HP: 30\n");
	printf("BASE ATTACK: 20\n");
	printf("BASE DEFENCE: 15\n");
	printf("BASE SPEED: 3\n");
	printf("-----------------------------------------------\n");
	printf("SPECIAL ATTACK:\n");
	printf("Damage all opponents at once.\n");
	printf("-----------------------------------------------\n");

	// Grifter Description
	printf("-----------------------------------------------\n");
	printf("## Grifter ##\n");
	printf("A sneaky and quick fighter.  They'll talk you into buying the air around you.\n");
	printf("-----------------------------------------------\n");
	printf("BASE HP: 20\n");
	printf("BASE ATTACK: 10\n");
	printf("BASE DEFENCE: 5\n");
	printf("BASE SPEED: 12\n");
	printf("-----------------------------------------------\n");
	printf("SPECIAL ATTACK:\n");
	printf("Speed x3 for 3 turns.\n");
	printf("Attack a target for 5 damage.\n");
	printf("-----------------------------------------------\n");

	// Manager Description
	printf("-----------------------------------------------\n");
	printf("## Manager ##\n");
	printf("A defensive juggernaut.  They'll make sure their employees do all the work.\n");
	printf("-----------------------------------------------\n");
	printf("BASE HP: 10\n");
	printf("BASE ATTACK: 20\n");
	printf("BASE DEFENCE: 35\n");
	printf("BASE SPEED: 1\n");
	printf("-----------------------------------------------\n");
	printf("SPECIAL ATTACK:\n");
	printf("Defence x3 for 3 turns.\n");
	printf("Attack a target for 1 damage.\n");
	printf("-----------------------------------------------\n");
}

void SetPlayerOrder()
{
	g_playerMutex.lock();
	for (int i = 0; i < g_characters.size(); i++)
	{
		g_playerIDs.push_back(g_characters[i]->GetNetworkID());
	}
	g_playerMutex.unlock();

	RakNet::NetworkID fastest = 0;
	int i = 0;
	do {
		fastest = GetFastestPlayer();
		if (fastest != 0)
		{
			for each (RakNet::NetworkID id in g_playerIDs)
			{
				if (fastest == id)
				{
					if (g_characters[0]->GetNetworkID() == id)
					{
						g_characters[0]->SetPlayerAtkOrder(fastest, true, i);
					}
					else
					{
						g_characters[0]->SetPlayerAtkOrder(fastest, false, 0);
					}
				}
			}

			for (int i = 0; i < g_playerIDs.size(); i++)
			{
				if (g_playerIDs[i] == fastest)
				{
					g_playerIDs.erase(g_playerIDs.begin() + i);
				}
			}
		}
		i++;
	} while (fastest != 0);

	g_playerMutex.lock();
	for (int i = 0; i < g_characters.size(); i++)
	{
		g_playerIDs.push_back(g_characters[0]->IsTurn(i));
	}
	g_playerMutex.unlock();
	// For testing
	//g_characters[0]->PrintAtkOrder(g_characters);
}

RakNet::NetworkID GetFastestPlayer()
{
	if (g_playerIDs.size() == 0)
	{
		return 0;
	}
	else
	{
		RakNet::NetworkID fastestPlayer = 0;
		int fastestSpeed = -1;
		for each (Character* character in g_characters)
		{
			for each (RakNet::NetworkID id in g_playerIDs)
			{
				if (character->GetNetworkID() == id)
				{
					if (character->GetSpeed() > fastestSpeed)
					{
						fastestPlayer = id;
						fastestSpeed = character->GetSpeed();
					}
				}
			}
		}

		return fastestPlayer;
	}
}
