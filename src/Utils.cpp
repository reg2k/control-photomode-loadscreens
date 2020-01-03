#include "Utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winver.h>
#pragma comment(lib, "Version.lib")

// reg2k
namespace Utils {
    bool ReadMemory(uintptr_t addr, void* data, size_t len) {
        DWORD oldProtect;
        if (VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy(data, (void*)addr, len);
            if (VirtualProtect((void*)addr, len, oldProtect, &oldProtect))
                return true;
        }
        return false;
    }

    void WriteMemory(uintptr_t addr, void* data, size_t len) {
        DWORD oldProtect;
        VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)addr, data, len);
        VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
    }

    uintptr_t GetRelative(uintptr_t start, int offset, int instructionLength) {
        int32_t rel32 = 0;
        ReadMemory(start + offset, &rel32, sizeof(int32_t));
        return start + instructionLength + rel32;
    }

    uint64_t GetGameVersion() {
        uint64_t version = 0;

        char szFileName[MAX_PATH + 1];
        GetModuleFileNameA(NULL, szFileName, MAX_PATH + 1);
        DWORD verSize = GetFileVersionInfoSizeA(szFileName, NULL);

        if (verSize != NULL) {
            LPSTR verData = new char[verSize];
            if (GetFileVersionInfoA(szFileName, NULL, verSize, verData)) {
                VS_FIXEDFILEINFO* verInfo = NULL;
                UINT size = 0;
                if (VerQueryValueA(verData, "\\", (void**)&verInfo, &size)) {
                    if (size) {
                        version = (((uint64_t)verInfo->dwFileVersionMS) << 32) | verInfo->dwFileVersionLS;
                    }
                }
            }
            delete[] verData;
        }

        return version;
    }
}