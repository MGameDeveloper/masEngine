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


#define MAS_LOG_ERROR(msg, ...) printf("%s: ERROR -> "msg, MAS_FUNC_NAME, ##__VA_ARGS__)
#define MAS_LOG_INFO(msg, ...)  printf("%s: INFO  -> "msg, MAS_FUNC_NAME, ##__VA_ARGS__)


#define MAS_ADDR_FROM(type, ptr, offset) (type*)(((uint8_t*)ptr)+offset)


#define MAS_FUNC_TYPE(RET, NAME, ...) typedef RET(*NAME)(##__VA_ARGS__)


#ifndef MAS_GAME_API
    #define MAS_API extern "C" __declspec(dllimport)
#else
	#define MAS_API extern "C" __declspec(dllexport)
#endif


#if defined(UNICODE) || defined(_UNICODE)
    #define MAS_TEXT(T) L##T
#else
	#define MAS_TEXT(T) T 
#endif
