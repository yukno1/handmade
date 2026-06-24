

#include <windows.h>
#include <wingdi.h>

LRESULT
MainWindowCallback(HWND Window,  // handle to window
                   UINT Message, // message that wiindow ask us to handle
                   WPARAM WParam, LPARAM LParam) {
  LRESULT Result = 0;

  switch (Message) {
  case WM_SIZE: {
    OutputDebugStringA("WM_SIZE\n");
  } break;
  case WM_DESTROY: {
    OutputDebugStringA("WM_DESTROY\n");
  } break;
  case WM_CLOSE: {
    OutputDebugStringA("WM_CLOSE\n");
  } break;
  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;
  case WM_PAINT: {
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(Window, &Paint);
    int X = Paint.rcPaint.left;
    int Y = Paint.rcPaint.top;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;

    static DWORD Operation = WHITENESS; // only for debugging
    PatBlt(DeviceContext, X, Y, Width, Height, Operation);
    Operation = (Operation == BLACKNESS) ? WHITENESS : BLACKNESS;
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
  WindowClass.lpfnWndProc = MainWindowCallback;
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
      MSG Message;
      while (true) {
        BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
        if (MessageResult > 0) {
          // currently have no way to handle error code
          TranslateMessage(&Message);
          DispatchMessage(&Message);
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