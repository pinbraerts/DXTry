#include "Input.hpp"

void Input::update(LPARAM lParam) {
	UINT sz;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &sz, sizeof(RAWINPUTHEADER));
	BYTE* buffer = new BYTE[sz];

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &sz,
		sizeof(RAWINPUTHEADER)) != sz) {
		delete[] buffer;
		throw Error(L"GetRawInputData does not return correct size !\n");
	}

	RAWINPUT* raw = (RAWINPUT*)buffer;

	switch (raw->header.dwType) {
	case RIM_TYPEKEYBOARD:
	{
		keyboard._previous = keyboard._current;
		auto& kb = raw->data.keyboard;
		if (kb.Flags == RI_KEY_BREAK)
			keyboard._current[kb.VKey] = 0;
		else if (kb.Flags == RI_KEY_MAKE)
			keyboard._current[kb.VKey] = 1;
	}
	break;
	case RIM_TYPEMOUSE:
	{
		auto& ms = raw->data.mouse;
		Vector2 newpos{ (float)ms.lLastX, (float)ms.lLastY };

		if (ms.usFlags | MOUSE_MOVE_ABSOLUTE) {
			mouse.delta = newpos;
			mouse.position += mouse.delta;
		}

		if (ms.usButtonFlags | RI_MOUSE_LEFT_BUTTON_DOWN) {
			mouse._changed[Mouse::Button::Left] = !mouse._pressed[Mouse::Button::Left];
			mouse._pressed[Mouse::Button::Left] = 1;
		}
		else if (ms.usButtonFlags | RI_MOUSE_LEFT_BUTTON_UP) {
			mouse._changed[Mouse::Button::Left] = mouse._pressed[Mouse::Button::Left];
			mouse._pressed[Mouse::Button::Left] = 0;
		}

		if (ms.usButtonFlags | RI_MOUSE_RIGHT_BUTTON_DOWN) {
			mouse._changed[Mouse::Button::Right] = !mouse._pressed[Mouse::Button::Right];
			mouse._pressed[Mouse::Button::Right] = 1;
		}
		else if (ms.usButtonFlags | RI_MOUSE_RIGHT_BUTTON_UP) {
			mouse._changed[Mouse::Button::Right] = mouse._pressed[Mouse::Button::Right];
			mouse._pressed[Mouse::Button::Right] = 0;
		}

		if (ms.usButtonFlags | RI_MOUSE_MIDDLE_BUTTON_DOWN) {
			mouse._changed[Mouse::Button::Middle] = !mouse._pressed[Mouse::Button::Middle];
			mouse._pressed[Mouse::Button::Middle] = 1;
		}
		else if (ms.usButtonFlags | RI_MOUSE_MIDDLE_BUTTON_UP) {
			mouse._changed[Mouse::Button::Middle] = mouse._pressed[Mouse::Button::Middle];
			mouse._pressed[Mouse::Button::Middle] = 0;
		}

		if (ms.usButtonFlags | RI_MOUSE_BUTTON_4_DOWN) {
			mouse._changed[Mouse::Button::Forward] = !mouse._pressed[Mouse::Button::Forward];
			mouse._pressed[Mouse::Button::Forward] = 1;
		}
		else if (ms.usButtonFlags | RI_MOUSE_BUTTON_4_UP) {
			mouse._changed[Mouse::Button::Forward] = mouse._pressed[Mouse::Button::Forward];
			mouse._pressed[Mouse::Button::Forward] = 0;
		}

		if (ms.usButtonFlags | RI_MOUSE_BUTTON_5_DOWN) {
			mouse._changed[Mouse::Button::Back] = !mouse._pressed[Mouse::Button::Back];
			mouse._pressed[Mouse::Button::Back] = 1;
		}
		else if (ms.usButtonFlags | RI_MOUSE_BUTTON_5_UP) {
			mouse._changed[Mouse::Button::Back] = mouse._pressed[Mouse::Button::Back];
			mouse._pressed[Mouse::Button::Back] = 0;
		}
		if (ms.usButtonFlags | RI_MOUSE_WHEEL) {
			mouse.scroll_delta = (SHORT)ms.usButtonData;
			mouse.scroll += mouse.scroll_delta;
		}
	}
	break;
	}

	delete[] buffer;
}
