// set_app_as_default.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <ShlObj.h>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

const IID g_iid_aar[] =
{
	{0x4CD6ED57, 0x618E, 0x4B47, { 0xBD, 0x93, 0x09, 0x63, 0xD3, 0xEB, 0x4F, 0xF4 } },
	{0xC7225171, 0xB9A7, 0x4CF7, { 0x86, 0x1F, 0x85, 0xAB, 0x7B, 0xA3, 0xC5, 0xB2 } },
	{0x229D59E2, 0xF94A, 0x402E, { 0x9A, 0x9F, 0x3B, 0x84, 0xA1, 0xAC, 0xED, 0x77 } },
	{0x1C5C9D10, 0x1225, 0x4C97, { 0x8C, 0x51, 0x98, 0xE1, 0xB6, 0xF0, 0xD4, 0xE0 } },
	{0x14EBCC88, 0x2831, 0x4FC8, { 0xA5, 0xDF, 0x9F, 0x36, 0xA8, 0x1D, 0xB1, 0x2C } }
};

void set_app_as_default(const IID &iid, const UINT off, const WCHAR *papp_reg_name,
	const WCHAR *pset, ASSOCIATIONTYPE set_type)
{
	CoInitializeEx(NULL, NULL);

	OLECHAR *bstr_iid;
	StringFromCLSID(iid, &bstr_iid);

	IApplicationAssociationRegistration *paar = NULL;

	HRESULT hr = CoCreateInstance(
		CLSID_ApplicationAssociationRegistration,
		NULL,
		CLSCTX_INPROC_SERVER,
		iid,
		(void **)&paar);
	if (SUCCEEDED(hr) && paar)
	{
		hr = (*(int(__stdcall **)(PVOID, PVOID, PVOID, DWORD))(*(DWORD *)paar + off))
			(paar, (PVOID)papp_reg_name, (PVOID)pset, set_type);
		if (SUCCEEDED(hr))
		{
			LOGD << papp_reg_name << " assoc " << pset << " succeeded";
		}
		else
		{
			LOGE << papp_reg_name << " assoc " << pset << " failed: " << GetLastError();
		}

		paar->Release();
	}
	else
	{
		LOGE << "CoCreateInstance " << bstr_iid << " failed: " << GetLastError();
	}

	::CoTaskMemFree(bstr_iid);

	CoUninitialize();
}

int main()
{
	static plog::ColorConsoleAppender<plog::TxtFormatter> console_appender;
	plog::init(plog::verbose, &console_appender);

	for (size_t i = 0; i < sizeof(g_iid_aar) / sizeof(g_iid_aar[0]); i++)
	{
		set_app_as_default(g_iid_aar[i], 0x10, L"YOO.pdf", L".pdf", AT_FILEEXTENSION);
	}
}
