#define GLAD_GL_IMPLEMENTATION
#include "gl.h"

#include "logging.h"
#include "platform.h"

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
	const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
	const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

typedef struct ExtraWindowData {
	WindowKeyCallback* keyCallback;
	WindowMouseMoveCallback* mouseMoveCallback;
	WindowMouseButtonCallback* mouseButtonCallback;
	WindowInputState inputState;
} ExtraWindowData;

Key vkcode_to_key(int vkcode)
{
	if (vkcode >= 0x30 && vkcode <= 0x39)
		return KEY_0 + (vkcode - 0x30);
	if (vkcode >= 0x41 && vkcode <= 0x5A)
		return KEY_A + (vkcode - 0x41);
	switch (vkcode)
	{
		case VK_SHIFT:
		case VK_LSHIFT: return KEY_LEFT_SHIFT;
		case VK_RSHIFT: return KEY_RIGHT_SHIFT;
		case VK_CONTROL:
		case VK_LCONTROL: return KEY_LEFT_CONTROL;
		case VK_RCONTROL: return KEY_RIGHT_CONTROL;
		case VK_SPACE: return KEY_SPACE;
		case VK_LEFT: return KEY_LEFT;
		case VK_RIGHT: return KEY_RIGHT;
		case VK_UP: return KEY_UP;
		case VK_DOWN: return KEY_DOWN;
	}
	return KEY_UNDEFINED;
}

void init_wgl_extensions()
{
	// Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
	// We use a dummy window because you can only set the pixel format for a window once. For the
	// real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
	// that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
	// have a context.
	WNDCLASSA wndClass = {
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = DefWindowProcA,
		.hInstance = GetModuleHandle(0),
		.lpszClassName = "WGLTEMP",
	};

	if (!RegisterClassA(&wndClass))
		LOGFATAL("Failed to register temporary OpenGL window class.");

	HWND hWnd = CreateWindowExA(
		0,
		wndClass.lpszClassName,
		wndClass.lpszClassName,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		wndClass.hInstance,
		0);

	if (!hWnd)
		LOGFATAL("Failed to create temporary OpenGL window.");

	HDC hDeviceContext = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd = {
		.nSize = sizeof(pfd),
		.nVersion = 1,
		.iPixelType = PFD_TYPE_RGBA,
		.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.cColorBits = 32,
		.cAlphaBits = 8,
		.iLayerType = PFD_MAIN_PLANE,
		.cDepthBits = 24,
		.cStencilBits = 8,
	};

	int pixelFormat = ChoosePixelFormat(hDeviceContext, &pfd);
	if (!pixelFormat)
		LOGFATAL("Failed to find a suitable pixel format for temporary OpenGL context creation.");
	if (!SetPixelFormat(hDeviceContext, pixelFormat, &pfd))
		LOGFATAL("Failed to set the pixel format for temporary OpenGL context creation.");

	HGLRC hRenderingContext = wglCreateContext(hDeviceContext);
	if (!hRenderingContext)
		LOGFATAL("Failed to create temporary OpenGL rendering context.");

	if (!wglMakeCurrent(hDeviceContext, hRenderingContext))
		LOGFATAL("Failed to make temporary OpenGL rendering context current.");

	HGLRC currentContext = wglGetCurrentContext();

	wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress("wglChoosePixelFormatARB");

	wglMakeCurrent(hDeviceContext, 0);
	wglDeleteContext(hRenderingContext);
	ReleaseDC(hWnd, hDeviceContext);
	DestroyWindow(hWnd);
}

HGLRC init_opengl(HDC hDeviceContext, int glVersionMajor, int glVersionMinor)
{
	init_wgl_extensions();

	// Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
	int pixelFormatAttribs[] =
	{
		WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
		WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,         32,
		WGL_DEPTH_BITS_ARB,         24,
		WGL_STENCIL_BITS_ARB,       8,
		0
	};

	int pixelFormat;
	UINT numFormats;
	wglChoosePixelFormatARB(hDeviceContext, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);
	if (numFormats == 0)
		LOGFATAL("Failed to set the OpenGL 3.3 pixel format.");

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(hDeviceContext, pixelFormat, sizeof(pfd), &pfd);
	if (!SetPixelFormat(hDeviceContext, pixelFormat, &pfd))
		LOGFATAL("Failed to set the OpenGL 3.3 pixel format.");

	// Specify that we want to create an OpenGL 3.3 core profile context
	int glAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, glVersionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, glVersionMinor,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC hRenderingContext = wglCreateContextAttribsARB(hDeviceContext, 0, glAttribs);
	if (!hRenderingContext)
		LOGFATAL("Failed to create OpenGL context.");

	if (!wglMakeCurrent(hDeviceContext, hRenderingContext))
		LOGFATAL("Failed to make OpenGL context current.");

	if (!gladLoaderLoadGL())
		LOGFATAL("Failed to load OpenGL function pointers.");

	glEnable(GL_DEPTH_TEST);

	return hRenderingContext;
}

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);

	switch (msg) {
		case WM_KEYDOWN:
		{
			int key = vkcode_to_key(wparam);
			if (key != KEY_UNDEFINED)
				windowData->inputState.keys[key].bIsDown = TRUE;
			if (windowData->keyCallback)
			{
				WindowKeyEventData e;
				e.key = key;
				e.action = BUTTON_ACTION_PRESS;
				windowData->keyCallback(e);
			}
			break;
		}

		case WM_KEYUP:
		{
			int key = vkcode_to_key(wparam);
			if (key != KEY_UNDEFINED)
				windowData->inputState.keys[key].bIsDown = FALSE;
			if (windowData->keyCallback)
			{
				WindowKeyEventData e;
				e.key = key;
				e.action = BUTTON_ACTION_RELEASE;
				windowData->keyCallback(e);
			}
			break;
		}

		case WM_SYSKEYDOWN:
		{
			break;
		}

		case WM_SYSKEYUP:
		{
			break;
		}

		case WM_CHAR:
			break;

		case WM_MOUSEMOVE:
		{
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			windowData->inputState.mouse.deltaX += x - windowData->inputState.mouse.clientX;
			windowData->inputState.mouse.deltaY += y - windowData->inputState.mouse.clientY;
			windowData->inputState.mouse.clientX = x;
			windowData->inputState.mouse.clientY = y;
			if (windowData->mouseMoveCallback)
			{
				WindowMouseMoveEventData e;
				e.x = windowData->inputState.mouse.clientX;
				e.y = windowData->inputState.mouse.clientY;
				e.deltaX = windowData->inputState.mouse.deltaX;
				e.deltaY = windowData->inputState.mouse.deltaY;
				windowData->mouseMoveCallback(e);
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_LEFT].bIsDown = TRUE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_LEFT;
				e.action = BUTTON_ACTION_PRESS;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_LEFT].bIsDown = FALSE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_LEFT;
				e.action = BUTTON_ACTION_RELEASE;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_MBUTTONDOWN:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_MIDDLE].bIsDown = TRUE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_MIDDLE;
				e.action = BUTTON_ACTION_PRESS;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_MBUTTONUP:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_MIDDLE].bIsDown = FALSE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_MIDDLE;
				e.action = BUTTON_ACTION_RELEASE;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_RIGHT].bIsDown = TRUE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_RIGHT;
				e.action = BUTTON_ACTION_PRESS;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_RBUTTONUP:
		{
			windowData->inputState.mouse.buttons[MOUSE_BUTTON_RIGHT].bIsDown = FALSE;
			if (windowData->mouseButtonCallback)
			{
				WindowMouseButtonEventData e;
				e.button = MOUSE_BUTTON_RIGHT;
				e.action = BUTTON_ACTION_RELEASE;
				windowData->mouseButtonCallback(e);
			}
			break;
		}

		case WM_CLOSE:
		case WM_DESTROY:
			free(windowData);
			PostQuitMessage(0);
			break;
		default:
			result = DefWindowProcA(hWnd, msg, wparam, lparam);
			break;
	}

	return result;
}

HWND create_window(HINSTANCE hInstance, int width, int height, const char* name)
{
	WNDCLASSA wndClass = {
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = wnd_proc,
		.hInstance = hInstance,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.hbrBackground = 0,
		.lpszClassName = "WGL WIndow",
		.cbWndExtra = sizeof(ExtraWindowData*)
	};

	if (!RegisterClassA(&wndClass))
		LOGFATAL("Failed to register window.");

	RECT rect = {
		.right = width,
		.bottom = height,
	};
	DWORD wndStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rect, wndStyle, FALSE);

	HWND hWnd = CreateWindowExA(
		0,
		wndClass.lpszClassName,
		name,
		wndStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0);

	ExtraWindowData* windowData = malloc(sizeof(ExtraWindowData));
	windowData->keyCallback = NULL;
	windowData->mouseMoveCallback = NULL;
	windowData->mouseButtonCallback = NULL;
	for (int i = 0; i < _KEY_MAX; ++i)
		windowData->inputState.keys[i].bIsDown = FALSE;
	for (int i = 0; i < _MOUSE_BUTTON_MAX; ++i)
		windowData->inputState.mouse.buttons[i].bIsDown = FALSE;
	SetWindowLongPtr(hWnd, 0, (LONG_PTR)windowData);

	if (!hWnd)
		LOGFATAL("Failed to create window.");

	return hWnd;
}

int platform_process_events()
{
	MSG msg;
	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT) {
			return FALSE;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	return TRUE;
}

void platform_window_flush_input(HWND hWnd)
{
	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);
	windowData->inputState.mouse.deltaX = 0;
	windowData->inputState.mouse.deltaY = 0;
}

void platform_set_window_key_callback(HWND hWnd, WindowKeyCallback* callback)
{
	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);
	windowData->keyCallback = callback;
}

void platform_set_window_mouse_move_callback(HWND hWnd, WindowMouseMoveCallback* callback)
{
	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);
	windowData->mouseMoveCallback = callback;
}

void platform_set_window_mouse_button_callback(HWND hWnd, WindowMouseButtonCallback* callback)
{
	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);
	windowData->mouseButtonCallback = callback;
}

const WindowInputState * platform_get_window_input_state(HWND hWnd)
{
	ExtraWindowData* windowData = (ExtraWindowData*)GetWindowLongPtr(hWnd, 0);
	return &windowData->inputState;
}

int platform_get_key_is_down(int key)
{
	return GetAsyncKeyState(key) & 0x8000 == 0x8000;
}