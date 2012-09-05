/*--------------------------------------
   CLOCK.C -- Analog Clock Program
              (c) Charles Petzold, 1998
  --------------------------------------*/

#include <windows.h>
#include <math.h>
#include "bass.h"
#include "resource.h"
#include <CommCtrl.h>
#include <sstream>
#include <Shobjidl.h>
#include <fstream>
#pragma comment(lib,"bass.lib")

#if defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define ID_TIMER    1
#define TWOPI       (2 * 3.14159)

TCHAR lac [] = TEXT("Lilybaby's alarm time:"); 
TCHAR at [] = TEXT("12:12");
TCHAR fumin_lovesLily [] = TEXT("Dear Lilybaby~");
TCHAR many_kisses [] = TEXT("Many Kisses to");
HINSTANCE hInst;
SYSTEMTIME alst, alarm_replay_st;
bool Alarm_or_Not = 1, On_Alarm_Replay = 0;
HWND MainWindowhwnd;
void Error(const char *es)
{
	int yy;
	yy = BASS_ErrorGetCode();
	yy = 9;
}
// messaging macros
#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)(w),(LPARAM)(l))
#define STLM(m,w,l) MESS(10,m,w,l)
#define MLM(m,w,l) MESS(20,m,w,l)
#define SLM(m,w,l) MESS(30,m,w,l)
#define GETSTR() STLM(LB_GETCURSEL,0,0)
#define GETMOD() MLM(LB_GETCURSEL,0,0)
#define GETSAM() SLM(LB_GETCURSEL,0,0)
int strc=0; HSTREAM *strs=NULL;
char file[MAX_PATH]="D:\\Documents\\ebooks\\Windows Programming\\Programming Windows - Charles Petzold 5th ed 1998\\Chap08\\Clock\\alarm.mp3";
LPWSTR Wfile;
HSTREAM str;

#define EZ_ATTR_BOLD          1
#define EZ_ATTR_ITALIC        2
#define EZ_ATTR_UNDERLINE     4
#define EZ_ATTR_STRIKEOUT     8
HFONT EzCreateFont (HDC hdc, TCHAR * szFaceName, int iDeciPtHeight,
                    int iDeciPtWidth, int iAttributes, BOOL fLogRes)
{
     FLOAT      cxDpi, cyDpi ;
     HFONT      hFont ;
     LOGFONT    lf ;
     POINT      pt ;
     TEXTMETRIC tm ;
     
     SaveDC (hdc) ;
     
     SetGraphicsMode (hdc, GM_ADVANCED) ;
     ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
     SetViewportOrgEx (hdc, 0, 0, NULL) ;
     SetWindowOrgEx   (hdc, 0, 0, NULL) ;
     
     if (fLogRes)
     {
          cxDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) ;
          cyDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) ;
     }
     else
     {
          cxDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, HORZRES) /
                                        GetDeviceCaps (hdc, HORZSIZE)) ;
          
          cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, VERTRES) /
                                        GetDeviceCaps (hdc, VERTSIZE)) ;
     }
     
     pt.x = (int) (iDeciPtWidth  * cxDpi / 72) ;
     pt.y = (int) (iDeciPtHeight * cyDpi / 72) ;
     
     DPtoLP (hdc, &pt, 1) ;
     
     lf.lfHeight         = - (int) (fabs ((double)pt.y) / 10.0 + 0.5) ;
     lf.lfWidth          = 0 ;
     lf.lfEscapement     = 0 ;
     lf.lfOrientation    = 0 ;
     lf.lfWeight         = iAttributes & EZ_ATTR_BOLD      ? 700 : 0 ;
     lf.lfItalic         = iAttributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
     lf.lfUnderline      = iAttributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
     lf.lfStrikeOut      = iAttributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
     lf.lfCharSet        = DEFAULT_CHARSET ;
     lf.lfOutPrecision   = 0 ;
     lf.lfClipPrecision  = 0 ;
     lf.lfQuality        = 0 ;
     lf.lfPitchAndFamily = 0 ;
     
     lstrcpy (lf.lfFaceName, szFaceName) ;
     
     hFont = CreateFontIndirect (&lf) ;
     
     if (iDeciPtWidth != 0)
     {
          hFont = (HFONT) SelectObject (hdc, hFont) ;
          
          GetTextMetrics (hdc, &tm) ;
          
          DeleteObject (SelectObject (hdc, hFont)) ;
          
          lf.lfWidth = (int) (tm.tmAveCharWidth *
                                        fabs ((double)pt.x) / fabs ((double)pt.y) + 0.5) ;
          
          hFont = CreateFontIndirect (&lf) ;
     }
     
     RestoreDC (hdc, -1) ;
     return hFont ;
}
HRESULT SimpleInvoke(HWND hwnd)
{
    IFileDialog *pfd;
    
    // CoCreate the dialog object.
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
                                  NULL, 
                                  CLSCTX_INPROC_SERVER, 
                                  IID_PPV_ARGS(&pfd));
    
    if (SUCCEEDED(hr))
    {
        // Show the dialog
        hr = pfd->Show(hwnd);
        
        if (SUCCEEDED(hr))
        {
            // Obtain the result of the user's interaction with the dialog.
            IShellItem *psiResult;
            hr = pfd->GetResult(&psiResult);
            
            if (SUCCEEDED(hr))
            {
                // Do something with the result.
				hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &Wfile);
				WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK & WC_NO_BEST_FIT_CHARS, Wfile, -1, file, MAX_PATH, 0, 0);
                psiResult->Release();
            }
        }
        pfd->Release();
    }
    return hr;
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("Lily's Alarm Clock") ;
     HWND         hwnd;
     MSG          msg;
     WNDCLASS     wndclass ;  
	 WIN32_FIND_DATA nouse;

	 Wfile = (LPWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
	 std::string str;
	 std::ifstream ffile ("LilyAlarm.lily");
	 TCHAR lpBuffer[200], config_dat[] = TEXT("\\alarm.mp3");
	 if(ffile.is_open()){
		std::getline(ffile, str);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, at, 5);
		GetLocalTime (&alst) ;
		alst.wHour = atoi(str.c_str());
		alst.wMinute = atoi(str.c_str() + 3);
		alst.wSecond = 0;
		std::getline(ffile, str);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, lpBuffer, 199);
		if(INVALID_HANDLE_VALUE != FindFirstFile(lpBuffer, &nouse))
			WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK & WC_NO_BEST_FIT_CHARS, lpBuffer, -1, file, MAX_PATH, 0, 0);
		else{
			int len = GetModuleFileName(hInstance, lpBuffer, 200);
			TCHAR* p_TCHAR = lpBuffer+len;
			while(p_TCHAR != lpBuffer)
				if(*(p_TCHAR--) == '\\') break;
			memcpy(p_TCHAR + 1, config_dat, sizeof(config_dat));
			if(INVALID_HANDLE_VALUE == FindFirstFile(lpBuffer, &nouse)){
				MessageBox(0, TEXT("Unable to open alarm.mp3"), TEXT("Error"), 0);
				return -1;
			}
		}
	 }
	 else{
		 MessageBox(0, TEXT("Can't open LilyAlarm.lily"), TEXT("Error"), 0);
		 return -1;
	 } 

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1)) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = TEXT("LilyAlarmMainMenu") ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
	 hwnd = CreateWindow (szAppName, TEXT ("Analog Clock"),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
	 char tempcharbuff[6];
	 ZeroMemory(tempcharbuff, 6);
	 WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK & WC_NO_BEST_FIT_CHARS, at, -1, tempcharbuff, MAX_PATH, 0, 0);
	 std::ofstream ofile ("LilyAlarm.lily");
	 ofile << std::string(tempcharbuff);
	 ofile << std::string("\n");
	 ofile << file;
	 ofile.close();
	 BASS_Free();
     return msg.wParam ;
}

void SetIsotropic (HDC hdc, int cxClient, int cyClient)
{
     SetMapMode (hdc, MM_ISOTROPIC) ;
     SetWindowExtEx (hdc, 1000, 1000, NULL) ;
     SetViewportExtEx (hdc, cxClient / 2, -cyClient / 2, NULL) ;
     SetViewportOrgEx (hdc, cxClient / 2,  cyClient / 2, NULL) ;
}

void RotatePoint (POINT pt[], int iNum, int iAngle)
{
     int   i ;
     POINT ptTemp ;
     
     for (i = 0 ; i < iNum ; i++)
     {
          ptTemp.x = (int) (pt[i].x * cos (TWOPI * iAngle / 360) +
               pt[i].y * sin (TWOPI * iAngle / 360)) ;
          
          ptTemp.y = (int) (pt[i].y * cos (TWOPI * iAngle / 360) -
               pt[i].x * sin (TWOPI * iAngle / 360)) ;
          
          pt[i] = ptTemp ;
     }
}

void DrawClock (HDC hdc)
{
     int   iAngle ;
     POINT pt[3] ;
     
     for (iAngle = 0 ; iAngle < 360 ; iAngle += 6)
     {
          pt[0].x =   0 ;
          pt[0].y = 900 ;
          
          RotatePoint (pt, 1, iAngle) ;
          
          pt[2].x = pt[2].y = iAngle % 5 ? 33 : 100 ;
          
          pt[0].x -= pt[2].x / 2 ;
          pt[0].y -= pt[2].y / 2 ;
          
          pt[1].x  = pt[0].x + pt[2].x ;
          pt[1].y  = pt[0].y + pt[2].y ;
          
          SelectObject (hdc, GetStockObject (BLACK_BRUSH)) ;
          
          Ellipse (hdc, pt[0].x, pt[0].y, pt[1].x, pt[1].y) ;
     }
}

void DrawHands (HDC hdc, SYSTEMTIME * pst, BOOL fChange)
{
     static POINT pt[3][5] = { 0, -150, 100, 0, 0, 600, -100, 0, 0, -150,
                               0, -200,  50, 0, 0, 800,  -50, 0, 0, -200,
                               0,    0,   0, 0, 0,   0,    0, 0, 0,  800 } ;
     int          i, iAngle[3] ;
     POINT        ptTemp[3][5] ;
     
     iAngle[0] = (pst->wHour * 30) % 360 + pst->wMinute / 2 ;
     iAngle[1] =  pst->wMinute  *  6 ;
     iAngle[2] =  pst->wSecond  *  6 ;
     
     memcpy (ptTemp, pt, sizeof (pt)) ;
     
     for (i = fChange ? 0 : 2 ; i < 3 ; i++)
     {
          RotatePoint (ptTemp[i], 5, iAngle[i]) ;
          
          Polyline (hdc, ptTemp[i], 5) ;
     }
}

void PaintRoutine (HWND hwnd, HDC hdc, int cxArea, int cyArea, const LPTSTR szString, double d, double iii)
{
	 //static TCHAR szString [] = TEXT("");
     HFONT        hFont ;
     SIZE         size ;

     hFont = EzCreateFont (hdc, TEXT ("Times New Roman"), (int)(cyArea*d), 0, 0, TRUE) ;

     SelectObject (hdc, hFont) ;
     SetBkMode (hdc, TRANSPARENT) ;

     GetTextExtentPoint32 (hdc, szString, lstrlen (szString), &size) ;

     BeginPath (hdc) ;
     TextOut (hdc, (cxArea - size.cx) / 2, (int)((cyArea * iii - size.cy) / 2),
                    szString, lstrlen (szString)) ;
     EndPath (hdc) ;

     SelectObject (hdc, CreateHatchBrush (HS_DIAGCROSS, RGB (255, 0, 0))) ;
     SetBkColor (hdc, RGB (0, 0, 255)) ;
     SetBkMode (hdc, OPAQUE) ;

     StrokeAndFillPath (hdc) ;

     DeleteObject (SelectObject (hdc, GetStockObject (WHITE_BRUSH))) ;
     SelectObject (hdc, GetStockObject (SYSTEM_FONT)) ;
     DeleteObject (hFont) ;
}

void Ran_PaintRoutine (HWND hwnd, HDC hdc, int cxArea, int cyArea, const LPTSTR szString)
{
	 //static TCHAR szString [] = TEXT("");
     HFONT        hFont ;
     SIZE         size ;

     hFont = EzCreateFont (hdc, TEXT ("Times New Roman"), (int)(cyArea*((rand()%20)/10 + 0.7)), 0, 0, TRUE) ;

     SelectObject (hdc, hFont) ;
     SetBkMode (hdc, TRANSPARENT) ;

     GetTextExtentPoint32 (hdc, szString, lstrlen (szString), &size) ;

     BeginPath (hdc) ;
     TextOut (hdc, (int)((cxArea * ((rand()%250)/100+0.4)- size.cx) / 2), (int)((cyArea * ((rand()%250)/100+0.4) - size.cy) / 2),
                    szString, lstrlen (szString)) ;
     EndPath (hdc) ;

     SelectObject (hdc, CreateHatchBrush (HS_DIAGCROSS, RGB (rand()%255, rand()%255, rand()%255))) ;
     SetBkColor (hdc, RGB (rand()%255, rand()%255, rand()%255)) ;
     SetBkMode (hdc, OPAQUE) ;

     StrokeAndFillPath (hdc) ;

     DeleteObject (SelectObject (hdc, GetStockObject (WHITE_BRUSH))) ;
     SelectObject (hdc, GetStockObject (SYSTEM_FONT)) ;
     DeleteObject (hFont) ;
}

void PaintRoutinef (HWND hwnd, HDC hdc, int cxArea, int cyArea){
	PaintRoutine (hwnd, hdc, cxArea, cyArea, lac, 0.65, 1.2);
	PaintRoutine (hwnd, hdc, cxArea, cyArea, at, 0.9, 1.5);
}

void WINAPI DoDateTimeChange(LPNMDATETIMECHANGE lpChange)
{
    if(!(lpChange->dwFlags == GDT_NONE)){
		alst = lpChange->st;
		std::stringstream TimeStream;
		if (alst.wHour > 9)
			TimeStream << alst.wHour;
		else{
			TimeStream << 0;
			TimeStream << alst.wHour;
		}
		TimeStream << ':';
		if(alst.wMinute > 9)
			TimeStream << alst.wMinute;
		else{
			TimeStream << 0;
			TimeStream << alst.wMinute;
		}
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, TimeStream.str().c_str(), -1, at, 5);
		InvalidateRect (MainWindowhwnd, NULL, TRUE) ;
	}
}

BOOL WINAPI LilyAlarmDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND aaaa; int ghj;
	static LPNMHDR hdr;
	switch(message){
	case WM_INITDIALOG:
		if(Alarm_or_Not)
			SendMessage( GetDlgItem (hwnd, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage( GetDlgItem (hwnd, IDC_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0);
		ghj=SendMessage(GetDlgItem(hwnd, IDC_DATETIMEPICKER2), DTM_SETSYSTEMTIME , GDT_VALID, (LPARAM)&alst);
		PostMessage (hwnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem (hwnd, IDC_CHECK1), TRUE);
		break;
	case WM_NOTIFY:
		hdr = (LPNMHDR)lParam;
		switch(hdr->code){
		case DTN_DATETIMECHANGE:
			 LPNMDATETIMECHANGE lpChange = (LPNMDATETIMECHANGE)lParam;
             DoDateTimeChange(lpChange);
			 break;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_CHECK1:
			Alarm_or_Not = !Alarm_or_Not;
			InvalidateRect (MainWindowhwnd, NULL, TRUE) ;
			break;
		case IDCANCEL:
            EndDialog (hwnd, FALSE) ;
            return TRUE ;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     static int        cxClient, cyClient ;
     static SYSTEMTIME stPrevious ;
     BOOL              fChange ;
     HDC               hdc ;
     PAINTSTRUCT       ps ;
     SYSTEMTIME        st ;
	 MainWindowhwnd = hwnd;
	 static QWORD music_len;
	 static double music_time;
	 static int dy, hr, mi, se;

     switch (message)
     {
	 case WM_COMMAND:
		  switch (LOWORD (wParam))
		  {
		  case 65535:
			   SimpleInvoke(hwnd);
               return 0 ;
		  case 65534:
               DialogBox (hInst, TEXT ("LilyAlarm"), hwnd, LilyAlarmDlgProc);
               return 0 ;
		  }
     case WM_CREATE :
          SetTimer (hwnd, ID_TIMER, 1000, NULL) ;
          GetLocalTime (&st) ;
          stPrevious = st ;
		  hInst = ((LPCREATESTRUCT) lParam)->hInstance ;
          return 0 ;
          
     case WM_SIZE :
          cxClient = LOWORD (lParam) ;
          cyClient = HIWORD (lParam) ;
          return 0 ;
          
     case WM_TIMER :
          GetLocalTime (&st) ;
                    
          fChange = st.wHour   != stPrevious.wHour ||
                    st.wMinute != stPrevious.wMinute ;
          
          hdc = GetDC (hwnd) ;
          
          SetIsotropic (hdc, cxClient, cyClient) ;
          
          SelectObject (hdc, GetStockObject (WHITE_PEN)) ;
          DrawHands (hdc, &stPrevious, fChange) ;
          
          SelectObject (hdc, GetStockObject (BLACK_PEN)) ;
          DrawHands (hdc, &st, TRUE) ;

		  if(On_Alarm_Replay)
			  InvalidateRect (MainWindowhwnd, NULL, TRUE) ;
		
		  ReleaseDC (hwnd, hdc) ;

		  stPrevious = st ;
		  
		  if(Alarm_or_Not){
			  hdc = GetDC (hwnd) ;
			  PaintRoutinef (hwnd, hdc, cxClient, cyClient) ;
              ReleaseDC (hwnd, hdc) ;
              
			  if((st.wHour == alst.wHour && st.wMinute == alst.wMinute && st.wSecond == alst.wSecond)
				  || (On_Alarm_Replay == 1 && st.wHour == alarm_replay_st.wHour && st.wMinute == alarm_replay_st.wMinute && st.wSecond == alarm_replay_st.wSecond)){
				  if (!BASS_Init(-1,44100,0,0,NULL))
				      Error("Can't initialize device");
		    	  if (str=BASS_StreamCreateFile(FALSE,file,0,0,0)) {
				      strc = 1;//strc++;
				      //strs=(HSTREAM*)realloc((void*)strs,strc*sizeof(*strs));
					  strs=(HSTREAM*)malloc(sizeof(str));
				      strs[strc-1]=str;
		    	  } else
				      Error("Can't open stream");
				  music_len=BASS_ChannelGetLength(strs[0], BASS_POS_BYTE); // the length in bytes
				  music_time=BASS_ChannelBytes2Seconds(strs[0], music_len); // the length in seconds
				  music_time += 5;
				  mi = (int)((st.wSecond + music_time) / 60);
				  alarm_replay_st.wSecond = (WORD)((st.wSecond + music_time) - mi*60);
				  hr = (int)((st.wMinute + mi) / 60);
				  alarm_replay_st.wMinute = st.wMinute + mi - hr*60;
				  dy = (int)((st.wHour + hr)/24);
				  alarm_replay_st.wHour = st.wHour + hr - dy;
				  On_Alarm_Replay = 1;
			      BASS_ChannelPlay(strs[strc-1],TRUE); // play the stream from the start
		      }
		  }
          return 0 ;
          
     case WM_PAINT :
          hdc = BeginPaint (hwnd, &ps) ;

		  if(lParam == 1)
			  lParam = 8;

		  if(Alarm_or_Not)
			  PaintRoutinef (hwnd, hdc, cxClient, cyClient) ;
		  if(On_Alarm_Replay){
			  Ran_PaintRoutine (hwnd, hdc, cxClient, cyClient, fumin_lovesLily);
			  Ran_PaintRoutine (hwnd, hdc, cxClient, cyClient, many_kisses);
			  Ran_PaintRoutine (hwnd, hdc, cxClient, cyClient, fumin_lovesLily);
			  Ran_PaintRoutine (hwnd, hdc, cxClient, cyClient, many_kisses);
			  GetLocalTime (&st);
			  if(st.wSecond % 2 == 0){
				  SetWindowText(MainWindowhwnd, TEXT("Lily Alarm Clock"));
			  }
			  else
				  SetWindowText(MainWindowhwnd, TEXT("Many Kisses to Dear Lilybaby~"));
		  }

          SetIsotropic (hdc, cxClient, cyClient) ;
          DrawClock    (hdc) ;
          DrawHands    (hdc, &stPrevious, TRUE) ;

		  EndPaint (hwnd, &ps) ;
          return 0 ;
          
     case WM_DESTROY :
		  KillTimer (hwnd, ID_TIMER) ;
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}
