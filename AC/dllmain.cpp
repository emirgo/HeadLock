#include "pch.h"
#include "Offset.h"
#include "Entity.h"

void Threaded()
{
    std::uintptr_t module_base = 0x00;
    entity* local_player = (entity*)(module_base + Offset::local_entity_base);

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
        MessageBox(0, 0, 0, 0);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Threaded, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

