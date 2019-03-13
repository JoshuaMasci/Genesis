#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"

#include <entt/entity/registry.hpp>

namespace Genesis
{
	struct Test
	{
		uint16_t num = 0;
	};

	class TestJob : public Job
	{
	public:
		TestJob(entt::view<unsigned int, Test>* view, entt::registry<>* registry, size_t start, size_t end)
		{
			this->view = view;
			this->entity_registry = registry;
			this->start = start;
			this->end = end;
		};

		virtual void run() override
		{
			for (size_t i = this->start; i < this->end; i++)
			{
				uint32_t value = (*view)[i];
				Test& test = this->entity_registry->get<Test>(value);
				test.num++;
			}
		};

	private:
		entt::view<unsigned int, Test>* view;
		entt::registry<>* entity_registry;
		size_t start;
		size_t end;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void runFrame(double delta_time);

		void close();
		bool isRunning();

		//Engine Systems
		JobSystem job_system;
		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;

		//EnTT
		entt::registry<> entity_registry;

	protected:
		bool is_running = true;
	};
};