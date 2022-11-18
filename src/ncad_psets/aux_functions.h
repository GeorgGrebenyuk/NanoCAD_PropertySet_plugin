#ifndef __AUX_FUNCTIONS_H_
#define __AUX_FUNCTIONS_H_
#include "stdafx.h"
#include "string"
#include <AtlBase.h>
#include <atlconv.h>
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
};
#endif
