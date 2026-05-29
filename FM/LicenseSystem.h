#pragma once
#include "stdafx.h"
#include "FMGAME Engine.h"
// IPLISANS
#if XTREME_LISANS == 1


static string serverAdlari[] = { xorstr("SunucuName") };
// gün, ay, yıl
static uint32 lisansTarih[] = { 01, 01, 2040 };
// iki lisans şekli de aynı anda çalışır
// IP lisansları: Sadece dış IP kullanılıyor
// External IP: 178.208.187.43 (dış bağlantılar için)
static std::string ipLisanslari[] = {
	xorstr("178.208.187.43")    // External IP (dış bağlantılar için)
};
// x den öncesine bakar - subnet pattern'leri
// 192.168.2.xx: Local network subnet'i (192.168.2.0/24)
static string subnetLisanlar[] = { 
	xorstr("192.168.2.xx")      // Local network subnet (192.168.2.0/24)
};

extern DWORD WINAPI LicenseEngine(FMGAMEEngine* e);
extern bool IsLicensed(std::string ip);
#endif
