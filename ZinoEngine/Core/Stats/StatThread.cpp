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
		// (j'ai tout cassé ici du coup pour le moment j'implemente pas ça issou)
	}
}

void CStatThread::Stop()
{
	Loop = false;
}