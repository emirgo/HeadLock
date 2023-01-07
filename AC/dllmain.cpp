#include "pch.h"
#include "Offset.h"
#include "Entity.h"

std::uintptr_t hook_addr = 0x0040AD9D; // _asm mov edi, eax

DWORD ori_call_addr = 0x4607C0;
DWORD ori_jump_addr = 0x0040AA2;

INPUT input = { 0 };
DWORD edi_value = 0;

__declspec(naked) void codecave()
{
    // call trigger hook and save all the registers
    // finally expose eax by returning
    __asm {
        call ori_cal_addr
        pushad
        mov edi_value, eax
    }
}

void Threaded()
{
    std::uintptr_t module_base = 0x00;
    entity* local_player = (entity*)(module_base + Offset::local_entity_base);

    DWORD old_protect = 0;
    while (true) {
        VirtualProtect((void*)hook_addr, 5, PAGE_EXECUTE_READWRITE, &old_protect);
        hook_addr = 0xE9;
        *(DWORD*)(hook_addr + 1) = (DWORD)&codecave - ((DWORD)hook_addr + 5);
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

