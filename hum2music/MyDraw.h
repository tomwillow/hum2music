#include <Windows.h>
class MyDraw
{
private:
	int width,height;
public:
	void SetSize(int width,int height)
	{
		MyDraw::width=width;
		MyDraw::height=height;
	}

	int DrawPolyline(HWND hwnd,int num_bit,PBYTE pBuffer,short * pwBuffer){
		if ((num_bit==8 && pBuffer==NULL)||(num_bit==16 && pwBuffer==NULL))
		{
			MessageBox(NULL,TEXT("no initialise"),TEXT(""),0);
			return 1;
		}
		HDC hdc;
		POINT *apt;
		apt=(POINT *)malloc(width*sizeof(POINT));
		if (num_bit==8)
			for (int i=0;i<width;i++)
			{
				apt[i].x=i;
				apt[i].y=(-pBuffer[i])*(height/(double)0xff)+height/2;
			}
		else
			for (int i=0;i<width;i++)
			{
				apt[i].x=i;
				apt[i].y=(-pwBuffer[i])*(height/(double)0xffff)+height/2;
			}
			hdc=GetDC(hwnd);
			Polyline(hdc,apt,width);
			ReleaseDC(hwnd,hdc);
			free(apt);
		return 0;
	}
};