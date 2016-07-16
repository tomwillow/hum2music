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
				player.Set(44100,1,8,1);
				player.SetVolume(0xffff);
				player.FillSin(5000);
				//player.draw();
				break;
			}
		case ID3:
			player.SaveWaveFile(TEXT("a.wav"));
			break;
		case ID4:
			player.ReadWaveFile(TEXT("a.wav"));
			break;
		case ID5:
			draw.SetSize(600,100);
			draw.DrawPolyline(hDlg,8,player.GetpBuffer(),player.GetpwBuffer());
			break;
		case ID6:
			
				player.PreparePlay();
				player.mywaveOutWrite();
				break;
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg,0);
		return TRUE;
	}
	return FALSE;
}