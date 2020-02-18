#pragma once

#ifdef _DEBUG
#define verify(condition) if(!(condition)) __debugbreak()
#define must(condition) if(!(condition)) { __debugbreak(); _CrtDumpMemoryLeaks(); exit(-1); }
#else
#define verify(condition)
#define must(condition)
#endif