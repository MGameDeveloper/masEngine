#include "masPrint.h"

#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#define MAX_PATH 260

static uint32_t masVPrint(masChar* Buf, uint32_t Count, const masChar* Fmt, va_list Args)
{
	uint32_t OutCount = 0;
#if defined(UNICODE) || defined(_UNICODE)
	masChar NewFmt[256] = {};
	int32_t Idx = 0;
	
	uint64_t FmtLen = wcslen(Fmt);	
	for(int32_t i = 0; i < FmtLen; ++i)
	{
		if(Fmt[i] == MAS_CHAR('%') && Fmt[i + 1] == MAS_CHAR('s'))
		{
			NewFmt[Idx++] = MAS_CHAR('%');
			NewFmt[Idx++] = MAS_CHAR('l');
			NewFmt[Idx++] = MAS_CHAR('s');
			
			i += 2;
		}
		else
			NewFmt[Idx++] = Fmt[i];
	}
	
	if(Idx >= (MAX_PATH - 1))
	{
		MAS_LOG_ERROR("NewFmt is not null terminated string so it cant be passed to vswprintf func\n");
		return 0;
	}
	
    OutCount = vswprintf(Buf, Count, (const masChar*)NewFmt, Args);
#else
	OutCount = vsprintf(Buf, Count, Fmt, Args);
#endif

    return OutCount;
}


/********************************************************************************************
*
*********************************************************************************************/
void masPrint(const masChar* Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
#if defined(UNICODE) || defined(_UNICODE)
	masChar NewFmt[MAX_PATH] = {};
	int32_t Idx = 0;
	
	uint64_t FmtLen = wcslen(Fmt);	
	for(int32_t i = 0; i < FmtLen; ++i)
	{
		if(Fmt[i] == MAS_CHAR('%') && Fmt[i + 1] == MAS_CHAR('s'))
		{
			NewFmt[Idx++] = MAS_CHAR('%');
			NewFmt[Idx++] = MAS_CHAR('l');
			NewFmt[Idx++] = MAS_CHAR('s');
			
			i += 2;
		}
		else
			NewFmt[Idx++] = Fmt[i];
	}
	
    if(Idx >= (MAX_PATH - 1))
	{
		MAS_LOG_ERROR("NewFmt is not null terminated string so it cant be passed to vprintf func\n");
		return;
	}
	
	const masChar* const pFmt = NewFmt;
	vprintf(pFmt, Args);
#else
	vprintf(Fmt, Args);
#endif
    
	va_end(Args);
}


uint32_t masPrintTo(masChar* Buf, uint32_t Count, const masChar* Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	uint32_t OutCount = masVPrint(Buf, BufSize, Fmt, Args);
	va_end(Args);
	
	return OutCount;
}