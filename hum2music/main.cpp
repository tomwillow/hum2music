#include <windows.h>
#include "resource.h"
#include "MyPlayer.h"
#include "MyDraw.h"

HINSTANCE hInst;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	hInst=hInstance;
	if (DialogBox (hInstance,TEXT ("Aboutdialog"), NULL, AboutDlgProc)==-1)
		OutputDebugString("Fail");
}


BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	static MyPlayer player(hInst);
	static MyDraw draw;
	switch (message)
	{
	case WM_INITDIALOG:
		player.Set(44100,1,16,1);
		player.PreparePlay();
		player.SetVolume(0xffff);
		return TRUE;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDPLAY:
			if (player.playing())
				player.pause();
			else
				player.restart();
			break;
		case ID2:
			{
				player.FillSin(50);
				player.mywaveOutWrite();
				draw.SetSize(600,100);
				draw.DrawPolyline(hDlg,16,player.GetpBuffer(),player.GetpwBuffer());
			//player.draw();
			break;
			}
		case ID3:
			player.SaveWaveFile(TEXT("a.wav"));
			break;
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg,0);
		return TRUE;
	}
	return FALSE;
}