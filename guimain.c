#include "stdio.h"
#include <conio.h>
#include <windows.h>
#include <windef.h>
#include <winuser.h>
#include <commctrl.h>   // includes the common control header
#include <mmsystem.h>

#include "decode.h"
#include "resource.h"

#define MAX_LOADSTRING 100




#define	IfCheckIt( id, check )	SendMessage( GetDlgItem( hWnd, id),  BM_GETCHECK, check, 0)
#define	IfCheckON( id )			SendMessage( GetDlgItem( hWnd, id),  BM_GETCHECK, 1, 0)
#define	IfCheckOFF( id )		SendMessage( GetDlgItem( hWnd, id),  BM_GETCHECK, 0, 0)
#define	CheckIt( id, check )	SendMessage( GetDlgItem( hWnd, id),  BM_SETCHECK, check, 0)
#define	CheckON( id )			SendMessage( GetDlgItem( hWnd, id),  BM_SETCHECK, 1, 0)
#define	CheckOFF( id )			SendMessage( GetDlgItem( hWnd, id),  BM_SETCHECK, 0, 0)
#define	Enable( id )			EnableWindow( GetDlgItem( hWnd, id),  TRUE )
#define	Disable( id )			EnableWindow( GetDlgItem( hWnd, id),  FALSE )
#define	SetEnable( id, x )		EnableWindow( GetDlgItem( hWnd, id),  x )
#define	Hide( id )				Disable(id);ShowWindow ( GetDlgItem( hWnd, id), SW_HIDE)
#define	Show( id )				Enable( id );ShowWindow ( GetDlgItem( hWnd, id), SW_SHOW)
#define	SetText( id, text )		SetDlgItemText( hWnd, id, text )
#define	GetText( id,t,n)		GetDlgItemText( hWnd, id, t , n );
#define SetPopupNum( id, val )	SendMessage( GetDlgItem(hWnd, id), CB_SETCURSEL , val , 0 )
#define GetPopupNum( id )		SendMessage( GetDlgItem(hWnd, id), CB_GETCURSEL , 0, 0 )
#define GetPopupTot( id )		SendMessage( GetDlgItem(hWnd, id), CB_GETCOUNT , 0, 0 )
#define GetPopupText( id, n, ptr )		SendMessage( GetDlgItem(hWnd, id), CB_GETLBTEXT , n, (LPARAM)ptr )

#define	AddPopupItem( id, string ) SendMessage( GetDlgItem(hWnd, id), CB_ADDSTRING, 0, (LPARAM) string );
#define	SetSliderPos( id,pos )	SendMessage( GetDlgItem(hWnd, id ), TBM_SETPOS, TRUE, pos ); \
								UpdateWindow( GetDlgItem(hWnd, id ) )
#define GetSliderPos( id )		SendMessage( GetDlgItem(hWnd, id ), TBM_GETPOS, 0, 0 )
#define	SetSliderRange( id, x ) SendMessage( GetDlgItem(hWnd, id ),TBM_SETRANGE , 1 , MAKELONG( 0, x ) )
#define	SetFont( id, f )		SendMessage( GetDlgItem(hWnd, id), WM_SETFONT, (UINT)(f), TRUE)



#define	ListAddString( id, string )		SendMessage( GetDlgItem(hWnd, id), LB_ADDSTRING, 0, (LPARAM)string)
#define	ListDelString( id, index )		SendMessage( GetDlgItem(hWnd, id), LB_DELETESTRING, (WPARAM) index, 0)
#define	ListDelFirst( id )				SendMessage( GetDlgItem(hWnd, id), LB_DELETESTRING, (WPARAM) 0, 0)
#define	ListGetSelected( id )			SendMessage( GetDlgItem(hWnd, id), LB_GETCURSEL, 0, 0)
#define	ListGetTotal( id )				SendMessage( GetDlgItem(hWnd, id), LB_GETCOUNT, 0, 0)
#define	ListGetText( id,index, string )	SendMessage( GetDlgItem(hWnd, id), LB_GETTEXT, (WPARAM)index, (LPARAM)string)


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HWND				gmainwindow, gDialog;


#define	SHOWSTATUS(txt)	SendMessage( gDialog, WM_USER+1, 0, (LPARAM) txt ); 


int		climode = 0;

void ShowControls( HWND hWnd, int on )
{
	SetEnable( IDC_MODE1, on );
	SetEnable( IDC_MODE2, on );
	SetEnable( IDC_CODEC, on );
	SetEnable( IDC_44100, on );
//	SetEnable( IDC_GAIN1, on );
//	SetEnable( IDC_GAIN2, on );
}

void ShowHTMLURL( HWND hWnd, char *doc )
{
	char *oper = "open";
	char file[320];
	char *dir = "c:\\";

	if ( !strstr( doc, "http://" ) )
		sprintf( file, "http://%s", doc );
	else
		sprintf( file, "%s", doc );
	ShellExecute( hWnd, oper,file,"",dir, SW_SHOWNORMAL  );
}

int ShowStatus( char *txt, ... )
{
	va_list		args;
static	char lineout[4000];

	if ( txt ){
		va_start( args, txt );
		vsprintf( lineout, txt, args );
		va_end( args );
		if ( climode )
			printf( lineout );
		else {
			OutputDebugString( lineout );
			OutputDebugString( "\n" );
			if ( lineout[0] == '\n' || lineout[0] == '\r' )
				SetDlgItemText( gDialog, IDC_STATUS, lineout+1 );
			else
				SetDlgItemText( gDialog, IDC_STATUS, lineout );
			//SendMessage( gDialog, SB_SETTEXT, (WPARAM) 0, (LPARAM) lineout ); 
		}
	}
	return 1;
}

void ErrorBox( char *txt )
{
	MessageBox(NULL, "AC3 ERROR", txt, MB_OK | MB_ICONSTOP);
}



int MsgBox_yesno( long txt, ... )
{
	char	msg[2048], *pt;
	long	ret, len;
	va_list		args;

	va_start( args, txt );
	vsprintf( msg, txt, args );
	ret = MessageBox( GetFocus(), "AC3", msg, MB_YESNO|MB_ICONQUESTION );
	va_end( args );

	return ret;
}



int HandleArgs( char *params )
{
	char	*argv[256] , *p;
	int 	argc;

	p = params;
	argc=0;
	memset( argv, 0, 255*sizeof(void*) );
	argv[ argc++ ] = params;
	while( (p=strtok( p, " " )) && (argc<256) ){
		argv[ argc++ ] = p;
		p=NULL;
	}
	return handle_args( argc, argv );
}


#include "fcntl.h"
void initcrt(void)
{
	int hCrt,i;
   FILE *hf;

   AllocConsole();
   hCrt = _open_osfhandle(
             (long) GetStdHandle(STD_OUTPUT_HANDLE),
             _O_TEXT
          );
   hf = _fdopen( hCrt, "w" );
   *stdout = *hf;
   i = setvbuf( stdout, NULL, _IONBF, 0 );
   //stderr = stdout;
}



int guimain( HINSTANCE hInstance, int nCmdShow )
{
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_AC3DEC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (InitInstance (hInstance, nCmdShow)) 
	{
		MSG msg;
		HACCEL hAccelTable;

		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_AC3DEC);

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return msg.wParam;
	}
	return 1;
}

#ifdef	_WINDOWS
int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	char	cline[4000];
	char	*argv[256] , *p;
	int 	argc;
	int		ret;

	strcpy( cline, lpCmdLine );
	p = cline;
	argc=0;
	memset( argv, 0, 255*sizeof(void*) );
	argv[ argc++ ] = cline;
	while( (p=strtok( p, " " )) && (argc<256) ){
		argv[ argc++ ] = p;
		p=NULL;
	}
	argv[ argc ] = NULL;
	if ( argc>1 )
		climode = 1;
#else
int main( int argc, char *argv[] )
{
	int nCmdShow = 0;
	int		ret;
	HINSTANCE hInstance = GetModuleHandle (NULL);
	climode=1;
#endif
	if ( !climode )
	{
		HWND w;
		char name[256];
		if( w = GetForegroundWindow() ){
			GetWindowText( w , name, 255 );
			if ( strstr( name, "Command Prompt" ) || strstr( name, "MS-DOS Prompt" )  ){
				climode = 1;
			}
		}
	}
	if ( climode ){
#ifdef	_WINDOWS
		initcrt();
#endif
		ret = ac3main( argc, argv );
		printf( "\n\nPress a key..." ); getch();
	} else
		ret = guimain( hInstance, nCmdShow );

	return ret;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_AC3);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_AC3DEC;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}



//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	//hWnd = DialogBox(hInst, (LPCTSTR)IDD_MAIN, 0, (DLGPROC)WndProc);
	//hWnd = CreateDialog(hInst, (LPCSTR)MAKEINTRESOURCE(IDD_MAIN), 0, (DLGPROC) WndProc);

	gmainwindow = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 450, NULL, NULL, hInstance, NULL);

	if (gmainwindow)
	{
		ShowWindow(gmainwindow, nCmdShow);
		UpdateWindow(gmainwindow);
		return TRUE;
	} else
		return FALSE;
}



















static int converting = 0;
static FILE *thread_fp;
static char *thread_newfilename;


void GoConvert( HWND hWnd )
{
	if ( IfCheckON( IDC_MODE1 ) )
		HandleArgs( "-pcmwav" );
	else
		HandleArgs( "-listen" );

	HandleArgs( "-overwrite" );

	Enable( IDC_STOP );
	ShowControls( hWnd, 0 );

	if ( !converting )
		convert_now_threaded( hWnd );
}


void ClearQueue( HWND hWnd )
{
	while( ListGetTotal( SOURCELIST ) ) {
		ListDelFirst( SOURCELIST );
	}
}


void EndConvert( HWND hWnd )
{
	StopNow();
	ClearQueue( hWnd );
	ShowControls( hWnd, 1 );
	Disable( IDC_STOP );
	SetWindowText( gmainwindow, "Ac3Dec" );
}


DWORD WINAPI ThreadDoAc3Convert( PVOID lpParam )
{
	int i=0;
	char srcname[256], tmp[256], *filename;
	HWND hWnd = lpParam, hDlg = lpParam;

	converting = 1;

	while( ListGetTotal( SOURCELIST ) ) {
		FILE *fp;
		ListGetText( SOURCELIST, 0, srcname );
		filename = srcname;
		if ( fp = fopen( filename,"rb") ){
			char newname[512], *p;
			strcpy( newname, filename );
			p = strrchr( newname, '.' );
			if ( p ){
				strcpy( p, ".wav" );

				sprintf( tmp , "Converting %s to %s...", filename, newname );
				ShowStatus( tmp );
				SetWindowText( gmainwindow, tmp );
				convert_now( fp, filename, newname );
				ShowStatus( "Done." );
				ListDelFirst( SOURCELIST );
			}
		}
		i++;
	}	
	ShowStatus( "All Done." );
	EndConvert( hWnd );
	converting = 0;
	return 0;
}







HANDLE 		ghThread = NULL;

long convert_now_threaded( HWND hWnd )
{
	DWORD dwThreadId;

	if ( ghThread )
		CloseHandle( ghThread );

	ghThread = CreateThread( 
		NULL,                        // no security attributes 
		0,                           // use default stack size  
		ThreadDoAc3Convert,		  // thread function 
		hWnd,					   // argument to thread function 
		0,                           // use default creation flags 
		&dwThreadId);                // returns the thread identifier  
    
	if (ghThread) {
		SetThreadPriority( ghThread, THREAD_PRIORITY_BELOW_NORMAL );
	}
	return(1);
}






BOOL GetLoadDialog( char *putNamehere, char *initDir, char *initfile, char *filter, char *title )
{
    char            szFile[256], szFileTitle[256], dir[256], path[256], *cptr;
    BOOL			bSuccess;
	OPENFILENAME    gFileNameStruct;

    bSuccess = TRUE;

	memset( &gFileNameStruct, 0, sizeof(OPENFILENAME));
    strcpy( szFile, initfile );
    gFileNameStruct.lStructSize = sizeof(OPENFILENAME);
    gFileNameStruct.hwndOwner = gmainwindow;
    gFileNameStruct.lpstrFilter = filter;
    gFileNameStruct.lpstrCustomFilter = (LPSTR) NULL;
    gFileNameStruct.nMaxCustFilter = 0L;
    gFileNameStruct.nFilterIndex = 1;
    gFileNameStruct.lpstrFile = szFile;
    gFileNameStruct.nMaxFile = sizeof(szFile);
    gFileNameStruct.lpstrFileTitle = szFileTitle;
    gFileNameStruct.nMaxFileTitle = sizeof(szFileTitle);
    gFileNameStruct.lpstrInitialDir = initDir;
    gFileNameStruct.lpstrTitle = title;
    gFileNameStruct.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOVALIDATE;
    gFileNameStruct.nFileOffset = 0;
    gFileNameStruct.nFileExtension = 0;
    gFileNameStruct.lpstrDefExt = "LOG";

    if (!GetOpenFileName(&gFileNameStruct)){
        return 0L;
	}
	strcpy( putNamehere, szFile );

	return bSuccess;
}



/*

gdFont gdFontSmallRep = {
	96,
	32,
	6,
	12,
	gdFontSmallData
};

gdFontPtr gdFontSmall = &gdFontSmallRep;
*/

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//



LRESULT CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_CLOSE:
			EndDialog(hWnd, TRUE);
			return (TRUE);
			break;

       case WM_INITDIALOG:
			gDialog = hWnd;
			ShowStatus( "Ready Sir." );
			CheckIt( IDC_MODE1, 0 );
			CheckIt( IDC_MODE2, 1 );
			SetSliderRange( IDC_GAIN1, 500 );
			SetSliderRange( IDC_GAIN2, 500 );

			Disable( IDC_STOP );
			ShowControls( hWnd, 1 );
			return TRUE;
			break;

		case WM_USER+1:
			ShowStatus( lParam );
			break;

		case WM_NOTIFY:
			// slider control bar code
			switch( LOWORD(wParam) ){
				case IDC_GAIN1:
					{	char arg[64];
						sprintf( arg, "-gain %d", 	GetSliderPos( IDC_GAIN1 ) );
						HandleArgs( arg );
					}
					break;
				case IDC_GAIN2:
					{	char arg[64];
						sprintf( arg, "-gain2 %d", 	GetSliderPos( IDC_GAIN2 ) );
						HandleArgs( arg );
					}
					break;
			}
			break;

		case WM_DROPFILES:
			//if ( !converting )
			{
        		char	filename[MAX_PATH];
				HDROP	hDrop = (HANDLE) wParam;  // handle of internal drop structure 
				long	filesNum, count, newlogs=0;

				filesNum = DragQueryFile( hDrop, -1, filename, MAX_PATH );

				for ( count=0; count < filesNum; count++ ){
					FILE *fp;
					DragQueryFile( hDrop, count, filename, MAX_PATH );
					ListAddString( SOURCELIST, filename );
				}
				DragFinish( hDrop );

				GoConvert( hWnd );
			}
			break;

 
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_CODEC:
					{
						char msg[256];
						int rc;

						ShowStatus( "Selecting codec..." );
						if ( IfCheckON( IDC_44100 ) )
							rc = select_codec( 16, 44100, 2, msg );
						else
							rc = select_codec( 16, 48000, 2, msg );

						if ( !rc )
							SetText( IDC_CODECNAME, msg );
					}
					break;

				case IDC_STOP:
					ShowStatus( "Stopped." );
					EndConvert( hWnd );
					break;

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			{
				RECT rt;
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				GetClientRect(hWnd, &rt);
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_CREATE:
			CreateDialog(hInst, (LPCSTR)MAKEINTRESOURCE(IDD_MAIN), hWnd, (DLGPROC) DialogProc);
			break;
		
		case WM_PAINT:
			{
				RECT rt;
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				GetClientRect(hWnd, &rt);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_CONVERT:
					ShowStatus( "Converting." );
					if ( !converting )
					{
						char filename[256];
						HWND hWnd = gDialog;

						GetLoadDialog( filename, 0, "*.ac3\0\0" , "AC3 Files (*.ac3)\0*.ac3\0VOB Files (*.vob)\0*.vob\0\0", "Select ac3 or vob file" );
						ListAddString( SOURCELIST, filename );
						GoConvert( gDialog );
					}
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HWND hWnd = hDlg;

	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case URL1:
					{
						char url[256];
						GetText( wmId, url, 256 );
						ShowHTMLURL( hWnd, url );
					}
					break;
			}

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
