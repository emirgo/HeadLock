#include "pch.h"
#include <Windows.h>
#include <TlHelp32.h>

#include "Driver.h"
#include "Offset.h"
#include "Entity.h"

void Threaded(HMODULE module)
{
    std::uintptr_t module_base = (std::uintptr_t)GetModuleHandle(L"ac_client.exe");
    while (true) {
        // std::uintptr_t* local_player = (std::uintptr_t*)(module_base + Offset::local_player);
        entity* local_player = *(entity**)(module_base + Offset::local_player);

        if (local_player) {
            // *(int*)(*local_player + Offset::health) = 1337;
            local_player->health = 1337;
        }
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

