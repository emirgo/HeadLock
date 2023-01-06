#include <Windows.h>

void Hook() 
{
	while (true) {
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