#pragma once

namespace genesis
{
	enum class ConsoleLevel
	{
		trace = 0,
		debug = 1,
		info = 2,
		warning = 3,
		error = 4,
		critical = 6,
		off = 7
	};

	struct ConsoleMessage
	{
		ConsoleLevel level;
		string message;
		//string source;
		size_t thread_id;
	};

	class ConsoleWindow
	{
	public:
		ConsoleWindow();
		~ConsoleWindow();

		void addMessage(ConsoleMessage& message);

		void draw();

	protected:
		string name;

		bool auto_scroll = false;
		vector<ConsoleMessage> messages;

		vec4f getColor(ConsoleLevel level);
		vec4f colors[7] =
		{
			{1.0, 1.0f, 1.0f, 1.0f},//trace: White
			{0.0, 1.0f, 1.0f, 1.0f},//debug: Cyan
			{0.0, 1.0f, 0.0f, 1.0f},//info: Green
			{1.0, 1.0f, 0.0f, 1.0f},//warning: Yellow
			{1.0, 0.0f, 0.0f, 1.0f},//error: Red
			{1.0, 0.6f, 0.0f, 1.0f},//critical: Orange
			{1.0, 1.0f, 1.0f, 1.0f}	//default: White
		};
	};
}
