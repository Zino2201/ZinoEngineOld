#include "Editor/LargeTask.h"
#include "Editor/ZEEditor.h"

namespace ze::editor
{

LargeTask::LargeTask(const std::string& in_text, const uint32_t& in_amount_of_work)
{
	EditorApp::get().push_task(EditorTask(in_text, in_amount_of_work));
}

LargeTask::~LargeTask()
{
	EditorApp::get().pop_task();
}

void LargeTask::work(const uint32_t& in_amount)
{
	EditorApp::get().get_top_task().completed_work += in_amount;
	EditorApp::get().draw_task();
}

}