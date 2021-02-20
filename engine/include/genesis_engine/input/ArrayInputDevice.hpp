#pragma once

#include "genesis_engine/input/InputDevice.hpp"

#include "genesis_engine/core/Fnv1aHash.hpp"

#define INVALID_INDEX 65535

namespace Genesis
{
	typedef uint32_t Timestamp;

	typedef uint16_t ButtonIndex;
	typedef uint16_t AxisIndex;

	/*struct AxisSettings
	{
		AxisSettings(uint16_t axis_index = INVALID_INDEX, double deadzone = 0.0, bool inverted = false, AxisRange range = AxisRange::FULL, double sensitivity = 1.0)
		{
			this->axis_index = axis_index;
			this->deadzone = deadzone;
			this->inverted = inverted;
			this->range = range;
			this->sensitivity = sensitivity;
		};

		uint16_t axis_index;
		double deadzone = 0.0;
		bool inverted = false;
		AxisRange range = AxisRange::FULL;
		double sensitivity = 1.0;
	};
	
	class ArrayInputDevice : public InputDevice
	{
	public:
		ArrayInputDevice(string device_name, uint16_t number_of_buttons, uint16_t number_of_axes);

		//Has a binding of that name
		virtual bool hasButton(string button_name);
		virtual ButtonValue getButton(string button_name);
		//Returns if the button is currently being pressed
		virtual bool getButtonDown(string button_name);
		//Returns true only once when the button is first pressed
		virtual bool getButtonPressed(string button_name);

		virtual bool hasAxis(string axis_name);
		virtual AxisValue getAxis(string axis_name);

		void addButton(string button_name, ButtonIndex index);
		void addAxis(string axis_name, AxisSettings settings);

		void updateButton(uint16_t index, bool state, Timestamp time);
		void updateAxis(uint16_t index, double value, Timestamp time);

		virtual void updateValues();

		virtual string getButtonName(uint16_t index);
		virtual string getAxisName(uint16_t index);
	protected:

		vector<ButtonValue> button_values;
		vector<AxisValue> axis_values;

		flat_hash_map<string, ButtonIndex> button_bindings;
		flat_hash_map<string, AxisSettings> axis_bindings;
	};*/

	enum class AxisRange
	{
		FULL,
		FORWARD,
		BACKWARD
	};

	struct AxisSettings
	{
		AxisSettings(float deadzone = 0.0f, bool inverted = false, AxisRange range = AxisRange::FULL, float sensitivity = 1.0f)
		{
			this->deadzone = deadzone;
			this->inverted = inverted;
			this->range = range;
			this->sensitivity = sensitivity;
		};

		float deadzone = 0.0f;
		bool inverted = false;
		AxisRange range = AxisRange::FULL;
		float sensitivity = 1.0f;
	};

	class ArrayInputDevice : public InputDevice
	{
		struct AxisSettingsInternal
		{
			fnv_hash32 binding;
			AxisSettings settings;
		};

	protected:
		vector<vector<fnv_hash32>> button_settings;
		vector<vector<AxisSettingsInternal>> axis_settings;

	public:
		ArrayInputDevice(const string& device_name, InputManager* manager, size_t number_of_buttons, size_t number_of_axes);

		void addButtonBinding(size_t index, fnv_hash32 binding);
		void removeButtonBinding(size_t index, fnv_hash32 binding);

		void addAxisBinding(size_t index, fnv_hash32 binding, AxisSettings settings);
		void removeAxisBinding(size_t index, fnv_hash32 binding);
		AxisSettings* getAxisSettings(size_t index, fnv_hash32 binding);

		void updateButtonValue(size_t index, bool state);
		void updateAxisValue(size_t index, float value);

	};
};