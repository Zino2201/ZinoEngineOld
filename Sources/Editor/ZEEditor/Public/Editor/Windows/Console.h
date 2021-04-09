#pragma once

#include "Window.h"
#include "Logger/Sink.h"
#include <array>

namespace ze::editor
{

class ConsoleSink : public logger::Sink
{
public:
	ConsoleSink();

	void log(const logger::Message& in_message) override;

	ZE_FORCEINLINE const auto& get_messages() const { return messages; }
private:
	std::vector<logger::Message> messages;
};

class Console : public Window
{
public:
	Console();
	int on_text_edited(ImGuiInputTextCallbackData* in_data);
protected:
	void draw();
private:
	std::array<char, 128> input;
	size_t current_console_size;
	ConsoleSink* sink;
};

}