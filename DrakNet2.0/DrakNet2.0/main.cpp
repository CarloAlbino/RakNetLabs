#include "MessageIdentifiers.h"
#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "ReadyEvent.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"
#include "Gets.h"
#include <thread>

using namespace RakNet;

enum {
	ID_GB3_NUMBER_GUESS = ID_USER_PACKET_ENUM,
	ID_GB3_CORRECT_GUESS
};

enum ReadyEventIDs
{
	ID_RE_PLAYER_JOIN = 0,
	ID_RE_GAME_OVER,
};

void PacketListener();
void NumberGuessing();
int GetRandomNumber(int min, int max);

RakPeerInterface* g_rakPeerInterface = nullptr;
int g_startingPort = 6500;
bool g_isRunning = true;
bool g_isGameRunning = false;

// For guessing game
const int MIN_NUM = 1;
const int MAX_NUM = 20;
int g_numberAnswer = 0;
int g_numberOfGuesses = 0;
char g_userName[32] = "Player";
int g_prevGuess = 0;

// Allows for ready events
ReadyEvent g_readyEventPlugin;
// These two plugins are just to automatically create a fully connected mesh so I don't have to call connect more than once
FullyConnectedMesh2 g_fcm2;
ConnectionGraph2 g_cg2;

int main()
{
	srand(time(NULL));

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
	const unsigned int maxConnections = 6;
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

	printf("Raknet has started!! Started Port: %i\n", g_startingPort);

	printf("Started listening for packets.. \n");
	std::thread packetListenerThread(PacketListener);

	// Start interacting with user
	printf("Press y to connect to a game that has been started.\nPress anything else to start a new instance of the game.\n");
	char userInput[32];
	Gets(userInput, sizeof(userInput));
	if (userInput[0] == 'y' || userInput[0] == 'Y')
	{
		printf("Time to connect..\n");
		printf("Please enter the port to connect to\n");
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

	printf("I have one question...\n");
	printf("ARE YOU READY TO GUESS NUMBERS!!!??!?!?!?!?!??!\n");
	printf("Type in y followed by enter if you are ready: \n\n");
	Gets(userInput, sizeof(userInput));
	if (userInput[0] == 'y' || userInput[0] == 'Y')
	{
		// Ask for user name
		printf("GREAT!\nEnter your name:\n");
		Gets(g_userName, sizeof(g_userName));
		char temp[2] = "[";
		strcat(temp, g_userName);
		strcat(temp, "]");
		strcpy(g_userName, temp);

		// Set ready event to start the game
		g_readyEventPlugin.SetEvent(ID_RE_PLAYER_JOIN, true);
		std::thread numberGuessThread(NumberGuessing);
		// this will make the program wait until the thread below is done executing
		packetListenerThread.join();
	}
	else
	{
		g_isRunning = false;
		printf("We understand..not everybody..is....ready\n");
	}

	return 0;
}

void NumberGuessing()
{
	while (g_isRunning)
	{
		while (g_isGameRunning)
		{
			// Get a random number if none has been chosen
			if (g_numberAnswer <= 0)
			{
				g_numberAnswer = GetRandomNumber(MIN_NUM, MAX_NUM);
			}

			// Main display
			system("cls");
			printf("################################\n\n");
			printf("Welcome to number guessing game.\n\n");
			printf("################################\n\n");
			printf("Please guess a number between %i - %i\n\n", MIN_NUM, MAX_NUM);
			printf("Current number of guesses: %i\n\n", g_numberOfGuesses);
			printf("################################\n\n");

			// If there has been a preveious guess.  This is player feedback for incorrect answers.
			if (g_numberOfGuesses > 0)
			{
				if (g_prevGuess > g_numberAnswer)
				{
					// Guess is too high
					printf("%i is too high. Guess again:\n", g_prevGuess);
				}
				else
				{
					// Guess is too low
					printf("%i is too low. Guess again:\n", g_prevGuess);
				}
			}

			// Guess
			char numberGuessChar[32];
			Gets(numberGuessChar, sizeof(numberGuessChar));
			int guess = atoi(numberGuessChar);

			// Add a guess
			g_numberOfGuesses++;

			// We have a guess, broadcast it to other players
			BitStream bs;

			// Process results locally, then braodcast to other players
			if (atoi(numberGuessChar) == g_numberAnswer)
			{
				// Guessed correctly
				// Write packet for a correct guess
				printf("CORRECT!\nThe answer was %i!\nYou guessed it in %i guesses.", g_numberAnswer, g_numberOfGuesses);
				// Broadcast that the answer is correct
				bs.Write((unsigned char)ID_GB3_CORRECT_GUESS);
				// Write user name to packet
				bs.Write(g_userName);
				// Write answer to packet
				bs.Write(g_numberAnswer);
				// Write number of guesses to packet
				bs.Write(g_numberOfGuesses);
				// Broadcast packet to all players
				g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				printf("\nWaiting for the other players to finish the game...\n\n");
				// Ask for permission to end the game
				g_readyEventPlugin.AddToWaitList(ID_RE_GAME_OVER, g_rakPeerInterface->GetMyGUID());
				// Set ready event to start the game
				g_readyEventPlugin.SetEvent(ID_RE_GAME_OVER, true);
				
				// Player has finished the game.  Ignore further input until all players are done.
				g_isGameRunning = false;
			}
			else
			{
				// Save guess for later use
				g_prevGuess = atoi(numberGuessChar);

				// Write packet for a false guess
				bs.Write((unsigned char)ID_GB3_NUMBER_GUESS);
				// Write user name to packet
				bs.Write(g_userName);
				// Write answer to packet
				bs.Write(g_numberAnswer);
				// Write guess to packet
				bs.Write(guess);
				// Write number of guesses to packet
				bs.Write(g_numberOfGuesses);
				// Broadcast packet to all players
				g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
		}
	}
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
				printf("     ID_CONNECTION_REQUEST_ACCEPTED\n");
				g_readyEventPlugin.AddToWaitList(ID_RE_PLAYER_JOIN, packet->guid);
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("     ID_CONNECTION_ATTEMPT_FAILED\n");
				break;
			case ID_ALREADY_CONNECTED:
				printf("     ID_ALREADY_CONNECTED\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("     ID_NEW_INCOMING_CONNECTION\n");
				g_readyEventPlugin.AddToWaitList(ID_RE_PLAYER_JOIN, packet->guid);
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("     ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_READY_EVENT_SET:
				printf("     ID_READY_EVENT_SET\n");
				break;
			case ID_READY_EVENT_ALL_SET:
				printf("     ID_READY_EVENT_ALL_SET\n");
				{
					BitStream bs(packet->data, packet->length, false);
					bs.IgnoreBytes(sizeof(MessageID));
					int readyEventId;
					bs.Read(readyEventId);
					if (readyEventId == ID_RE_PLAYER_JOIN && !g_isGameRunning)
					{
						g_isGameRunning = true;
					}
					if (readyEventId == ID_RE_GAME_OVER && g_isGameRunning)
					{
						printf("All players have finished. Quitting...\n");
						g_isRunning = false;
					}
				}
				break;
			case ID_READY_EVENT_UNSET:
				printf("     ID_READY_EVENT_UNSET\n");
				break;
			case ID_GB3_NUMBER_GUESS:
			{
				// Other user has guess a number
				BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(MessageID));
				// Read all the data passed in with the packet
				char userName[sizeof(g_userName)];
				bs.Read(userName);
				int answer;
				bs.Read(answer);
				int guess;
				bs.Read(guess);
				int numOfGuesses;
				bs.Read(numOfGuesses);
				// Print feedback
				printf("  %s is trying to guess the number %i\nThey guessed...%i\nThey have guessed %i times.\n", userName, answer, guess, numOfGuesses);
				printf("YOUR GUESS: ");
			}
			break;
			case ID_GB3_CORRECT_GUESS:
			{
				// Other user has guessed correctly
				BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(MessageID));
				// Read all the data passed in with the packet
				char userName[sizeof(g_userName)];
				bs.Read(userName);
				int answer;
				bs.Read(answer);
				int numOfGuesses;
				bs.Read(numOfGuesses);
				// Print feedback
				printf("  %s guessed the number %i in %i guesses. They have finished the game.\n", userName, answer, numOfGuesses);
				printf("YOUR GUESS: ");
			}
				break;
			default:
				printf("     Packet received %i\n", packetIdentifier);
				break;
			}
		}
	}
}

int GetRandomNumber(int min, int max)
{
	srand(time(NULL));

	return rand() % max + min;
}