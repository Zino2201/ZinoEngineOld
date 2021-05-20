#include "editor/windows/Console.h"
#include "console/Console.h"
#include "threading/Thread.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace ze::editor
{

ImVec4 to_color(const logger::SeverityFlagBits& severity)
{
	switch (severity)
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

ConsoleSink::ConsoleSink() : Sink("ConsoleWindow") {}

void ConsoleSink::log(const logger::Message& in_message)
{
	messages.emplace_back(in_message);
}

Console::Console() : Window("Console"), current_console_size(0), sink(nullptr) 
{
	std::unique_ptr<ConsoleSink> con_sink = std::make_unique<ConsoleSink>();
	sink = con_sink.get();
	ze::logger::add_sink(std::move(con_sink));

	memset(input.data(), 0, input.size());
}

std::vector<std::string> tokenize(const std::string& string, 
	const char& delimiter)
{
	std::vector<std::string> tokens;
	tokens.reserve(5);

	std::stringstream stream(string);
	std::string token;
	while (std::getline(stream, token, delimiter)) 
	{
		/** Remove tabs */
		token.erase(std::remove(token.begin(), token.end(), '\t'), token.end());
		if(!token.empty())
			tokens.push_back(std::move(token));
	}

	return tokens;
}

void Console::draw()
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

	/** Console */
	if(ImGui::BeginChild("ScrollingRegion", 
		ImVec2(0, -FooterHeight), true))
	{
		const auto& messages = sink->get_messages();
		for (const auto& Msg : messages)
		{
			{
				ui::SImGuiAutoStyleColor TextCol(ImGuiCol_Text, to_color(Msg.severity));
				ImGui::TextWrapped("%s", Msg.message.c_str());
			}

			if (ImGui::IsItemHovered())
			{
				// TODO: Optimize ?
				std::time_t Time = std::chrono::system_clock::to_time_t(Msg.time);
				std::tm* LocalTime = localtime(&Time);

				std::stringstream time_str;
				time_str << std::put_time(LocalTime, "%H:%M:%S");

				ImGui::SetTooltip("%s - %s", time_str.str().c_str(), 
					ze::threading::get_thread_name(Msg.thread_id).data());
			}
		}

		if (current_console_size != messages.size() &&
			ImGui::GetScrollY() == ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY();
			current_console_size = messages.size();
		}
	}
	ImGui::EndChild();

	/** Input */
	if(ImGui::InputTextWithHint("Command", "...", input.data(), input.size(),
		ImGuiInputTextFlags_EnterReturnsTrue,
		[](ImGuiInputTextCallbackData* InData) -> int
		{
			return reinterpret_cast<Console*>(InData->UserData)->on_text_edited(InData);
		},
		this))
	{
		std::vector<std::string> input_str = tokenize(input.data(), ' ');
		if(!input_str.empty())
		{
			std::vector<std::string_view> parameters;
			parameters.reserve(input_str.size() - 1);
			if(parameters.capacity() > 0)
			{
				if (parameters.capacity() > 1)
					std::copy(input_str.begin() + 1, input_str.end(), parameters.begin());
				else
					parameters.emplace_back(input_str[1]);
			}
			ze::logger::info("> {}", input.data());
			CConsole::Get().Execute(input_str[0], parameters);
			memset(input.data(), 0, input.size());
		}
		ImGui::SetKeyboardFocusHere(0);
	}

	ImGui::End();
}

int Console::on_text_edited(ImGuiInputTextCallbackData* in_data)
{
	switch(in_data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackHistory:
		{
			break;
		}
	}

	return 0;
}

}