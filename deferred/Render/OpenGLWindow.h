#ifndef _OPENGLWINDOW_H_
#define _OPENGLWINDOW_H_

#include <windows.h>

void InitOpenGLWindow(const char* title, const int width, const int height, const int bits, const bool fullscreenflag);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void KillOpenGLWindow();
void RunOpenGLWindow(void);

#endif