#include "stdafx.h"

void(__stdcall* kailleraGetVersion) (char *version);
void(__stdcall* kailleraInit) ();
void(__stdcall* kailleraShutdown) ();
void(__stdcall* kailleraSetInfos) (kailleraInfos *infos);
void(__stdcall* kailleraSelectServerDialog) (HWND parent);
int(__stdcall* kailleraModifyPlayValues)  (void *values, int size);
void(__stdcall* kailleraChatSend)  (char *text);
void(__stdcall* kailleraEndGame) ();

extern CKaillera *ck;

DWORD oldControllers[4];

void ShowError(char* str)
{
	MessageBox(NULL, str, str, NULL);
}

int WINAPI kailleraGameCallback(char *game, int player, int numplayers)
{
	ck->playerNumber = player-1; // since our player #1 is in index zero
	ck->numberOfPlayers = numplayers;

	// find game in local list and run it based off of the full path
	char *temp = ck->szKailleraNamedRoms;

	for (int x = 0; x < ck->numberOfGames; x++)
	{
		if (strncmp(temp, game, strlen(temp)) == 0)
		{
			//MessageBox(NULL, temp, "You must want to play", NULL);
			//MessageBox(NULL, ck->szFullFileNames[x], "Location", NULL);

			temp = ck->szFullFileNames[x];
			break;
		}
		temp += strlen(temp) + 1;
	}

	g_Notify->BringToTop();

	ck->isPlayingKailleraGame = true;
	g_BaseSystem->RunFileImage(temp);

	return 0;
}

void WINAPI kailleraChatReceivedCallback(char *nick, char *text)
{
	/* Do what you want with this :) */
	//ShowInfo("Kaillera : <%s> : %s", nick, text);
}

void WINAPI kailleraClientDroppedCallback(char *nick, int playernb)
{
	/* Do what you want with this :) */
	//ShowInfo("Kaillera : <%s> dropped (%d)", nick, playernb);
	if ((playernb - 1) == ck->playerNumber) // that means we dropped, so stop emulation!
	{
		ck->endGame();
		g_BaseSystem->CloseSystem();
		ck->isPlayingKailleraGame = false;
	}
}

void WINAPI kailleraMoreInfosCallback(char *gamename)
{
	/* Do what you want with this :) */
	//ShowInfo("Kaillera : MoreInfosCallback %s ", gamename);
}

CKaillera::CKaillera()
{
	kInfos.appName = sAppName;
	kInfos.gameList = szKailleraNamedRoms;
	kInfos.gameCallback = kailleraGameCallback;
	kInfos.chatReceivedCallback = kailleraChatReceivedCallback;
	kInfos.clientDroppedCallback = kailleraClientDroppedCallback;
	kInfos.moreInfosCallback = kailleraMoreInfosCallback;

	LoadKailleraFuncs();

	kailleraInit();

	isPlayingKailleraGame = false;
	numberOfGames = playerNumber = numberOfPlayers = 0;
	memset(values, 0, sizeof(values));
	playValuesLength = 0;
}

CKaillera::~CKaillera()
{
	kailleraShutdown();
}

void CKaillera::clearGameList()
{
	// clear the list
	pszKailleraNamedRoms = szKailleraNamedRoms;

	memset(szKailleraNamedRoms, 0, sizeof(szKailleraNamedRoms));
	memset(szFullFileNames, 0, sizeof(szFullFileNames));

	// seed it with the two basic rooms
	addGame("*Chat (not game)", " ");
	addGame("*Away (leave messages)", " ");

	numberOfGames = 2;
}

void CKaillera::addGame(char *gameName, char *szFullFileName)
{
	strncpy(pszKailleraNamedRoms, gameName, strlen(gameName) + 1);
	pszKailleraNamedRoms += strlen(gameName) + 1;

	strncpy(szFullFileNames[numberOfGames], szFullFileName, strlen(szFullFileName) + 1);

	numberOfGames++;
}

void CKaillera::terminateGameList()
{
	*(++pszKailleraNamedRoms) = '\0';
}

void CKaillera::setInfos()
{
	kailleraSetInfos(&kInfos);
}

void CKaillera::selectServerDialog(HWND hWnd)
{
	kailleraSelectServerDialog(hWnd);
}

void CKaillera::modifyPlayValues(DWORD val)
{
	memset(&values, 0, sizeof(values));
	values[0] = val;
	playValuesLength = kailleraModifyPlayValues(&values, sizeof(DWORD));
}

DWORD CKaillera::getValues(int player)
{
	return values[player];
}

void CKaillera::endGame()
{
	kailleraEndGame();
}

int CKaillera::LoadKailleraFuncs()
{
	KailleraHandle = LoadLibrary("kailleraclient.dll");

	if (KailleraHandle) {
		//ShowInfo("Kaillera Library found");
		kailleraGetVersion = (void(__stdcall*) (char*)) GetProcAddress(KailleraHandle, "_kailleraGetVersion@4");
		if (kailleraGetVersion == NULL) {
			ShowError("kailleraGetVersion not found");
			return 0;
		}

		kailleraInit = (void(__stdcall *)(void)) GetProcAddress(KailleraHandle, "_kailleraInit@0");
		if (kailleraInit == NULL) {
			ShowError("kailleraInit not found");
			return 0;
		}

		kailleraShutdown = (void(__stdcall *) (void)) GetProcAddress(KailleraHandle, "_kailleraShutdown@0");
		if (kailleraShutdown == NULL) {
			ShowError("kailleraShutdown not found");
			return 0;
		}

		kailleraSetInfos = (void(__stdcall *) (kailleraInfos *)) GetProcAddress(KailleraHandle, "_kailleraSetInfos@4");
		if (kailleraSetInfos == NULL) {
			ShowError("kailleraSetInfos not found");
			return 0;
		}

		kailleraSelectServerDialog = (void(__stdcall*) (HWND parent)) GetProcAddress(KailleraHandle, "_kailleraSelectServerDialog@4");
		if (kailleraSelectServerDialog == NULL) {
			ShowError("kailleraSelectServerDialog not found");
			return 0;
		}

		kailleraModifyPlayValues = (int(__stdcall *) (void *values, int size)) GetProcAddress(KailleraHandle, "_kailleraModifyPlayValues@8");
		if (kailleraModifyPlayValues == NULL) {
			ShowError("kailleraModifyPlayValues not found");
			return 0;
		}

		kailleraChatSend = (void(__stdcall *) (char *)) GetProcAddress(KailleraHandle, "_kailleraChatSend@4");
		if (kailleraChatSend == NULL) {
			ShowError("kailleraChatSend not found");
			return 0;
		}

		kailleraEndGame = (void(__stdcall *) (void)) GetProcAddress(KailleraHandle, "_kailleraEndGame@0");
		if (kailleraEndGame == NULL) {
			ShowError("kailleraEndGame not found");
			return 0;
		}
	}

	return 1;
}