
/******************************************
Class: OpenGLWindow

Purpose: Manages the window and opengl thread
Mouse movement and window input are taken
care of via the window thread.  The window
thread signals to the opengl thread of certain
events, i.e resizing the window.  The goal
is to increase performance by using
multiple cpu cores

By: Brian Washburn (10-20-09)
*******************************************/

#include <windows.h>
#include "Render\OpenGLWindow.h"
#include "Render\RenderManager.h"
#include "Utilities\glew.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <stdio.h>
#include <math.h>

bool keys[256];

int mouseX = 0, mouseY = 0;
bool moveMode = true;
int windowXPosition = 0, windowYPosition = 0;
int windowWidth = 0, windowHeight = 0;
bool active = false;
bool fullscreen = true;
bool mouseMoved = false;
bool sized = false;
bool resetMouse = false;

int depthDarkeningEnabled = 1;
int softShadowsEnabled = 1;
int motionBlurEnabled = 1;
int depthOfFieldEnabled = 1;
int Depthcounter = 0;

bool debounce = false;

HGLRC		hRC;  // Windows data structures
HDC         hDC;
HWND		hWnd;		
HINSTANCE	hInstance;

using namespace OpenGL;

bool openGLinit = false;

#define FINAL 99

#define G_BUFFER_DIFFUSE 101
#define G_BUFFER_NORMAL 102
#define G_BUFFER_SPECULAR 103
#define G_BUFFER_DEPTH 104

#define EFFECT_DEPTH_DARKENING 105
#define EFFECT_SOFT_SHADOWS 106
#define EFFECT_MOTION_BLUR 107
#define EFFECT_DEPTH_OF_FIELD 108

#define LIGHT_MAP 109
#define SHADOW_MAP 110

#define EXIT 900

/******************************************
OpenglWindow constructor

Purpose:  This constructor uses
the windows API to interface with opengl
and create a context in which it can be
rendered.  A new thread is created that
manages all Opengl calls

*******************************************/

void InitOpenGLWindow(const char* title, int width, int height, const int bits, const bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	windowWidth = width;
	windowHeight = height;

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	ShowCursor(FALSE);		

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}




	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			24,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			32,											// 16Bit Z-Buffer (Depth Buffer)  
			1,											// Yes Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
	};



	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}
	if (!(hRC=wglCreateContext(hDC)))				
	{							
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);							
	}

	if(!wglMakeCurrent(hDC,hRC))					
	{						
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);							
	}

	OpenGL::Initialize();
	OpenGL::ResizeGLScene(windowWidth, windowHeight);

	openGLinit = true;

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
}

LRESULT CALLBACK PopupProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	HMENU hMenu, gBufferMenu, effectsMenu;
	POINT point;
	switch(msg)  
	{
	case WM_MBUTTONUP:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		hMenu = CreatePopupMenu();
		ClientToScreen(hwnd, &point);

		gBufferMenu = CreatePopupMenu();
		AppendMenu(gBufferMenu, MF_STRING, G_BUFFER_DIFFUSE, "Diffuse");
		AppendMenu(gBufferMenu, MF_STRING, G_BUFFER_NORMAL, "Normal");
		AppendMenu(gBufferMenu, MF_STRING, G_BUFFER_SPECULAR, "Specular");
		AppendMenu(gBufferMenu, MF_STRING, G_BUFFER_DEPTH, "Linear Depth");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)gBufferMenu, "G_Buffer");

		effectsMenu = CreatePopupMenu();
		AppendMenu(effectsMenu, MF_STRING, EFFECT_DEPTH_DARKENING, "Depth Darkening");
		AppendMenu(effectsMenu, MF_STRING, EFFECT_SOFT_SHADOWS, "Soft Shadows");
		AppendMenu(effectsMenu, MF_STRING, EFFECT_MOTION_BLUR, "Motion Blur");
		AppendMenu(effectsMenu, MF_STRING, EFFECT_DEPTH_OF_FIELD, "Depth of Field");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)effectsMenu, "Effects");

		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, FINAL, TEXT("Final Result"));
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, SHADOW_MAP, TEXT("Shadow Map"));
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, LIGHT_MAP, TEXT("Light Map"));
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, EXIT, TEXT("Quit"));

		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
		DestroyMenu(hMenu);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/******************************************
OpenglWindow.Run

Purpose:  This function contains the main
loop of the window thread.  Windows messages
and dispatched from here.


*******************************************/

void RunOpenGLWindow(void)
{

	MSG msg	= {0};

	//SetCursorPos(windowXPosition+windowWidth/2, windowYPosition+windowHeight/2);

	while(true)									    // Loop That Runs While done=FALSE
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				return;
			}
			else								// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		} else if (active) {							// Program Acti{

			if (keys[VK_ESCAPE])				
			{
				return;				
			}

			if (keys[87]) { //'W'
				OpenGL::UpdateViewVelocity(27.0f);
			} else if (keys[83]) {
				OpenGL::UpdateViewVelocity(-27.0f);
			}

			//if (keys[88]) {
			//keys[88] = false;

			//}

			//if (keys[89]) {
			//	OpenGL::setSSAO(SSAOcounter);
			//}

			Draw(); // Draw anything opengl	
			SwapBuffers(hDC); // Swap buffers!


			if (moveMode && mouseMoved) {

				resetMouse = true;
				SetCursorPos(windowXPosition+windowWidth/2, windowYPosition+windowHeight/2);
				mouseX = windowWidth/2;
				mouseY = windowHeight/2;

			}
			mouseMoved = false;

		}

	}

	if (hRC)										
	{

		if (!wglMakeCurrent(NULL,NULL))				
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;									
	}

}

/******************************************
OpenglWindow.Wndproc

Purpose:  Windows messages are received
and processed here

*******************************************/

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
						 UINT	uMsg,			// Message For This Window
						 WPARAM	wParam,			// Additional Message Information
						 LPARAM	lParam)			// Additional Message Information
{
	RECT rcClient = {0};

	switch (uMsg)									// Check For Windows Messages
	{

	case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
		{


			if (wParam == 89 && !debounce) {
				Depthcounter = (Depthcounter + 1) % 2;
				OpenGL::setDepth(Depthcounter);
			}
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			debounce = true;
			break;								// Jump Back
		}

	case WM_KEYUP:								// Has A Key Been Released?
		{
			debounce = false;
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			break;								// Jump Back
		}

	case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

	case WM_MOUSEMOVE:
		{
			float dist = sqrtf((LOWORD(lParam) - mouseX)*(LOWORD(lParam) - mouseX) + (HIWORD(lParam) - mouseY)*(HIWORD(lParam) - mouseY));
			if (moveMode && !resetMouse && dist >= 2.0) {

				OpenGL::UpdateViewHeading(GLfloat(LOWORD(lParam) - mouseX));	
				OpenGL::UpdateViewPitch(GLfloat(HIWORD(lParam) - mouseY));	
				mouseX = LOWORD(lParam);
				mouseY = HIWORD(lParam);
				mouseMoved = true;

			}

			resetMouse = false;
			break;
		}

	case WM_MOVE:
		{
			windowXPosition = LOWORD(lParam);
			windowYPosition = HIWORD(lParam);
			break;
		}

	case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
			case SC_SCREENSAVE:					// Screensaver Trying To Start?
			case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

	case WM_RBUTTONUP:
		{
			moveMode = !moveMode;
			if (!moveMode) {
				ShowCursor(TRUE);
			} else {
				ShowCursor(FALSE);
			}
			break;
		}

	case WM_MBUTTONUP:
		{
			CallWindowProc (PopupProc, hWnd, uMsg, wParam, lParam); 
			break;
		}


	case WM_LBUTTONUP:
		{
			OpenGL::AddLight();
			break;
		}

	case WM_SIZE:								// Resize The OpenGL Window
		{
			if (openGLinit == true) {
				GetClientRect(hWnd, &rcClient);
				windowWidth = rcClient.right;
				windowHeight = rcClient.bottom;
				OpenGL::ResizeGLScene(windowWidth, windowHeight);
				sized = true;
			}
			return 0;								// Jump Back
		}

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case EXIT:
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}

			case G_BUFFER_DIFFUSE:
				{
					OpenGL::setDisplayedTexture("Diffuse");
					break;
				}

			case G_BUFFER_NORMAL:
				{
					OpenGL::setDisplayedTexture("Normal");
					break;
				}

			case G_BUFFER_SPECULAR:
				{
					OpenGL::setDisplayedTexture("Specular");
					break;
				}

			case G_BUFFER_DEPTH:
				{
					OpenGL::setDisplayedTexture("Depth");
					break;
				}

			case EFFECT_DEPTH_DARKENING:
				{
					depthDarkeningEnabled = (depthDarkeningEnabled + 1) % 2;
					OpenGL::SetDepthDarkening(depthDarkeningEnabled);
					break;
				}

			case EFFECT_SOFT_SHADOWS:
				{
					softShadowsEnabled = (softShadowsEnabled + 1) % 2;
					OpenGL::SetSoftShadows(softShadowsEnabled);
					break;
				}

			case EFFECT_MOTION_BLUR:
				{
					motionBlurEnabled = (motionBlurEnabled + 1) % 2;
					OpenGL::SetMotionBlur(motionBlurEnabled);
					break;
				}

			case EFFECT_DEPTH_OF_FIELD:
				{
					depthOfFieldEnabled = (depthOfFieldEnabled + 1) % 2;
					OpenGL::SetDepthOfField(depthOfFieldEnabled);
					break;
				}

			case LIGHT_MAP:
				{
					OpenGL::setDisplayedTexture("Lightmap");
					break;
				}

			case SHADOW_MAP:
				{
					OpenGL::setDisplayedTexture("Shadowmap");
					break;
				}

			case FINAL:
				{
					OpenGL::setDisplayedTexture("Result");
					break;
				}


			}
			break;
		}


	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

/******************************************
OpenglWindow destructor

Purpose:  Resources are deallocated here

*******************************************/

void KillOpenGLWindow()
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

