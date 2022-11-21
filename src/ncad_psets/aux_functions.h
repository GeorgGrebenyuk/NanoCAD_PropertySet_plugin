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
	
	static std::string ToStringFromBSTR(BSTR bstr) 
	{
		if (0 == wcscmp(bstr, L"")) return "";
		//int wslen = ::SysStringLen(bstr);
		//return ConvertWCSToMBS((wchar_t*)bstr, wslen);
		return ToStringFromWString(bstr, std::locale("ru_RU.UTF-8"));

	}
	static std::string ToStringFromGuid(GUID guid)
	{
		OLECHAR* guidString;
		HRESULT hr = StringFromCLSID(guid, &guidString);
		std::wstring w_guid = guidString;
		std::string s_guid(w_guid.begin(), w_guid.end());
		//return ConvertWCSToMBS(guidString, 36);
		return s_guid;

	}
	static std::string NewGuidToWCharString() {
		GUID guid;
		HRESULT hr1 = CoCreateGuid(&guid);
		return ToStringFromGuid(guid);
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
	static std::string ToStringFromWString(const std::wstring& wstr, const std::locale& loc)
	{
		if (wstr.empty())
			return std::string();

		typedef std::wstring::traits_type::state_type state_type;
		typedef std::codecvt<wchar_t, char, state_type> convert;

		const convert& cvt = std::use_facet<convert>(loc);
		std::string str(cvt.max_length() * wstr.size(), '\0');
		state_type state = state_type();

		const wchar_t* from_beg = &wstr[0];
		const wchar_t* from_end = from_beg + wstr.size();
		const wchar_t* from_nxt;
		char* to_beg = &str[0];
		char* to_end = to_beg + str.size();
		char* to_nxt;

		std::string::size_type sz = 0;
		std::codecvt_base::result r;
		do
		{
			r = cvt.out(state, from_beg, from_end, from_nxt,
				to_beg, to_end, to_nxt);
			switch (r)
			{
			case std::codecvt_base::error:
				throw std::runtime_error("error converting wstring to string");

			case std::codecvt_base::partial:
				sz += to_nxt - to_beg;
				str.resize(2 * str.size());
				to_beg = &str[sz];
				to_end = &str[0] + str.size();
				break;

			case std::codecvt_base::noconv:
				str.resize(sz + (from_end - from_beg) * sizeof(wchar_t));
				std::memcpy(&str[sz], from_beg, (from_end - from_beg) * sizeof(wchar_t));
				r = std::codecvt_base::ok;
				break;

			case std::codecvt_base::ok:
				sz += to_nxt - to_beg;
				str.resize(sz);
				break;
			}
		} while (r != std::codecvt_base::ok);

		return str;
	}
};
#endif
