#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>



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
			std::cout << std::endl;
			std::cout << "found" << std::endl;
		}
	} while (Process32Next(snapshot, &process_entry));

	while (true) {

	}

	return 0;
}
