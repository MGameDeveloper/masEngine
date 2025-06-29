#pragma once 

/*
__GNUC__
__clang__
__INTEL_COMPILER
*/

#ifdef _MSC_VER
    #define MAS_FUNC_NAME __FUNCTION__ 
#else
    #define MAS_FUNC_NAME "UNKNOWN_COMPILER_TYPE_FUNC_NAME"
#endif


//
#define MAS_LOG_ERROR(msg, ...)          _tprintf(_T("%s: ERROR -> "msg), _T(MAS_FUNC_NAME), ##__VA_ARGS__)
#define MAS_LOG_INFO(msg, ...)           _tprintf(_T("%s: INFO  -> "msg), _T(MAS_FUNC_NAME), ##__VA_ARGS__)
#define MAS_ADDR_FROM(type, ptr, offset) (type*)(((uint8_t*)ptr)+offset)
#define MAS_FUNC_TYPE(RET, NAME, ...)    typedef RET(*NAME)(##__VA_ARGS__)

#define MAS_ASSERT(x, msg, ...)                                               \
    if(!(x))                                                                  \
    {                                                                         \
    	_tprintf(_T("%s: ASSERT  -> "msg), _T(MAS_FUNC_NAME), ##__VA_ARGS__); \
		*(char*)0x0 = 0;                                                      \
	}

//
#if defined(UNICODE) || defined(_UNICODE)
    #define MAS_TEXT(T) L##T
	#define MAS_CHAR(C) L##C
	typedef wchar_t masChar;
#else
	#define MAS_TEXT(T) T
    #define MAS_CHAR(C) C
    typedef char masChar;
#endif


// To export API from game to be called by the engine
#if defined(MAS_GAME_EXPORT_API)
    #define MAS_GAME_API extern "C" __declspec(dllexport)
#else
	#define MAS_GAME_API extern "C" __declspec(dllimport)
#endif


// To export API from the engine to be called by the game
#if defined(MAS_ENGINE_EXPORT_API)
    #define MAS_ENGINE_API extern "C" __declspec(dllexport)
#else
	#define MAS_ENGINE_API extern "C" __declspec(dllimport)
#endif


