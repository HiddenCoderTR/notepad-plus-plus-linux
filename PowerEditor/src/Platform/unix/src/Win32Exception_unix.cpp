#include "Win32Exception.h"

void Win32Exception::installHandler() {}
void Win32Exception::removeHandler() {}
void Win32Exception::translate(unsigned, EXCEPTION_POINTERS*) {}
Win32Exception::Win32Exception(EXCEPTION_POINTERS* info)
{
	_info = info;
	_event = "Unix exception";
	_location = nullptr;
	_code = 0;
}
Win32AccessViolation::Win32AccessViolation(EXCEPTION_POINTERS* info) : Win32Exception(info) {}
