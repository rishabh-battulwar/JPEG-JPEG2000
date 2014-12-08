//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Used to read in a standard RGB image and display it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************

#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>

// Include class files
#include "Image.h"
#include "ImageCompressor.h"
#include "Hash.h"


#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage, DCTImage[16], DWTImage[16];	// image objects
JPEGCompressor	compressor;						// JPEGCompressor objects
float *compressedStreamDCT, *compressedStreamDWT;
int m, n, superflag = 0;
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Read in the image and its copy
	int w = 512, h = 512;
	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d", &ImagePath, &n);
	inImage.setWidth(w);
	inImage.setHeight(h);
	compressedStreamDCT = new float[w*h * 3];
	compressedStreamDWT = new float[w*h * 3];
	m = (int)(round(1.0 * n / 4096));
	if ( strstr(ImagePath, ".rgb" )==NULL )
	{ 
		AfxMessageBox( "Image has to be a '.rgb' file\nUsage - Image.exe image.rgb w h");
		//return FALSE;
	}
	else
	{
		inImage.setImagePath(ImagePath);
		if ( !inImage.ReadImage() )
		{ 
			AfxMessageBox( "Could not read image\nUsage - Image.exe image.rgb w h");
			//return FALSE;
		}
		//else
			//outImage = inImage;
	}

	//####################
	for (int i = 0; i < 16; i++)
	{
		DCTImage[i].setHeight(inImage.getHeight());
		DCTImage[i].setWidth(inImage.getWidth());
		DCTImage[i].allocateMemoryforData();

		DWTImage[i].setHeight(inImage.getHeight());
		DWTImage[i].setWidth(inImage.getWidth());
		DWTImage[i].allocateMemoryforData();
	}
	std::cout << " \n Processing DCT...\n" << std::endl;
	compressor.DCTCompression(inImage, compressedStreamDCT);
	/*int iter = 0;
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				DCTImage.setValue(i, j, k, compressedStreamDCT[iter++]);*/

	std::cout << " Processing DWT...\n" << std::endl;
	compressor.DWTCompression(inImage, compressedStreamDWT);
	/*iter = 0;
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				DWTImage.setValue(i, j, k, compressedStreamDWT[iter++]);*/

	if (n == -1)
	{
		for (int j = 0, i = 1; j < 16; j++, i++)
		{
			std::cout << " Processing IDCT for " << (4*i) << " coefficients...\n" << std::endl;
			compressor.DCTDecompression(compressedStreamDCT, DCTImage[j], (4*i));
			std::cout << " Processing IDWT for " << 4096 * (4*i) << " coefficients...\n" << std::endl;
			compressor.DWTDecompression(compressedStreamDWT, DWTImage[j], (4096 * (4*i)));
		}

	}
	else
	{
		std::cout << " Processing IDCT for " << m << " coefficients...\n" << std::endl;
		compressor.DCTDecompression(compressedStreamDCT, DCTImage[0], m);
		std::cout << " Processing IDWT for " << n << " coefficients...\n" << std::endl;
		compressor.DWTDecompression(compressedStreamDWT, DWTImage[0], n);
	}

	//####################

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
					//std::cout << " \n Processing DCT...\n" << std::endl;
					//compressor.DCTCompression(inImage, compressedStreamDCT);

					//std::cout << " Processing DWT...\n" << std::endl;
					//compressor.DWTCompression(inImage, compressedStreamDWT);
					
					//if (n == -1)
					//{
					//	//superflag = 1;
					//	for (int i = 1; i <= 64; i++)
					//	{
					//		DCTImage.setHeight(inImage.getHeight());
					//		DCTImage.setWidth(inImage.getWidth());
					//		DCTImage.allocateMemoryforData();

					//		DWTImage.setHeight(inImage.getHeight());
					//		DWTImage.setWidth(inImage.getWidth());
					//		DWTImage.allocateMemoryforData();

					//		std::cout << " Processing IDCT for " << i << " coefficients...\n" << std::endl;
					//		compressor.DCTDecompression(compressedStreamDCT, DCTImage, i);
					//		std::cout << " Processing IDWT for " << 4096 * i << " coefficients...\n" << std::endl;
					//		compressor.DWTDecompression(compressedStreamDWT, DWTImage, (4096 * i));
					//		SendMessage(hWnd, WM_PAINT, NULL, NULL);
					//		//InvalidateRect(hWnd, &rt, false);
					//		//UpdateWindow(hWnd);

					//		DCTImage.~MyImage();
					//		DWTImage.~MyImage();
					//	}

					//}
					//else
					//{
					//	std::cout << " Processing IDCT for " << m << " coefficients...\n" << std::endl;
					//	compressor.DCTDecompression(compressedStreamDCT, DCTImage, m);
					//	std::cout << " Processing IDWT for " << n << " coefficients...\n" << std::endl;
					//	compressor.DWTDecompression(compressedStreamDWT, DWTImage, n);
					//}
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				char text[1000];
				strcpy(text, "Original image (Left)  Image after modification (Right)\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nUpdate program with your code to modify input image");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();

				//static int count = 0;
				//std::cout << "count : " << count << std::endl;
				//InvalidateRect(hWnd, &rt, false);
				if (n != -1)
				{
					SetDIBitsToDevice(hdc,
						0, 100, inImage.getWidth(), inImage.getHeight(),
						0, 0, 0, inImage.getHeight(),
						DCTImage[0].getImageData(), &bmi, DIB_RGB_COLORS);

					SetDIBitsToDevice(hdc,
						inImage.getWidth() + 50, 100, inImage.getWidth(), inImage.getHeight(),
						0, 0, 0, inImage.getHeight(),
						DWTImage[0].getImageData(), &bmi, DIB_RGB_COLORS);
				}
				
				std::chrono::milliseconds interval(3000);
				if (n == -1) // && superflag != 0)
					for (int i = 0; i < 16; i++)
					{
						SetDIBitsToDevice(hdc,
							0, 100, inImage.getWidth(), inImage.getHeight(),
							0, 0, 0, inImage.getHeight(),
							DCTImage[i].getImageData(), &bmi, DIB_RGB_COLORS);

						SetDIBitsToDevice(hdc,
							inImage.getWidth() + 50, 100, inImage.getWidth(), inImage.getHeight(),
							0, 0, 0, inImage.getHeight(),
							DWTImage[i].getImageData(), &bmi, DIB_RGB_COLORS);

						std::this_thread::sleep_for(interval);
					}

				EndPaint(hWnd, &ps);
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
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


