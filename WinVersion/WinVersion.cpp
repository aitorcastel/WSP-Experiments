// WinVersion.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BUILD_WINDOWS

#include <Windows.h>
#include <stdio.h>
#include <VersionHelpers.h>

#include <string>
#include <exception>

#pragma comment(lib, "version")


/*
GetVersionEx deprecated

new version helper apis (versionshelpers.h)
    iswindowsxpsp30orgreater, ...
implemented with verfyversioninfo

*Requieres a manifest to get correct information: 
    This is extracted from the c# project, just create a empty project, i create a console application and add a manifest file, in that file you have the manifest information the one we use in this project, just copy all and paste in our xml file ( remove some data, we need the compability secction)
*/

int main()
{
    // method 1: requires manifest.xml

    OSVERSIONINFO vi = { sizeof(vi) };
    ::GetVersionEx(&vi);
    printf("%u.%u.%u\n", vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber);

    // method 2: requires VersionHelpers.h

    if (!IsWindows10OrGreater())
    {
        /*TO DO
        * https://stackoverflow.com/questions/47581146/getting-os-build-version-from-win32-api-c
        * https://wikileaks.org/ciav7p1/cms/page_14587126.html
        * https://stackoverflow.com/questions/34065/how-to-read-a-value-from-the-windows-registry
        */

    }else
        MessageBox(NULL, L"You need at least Windows 10", L"Version Not Supported", MB_OK);

    // ---------------------------------------



    return 0;
}

