#ifndef PLATFORM_H
#define PLATFORM_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef enum {
	KEY_UNDEFINED = -1,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_NUMPAD_0,
	KEY_NUMPAD_1,
	KEY_NUMPAD_2,
	KEY_NUMPAD_3,
	KEY_NUMPAD_4,
	KEY_NUMPAD_5,
	KEY_NUMPAD_6,
	KEY_NUMPAD_7,
	KEY_NUMPAD_8,
	KEY_NUMPAD_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_ESC,
	KEY_GRAVE_ACCENT,
	KEY_TAB,
	KEY_CAPS_LOCK,
	KEY_LEFT_SHIFT,
	KEY_RIGHT_SHIFT,
	KEY_LEFT_CONTROL,
	KEY_RIGHT_CONTROL,
	KEY_ALT,
	KEY_SPACE,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_INSERT,
	KEY_DELETE,
	KEY_ENTER,
	KEY_BACKSPACE,
	KEY_DASH,
	KEY_EQUALS,
	KEY_LEFT_BRACE,
	KEY_RIGHT_BRACE,
	KEY_SEMI_COLON,
	KEY_SINGLE_QUOTE,
	KEY_TILDE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_BACKWARD_SLASH,
	KEY_FORWARD_SLASH,
	_KEY_MAX
} Key;

typedef enum {
	MOUSE_BUTTON_UNDEFINED = -1,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	_MOUSE_BUTTON_MAX
} MouseButton;

typedef enum {
	BUTTON_ACTION_UNDEFINED = -1,
	BUTTON_ACTION_PRESS,
	BUTTON_ACTION_RELEASE
} ButtonAction;

typedef struct WindowKeyEventData {
	Key key;
	ButtonAction action;
} WindowKeyEventData;

typedef struct WindowMouseMoveEventData {
	int x;
	int y;
	int deltaX;
	int deltaY;
} WindowMouseMoveEventData;

typedef struct WindowMouseButtonEventData {
	MouseButton button;
	ButtonAction action;
} WindowMouseButtonEventData;

typedef void WindowKeyCallback(WindowKeyEventData);
typedef void WindowMouseMoveCallback(WindowMouseMoveEventData);
typedef void WindowMouseButtonCallback(WindowMouseButtonEventData);

typedef struct KeyState {
	int bIsDown;
} KeyState;

typedef struct MouseButtonState {
	int bIsDown;
} MouseButtonState;

typedef struct MouseState {
	int clientX;
	int clientY;
	int deltaX;
	int deltaY;
	MouseButtonState buttons[_MOUSE_BUTTON_MAX];
} MouseState;

typedef struct WindowInputState {
	KeyState keys[_KEY_MAX];
	MouseState mouse;
} WindowInputState;

HGLRC init_opengl(HDC hDeviceContext, int glVersionMajor, int glVersionMinor);

HWND create_window(HINSTANCE hInstance, int width, int height, const char* name);

int platform_process_events();

void platform_window_flush_input(HWND hWnd);

void platform_set_window_key_callback(HWND hWnd, WindowKeyCallback* callback);

void platform_set_window_mouse_move_callback(HWND hWnd, WindowMouseMoveCallback* callback);

void platform_set_window_mouse_button_callback(HWND hWnd, WindowMouseButtonCallback* callback);

const WindowInputState * platform_get_window_input_state(HWND hWnd);

int platform_get_key_is_down(int key);

#endif