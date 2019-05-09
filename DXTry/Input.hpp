#ifndef DXTRY_INPUT_HPP
#define DXTRY_INPUT_HPP

#include "includes.hpp"
#include "Error.hpp"

struct Input {
	struct Mouse {
		Vector2 position = Vector2::Zero;
		Vector2 delta = Vector2::Zero;
		float scroll = 0;
		float scroll_delta = 0;

		enum Button {
			Left, Middle, Right,
			Forward, Back,
			_Last
		};
		std::bitset<Button::_Last> _pressed;
		std::bitset<Button::_Last> _changed;
	} mouse;

	struct Keyboard {
		std::bitset<256> _current;
		std::bitset<256> _previous;

		bool pressed(BYTE vkey) const {
			return _current[vkey];
		}
		bool released(BYTE vkey) const {
			return !pressed(vkey);
		}
		bool just_pressed(BYTE vkey) const {
			return pressed(vkey) && !_previous[vkey];
		}
		bool just_released(BYTE vkey) const {
			return released(vkey) && _previous[vkey];
		}
	} keyboard;

	void update(LPARAM lParam);
};

#endif // !DXTRY_INPUT_HPP
