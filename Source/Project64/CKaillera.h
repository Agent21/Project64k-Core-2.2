#pragma once

#define KAILLERA_CLIENT_API_VERSION "0.8"

#define MAX_NUMBER_OF_GAMES 1024

struct kailleraInfos
{
	char *appName;
	char *gameList;

	int (WINAPI *gameCallback)(char *game, int player, int numplayers);

	void (WINAPI *chatReceivedCallback)(char *nick, char *text);
	void (WINAPI *clientDroppedCallback)(char *nick, int playernb);

	void (WINAPI *moreInfosCallback)(char *gamename);
};

#define PACKET_TYPE_INPUT 1
#define PACKET_TYPE_CHEAT 2

struct CKailleraPacket
{
	BYTE Type;
	DWORD Command;
	WORD  Value;
};

class CKaillera
{
public:
	CKaillera();
	~CKaillera();

	void clearGameList();
	void addGame(char *gameName, char *szFullFileName);
	void terminateGameList();
	void selectServerDialog(HWND hWnd);
	void modifyPlayValues(DWORD values);
	void setInfos();

	int numberOfGames;
	char szKailleraNamedRoms[MAX_NUMBER_OF_GAMES * 2000];
	char szFullFileNames[MAX_NUMBER_OF_GAMES][2000];
	bool isPlayingKailleraGame;

	int playerNumber;
	int numberOfPlayers;

	DWORD getValues(int player);
	void endGame();
private:
	kailleraInfos   kInfos;
	HMODULE KailleraHandle;
	int LoadKailleraFuncs();
	void processResult(CKailleraPacket ckp[]);
	char *pszKailleraNamedRoms;
	char *sAppName = "Project 64k Core 2.2";
	//char *sAppName = "Project 64k 0.13 (01 Aug 2003)"; // CHANGE THIS
	DWORD values[4]; // for a maximum of 4 players
	int playValuesLength;
};