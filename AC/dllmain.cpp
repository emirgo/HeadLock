#include "pch.h"
#include <Windows.h>
#include <TlHelp32.h>

#include "Driver.h"
#include "Offset.h"
#include "Entity.h"

void Threaded(HMODULE module)
{
    Offset::module_base = (std::uintptr_t)GetModuleHandle(L"ac_client.exe");
    while (true) {

        Sleep(1);
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Threaded, hModule, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

