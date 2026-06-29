#include <windows.h>
#include <wingdi.h>
#include <winuser.h>

#define internal static
#define local_persist static
#define global_var static

// TODO: global for now
global_var bool Running;
global_var BITMAPINFO BitmapInfo;
global_var void *BitmapMemory;
global_var HBITMAP BitmapHandle;
global_var HDC BitmapDeviceContext;

// DIB: device independent bitmap
internal void Win32ResizeDIBSection(int Width, int Height) {
  // TODO: bulletproof
  // maybe not free first, free after, then free first if that fails

  // TODO: freeDIBSection
  if (BitmapInfo.bmiHeader.biSize) {
    DeleteObject(BitmapHandle);
  }
  if (!BitmapDeviceContext) {
    // TODO: should we recreate?
    BitmapDeviceContext = CreateCompatibleDC(0);
  }

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = Width;
  BitmapInfo.bmiHeader.biHeight = Height;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;
  // BitmapInfo.bmiHeader.biSizeImage = 0;
  // BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
  // BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
  // BitmapInfo.bmiHeader.biClrUsed = 0;
  // BitmapInfo.bmiHeader.biClrImportant = 0;

  BitmapHandle = CreateDIBSection(BitmapDeviceContext, &BitmapInfo,
                                  DIB_RGB_COLORS, &BitmapMemory, 0, 0);
}

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width,
                                int Height) {
  StretchDIBits(DeviceContext, X, Y, Width, Height, X, Y, Width, Height,
                BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT
Win32MainWindowCallback(HWND Window,  // handle to window
                        UINT Message, // message that wiindow ask us to handle
                        WPARAM WParam, LPARAM LParam) {
  LRESULT Result = 0;

  switch (Message) {
  case WM_SIZE: {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Height = ClientRect.bottom - ClientRect.top;
    int Width = ClientRect.right - ClientRect.left;
    Win32ResizeDIBSection(Width, Height);
  } break;
  case WM_CLOSE: {
    // TODO: hadle with message?
    Running = false;
  } break;
  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;
  case WM_DESTROY: {
    // handle as error; recreate window
    Running = false;
  } break;

  case WM_PAINT: {
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(Window, &Paint);
    int X = Paint.rcPaint.left;
    int Y = Paint.rcPaint.top;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;
    Win32UpdateWindow(DeviceContext, X, Y, Width, Height);

    // local_persist DWORD Operation = WHITENESS; // only for debugging
    // PatBlt(DeviceContext, X, Y, Width, Height, Operation);
    // Operation = (Operation == BLACKNESS) ? WHITENESS : BLACKNESS;
    EndPaint(Window, &Paint);
  } break;
  default: {
    Result = DefWindowProc(Window, Message, WParam, LParam);
  }
  }

  return Result;
}

int APIENTRY
WinMain(HINSTANCE Instance,     // entrypoint where windows pass a handle to exe
        HINSTANCE PrevInstance, // legacy
        LPSTR CommandLine,
        int ShowCode) // software window size
{

  WNDCLASS WindowClass{};
  /*
  UINT      style;
  WNDPROC   lpfnWndProc;
  int       cbClsExtra;
  int       cbWndExtra;
  HINSTANCE hInstance;
  HICON     hIcon;
  HCURSOR   hCursor;
  HBRUSH    hbrBackground;
  LPCSTR    lpszMenuName;
  LPCSTR    lpszClassName;
  */

  // TODO: check if HREDRAW/VREDRAW/OWNDC still matter
  WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  // WindowClass.hIcon;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClass(&WindowClass)) {
    HWND WindowHandle = CreateWindowExA(  //
        0,                                // DWORD dwExStyle
        WindowClass.lpszClassName,        // LPCSTR lpClassName
        "Handmade Hero",                  // LPCSTR lpWindowName
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // DWORD dwStyle
        CW_USEDEFAULT,                    // int X
        CW_USEDEFAULT,                    // int Y
        CW_USEDEFAULT,                    // int nWidth
        CW_USEDEFAULT,                    // int nHeight
        0,                                // HWND hWndParent
        0,                                // HMENU hMenu
        Instance,                         // HINSTANCE hInstance
        0                                 // LPVOID lpParam
    ); // 如果函数成功，则返回值是新窗口的句柄; 如果函数失败，则返回值 NULL。
    if (WindowHandle) {
      Running = true;
      MSG Message;
      while (Running) {
        BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
        if (MessageResult > 0) {
          // currently have no way to handle error code
          TranslateMessage(&Message);
          DispatchMessageA(&Message);
        } else {
          break;
        }
      }
    } else {
      // TODO:
    }
  } else {
    // TODO: logging
  }

  return 0;
}