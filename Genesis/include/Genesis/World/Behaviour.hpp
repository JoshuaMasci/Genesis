#pragma once

namespace Genesis
{
	class GameObject;

	struct BehaviourEvent
	{
		size_t type_hash = 0;
		void* data = nullptr;
	};

	class Behaviour
	{
	protected:
		GameObject* parent = nullptr;

	public:
		virtual const char* get_name() = 0;
		virtual void on_enabled(GameObject* parent) { this->parent = parent; };
		virtual void on_disable() = 0;
		virtual void on_update(const TimeStep& time_step) {};
		virtual void on_event(const BehaviourEvent& behaviour_event) {};
		virtual void editor_imgui_display() {};
	};
}