#include "StatThread.h"

std::thread CStatThread::Handle;
std::atomic_bool CStatThread::Loop;
std::thread::id StatThreadID;

void CStatThread::Start()
{
	Loop = true;

	Handle = std::thread(&CStatThread::Main);
}

void CStatThread::Main()
{
	StatThreadID = std::this_thread::get_id();

	while (Loop)
	{
		// TODO: Finish
		// (j'ai tout cass� ici du coup pour le moment j'implemente pas �a issou)
	}
}

void CStatThread::Stop()
{
	Loop = false;
}