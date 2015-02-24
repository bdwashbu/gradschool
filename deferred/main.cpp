// Final Project
// 12-12-09

#include <windows.h>		// Header File For Windows
#include <stdio.h>

#include "Utilities\glew.h"
#include <gl\gl.h>

#include "OpenGLWindow.h"

#pragma comment( lib, "opengl32.lib" )

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{

	bool fullscreen = TRUE;
	RECT rc;

	AllocConsole(); // opens console for debugging
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Hi-Res Mode?", "Final Project",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		InitOpenGLWindow("Brian Washburn Final Project", 640, 480, 32, false);
	} else {
		InitOpenGLWindow("Brian Washburn Final Project", 1024, 756, 32, false);
		//GetWindowRect(GetDesktopWindow(), &rc);
		//InitOpenGLWindow("Brian Washburn Homework 5: Shadows", rc.right, rc.bottom, 32, true);
	}

	RunOpenGLWindow(); // run the app!

	KillOpenGLWindow();
	return 0;
}
