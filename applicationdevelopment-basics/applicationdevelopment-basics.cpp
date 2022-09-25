// applicationdevelopment-basics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>
#include <Psapi.h>


// this function is for help to display de const char * msg in the messagebox
// ref: https://stackoverflow.com/questions/19715144/how-to-convert-char-to-lpcwstr
wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

// the error function
int ErrorForTerminal(const char* msg) {

    printf("%s (%u)\n", msg, ::GetLastError());
    //return 1;
    return ::GetLastError();
}

int ErrorForMessageBox1(const char* msg) {

    DWORD errorMessageID = ::GetLastError();
    WCHAR body_messagebox[30];
    swprintf_s(body_messagebox, L"Error code: %d", errorMessageID);

    LPCWSTR title_messagebox = convertCharArrayToLPCWSTR(msg);

    // using MB_OK because MB_ICONERROR dont look very nice
    MessageBox(nullptr, body_messagebox, title_messagebox, MB_OK);
    //return 1;
    return ::GetLastError();
}

int ErrorForMessageBox2(LPTSTR lpszFunction)
{
    // mod version from: https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
    // Retrieve the system error message for the last-error code


    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("Error %d: %s"),
        dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, lpszFunction, MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

int main()
{
    SYSTEM_INFO sysinfo;
    ::GetNativeSystemInfo(&sysinfo);

    printf("Processors: %u\n"
        "Page Size: %u bytes\n"
        "Processor mask: 0x%zX\n" // this z is for help conversion between 32/64 bit
        "Minimun process address: 0x%p\n"
        "Maximun process address: 0x%p\n",
        sysinfo.dwNumberOfProcessors,
        sysinfo.dwPageSize,
        sysinfo.dwActiveProcessorMask,
        sysinfo.lpMinimumApplicationAddress,
        sysinfo.lpMaximumApplicationAddress
    );

    // working with errors ...

    //HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, 6420);
    //
    //if (hProcess) {
    //    ::TerminateProcess(hProcess, 0);
    //}
    //else {
    //    //return ErrorForTerminal("Failed to open process");
    //    //return ErrorForMessageBox1("Failed to open process");
    //    ErrorForMessageBox2(convertCharArrayToLPCWSTR("Failed to open process"));
    //}


    // working with strings ...

    // ANSI version
    //::MessageBoxA(nullptr, "This is my string", "Strings DEMO ANSI", MB_OK | MB_ICONINFORMATION);

    //WCHAR buffer[128];
    //::StringCchPrintf(buffer, _countof(buffer), L"This is my string from process %u", ::GetCurrentProcessId());
    //::MessageBox(nullptr, buffer, L"Strings DEMO", MB_OK | MB_ICONINFORMATION);

    WCHAR path[MAX_PATH];
    ::GetSystemDirectory(path, _countof(path));
    printf("System Directory: %ws\n", path);

    WCHAR computerName[MAX_COMPUTERNAME_LENGTH+1]; // error 111 if we dont add "+1": https://microsoft.public.platformsdk.security.narkive.com/ImhMHeDB/getcomputername-and-lookupaccountsid
    DWORD len = _countof(computerName);
    if (::GetComputerName(computerName, &len))
        printf("Computer Name: %ws (%u)\n", computerName, len);
    else
        ErrorForMessageBox2(convertCharArrayToLPCWSTR("failed to get computer name"));


    // working with structures ...


    SHELLEXECUTEINFO sei{ sizeof(sei) };

    // the same that sei{sizeof...}
    //memset(&sei, 0, sizeof(sei));
    //sei.cbSize = sizeof(sei);

    sei.lpFile = L"c:\\windows\\win.ini";
    sei.lpVerb = L"open";
    sei.nShow = SW_SHOWNORMAL;

    ::ShellExecuteEx(&sei);


    // working with systeminformation
    /*some apis:
    * getsysteminfo, getnativesysteminfo (static info)
    * getperformanceinfo - psapi.h - (dinamic info)
    *  getcomputnername, getproductinfo, getsystemdirectory, getintegrateddisplaysize, getusername...
    */

    WCHAR name[64];
    DWORD size = _countof(name);
    if (::GetComputerName(name, &size))
        printf("Computer Name: %ws\n", name);

    size = _countof(name);
    if (::GetUserName(name, &size))
        printf("User Name: %ws\n", name);


    // hide de underline cursor in the terminal
    CONSOLE_CURSOR_INFO ci;
    if (::GetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &ci)) {
        ci.bVisible = FALSE;
        ::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    }

    PERFORMANCE_INFORMATION pi;
    printf("\n");

    for (;;) {
        if (!::GetPerformanceInfo(&pi, sizeof(pi)))
            break;
        printf("Processes: %4u\n", pi.ProcessCount);
        printf("Threads: %5u\n", pi.ThreadCount);
        printf("Handles: %7u\n", pi.HandleCount);
        printf("Committed: %7zu KB\n", pi.CommitTotal >> 10);
        printf("Kernel Pool: %5zu KB\n", pi.KernelTotal >> 10);
        ::Sleep(500);
        printf("\r\033[5A");
    }

    return 0;
}