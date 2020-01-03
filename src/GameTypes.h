#pragma once
#include <cstdint>

//===============================
// Type Defintions
//===============================
namespace GameTypes {
    class UIGTView {
    public:
        virtual void Unk_00();
        virtual void Unk_01();
        virtual void Unk_02();
        virtual void Unk_03();
        virtual void Unk_04();
        virtual void Unk_05();
        virtual void Unk_06();
        virtual void Unk_07();
        virtual void Unk_08();
        virtual void Unk_09();
        virtual void Unk_10();
        virtual void Unk_11();
        virtual void Unk_12();
        virtual void Unk_13();
        virtual void Unk_14();
        virtual void Unk_15();
        virtual void Unk_16();
        virtual void Unk_17();
        virtual void Unk_18();
        virtual void Unk_19();
        virtual void Unk_20();
        virtual void Unk_21();
        virtual void Unk_22();
        virtual void Unk_23();
        virtual void Unk_24();
        virtual void Unk_25();
        virtual void Unk_26();
        virtual void Unk_27();
        virtual void Unk_28();
        virtual void Unk_29();
        virtual void Unk_30();
        virtual void Unk_31();
        virtual void Unk_32();
        virtual void Unk_33();
        virtual void Unk_34();
        virtual void Unk_35();
        virtual void Unk_36();
        virtual void Unk_37();
        virtual void Unk_38();
        virtual void Unk_39();
        virtual void Unk_40();
        virtual void Unk_41();
        virtual void Unk_42();
        virtual void Unk_43();
        virtual void Unk_44();
        virtual void Unk_45();
        virtual void Unk_46();
        virtual void Unk_47();
        virtual void Unk_48();
        virtual void Unk_49();
        virtual void Unk_50();
        virtual void Unk_51();
        virtual void Unk_52();
        virtual void Unk_53();
        virtual void Unk_54();
        virtual void Unk_55();
        virtual void Unk_56();
        virtual void Unk_57();
        virtual void Unk_58();
        virtual void Unk_59();
        virtual void Unk_60();
        virtual void ExecuteScript(const char* script, const char* frameSelector = nullptr); // 1E8 = hex 3D = 61
        // ...
    };

    class app_EventHandler {
    public:
        virtual ~app_EventHandler();
        virtual void dispatchAppEvent();
        virtual void Unk_02();
        virtual void Unk_03();
        virtual void Unk_04();
        virtual void Unk_05();
        virtual void Unk_06();
        virtual void Unk_07();
        virtual void Unk_08();
        virtual void Unk_09();
        virtual void Unk_10();
        virtual void Unk_11();
        virtual void Unk_12();
        virtual void Unk_13();
        virtual void Unk_14();
        virtual void Unk_15();
        virtual void Unk_16();
        virtual void Unk_17();
        virtual void Unk_18();
        virtual void Unk_19();
        virtual void Unk_20();
        virtual void Unk_21();
        virtual void Unk_22();
        virtual void Unk_23();
        virtual void Unk_24();
    };

    // ui::Page
    class UIPage : public app_EventHandler {
    public:
        virtual void onPageLoaded();       // 25
        virtual void update();             // 26
        virtual void onReadyForBindings(); // 27
        virtual void Unk_28();
    };

    //struct GameClient {
    //    void* vtbl;                     // 00
    //    uint64_t unk08[(0x290-0x08)/8]; // 08
    //    UIPage* uiIntroScreen;          // 290
    //    UIPage* uiSplashScreen;         // 298
    //    UIPage* uiMainMenu;             // 2A0
    //    UIPage* uiHud;                  // 2A8
    //    UIPage* uiLoadingScreen;        // 2B0
    //};

    //struct GameWindow {
    //    void* vtbl;                     // 00
    //    uint64_t unk08[(0x30-0x08)/8];  // 08
    //    void* gameClient;               // 30
    //};
}