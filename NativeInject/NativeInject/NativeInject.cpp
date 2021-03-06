// NativeInject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);
	

	return message;
}

int main()
{
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	NTSTATUS nt = CreateProcess(L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\GarrysMod\\bin\\hammer.exe", NULL, NULL, NULL, true, CREATE_SUSPENDED, NULL, NULL, &info, &processInfo);
	if (!nt) {
		std::cout << GetLastErrorAsString() << std::endl;
		return 0;
	}
	else {
		std::cout << "Successfuly created the process. ID: " << processInfo.dwProcessId << std::endl;
	}

	HANDLE token = NULL;

	TOKEN_PRIVILEGES privileges;
	
	NTSTATUS processtoken_status = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

	if (processtoken_status != FALSE)
	{
		LUID luid;
		if (LookupPrivilegeValueW(NULL, L"SeDebugPrivilege", &luid) != FALSE) {
			privileges.PrivilegeCount = 1;
			privileges.Privileges[0].Luid = luid;
			privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
				std::cout << "Could not escalate privileges!" << std::endl;;
				std::cout << GetLastErrorAsString() << std::endl;;
				return 0;
			}
		}
	}
	else {
		std::cout << "Could not adjust privileges. Oops." << std::endl;;
		return 0;
	}

	HANDLE newhandle = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		processInfo.dwProcessId);

	if (newhandle == NULL) {
		std::cout << "Target process is null" << std::endl;;
		return 0;
	}

	TCHAR full_dll_path[MAX_PATH];

	wchar_t dllnameshort[] = L"NativeInjectDLL.dll";

	LPVOID dll_path_addr = VirtualAllocEx(newhandle, NULL, sizeof(full_dll_path), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	if (!WriteProcessMemory(newhandle, dll_path_addr, (LPCVOID)full_dll_path, sizeof(full_dll_path), NULL)) {
		std::cout << "Could not write the path to target memory" << std::endl;;
		return 0;
	}

	HMODULE kernelhandle = GetModuleHandleW(L"Kernel32.dll");
	if (kernelhandle == NULL) {
		std::cout << "Could not find kernel32" << std::endl;
		return 0;
	}

	HANDLE library_addr = GetProcAddress(kernelhandle, "LoadLibraryW");
	if (library_addr == NULL) {
		std::cout << "Could not find LoadLibraryW" << std::endl;
		return 0;
	}

	LPTHREAD_START_ROUTINE lp_start_addr = (LPTHREAD_START_ROUTINE)library_addr;
	LPVOID lp_exec_param = (LPVOID)dll_path_addr;

	HANDLE remotethreadhandle =	CreateRemoteThread(newhandle, NULL, 0, lp_start_addr, lp_exec_param, 0, NULL);
	
	if (remotethreadhandle == NULL) {
		std::cout << "Remote thread creation failed!" << std::endl;
		return 0;
	}

	ResumeThread(remotethreadhandle);
	ResumeThread(processInfo.hThread);

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(newhandle);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
