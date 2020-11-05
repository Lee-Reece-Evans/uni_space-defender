#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.OpenConsole();
	app.OpenWindow(800, 600, "Galaxian");
	//app.OpenFullScreen(800, 600, 32);
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
