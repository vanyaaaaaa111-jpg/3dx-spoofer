#include <windows.h>
#include <iphlpapi.h>
#include <iostream>
#include <random>
#include "MinHook.h"

#pragma comment(lib, "IPHLPAPI.lib")

// Random serial generator
DWORD GetRandomSerial() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<DWORD> dis(0x10000000, 0xFFFFFFFF);
    return dis(gen);
}

// 1. GetVolumeInformationW Hook (Disk Serial)
typedef BOOL(WINAPI* pGetVolumeInformationW)(
    LPCWSTR, LPWSTR, DWORD, LPDWORD, LPDWORD, LPDWORD, LPWSTR, DWORD
);
pGetVolumeInformationW OrigGetVolumeInformationW = NULL;

BOOL WINAPI HookGetVolumeInformationW(
    LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize
) {
    BOOL result = OrigGetVolumeInformationW(
        lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize,
        lpVolumeSerialNumber, lpMaximumComponentLength,
        lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize
    );

    if (result && lpVolumeSerialNumber) {
        *lpVolumeSerialNumber = GetRandomSerial();
    }
    return result;
}

// 2. GetAdaptersInfo Hook (MAC Address)
typedef DWORD(WINAPI* pGetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);
pGetAdaptersInfo OrigGetAdaptersInfo = NULL;

DWORD WINAPI HookGetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen) {
    DWORD result = OrigGetAdaptersInfo(pAdapterInfo, pOutBufLen);

    if (result == ERROR_SUCCESS && pAdapterInfo) {
        PIP_ADAPTER_INFO curr = pAdapterInfo;
        while (curr) {
            for (UINT i = 0; i < curr->AddressLength; i++) {
                curr->Address[i] = static_cast<BYTE>(rand() % 255);
            }
            curr = curr->Next;
        }
    }
    return result;
}

// Hook Initialization
DWORD WINAPI MainThread(LPVOID lpParam) {
    if (MH_Initialize() == MH_OK) {
        MH_CreateHook(&GetVolumeInformationW, &HookGetVolumeInformationW, reinterpret_cast<LPVOID*>(&OrigGetVolumeInformationW));
        MH_CreateHook(&GetAdaptersInfo, &HookGetAdaptersInfo, reinterpret_cast<LPVOID*>(&OrigGetAdaptersInfo));
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    }
    return TRUE;
}
