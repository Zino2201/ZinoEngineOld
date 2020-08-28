#pragma once

#include "EngineCore.h"
#include <array>
#include "Logger/Sink.h"

struct ImGuiInputTextCallbackData;
struct ImVec4;

namespace ZE
{

class ENGINE_API CConsoleSink : public Logger::CSink
{
public:
	CConsoleSink();

	void Log(const Logger::SMessage& InMessage) override;

	ZE_FORCEINLINE const auto& GetMessages() const { return Messages; }
private:
	std::vector<Logger::SMessage> Messages;
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
	ImVec4 ToColor(const Logger::ESeverityFlagBits& InSeverity) const;
private:
	std::array<char, 32> Input;
	size_t CurrentConsoleSize;
	CConsoleSink* Sink;
};

}