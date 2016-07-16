#include <stdio.h>
#include <tchar.h>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma pack(2)

class MyPlayer
{
private:
	HINSTANCE hInstance;
	HWND hwnd;
	static bool bplaying;
	unsigned long rate,out_buffer_size;
	int channel,precision;
	double iFreq;
	static HWAVEOUT hWaveOut;
	WAVEFORMATEX waveformat;
	static PWAVEHDR pWaveHdr;
	PBYTE pBuffer;
	short * pwBuffer;
	struct WaveFileHead
	{
		char type[4];//4
		DWORD filesize;//4
		char WAVEfmt[8];//8
		DWORD formatsize;//4
		WORD wFormatTag;//2
		WORD nChannels;//2
		DWORD nSamplesPerSec;//4
		DWORD nAvgBytesPerSec;//4
		WORD nBlockAlign;//2
		DWORD wBitsPerSample;//4
		char data[4];//4
		DWORD size;//4
	} wavefilehead;//44

	void FillWaveFileHead();

	static LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:

			return 0;
		case MM_WOM_OPEN:
			{
				//MessageBox(0,TEXT(""),TEXT(""),0);
				mywaveOutWrite();
				return 0;
			}
		case MM_WOM_DONE:
			return 0;
		case MM_WOM_CLOSE:
			return 0 ;
		case WM_PAINT:
			HDC hdc;
			PAINTSTRUCT ps;
			RECT rect;
			hdc = BeginPaint(hwnd, &ps);
			GetClientRect (hwnd, &rect);
			DrawText (hdc, TEXT("draw"), -1, &rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_QUIT:
			return 0;
		}
		return DefWindowProc (hwnd, message, wParam, lParam);
	}
public:
	MyPlayer(HINSTANCE hInst);
	~MyPlayer(void);
	void Set(unsigned long irate,int ichannel,int iprecision,double len);
	void PreparePlay();

	void SaveWaveFile(char *filename);

	void FillSquare(double iFreq);
	void FillSin(double iFreq);

	void SetVolume(DWORD volume);
	static void mywaveOutWrite();
	bool playing();
	PBYTE GetpBuffer();
	short * GetpwBuffer();
	void pause();
	void restart();
};

bool MyPlayer::bplaying=false;
HWAVEOUT MyPlayer::hWaveOut=NULL;
PWAVEHDR MyPlayer::pWaveHdr=NULL;

	MyPlayer::MyPlayer(HINSTANCE hInst)
	{
		hInstance=hInst;
		static TCHAR szAppName[]=TEXT("MyPlayer");
		WNDCLASS wndclass ;
		wndclass.style        = CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc  = WndProc ;
		wndclass.cbClsExtra   = 0 ;
		wndclass.cbWndExtra   = 0 ;
		wndclass.hInstance    = hInstance;
		wndclass.hIcon        = LoadIcon (NULL, IDI_APPLICATION);
		wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground= (HBRUSH) GetStockObject (20);
		wndclass.lpszMenuName  = NULL ;
		wndclass.lpszClassName= szAppName ;

		if (!RegisterClass (&wndclass))
		{
			MessageBox (  NULL, TEXT ("Register Class Fail!"),szAppName, MB_ICONERROR) ;
		}
		hwnd = CreateWindow(szAppName,      // window class name
			TEXT ("The Hello Program"),   // window caption
			WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
			CW_USEDEFAULT,// initial x position
			CW_USEDEFAULT,// initial y position
			CW_USEDEFAULT,// initial x size
			CW_USEDEFAULT,// initial y size
			NULL,                 // parent window handle
			NULL,            // window menu handle
			hInstance,   // program instance handle
			NULL) ;      // creation parameters
	}
	MyPlayer::~MyPlayer(void){
		//waveOutReset (hWaveOut) ;
		if (hWaveOut)
		{
			free(pWaveHdr);
			free(pBuffer);
			hWaveOut = NULL;
		}
	}
	void MyPlayer::Set(unsigned long irate,int ichannel,int iprecision,double len)
	{
		channel=ichannel;
		iFreq=440;
		out_buffer_size=(DWORD)(irate*len);
		precision=iprecision;
		rate=irate;
	}
	void MyPlayer::PreparePlay()
	{
		pWaveHdr=(PWAVEHDR)malloc(sizeof(WAVEHDR));
		if (precision==8)
			pBuffer=(PBYTE)malloc(out_buffer_size);
		else
			pwBuffer=(short *)malloc(out_buffer_size*sizeof(short));
		int err=0;
		if (!pWaveHdr)
		{free(pWaveHdr);err=1;}
		if (precision==8)
		{
			if (!pBuffer)
			{free(pBuffer);err=1;} 
			else ;
		}
		else
		{
			if (!pwBuffer)
			{free(pwBuffer);err=1;}
		}
		if (err)
		{
			MessageBeep (MB_ICONEXCLAMATION);
			MessageBox (hwnd, TEXT ("Error allocating memory!"),TEXT(""), MB_ICONEXCLAMATION | MB_OK);
		}

		waveformat.wFormatTag      = WAVE_FORMAT_PCM;
		waveformat.nChannels       = channel;
		waveformat.nSamplesPerSec  = rate;
		waveformat.nAvgBytesPerSec = channel*rate*precision/8;
		waveformat.nBlockAlign     = channel*precision/8;
		waveformat.wBitsPerSample  = channel*precision;
		waveformat.cbSize          = 0;


		pWaveHdr->lpData          = (precision==8?(LPSTR)pBuffer:(LPSTR)pwBuffer);
		pWaveHdr->dwBufferLength  = out_buffer_size*precision/8;
		pWaveHdr->dwBytesRecorded = 0 ;
		pWaveHdr->dwUser          = 0 ;
		pWaveHdr->dwFlags         = 0 ;
		pWaveHdr->dwLoops         = 1;
		pWaveHdr->lpNext          = NULL ;
		pWaveHdr->reserved        = 0 ;

		if (waveOutOpen (&hWaveOut,WAVE_MAPPER,&waveformat,(DWORD)hwnd,0,CALLBACK_WINDOW)!= MMSYSERR_NOERROR)
		{
			free(pWaveHdr);
			free(pBuffer);

			hWaveOut = NULL ;
			MessageBox(hwnd,TEXT("Error opening waveform audio device!"),TEXT(""),MB_ICONEXCLAMATION | MB_OK) ;
		}
		else
		{
			waveOutPrepareHeader (hWaveOut, pWaveHdr,sizeof (WAVEHDR));
		}
	}
	void MyPlayer::SaveWaveFile(char *filename)
	{
		FILE *file;
		if (_tfopen_s(&file,filename,TEXT("w+,ccs=UNICODE"))==0)//成功则为0
		{
			FillWaveFileHead();
			fwrite(&wavefilehead,46,1,file);
			if (precision==8)
				fwrite(pBuffer,out_buffer_size,1,file);//sizeof(out_buffer_size*sizeof(WORD))
			else
				fwrite(pwBuffer,out_buffer_size*sizeof(WORD),1,file);
			_fcloseall();
			MessageBox(NULL,TEXT("Success to save the file"),TEXT(""),0);
		}
		else
			MessageBox(NULL,TEXT("Fail to open this file"),TEXT(""),0);
	}
	void MyPlayer::FillSquare(double iFreq)
	{
		unsigned int i ;

		int count=0,od=1;

		if (precision/8==1)
			for (i = 0 ; i < out_buffer_size; i++)
			{
				if (od==1)
					pBuffer[i]=255;
				else
					pBuffer[i]=0;

				if (count<rate/iFreq)
					count++;
				else
				{
					od*=-1;
					count=0;
				}
			}
		else
			for (i = 0 ; i < out_buffer_size; i++)
			{
				if (od==1)
					pwBuffer[i]=32767;
				else
					pwBuffer[i]=-32767;

				if (count<rate/iFreq)
					count++;
				else
				{
					od*=-1;
					count=0;
				}
			}
	}

	void MyPlayer::FillSin(double iFreq)
	{
		static double fAngle;
		unsigned int i;
		
		fAngle=0;
		if (precision/8==1)
			for (i=0;i<out_buffer_size;i++)
			{
				pBuffer[i]=(BYTE)(127+127*sin(fAngle));
				fAngle+=2*M_PI*iFreq/rate;

				if (fAngle>=2*M_PI)
					fAngle=0;
			}
		else
			for (i=0;i<out_buffer_size;i++)
			{
				pwBuffer[i]=0x7fff*sin(fAngle);

				fAngle+=2*M_PI*iFreq/rate;

				if (fAngle>2*M_PI)
					fAngle=0;
			}
	}
void MyPlayer::FillWaveFileHead()
	{
		strcpy(wavefilehead.type,"RIFF");
		wavefilehead.filesize=46-sizeof(DWORD)+out_buffer_size*precision/8;
		strcpy(wavefilehead.WAVEfmt,"WAVEfmt ");
		wavefilehead.formatsize=0x12;
		wavefilehead.wFormatTag=WAVE_FORMAT_PCM;
		wavefilehead.nChannels=channel;
		wavefilehead.nSamplesPerSec=rate;
		wavefilehead.nAvgBytesPerSec=channel*rate*precision/8;
		wavefilehead.nBlockAlign=channel*precision/8;
		wavefilehead.wBitsPerSample=channel*precision;
		strcpy(wavefilehead.data,"data");
		wavefilehead.size=out_buffer_size*channel*precision/8;
	}

	void MyPlayer::SetVolume(DWORD volume)
	{
		waveOutSetVolume(hWaveOut,volume);
	}

	void MyPlayer::mywaveOutWrite(){
		waveOutWrite(hWaveOut,pWaveHdr,sizeof(WAVEHDR));
		bplaying=true;
	}

	bool MyPlayer::playing()
	{
		return bplaying;
	}

	PBYTE MyPlayer::GetpBuffer()
	{
		return pBuffer;
	}

	short * MyPlayer::GetpwBuffer()
	{
		return pwBuffer;
	}

	void MyPlayer::pause()
	{
		waveOutPause(hWaveOut);
		bplaying=false;
	}

	void MyPlayer::restart()
	{
		waveOutRestart(hWaveOut);
		bplaying=true;
	}