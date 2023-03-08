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

static const std::locale ru_loc = std::locale("ru_RU.UTF-8");
static const std::locale en_loc = std::locale("en_US.UTF-8");

class aux_functions {
public:
	static std::wstring ToWStringFromString(std::string str)
	{
		CA2W ca2w(str.c_str());
		std::wstring wide = ca2w;
		return wide;
	}
	static std::wstring ToWStringFromConstChar(const char* str)
	{
		CA2W ca2w(str);
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
	static std::string NewGuidToCharString(bool without_bracets = true) {
		GUID guid3;
		HRESULT hr1 = CoCreateGuid(&guid3);
		std::string guid_new_str = aux_functions::ToStringFromGuid(guid3);
		if (without_bracets)
		{
			guid_new_str = guid_new_str.replace(guid_new_str.find("{"), 1, "");
			guid_new_str = guid_new_str.replace(guid_new_str.find("}"), 1, "");
		}
		return guid_new_str;
	}
	static std::wstring NewGuidToWCharString(bool without_bracets = true) {
		return ToWStringFromString(NewGuidToCharString(without_bracets));
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
	static std::vector<std::string> StringSplit(const std::string& s, char delim) {
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	static std::string ToJoinedStringFromVectorString(const std::vector<std::string>& tokens, char delimiter)
	{
		std::string a;
		for (auto x : tokens)
		{
			a += x;
			a += delimiter;
		}
		a.pop_back();
		return a;
	}
	static std::vector<std::string> ReadFileByPath(const char* file_path)
	{
		std::string line_row;
		std::vector<std::string> lines;
		std::ifstream file_data(file_path);

		if (file_data.is_open())
		{
			while (getline(file_data, line_row))
			{
				if (line_row.substr(0, 1) != "*")
				{
					lines.push_back(line_row);
				}
			}
		}
		file_data.close();
		return lines;
	}
	static const char* ReadFileByPathAsConstChar(const char* file_path)
	{
		std::string line_row;
		//std::vector<std::string> lines;
		std::ifstream file_data(file_path);
		std::stringstream ss;

		if (file_data.is_open())
		{
			while (getline(file_data, line_row))
			{
				ss << line_row;
			}
		}
		file_data.close();
		return ss.str().c_str();
	}
	static std::map<std::string, AcDbObjectId> GetDrawingHandles(AcDbDatabase* db)
	{
		//AcDbDatabase* denneDatabasen = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTable* pBlockTable;
		Acad::ErrorStatus es;
		es = db->getBlockTable(pBlockTable, AcDb::kForRead);

		//AcDbObjectIdArray objekter;
		std::map<std::string, AcDbObjectId> handle2ids;
		if (es == Acad::eOk)
		{
			AcDbBlockTableRecord* pMSpaceRecord;
			es = pBlockTable->getAt(ACDB_MODEL_SPACE, pMSpaceRecord, AcDb::kForWrite);
			if (es == Acad::eOk)
			{
				AcDbBlockTableRecordIterator* pIterator;
				es = pMSpaceRecord->newIterator(pIterator);
				if (es == Acad::eOk)
				{
					for (pIterator->start(); !pIterator->done(); pIterator->step())
					{
						AcDbEntity* enEnt;
						es = pIterator->getEntity(enEnt, AcDb::kForWrite);
						if (es == Acad::eOk)
						{
							AcDbObjectId entId;
							pIterator->getEntityId(entId);
							AcDbHandle handle = entId.handle();
							ACHAR buffer[32];
							handle.getIntoAsciiBuffer(buffer);
							std::string s_buffer = aux_functions::ToStringFromWString(buffer, en_loc);
							//handles.push_back(s_buffer);
							handle2ids.insert(std::make_pair(s_buffer, entId));
							enEnt->close();
						}
					}
					delete pIterator;
				}
				else
				{
					acutPrintf(L"\nНе удалось создать новый blocktablerecorditerator");
				}
				pMSpaceRecord->close();
			}
			else
			{
				acutPrintf(L"\nНе удалось открыть modelspace");
			}
			pBlockTable->close();
		}
		else
		{
			acutPrintf(L"\nНе удалось открыть  blocktablerecord");
		}
		return handle2ids;
	}
	/// <summary>
	/// Получение идентификатора у документа NCAD_PSETS_PARAMETERS в AcDbDatabaseSummaryInfo
	/// </summary>
	/// <returns></returns>
	static std::wstring get_doc_id() {
		std::wstring need_id;
		AcDbDatabase* pCurDb = acDocManager->curDocument()->database();
		AcDbDatabaseSummaryInfo* info;
		auto es = acdbGetSummaryInfo(pCurDb, info);

		const NCHAR* parameter_name = L"NCAD_PSETS_PARAMETER";//NCAD_PSETS_PARAMETER

		NCHAR* PARAM_DATA;
		auto check_exist_value = info->getCustomSummaryInfo(parameter_name, PARAM_DATA);
		//std::wstring temp_data = PARAM_DATA;

		if (check_exist_value != Acad::eOk)
		{
			//(check_exist_value == Acad::eOk && wcslen(PARAM_DATA) < 5)
			//set value
			auto need_id_temp = aux_functions::NewGuidToWCharString();
			auto check_set_value = info->addCustomSummaryInfo(parameter_name, need_id_temp.c_str());
			if (check_set_value == Acad::eOk)
			{
				need_id = need_id_temp;
				acutPrintf(L"Для документа установлен уникальный идентификатор NCAD_PSETS_PARAMETER");
				acutPrintf(need_id_temp.c_str());
				acutPrintf(L"\n");
			}
		}
		else {
			//set to need_id the getted parameter
			need_id = PARAM_DATA;
		}
		es = acdbPutSummaryInfo(info);
		return need_id;
	}
};
#endif
