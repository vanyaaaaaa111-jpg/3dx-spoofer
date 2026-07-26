#include <windows.h>
#include <winternl.h>
#include <iostream>

typedef struct _PEB_LDR_DATA_FULL {
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA_FULL, *PPEB_LDR_DATA_FULL;

typedef struct _LDR_DATA_TABLE_ENTRY_FULL {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY_FULL, *PLDR_DATA_TABLE_ENTRY_FULL;

// 1. Hide DLL from PEB
void UnlinkModuleFromPEB(HMODULE hModule) {
    DWORD ppeb;
    __asm {
        mov eax, fs:[0x30]
        mov ppeb, eax
    }

    PPEB_LDR_DATA_FULL ldr = (PPEB_LDR_DATA_FULL)*(DWORD*)(ppeb + 0x0C);
    PLDR_DATA_TABLE_ENTRY_FULL curEntry = (PLDR_DATA_TABLE_ENTRY_FULL)ldr->InLoadOrderModuleList.Flink;

    while (curEntry->DllBase != NULL) {
        if (curEntry->DllBase == hModule) {
            curEntry->InLoadOrderLinks.Blink->Flink = curEntry->InLoadOrderLinks.Flink;
            curEntry->InLoadOrderLinks.Flink->Blink = curEntry->InLoadOrderLinks.Blink;

            curEntry->InMemoryOrderLinks.Blink->Flink = curEntry->InMemoryOrderLinks.Flink;
            curEntry->InMemoryOrderLinks.Flink->Blink = curEntry->InMemoryOrderLinks.Blink;

            curEntry->InInitializationOrderLinks.Blink->Flink = curEntry->InInitializationOrderLinks.Flink;
            curEntry->InInitializationOrderLinks.Flink->Blink = curEntry->InInitializationOrderLinks.Blink;
            break;
        }
        curEntry = (PLDR_DATA_TABLE_ENTRY_FULL)curEntry->InLoadOrderLinks.Flink;
    }
}

// 2. Block telemetry & crash reports
void BlockTelemetry() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        DWORD oldProtect;
        PVOID pNtQuery = GetProcAddress(hNtdll, "NtQueryInformationProcess");
        if (pNtQuery) {
            VirtualProtect(pNtQuery, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)pNtQuery = 0xC3; 
            VirtualProtect(pNtQuery, 5, oldProtect, &oldProtect);
        }
    }
}

void EnableAntiBanProtection(HMODULE hModule) {
    UnlinkModuleFromPEB(hModule);
    BlockTelemetry();
}
