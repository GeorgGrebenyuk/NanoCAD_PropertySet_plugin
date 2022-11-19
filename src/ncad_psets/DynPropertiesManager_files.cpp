#include "stdafx.h"
#include "DynPropertiesManager.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "aux_functions.h"

void DynPropertiesManager::ImportPropertiesByFile() {
	ACHAR file_path[256];
	int ret;
	ret = acedGetString(1, L"\nУкажите абсолютный файловый путь: ", file_path);

	if (ret == RTNORM)
	{
        std::string line;
        std::ifstream file_data(file_path);

        if (file_data.is_open())
        {
            while (getline(file_data, line))
            {
                std::vector<BSTR> str_data;
                if (line.substr(0, 1) != "*")
                {
                    int last_pos = 0;
                    for (int i = 0; i < line.size(); i++)
                    {
                        if (line[i] == ',')
                        {
                            std::string part = line.substr(last_pos, i - last_pos);
                            auto m = ::SysAllocString(aux_functions::
                                ToWStringFromString(part.c_str()).c_str());
                            str_data.push_back(m);
                            //delete[] cstr;
                            last_pos = i + 1;
                        }
                    }
                    std::string part = line.substr(last_pos);
                    CA2W ca2w(part.c_str());
                    std::wstring wide = ca2w;
                    auto m = ::SysAllocString(wide.c_str());
                    str_data.push_back(m);
                    //char* cstr = strcpy(new char[part.length() + 1], part.c_str());

                    auto prop_type = str_data[2];
                    VARENUM type = VARENUM::VT_UNKNOWN;
                    if (0 == wcscmp(prop_type, L"string")) type = VARENUM::VT_BSTR;
                    else if (0 == wcscmp(prop_type, L"double")) type = VARENUM::VT_R8;
                    //else if (prop_type == L"bool") type = VARENUM::VT_BOOL;
                    //else if (prop_type == L"datetime") type = VARENUM::VT_DATE;
                    else if (0 == wcscmp(prop_type, L"int")) type = VARENUM::VT_I4;
                    //else if (prop_type == L"long") type = VARENUM::VT_UI4;

                    CComBSTR prop_guid(str_data[4]);

                    GUID created_id;

                    
                    //CreateSingleDynProperty(prop_name, prop_desc, type, prop_cat_name);
                    CreateSingleDynProperty(str_data[0], str_data[1], type, str_data[3], {}, NULL);
                }
            }
        }
        file_data.close();

	}
}
void DynPropertiesManager::WritePropsToConsole()
{
}
void DynPropertiesManager::LoadPropertiesAndValuesFromFile() {

}
#include <fstream>
#include <sstream>
void DynPropertiesManager::SavePropertiesAndValueToFile() {

    //static std::map<AcDbObjectId, std::map<GUID, _variant_t>> objects2properties;
    for (auto r : objects2properties)
    {
        NcDbHandle handle = r.first.handle();
        NCHAR buffer;
        bool ckech_succ = handle.getIntoAsciiBuffer(&buffer, 16);
        //acutPrintf(_T("\nhandle = %s"), buffer);

        std::stringstream ss;
        ss << buffer << std::endl;

        std::ofstream out;          // поток для записи
        out.open("E:\\Temp\\handle.txt"); // окрываем файл для записи
        if (out.is_open())
        {
            out << ss.str();
        }
        out.close();

        acutPrintf(_T("\nОбъектный идентфикатор = %s"), ss.str().c_str());
        //for (auto r2 : r.second)
        //{
        //    OLECHAR* guidString;
        //    ::StringFromCLSID(r2.first, &guidString);

        //    acutPrintf(_T("\nИдентификатор свойства %s, значение свойства %s"), guidString, r2.second.bstrVal);
        //}
    }
}