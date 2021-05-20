#include "App.h"
#include <cstdlib>

namespace ze::app
{

App::App(const int& argc, const char** argv) : err_code(0), _run(true)
{ 
	ZE_ASSERT(!current_app); current_app = this; 
}

int App::run()
{
	while(_run)
	{
		process_events();
		loop();
	}

	return err_code;
}

void App::exit(const int& in_err_code)
{
	err_code = in_err_code;
	_run = false;
}

void exit(const int& err_code)
{
	App::get()->exit(err_code);
}

}