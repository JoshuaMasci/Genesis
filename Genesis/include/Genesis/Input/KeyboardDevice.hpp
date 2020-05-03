#pragma once

#include "Genesis/Input/ArrayInputDevice.hpp"

namespace Genesis
{
	enum class KeyboardButton
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,
		Key_0,

		LShift,
		LCtrl,
		LAlt,

		RShift,
		RCtrl,
		RAlt,

		Enter,
		Backspace,
		Tab,
		Space,

		Minus,
		Equals,
		LBracket,
		RBracket,
		Backslash,

		Semicolon,
		Apostrophe,

		Comma,
		Period,
		Slash,

		Capslock,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		Printscreen,
		Scrolllock,
		Pause,
		Insert,

		Home,
		Pageup,
		Delete,
		End,
		Pagedown,
		Right,
		Left,
		Down,
		Up,

		Pad_Divide,
		Pad_Multiply,
		Pad_Minus,
		Pad_Plus,
		Pad_Enter,
		Pad_1,
		Pad_2,
		Pad_3,
		Pad_4,
		Pad_5,
		Pad_6,
		Pad_7,
		Pad_8,
		Pad_9,
		Pad_0,
		Pad_Period,
		SIZE
	};

	const string names[]
	{
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",

		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",

		"Left Shift",
		"Left Ctrl",
		"Left Alt",

		"Right Shift",
		"Right Ctrl",
		"Right Alt",

		"Return",
		"Backspace",
		"Tab",
		"Space",

		"-",
		"=",
		"[",
		"]",
		"\\",

		";",
		"'",

		",",
		".",
		"/",

		"Capslock",

		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",

		"Printscreen",
		"Scrolllock",
		"Pause",
		"Insert",

		"Home",
		"Pageup",
		"Delete",
		"End",
		"Pagedown",
		"Right",
		"Left",
		"Down",
		"Up",

		"Keypad /",
		"Keypad *",
		"Keypad -",
		"Keypad +",
		"Keypad Enter",
		"Keypad 1",
		"Keypad 2",
		"Keypad 3",
		"Keypad 4",
		"Keypad 5",
		"Keypad 6",
		"Keypad 7",
		"Keypad 8",
		"Keypad 9",
		"Keypad 0",
		"Keypad ."
	};


	class KeyboardDevice : public ArrayInputDevice
	{
	public:
		KeyboardDevice(string name);
		~KeyboardDevice();

		virtual void updateValues() override;

		void updateKeyboardButton(KeyboardButton button, bool state, Timestamp time);
		void updateInputText(string input_text);
		string getInputText();

		bool getButtonState(KeyboardButton button);

		virtual string getAxisName(uint16_t index) override;
		virtual string getButtonName(uint16_t index) override;
	protected:

		string input_text;
	};
};