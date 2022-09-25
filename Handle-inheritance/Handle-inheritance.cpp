// Handle-inheritance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <string>

int main() {

	HANDLE hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	printf("HANDLE: 0x%p\n", hEvent);

	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };
	std::wstring name(L"Notepad ");
	name += std::to_wstring((long long)hEvent);
	
	::SetHandleInformation(hEvent, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

	if (::CreateProcess(nullptr, (WCHAR*)name.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
		printf("PID: %u\n", pi.dwProcessId);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	return 0;
}