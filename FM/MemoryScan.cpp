#include "StdAfx.h"
#include "FMGAME Engine.h"

void checkWriteProcessMemory() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    MEMORY_BASIC_INFORMATION mbi;
    char* address = nullptr;

    //Xtreme  Geçersiz kýlmak istediðiniz bellek adresleri buraya ke
    const std::vector<void*> exemptAddresses = {
        (void*)0x4A5620, // Örneðin: G_private_key2472
        (void*)0x4A65C3, // Örneðin: G_private_key2472
        (void*)0x4DDD7F, // Örneðin: G_private_key2472
        // ve diðerleri bitmez mk
    };

    while (address < sysInfo.lpMaximumApplicationAddress) {
        if (VirtualQuery(address, &mbi, sizeof(mbi))) {
            // Ýzin verilen bellek bölgelerini tanýmlayýn
            bool isExempt = false;
            for (const auto& exemptAddress : exemptAddresses) {
                if (address == exemptAddress) {
                    isExempt = true;
                    break;
                }
            }

            // Eðer adres geçersiz kýlýnacaksa kontrolü atla
            if (!isExempt &&
                (mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_READWRITE) &&
                !(mbi.State == MEM_COMMIT && (mbi.Type == MEM_PRIVATE || mbi.Type == MEM_IMAGE))) {
                // Xtreme Oyun içinde gerekli olan bellek alanlarýný geçersiz kýlmak için kontrolü atla
                Engine->Shutdown(xorstr("Cheat Memory Edited Detected! 0x01M"));
                return;
            }
            address += mbi.RegionSize;
        }
        else {
            break;
        }
    }
}


DWORD WINAPI memory_loop(LPVOID) {
    while (true) {
        checkWriteProcessMemory();
        Sleep(2000);
    }
    return 0;
}

void memory_scan() {
    CreateThread(NULL, 0, memory_loop, NULL, 0, NULL);
}
