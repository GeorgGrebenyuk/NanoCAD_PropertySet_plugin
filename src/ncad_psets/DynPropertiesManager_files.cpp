#include "stdafx.h"
#include "DynPropertiesManager.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include <tinyxml2.h>
namespace xml = tinyxml2;

#include "aux_functions.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

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
void DynPropertiesManager::LoadPropertiesAndValuesFromFile() 
{

}
//////////////////////////////////////////////////////////////////////////////////////
/*Для сохранения свойства*/
//////////////////////////////////////////////////////////////////////////////////////

void xml_create_property(xml::XMLDocument* doc, xml::XMLElement* prop_list,
    CComObject<CategorizedSingleDynProperty>* prop)
{
    xml::XMLElement* prop_def = doc->NewElement("property");

    prop_def->SetAttribute("name", aux_functions::ToStringFromBSTR( prop->p_name).c_str());
    prop_def->SetAttribute("description", aux_functions::ToStringFromBSTR(prop->p_description).c_str());
    prop_def->SetAttribute("type", prop->p_valueType);
    prop_def->SetAttribute("category", aux_functions::ToStringFromBSTR(prop->p_CatName).c_str());
    /*OLECHAR* guidString;
    HRESULT hr = StringFromCLSID(prop->p_guid, &guidString);*/
    prop_def->SetAttribute("id", aux_functions::ToStringFromGuid(prop->p_guid).c_str());
    
    std::stringstream ss;
    for (auto o_class : prop->p_class_names) { ss << aux_functions::ToStringFromBSTR(o_class) << ";"; }
    prop_def->SetAttribute("to_classes", ss.str().c_str());
    prop_list->InsertEndChild(prop_def);
}
void DynPropertiesManager::SavePropertiesAndValueToFile() 
{
    //stringstream ss;  
    const std::locale ru_loc = std::locale("ru_RU.UTF-8");
    const std::locale en_loc = std::locale("en_US.UTF-8");

    xml::XMLDocument doc = new xml::XMLDocument();
    tinyxml2::XMLElement* root = doc.NewElement("psets_info");
    /*metadata*/
    xml::XMLElement* meta = root->InsertNewChildElement("metadata");
    xml::XMLElement* project_info = meta->InsertNewChildElement("project_info");

    AcApDocument* doc_model = acDocManager->curDocument();
    const NCHAR* path = doc_model->fileName();
    
    std::string p0 = aux_functions::ToStringFromWString(path, ru_loc);
    //std::string converted_str = converter.to_bytes(path);
    project_info->SetAttribute("project_name", p0.c_str());

    /*properties info*/
    std::map<std::string, VARTYPE> props2types;
    xml::XMLElement* props_list = root->InsertNewChildElement("properties_info");
    for (auto s_dyn_property : dyn_s_props)
    {
        xml_create_property(&doc, props_list, s_dyn_property);
        props2types.insert({ aux_functions::ToStringFromGuid(s_dyn_property->p_guid), s_dyn_property->p_valueType });
    }
    //list properties

    /*values of objects*/
    xml::XMLElement* props_values = root->InsertNewChildElement("properties_values");
    for (auto o_info : objects2properties) {
        NcDbHandle handle = o_info.first.handle();
        NCHAR buffer[32];
        handle.getIntoAsciiBuffer(buffer);

        std::string s_buffer = aux_functions::ToStringFromWString(buffer, en_loc);

        xml::XMLElement* object_def = props_values->InsertNewChildElement("object");
        object_def->SetAttribute("handle", s_buffer.c_str());
        for (auto o_info_value : o_info.second)
        {
            xml::XMLElement* record_property = object_def->InsertNewChildElement("property");

            //stringstream ss;
            //ss << guidString2;
            std::string current_guid = aux_functions::ToStringFromGuid(o_info_value.first);
            VARTYPE prop_type = props2types[current_guid];
            record_property->SetAttribute("id", current_guid.c_str());
            switch (prop_type) {
            case VT_I4:
                record_property->SetAttribute("value", o_info_value.second.intVal);
                break;
            case VT_R8:
                record_property->SetAttribute("value", o_info_value.second.fltVal);
                break;
            case VT_BSTR:
                record_property->SetAttribute("value", aux_functions::ToStringFromBSTR(o_info_value.second.bstrVal).c_str());
                break;
            }
        }
    }


    /*saving and writing to file*/
    doc.InsertFirstChild(root);
    //auto save_path = aux_functions::GetTempXmlSavePath();
    TCHAR username[UNLEN + 1];
    DWORD size = UNLEN + 1;
    GetUserName((TCHAR*)username, &size);

    GUID guid3;
    HRESULT hr1 = CoCreateGuid(&guid3);
    OLECHAR* guidString3;
    StringFromCLSID(guid3, &guidString3);

    stringstream ss3;
    //ss3 << "C:\\Users\\" << username << "\\AppData\\Local\\psets_folder" << "\\" << guidString3 << ".xml";
    //if (!fs::is_directory(ss3.str()) || !fs::exists(ss3.str())) {
    //    fs::create_directory(ss3.str());
    //}
    //ss3 << "\\" << guidString3 << ".xml";
    //auto path_3 = ss3.str().c_str();
    const char* path3 = "C:\\Users\\Georg\\AppData\\Local\\psets_folder\\test.xml";
    doc.SaveFile(path3);
}