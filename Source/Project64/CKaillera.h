// plug in the appropraite number of controllers
// check endian-ness since analogs are messed up
// check why totally out of sync	 

#pragma once

#define KAILLERA_CLIENT_API_VERSION "0.8"

struct kailleraInfos
{
	char *appName;
	char *gameList;

	int (WINAPI *gameCallback)(char *game, int player, int numplayers);

	void (WINAPI *chatReceivedCallback)(char *nick, char *text);
	void (WINAPI *clientDroppedCallback)(char *nick, int playernb);

	void (WINAPI *moreInfosCallback)(char *gamename);
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
	char szKailleraNamedRoms[50 * 2000];
	char szFullFileNames[50][2000];
	bool isPlayingKailleraGame;

	int playerNumber;
	int numberOfPlayers;

	DWORD getValues(int player);
	void endGame();
private:
	kailleraInfos   kInfos;
	HMODULE KailleraHandle;
	int LoadKailleraFuncs();
	char *pszKailleraNamedRoms;
	char *sAppName = "Project 64k Core 2.2";
	//char *sAppName = "Project 64k 0.13 (01 Aug 2003)"; // CHANGE THIS
	DWORD values[8]; // for a maximum of 8 players
	int playValuesLength;
};

// Emulator functions 
/*int LoadKaillera();
void EndGameKaillera();
void CloseKaillera();
void EndGameKaillera();
void KailleraPlay(void);
DWORD WINAPI KailleraThread(LPVOID lpParam);
int isKailleraExist();*/

//#endif