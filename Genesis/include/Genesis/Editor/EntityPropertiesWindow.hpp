#pragma once

namespace Genesis
{
	class Entity;

	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void drawWindow(Entity* selected_entity);
	};
}