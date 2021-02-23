#pragma once

#include "genesis_engine/input/input_device.hpp"

#include "genesis_core/hash/fnv1a_hash.hpp"

#define INVALID_INDEX 65535

namespace genesis
{
	typedef uint32_t Timestamp;

	typedef uint16_t ButtonIndex;
	typedef uint16_t AxisIndex;

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