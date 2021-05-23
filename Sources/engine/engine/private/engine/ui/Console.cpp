#include "engine/ui/Console.h"
#include "imgui/ImGui.h"
#include <iostream>
#include "console/Console.h"
#include <sstream>
#include "threading/Thread.h"
#include <ctime>
#include <iomanip>

namespace ze
{

/** Console sink */

ConsoleSink::ConsoleSink() : Sink("ConsoleWidget") {}	
	
void ConsoleSink::log(const logger::Message& InMessage)
{
	messages.emplace_back(InMessage);
}

CConsoleWidget::CConsoleWidget() : CurrentConsoleSize(0), Sink(nullptr)
{
	std::unique_ptr<ConsoleSink> ConSink = std::make_unique<ConsoleSink>();
	Sink = ConSink.get();
	ze::logger::add_sink(std::move(ConSink));

	memset(Input.data(), 0, Input.size());
}

std::vector<std::string> Tokenize(const std::string& InString, 
	const char& InDelimiter)
{
	std::vector<std::string> Tokens;
	Tokens.reserve(5);

	std::stringstream Stream(InString);
	std::string Token;
	while (std::getline(Stream, Token, InDelimiter)) 
	{
		/** Remove tabs */
		Token.erase(std::remove(Token.begin(), Token.end(), '\t'), Token.end());
		if(!Token.empty())
			Tokens.push_back(std::move(Token));
	}

	return Tokens;
}

void CConsoleWidget::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
	if (!ImGui::Begin("Console"))
	{
		ImGui::End();
		return;
	}

	const float FooterHeight =
		ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	using namespace logger;

	/** Filters */
	ImGui::PushStyleColor(ImGuiCol_Text, ToColor(SeverityFlagBits::Verbose));
	ImGui::Button("VERBOSE");
	ImGui::PopStyleColor();

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, ToColor(SeverityFlagBits::Info));
	ImGui::Button("INFO");
	ImGui::PopStyleColor();

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, ToColor(SeverityFlagBits::Warn));
	ImGui::Button("WARN");
	ImGui::PopStyleColor();

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, ToColor(SeverityFlagBits::Error));
	ImGui::Button("ERROR");
	ImGui::PopStyleColor();

	ImGui::Separator();

	/** Console */
	if(ImGui::BeginChild("ScrollingRegion",
		ImVec2(0, -FooterHeight), true, ImGuiWindowFlags_HorizontalScrollbar))
	{
		const auto& Messages = Sink->get_messages();
		for (const auto& Msg : Messages)
		{
			{
				ui::SImGuiAutoStyleColor TextCol(ImGuiCol_Text, ToColor(Msg.severity));
				ImGui::TextWrapped("%s", Msg.message.c_str());
			}

			if (ImGui::IsItemHovered())
			{
				// TODO: Optimize ?
				std::time_t Time = std::chrono::system_clock::to_time_t(Msg.time);
				std::tm* LocalTime = localtime(&Time);

				std::stringstream TimeStr;
				TimeStr << std::put_time(LocalTime, "%H:%M:%S");

				ImGui::SetTooltip("%s - %s", TimeStr.str().c_str(), 
					ze::threading::get_thread_name(Msg.thread_id).data());
			}
		}

		if (CurrentConsoleSize != Messages.size() &&
			ImGui::GetScrollY() == ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY();
			CurrentConsoleSize = Messages.size();
		}
	}
	ImGui::EndChild();

	/** Input */
	if(ImGui::InputTextWithHint("Command", "...", Input.data(), Input.size(),
		ImGuiInputTextFlags_EnterReturnsTrue,
		[](ImGuiInputTextCallbackData* InData) -> int
		{
			return reinterpret_cast<CConsoleWidget*>(InData->UserData)->OnTextEdited(InData);
		},
		this))
	{
		std::vector<std::string> InputStr = Tokenize(Input.data(), ' ');
		if(!InputStr.empty())
		{
			std::vector<std::string_view> Parameters;
			Parameters.reserve(InputStr.size() - 1);
			if(Parameters.capacity() > 0)
			{
				if (Parameters.capacity() > 1)
					std::copy(InputStr.begin() + 1, InputStr.end(), Parameters.begin());
				else
					Parameters.emplace_back(InputStr[1]);
			}
			ze::logger::info("> {}", Input.data());
			CConsole::Get().Execute(InputStr[0], Parameters);
			memset(Input.data(), 0, Input.size());
		}
		ImGui::SetKeyboardFocusHere(0);
	}

	ImGui::End();
}

int CConsoleWidget::OnTextEdited(ImGuiInputTextCallbackData* InData)
{
	switch(InData->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackHistory:
		{
			
			break;
		}
	}

	return 0;
}

ImVec4 CConsoleWidget::ToColor(const logger::SeverityFlagBits& InSeverity) const
{
	switch (InSeverity)
	{
	case logger::SeverityFlagBits::Verbose:
		return ImVec4(0.22f, 0.58f, 0.62f, 1);
	default:
	case logger::SeverityFlagBits::Info:
		return ImVec4(1, 1, 1, 1);
	case logger::SeverityFlagBits::Warn:
		return ImVec4(0.97f, 0.94f, 0.47f, 1);
	case logger::SeverityFlagBits::Error:
		return ImVec4(0.90f, 0.28f, 0.24f, 1);
	case logger::SeverityFlagBits::Fatal:
		return ImVec4(0.9f, 0, 0, 1);
	}
}

}