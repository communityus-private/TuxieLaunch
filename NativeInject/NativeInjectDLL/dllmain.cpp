// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include <fstream>

DWORD WINAPI Thread_no_1(LPVOID lpParam)
{
	std::fstream myfile;
	myfile.open("C:\\logs\\example.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	myfile << "Writing this to a file.\n";
	myfile.close();

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	std::fstream myfile;
	myfile.open("C:\\logs\\example.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	myfile << "Writing this to a file.\n";
	myfile.close();

	return 0;
	/*
	HANDLE Handle_Of_Thread_1 = CreateThread(NULL, 0, Thread_no_1, 0, 0, NULL);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
	*/
}

