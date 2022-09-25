// PrivateSharing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#define PRIVATE_NAMESPACE L"MyPrivateNamespace"

void Write(void* p);
void Read(void* p);

int Error(const char* msg) {
	printf("%s (%u)\n", msg, ::GetLastError());
	return 1;
}

int main() {
	// create the boundary descriptor
	HANDLE hBD = ::CreateBoundaryDescriptor(L"MyDescriptor", 0);
	if (!hBD)
		return Error("Failed to create boundary descriptor");

	BYTE sid[SECURITY_MAX_SID_SIZE];
	auto psid = (PSID)sid;
	DWORD sidLen;
	if (!::CreateWellKnownSid(WinBuiltinUsersSid, nullptr, psid, &sidLen))
		return Error("Failed to create SID");

	if (!::AddSIDToBoundaryDescriptor(&hBD, psid))
		return Error("Failed to add SID to Boundary Descriptor");

	HANDLE hNamespace = ::CreatePrivateNamespace(nullptr, hBD, PRIVATE_NAMESPACE);
	if (!hNamespace) { // maybe created already?
		hNamespace = ::OpenPrivateNamespace(hBD, PRIVATE_NAMESPACE);
		if (!hNamespace)
			return Error("Failed to create/open private namespace");
	}

	HANDLE hMemMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 1 << 16,
		PRIVATE_NAMESPACE L"\\MySharedMemory");
	if (!hMemMap)
		return Error("Failed to create/open shared memory");

	void* buffer = ::MapViewOfFile(hMemMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!buffer)
		return Error("Failed to map shared memory");

	printf("PID: %u: Shared memory created/opened (H=0x%p), mapped to 0x%p\n",
		::GetCurrentProcessId(), hMemMap, buffer);

	bool quit = false;
	while (!quit) {
		printf("1=write, 2=read, 0=quit: ");
		int selection = _getch();
		printf("%c\n", selection);
		switch (selection) {
		case '1':
			Write(buffer);
			break;

		case '2':
			Read(buffer);
			break;

		case '0':
			quit = true;
			break;
		}
	}
	::UnmapViewOfFile(buffer);
	::CloseHandle(hMemMap);

}

void Write(void* p) {
	printf(">> ");
	char text[128];
	gets_s(text);
	strcpy_s((char*)p, _countof(text), text);
}

void Read(void* p) {
	printf("%s\n", (const char*)p);
}
