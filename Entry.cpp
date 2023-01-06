#include <Windows.h>


void Hook() 
{
	DWORD* local_entity = (DWORD*)0x00;

	while (true) {
		if (GetAsyncKeyState(VK_INSERT)) {
			// write
		}
		Sleep(1);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinst_dll, DWORD fdw_reason, LPVOID lpv_reserved) 
{
	if (fdw_reason == DLL_PROCESS_ATTACH) {
		MessageBox(0, 0, 0, 0);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Hook, NULL, 0, NULL);
	}

	return true;
}