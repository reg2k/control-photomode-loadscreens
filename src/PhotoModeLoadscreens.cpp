#include "PhotoModeLoadscreens.h"

#include "Logger.h"
#include "PhotoServer.h"
#include "GameTypes.h"
#include "Utils.h"
#include "rva/RVA.h"
#include "minhook/include/MinHook.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>

#include <cinttypes>
#include <cstdio>
#include <string>
#include <unordered_map>

#define INI_LOCATION "./plugins/PhotomodeLoadscreens.ini"

using namespace GameTypes;

//===================
// Globals
//===================
Logger g_logger;
std::unordered_map<std::string, UIPage*> g_menuPagesMap; // map of menu paths to UI pages.
int g_photoServerPort = 0;
std::string g_scriptToRun = "";

//===================
// Addresses [1]
//===================
using _GameUIPage_Ctor = void*(*)(UIPage* page, const char* uiPath, bool a3, int a4);
RVA<_GameUIPage_Ctor> GameUIPage_Ctor("E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03 48 8D 05 ? ? ? ? 48 89 43 08 48 8D 8B 00 01 00 00", 0, 1, 5);
_GameUIPage_Ctor GameUIPage_Ctor_Original = nullptr;

//===============================
// Methods retrieved at runtime
//===============================

using _ui_Page_getView = UIGTView*(*)(UIPage*);
_ui_Page_getView ui_Page_getView = nullptr;

using _uiLoadingScreen_onReadyForBindings = void(*)(UIPage*);
_uiLoadingScreen_onReadyForBindings uiLoadingScreen_onReadyForBindings_Original = nullptr;

//===============================
// Utilities
//===============================

HMODULE GetRMDModule(const char* modName) {
    char szModuleName[MAX_PATH] = "";
    snprintf(szModuleName, sizeof(szModuleName), "%s_rmdwin7_f.dll", modName);
    HMODULE hMod = GetModuleHandleA(szModuleName);

    if (!hMod) {
        snprintf(szModuleName, sizeof(szModuleName), "%s_rmdwin10_f.dll", modName);
        hMod = GetModuleHandleA(szModuleName);
    }

    if (!hMod) {
        _LOG("WARNING: Could not get module: %s.", modName);
    }
    return hMod;
}

//=============
// Main
//=============

const char* scriptToRun1 = R"(!function(){var PORT=)";
const char* scriptToRun2 = R"(;function waitForSelector(t,o,e){var i=document.querySelector(t);i?e(i):window.setTimeout(function(){waitForSelector(t,o,e)},o)}function main(t){var o=t.cloneNode();t.style.opacity=0,o.style.opacity=0,o.style.transition="opacity 500ms linear",o.onload=function(){o.style.opacity=1},o.setAttribute("src","http://127.0.0.1:"+PORT+"/random_photo"),t.parentNode.insertBefore(o,t)}window.setTimeout(function(){waitForSelector(".loading-video",100,main)},50)}();)";

namespace PhotoModeLoadscreens {
    void uiLoadingScreen_onReadyForBindings_Hook(UIPage* uiPage) {
        uiLoadingScreen_onReadyForBindings_Original(uiPage);
        UIGTView* uiView = ui_Page_getView(uiPage);
        uiView->ExecuteScript(g_scriptToRun.c_str());
        _LOG("Loading screen ready.");
    }

    // Hook UI::Page::onReadyForBindings
    void HookPageReady(UIPage* uiPage) {
        _LOG("Hooking loading screen: %p", uiPage);
        uintptr_t* vtbl = reinterpret_cast<uintptr_t**>(uiPage)[0];
        uintptr_t* vtbl_onReadyForBindings = &vtbl[27]; // (+0xD8)

        _LOG("VFT start: %p, target: %p", vtbl, vtbl_onReadyForBindings);

        // Store original function
        uiLoadingScreen_onReadyForBindings_Original = reinterpret_cast<_uiLoadingScreen_onReadyForBindings>(*vtbl_onReadyForBindings);
        _LOG("Original func at: %p", uiLoadingScreen_onReadyForBindings_Original);

        // Commit hook to VFT
        uintptr_t hookAddr = (uintptr_t)uiLoadingScreen_onReadyForBindings_Hook;
        Utils::WriteMemory((uintptr_t)vtbl_onReadyForBindings, &hookAddr, sizeof(uintptr_t));

        _LOG("Hook done");
    }

    void* GameUIPage_Ctor_Hook(UIPage* uiPage, const char* uiPath, bool a3, int a4) {
        //_LOG("GameUIPage_Ctor_Hook for uiPath %s and uipage at %p", uiPath, uiPage);

        if (strcmp(uiPath, R"(uiresources\p7\system.ui)") != 0) {
            // Store pointers to UIPages
            g_menuPagesMap.emplace(uiPath, uiPage);
        }
        else {
            // Ready to hook
            auto it = g_menuPagesMap.find(R"(uiresources\p7\loadingscreen.ui)");
            if (it != g_menuPagesMap.end()) {
                HookPageReady(it->second);
            }
            else {
                _LOG("FATAL: Could not hook loading screen.");
            }
        }
        return GameUIPage_Ctor_Original(uiPage, uiPath, a3, a4);
    }

    bool InitAddresses() {
        _LOG("Sigscan start");
        RVAUtils::Timer tmr; tmr.start();
        RVAManager::UpdateAddresses(0);

        _LOG("Sigscan elapsed: %llu ms.", tmr.stop());

        // Check if all addresses were resolved
        for (auto rvaData : RVAManager::GetAllRVAs()) {
            if (!rvaData->effectiveAddress) {
                _LOG("Not resolved: %s", rvaData->sig);
            }
        }
        if (!RVAManager::IsAllResolved()) return false;

        return true;
    }

    void Init() {
        char logPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, NULL, logPath))) {
            strcat_s(logPath, "\\Remedy\\Control\\PhotomodeLoadscreens.log");
            g_logger.Open(logPath);
        }
        _LOG("Photomode Loadscreens v1.0 by reg2k");
        _LOG("Game version: %" PRIX64, Utils::GetGameVersion());
        _LOG("Module base: %p", GetModuleHandle(NULL));

        // Get required modules
        HMODULE hUIModule = GetRMDModule("ui");
        if (!hUIModule) {
            _LOG("FATAL: Could not get ui module.");
            return;
        }

        // Get required function addresses
        ui_Page_getView = reinterpret_cast<_ui_Page_getView>(GetProcAddress(hUIModule, "?getView@Page@ui@@QEAAPEAVView@UIGT@Coherent@@XZ"));
        if (!ui_Page_getView) {
            _LOG("FATAL: No ui_Page_getView");
            return;
        }

        // Sigscan
        if (!InitAddresses()) {
            MessageBoxA(NULL, "Photomode Loadscreens is not compatible with this version of Control.\nPlease visit the mod page for updates.", "Photomode Loadscreens", MB_OK | MB_ICONEXCLAMATION);
            _LOG("FATAL: Incompatible version");
            return;
        }
        _LOG("Addresses set");        

        // Get photo directory
        // Use user-provided directory, if explicitly specified.
        char photosDir[MAX_PATH] = "";
        GetPrivateProfileStringA("PhotomodeLoadscreens", "sPhotosDir", "", photosDir, sizeof(photosDir), INI_LOCATION);

        if (photosDir[0] == NULL) {
            // Otherwise, use default photomode directory
            if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, NULL, photosDir))) {
                strcat_s(photosDir, "\\Control Game\\screenshots");
            }
            else {
                _LOG("FATAL: Unable to locate photos directory.");
                return;
            }
        }
        else {
            _LOG("INFO: Custom photo directory specified.");
        }
        
        _LOG("Photos directory: %s", photosDir);

        // Start photo server (local loopback access only, no remote access)
        _LOG("Starting photo server...");
        int port = PhotoServer::Start(photosDir);
        g_photoServerPort = port;
        if (g_photoServerPort) {
            _LOG("Photo server started on port %d. (local loopback access only)", port);
            g_scriptToRun = scriptToRun1;
            g_scriptToRun += std::to_string(port);
            g_scriptToRun += scriptToRun2;
            //_LOG("Script is: %s", g_scriptToRun.c_str());
        }
        else {
            _LOG("FATAL: Failed to start photo server.");
            return;
        }

        // Apply hooks
        MH_Initialize();
        MH_CreateHook(GameUIPage_Ctor, GameUIPage_Ctor_Hook, reinterpret_cast<LPVOID*>(&GameUIPage_Ctor_Original));
        if (MH_EnableHook(GameUIPage_Ctor) != MH_OK) {
            _LOG("FATAL: Failed to install UI hook.");
            return;
        }
    }
}