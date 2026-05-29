#include "LicenseSystem.h"
#if XTREME_LISANS == 1

#define GUN 0
#define AY 1
#define YIL 2

static const string SERVER_INI = xorstr("Server.ini");
struct tm* timeInfoEx;

DWORD WINAPI LicenseEngine(FMGAMEEngine* e)
{
	time_t rawtime;
	time(&rawtime);
	timeInfoEx = localtime(&rawtime);

	string Path = e->m_BasePath + SERVER_INI;
	char INICurrentIP[16];
	while (true)
	{
		// Server.ini'den IP okuma: Default IP 178.208.187.43 (dış IP)
		GetPrivateProfileStringA(xorstr("Server"), xorstr("IP0"), xorstr("178.208.187.43"), INICurrentIP, 16, Path.c_str());

		bool kappa = false;

		uint32 yil = timeInfoEx->tm_year + 1900;

		if (yil > lisansTarih[YIL])
			kappa = true;
		else if (timeInfoEx->tm_mon > lisansTarih[AY] - 1 && yil == lisansTarih[YIL])
			kappa = true;
		else if (timeInfoEx->tm_mday > lisansTarih[GUN] && timeInfoEx->tm_mon == lisansTarih[AY] - 1 && yil == lisansTarih[YIL])
			kappa = true;

		/*if (!IsLicensed(string(INICurrentIP)))
			kappa = true;*/     // Server.ini Lisans� kapat�ld�.

		if (!e->m_connectedIP.empty() && !IsLicensed(e->m_connectedIP))
			kappa = true;

		if (kappa)
		{
			e->power = false;
			exit(0);
			FreeLibrary(GetModuleHandle(NULL));
			TerminateProcess(GetCurrentProcess(), 0);
		}

		Sleep(5000);
	}
}

bool IsLicensed(std::string ip)
{
	bool ret = false;
	for (string pattern : subnetLisanlar)
	{
		const char* tmp = pattern.c_str(); // Subnet pattern (örn: 192.168.2.xx)
		bool f = true;
		for (size_t i = 0; i < pattern.length(); i++)
			if (tmp[i] != 'x' && tmp[i] != ip.c_str()[i])
				f = false;
		ret = f;
	}

	for (string license : ipLisanslari)
		if (ip == license)
			ret = true;

	return ret;
}

#endif