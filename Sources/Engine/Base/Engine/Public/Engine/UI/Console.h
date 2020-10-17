#pragma once

#include "EngineCore.h"
#include <array>
#include "Logger/Sink.h"

struct ImGuiInputTextCallbackData;
struct ImVec4;

namespace ze
{

class ENGINE_API ConsoleSink : public logger::Sink
{
public:
	ConsoleSink();

	void log(const logger::Message& InMessage) override;

	ZE_FORCEINLINE const auto& get_messages() const { return messages; }
private:
	std::vector<logger::Message> messages;
};

/**
 * Console widget
 */
class ENGINE_API CConsoleWidget
{
public:
	CConsoleWidget();

	void Draw();
private:
	int OnTextEdited(ImGuiInputTextCallbackData* InData);
	ImVec4 ToColor(const logger::SeverityFlagBits& InSeverity) const;
private:
	std::array<char, 32> Input;
	size_t CurrentConsoleSize;
	ConsoleSink* Sink;
};

}