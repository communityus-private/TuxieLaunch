//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (simple.cpp of simple.dll)
//
//  Microsoft Research Detours Package
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This DLL will detour the Windows SleepEx API so that TimedSleep function
//  gets called instead.  TimedSleepEx records the before and after times, and
//  calls the real SleepEx API through the TrueSleepEx function pointer.
//

#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "detours.h"
#include <string>

//#pragma warning( disable : 4996;)

std::wstring filestoredirect[] = {L"GameConfig.txt", L"CmdSeq.wc", L"" };


static HANDLE(WINAPI * TrueCreateFileA)(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) = CreateFileA;

static HANDLE(WINAPI * TrueCreateFileW)(
	LPCWSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
	) = CreateFileW;

static HANDLE (WINAPI * TrueFindFirstFileExW)(
	LPCWSTR            lpFileName,
	FINDEX_INFO_LEVELS fInfoLevelId,
	LPVOID             lpFindFileData,
	FINDEX_SEARCH_OPS  fSearchOp,
	LPVOID             lpSearchFilter,
	DWORD              dwAdditionalFlags
) = FindFirstFileExW;

static LSTATUS(WINAPI * TrueRegCreateKeyExA)(
	HKEY                        hKey,
	LPCSTR                      lpSubKey,
	DWORD                       Reserved,
	LPSTR                       lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
) = RegCreateKeyExA;

LSTATUS WINAPI RedirectedRegCreateKeyExA(
	HKEY                        hKey,
	LPCSTR                      lpSubKey,
	DWORD                       Reserved,
	LPSTR                       lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
) {
	if (strstr(lpSubKey, "Hammer") != NULL)
	{
		std::string changy = std::string(lpSubKey);
		changy = changy.replace(changy.find("Hammer"), sizeof("HammerTuxieLauncher") - 1, "HammerTuxieLauncher");
		//return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return TrueRegCreateKeyExA(hKey, changy.c_str(), Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	}
	else {
		return TrueRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	}
	//return TrueRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}


static LONG dwSlept = 0;
static DWORD(WINAPI * TrueSleepEx)(DWORD dwMilliseconds, BOOL bAlertable) = SleepEx;

HANDLE WINAPI RedirectedFindFirstFileExW(
	LPCWSTR             lpFileName,
	FINDEX_INFO_LEVELS fInfoLevelId,
	LPVOID             lpFindFileData,
	FINDEX_SEARCH_OPS  fSearchOp,
	LPVOID             lpSearchFilter,
	DWORD              dwAdditionalFlags
	)
{
	int arraypos = 0;
	while (!filestoredirect[arraypos].empty()) {
		if(wcsstr(lpFileName, filestoredirect[arraypos].c_str()))
		{
			std::wstring changy = std::wstring(lpFileName);
			changy = changy.substr(0, changy.length() - filestoredirect[arraypos].length());
			changy = changy + L"../TuxieLauncher/" + filestoredirect[arraypos];
			return TrueFindFirstFileExW(
				changy.c_str(),
				fInfoLevelId,
				lpFindFileData,
				fSearchOp,
				lpSearchFilter,
				dwAdditionalFlags
			);
		}
		++arraypos;
	}
	return TrueFindFirstFileExW(
		lpFileName,
		fInfoLevelId,
		lpFindFileData,
		fSearchOp,
		lpSearchFilter,
		dwAdditionalFlags
	);
}

DWORD WINAPI TimedSleepEx(DWORD dwMilliseconds, BOOL bAlertable)
{
	DWORD dwBeg = GetTickCount();
	DWORD ret = TrueSleepEx(dwMilliseconds, bAlertable);
	DWORD dwEnd = GetTickCount();

	InterlockedExchangeAdd(&dwSlept, dwEnd - dwBeg);

	return ret;
}

HANDLE WINAPI RedirectedCreateFileA(LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	
/*
	FILE* fp;
//	fopen_s()
	fopen_s(&fp, "C:\\logs\\detourdll.log", "rwa");
	fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
	fclose(fp);
	*/

	/*
	if (strstr(lpFileName, "GameConfig.txt") != NULL)
	{
		//return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return TrueCreateFileA("C:\\logs\\testconfig.txt", dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	} else {
		return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	*/
	return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI RedirectedCreateFileW(LPCWSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	int arraypos = 0;
	while (!filestoredirect[arraypos].empty()) {
		if (wcsstr(lpFileName, filestoredirect[arraypos].c_str()))
		{
			std::wstring changy = std::wstring(lpFileName);
			changy = changy.substr(0, changy.length() - filestoredirect[arraypos].length());
			changy = changy + L"../TuxieLauncher/" + filestoredirect[arraypos];
			return TrueCreateFileW(changy.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		}
		++arraypos;
	}
	return TrueCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	/*
	if (wcsstr(lpFileName, L"GameConfig.txt") != NULL)
	{
		std::wstring changy = std::wstring(lpFileName);
		changy = changy.substr(0,changy.length() - 14);
		changy = changy + L"../TuxieLauncher/GameConfig.txt";
		//return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return TrueCreateFileW(changy.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else 	if (wcsstr(lpFileName, L"CmdSeq.wc") != NULL)
	{
		std::wstring changy = std::wstring(lpFileName);
		changy = changy.substr(0, changy.length() - sizeof("CmdSeq.wc") + 1);
		changy = changy + L"../TuxieLauncher/CmdSeq.wc";
		//return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return TrueCreateFileW(changy.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	} else {
		return TrueCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	*/
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();

		printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
			" Starting.\n");
		fflush(stdout);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)TrueSleepEx, TimedSleepEx);
		DetourAttach(&(PVOID&)TrueCreateFileA, RedirectedCreateFileA);
		DetourAttach(&(PVOID&)TrueCreateFileW, RedirectedCreateFileW);
		DetourAttach(&(PVOID&)TrueFindFirstFileExW, RedirectedFindFirstFileExW);
		DetourAttach(&(PVOID&)TrueRegCreateKeyExA, RedirectedRegCreateKeyExA);
		error = DetourTransactionCommit();

		if (error == NO_ERROR) {
			printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
				" Detoured SleepEx().\n");
		}
		else {
			printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
				" Error detouring SleepEx(): %d\n", error);
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueSleepEx, TimedSleepEx);
		DetourDetach(&(PVOID&)TrueCreateFileA, RedirectedCreateFileA);
		DetourDetach(&(PVOID&)TrueCreateFileW, RedirectedCreateFileW);
		DetourDetach(&(PVOID&)TrueFindFirstFileExW, RedirectedFindFirstFileExW);
		DetourDetach(&(PVOID&)TrueRegCreateKeyExA, RedirectedRegCreateKeyExA);
		error = DetourTransactionCommit();

		printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
			" Removed SleepEx() (result=%d), slept %d ticks.\n", error, dwSlept);
		fflush(stdout);
	}
	return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
