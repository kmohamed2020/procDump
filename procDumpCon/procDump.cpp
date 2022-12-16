// procDumpCon.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <minidumpapiset.h>
#include <processthreadsapi.h>
#include <stdio.h>
#include <tchar.h>
#include <fileapi.h>
#include <minidumpapiset.h>
#include <tlhelp32.h>


void printError(const WCHAR* msg);
BOOL ProcMemDumper(HANDLE hProcess, DWORD ProcID, HANDLE hFile, MINIDUMP_TYPE DmpType);

// Function to List all running Thread for specific Process, by ProcessName
bool ExistingProcessDumper(WCHAR* ProcName, HANDLE hFile, MINIDUMP_TYPE DmpType);


int main(int argc, TCHAR* argv[])
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    HANDLE hFile;
    MINIDUMP_TYPE DmpType = MiniDumpWithFullMemory;
    TCHAR  ProcessName[MAX_PATH];



    std::string str = "C:\Windows\System32\lsass.exe"; // EXEMPLO
    std::wstring temp(str.begin(), str.end());

    LPWSTR imgPath = (LPWSTR)temp.c_str();

    printf("Enter the New Process Name (for ex, cmd.exe): ");
    std::wcin >> ProcessName;



    hFile = CreateFile(L"ProcMiniDump.DMP", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (!hFile)
    {
        printError(TEXT("\nFile Handle Error."));

        return 0;
    }


    if (!ExistingProcessDumper(ProcessName, hFile, DmpType))
    {
        printf("Enter the New Process path (for ex, C:\\Windows\\System32\\cmd.exe): ");
        std::wcin >> ProcessName;
        _tprintf(TEXT("NewProcName : %s.\n"), ProcessName);

        // Start the child process. 
        if (!CreateProcess(NULL, ProcessName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            printf("CreateProcess failed (%d).\n", GetLastError());
            return 0;
        }
        printf("\n Process Succesffuly Created with PRocess ID: %d.\n", pi.dwProcessId);
        ProcMemDumper(pi.hProcess, pi.dwProcessId, hFile, DmpType);
    }
    return TRUE;

}

void printError(const WCHAR* msg)
{
    DWORD errID;
    TCHAR errMSG[255];

    errID = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        errMSG, 256, NULL);

    _tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, errID, errMSG);
}
BOOL ProcMemDumper(HANDLE hProcess, DWORD ProcID, HANDLE hFile, MINIDUMP_TYPE DmpType)
{

    if (MiniDumpWriteDump(hProcess, ProcID, hFile, DmpType, NULL, NULL, NULL))
    {
        std::cout << "\nDump is Created Successfully.";
        return TRUE;
    }
    else
    {
        printError(TEXT("\nDump Not Created"));
        return FALSE;
    }
}

// Function to List all running Thread for specific Process, by ProcessName
bool ExistingProcessDumper(WCHAR* ProcName, HANDLE hFile, MINIDUMP_TYPE DmpType)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    HANDLE hProcess;
    bool procFound = FALSE;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
        return(FALSE);
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        printError(TEXT("Can not First Process")); // show cause of failure
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return(FALSE);
    }



    do
    {
        if (wcscmp(pe32.szExeFile, ProcName) == 0)
        {
            // Print OutputHeader
            _tprintf(TEXT("\n--------------------------------------------------------------------------------------------------------------"));
            _tprintf(TEXT("\nProcess Name\t\t\t|\t\tProcess ID\t\t|\tProcess Parent ID"));
            _tprintf(TEXT("\n--------------------------------------------------------------------------------------------------------------"));
            _tprintf(TEXT("\n%s"), pe32.szExeFile); //Print Process name
            _tprintf(TEXT("\t\t|\t\t%d"), pe32.th32ProcessID); //Print Process ID
            _tprintf(TEXT("\t\t|\t\t%d"), pe32.th32ParentProcessID); //Print Process Parent ID
            _tprintf(TEXT("\n--------------------------------------------------------------------------------------------------------------"));
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
            if (!hProcess) { printError(TEXT("Could not get Handle on the Process")); return FALSE; }
            if (ProcMemDumper(hProcess, pe32.th32ProcessID, hFile, DmpType))
            {
                CloseHandle(hProcessSnap);
                CloseHandle(hProcess);
                procFound = TRUE;
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    if (!procFound) { printError(TEXT("Targeted Process Not Found")); return FALSE; }
    return(procFound);
}
