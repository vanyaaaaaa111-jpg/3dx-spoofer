#ifndef MINHOOK_H
#define MINHOOK_H

#include <windows.h>

#define MH_ALL_HOOKS NULL

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MH_STATUS {
    MH_UNKNOWN = -1,
    MH_OK = 0,
    MH_ERROR_ALREADY_INITIALIZED,
    MH_ERROR_NOT_INITIALIZED,
    MH_ERROR_ALREADY_CREATED,
    MH_ERROR_NOT_CREATED,
    MH_ERROR_ENABLED,
    MH_ERROR_DISABLED,
    MH_ERROR_NOT_EXECUTABLE,
    MH_ERROR_UNSUPPORTED_FUNCTION,
    MH_ERROR_MEMORY_ALLOC,
    MH_ERROR_MEMORY_PROTECT,
    MH_ERROR_MODULE_NOT_FOUND,
    MH_ERROR_FUNCTION_NOT_FOUND
} MH_STATUS;

inline MH_STATUS __stdcall MH_Initialize(VOID) { return MH_OK; }
inline MH_STATUS __stdcall MH_Uninitialize(VOID) { return MH_OK; }
inline MH_STATUS __stdcall MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal) { return MH_OK; }
inline MH_STATUS __stdcall MH_EnableHook(LPVOID pTarget) { return MH_OK; }
inline MH_STATUS __stdcall MH_DisableHook(LPVOID pTarget) { return MH_OK; }
inline MH_STATUS __stdcall MH_RemoveHook(LPVOID pTarget) { return MH_OK; }

#ifdef __cplusplus
}
#endif

#endif // MINHOOK_H
