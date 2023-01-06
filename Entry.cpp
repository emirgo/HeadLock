#include <iostream>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>

const char* dll_path = "C:\\dev\\headlock\\AC.dll";

int main(void)
{
	HANDLE snapshot, process = 0;
	PROCESSENTRY32 process_entry = { 0 };

	process_entry.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(snapshot, &process_entry);

	std::cout << "scanning";
	do {
		std::cout << ".";
		if (wcscmp(process_entry.szExeFile, L"ac_client.exe") == 0) {
			std::cout << "\n\n\n\n[target located]" << std::endl;

			std::cout << "opening handle to: ";
			std::wcout << process_entry.szExeFile;
			process = OpenProcess(PROCESS_ALL_ACCESS, true, process_entry.th32ProcessID);
			std::cout << " [OK]" << std::endl;

			std::cout << "allocating memory in target process";
			void *base_addr = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
			WriteProcessMemory(process, base_addr, dll_path, strlen(dll_path) + 1, NULL);
			std::cout << " [OK]" << std::endl;

			std::cout << "fetching kernel base";
			HMODULE kernel_base = GetModuleHandle(L"kernel32.dll");
			std::cout << " [OK]" << std::endl;

			std::cout << "launching thread in target process";
			HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel_base, "LoadLibraryA"), base_addr, 0, NULL);
			std::cout << " [OK]" << std::endl;

			std::cout << "checking.";
			WaitForSingleObject(thread, INFINITE);
			std::cout << ".";
			DWORD exit_code = 0;
			GetExitCodeThread(thread, &exit_code);
			std::cout << ". [OK]" << std::endl;

			std::cout << "freeing memory and closing handles";
			VirtualFreeEx(process, base_addr, 0, MEM_RELEASE);
			CloseHandle(thread);
			CloseHandle(process);
			std::cout << " [OK]" << std::endl;
			break;
		}
	} while (Process32Next(snapshot, &process_entry));

	std::cout << "process finished" << std::endl;

	while (true) {
		
	}

	return 0;
}
