#include "masPrint.h"

#include "masDefs.h"

static uint32_t masVPrint(masChar* Buf, uint32_t Count, const masChar* Fmt, va_list Args)
{
	uint32_t OutCount = 0;
#if defined(UNICODE) || defined(_UNICODE)
	masChar NewFmt[256] = {};
	int32_t Idx = 0;
	
	int32_t Count = 0;
	while(*Fmt++)
		Count++;
	Fmt -= Count;
	
	for(int32_t i = 0; i < Count; ++i)
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
    OutCount = vswprintf(Buf, Count, NewFmt, Args);
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
	masVPrint(Buf, BufSize, Fmt, Args);
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