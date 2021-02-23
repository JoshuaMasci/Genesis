#pragma once

#include "genesis_engine/input/input_device.hpp"

#include "genesis_engine/input/mouse_device.hpp"
#include "genesis_engine/input/keyboard_device.hpp"

#include "genesis_core/hash/fnv1a_hash.hpp"

namespace genesis
{
	class InputManager
	{
	protected:
		struct ButtonValue
		{
			bool current_state;
			bool previous_state;
		};

		struct AxisValue
		{
			float value;
		};

		//Keyboard
		ButtonValue keyboard_buttons[(size_t)KeyboardButton::SIZE] = { {false, false} };
		string keyboard_input_text = "";

		//Mouse
		ButtonValue mouse_buttons[(size_t)MouseButton::SIZE] = { {false, false} };
		vec2f mouse_postion = vec2f(0.0f);

		//Generic Input
		flat_hash_map<fnv_hash32, ButtonValue> button_values;
		flat_hash_map<fnv_hash32, AxisValue> axis_values;

	public:
		InputManager(string config_folder_path);
		~InputManager();

		//Keyboard
		bool getKeyboardDown(KeyboardButton button);
		bool getKeyboardPressed(KeyboardButton button);
		void updateKeyboardState(KeyboardButton button, bool state);

		string& getInputText();
		void updateInputText(const string& text);

		//Mouse
		bool getMouseDown(MouseButton button);
		bool getMousePressed(MouseButton button);
		void updateMouseState(MouseButton button, bool state);

		vec2f getMousePosition();
		void updateMousePosition(const vec2f& position);

		//Generic Input
		bool getButtonDown(fnv_hash32 string_hash);
		bool getButtonPressed(fnv_hash32 string_hash);

		float getAxis(fnv_hash32 string_hash);

		float getButtonAxis(fnv_hash32 pos_button, fnv_hash32 neg_button, fnv_hash32 axis, bool clamp_value = true);

		//Updates
		void update();
		void updateButtonValue(fnv_hash32 string_hash, bool state);
		void updateAxisValue(fnv_hash32 string_hash, float value);
	};
};