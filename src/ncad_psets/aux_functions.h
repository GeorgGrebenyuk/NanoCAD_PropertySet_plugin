#ifndef __AUX_FUNCTIONS_H_
#define __AUX_FUNCTIONS_H_
#include "stdafx.h"
#include "string"
#include <AtlBase.h>
#include <atlconv.h>
#include <windows.h>
#include <Lmcons.h>
#include <sstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
class aux_functions {
public:
	static std::wstring ToWStringFromString(std::string str)
	{
		CA2W ca2w(str.c_str());
		std::wstring wide = ca2w;
		return wide;
	}
	static BSTR ToBSTRFromString(std::string str)
	{
		BSTR bstr = ::SysAllocString(aux_functions::
			ToWStringFromString(str.c_str()).c_str());
		return bstr;
	}
	static wchar_t* NewGuidToWCharString() {
		GUID guid;
		HRESULT hr1 = CoCreateGuid(&guid);
		OLECHAR* guidString;
		StringFromCLSID(guid, &guidString);
		return guidString;
	}
	static std::string GetTempXmlSavePath() {
		TCHAR username[UNLEN + 1];
		DWORD size = UNLEN + 1;
		GetUserName((TCHAR*)username, &size);

		stringstream ss;
		ss << "C:\\Users\\" << username << "\\AppData\\Local\\" << "psets_folder";
		if (!fs::is_directory(ss.str()) || !fs::exists(ss.str())) {
			fs::create_directory(ss.str());
		}
		ss << "\\" << NewGuidToWCharString() << ".xml";
		return ss.str();
	}
};
#endif
