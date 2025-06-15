#include "Core/EtherealEngine.h"
#include "Core/GameApp.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	GameApp gameApp;
	gameApp.Run();

	return 0;
}

int main(int argc, char* argv[])
{
	// Call WinMain to start the application
	WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
	return 0;
}