#include "stdafx.h"
#include "DynPropertiesManager.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include <tinyxml2.h>
namespace xml = tinyxml2;
#pragma warning(disable: 2038)

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

    prop_def->SetAttribute("name", prop->p_name);
    prop_def->SetAttribute("description", prop->p_description);
    prop_def->SetAttribute("type", prop->p_valueType);
    prop_def->SetAttribute("category", prop->p_CatName);
    OLECHAR* guidString;
    HRESULT hr = StringFromCLSID(prop->p_guid, &guidString);
    prop_def->SetAttribute("id", guidString);
    switch (prop->p_valueType)
    {
    case VT_I4:  
        prop_def->SetAttribute("default_value", prop->p_default_value.intVal);
        break;
    case VT_R8:
        prop_def->SetAttribute("default_value", prop->p_default_value.fltVal);
        break;
    case VT_BSTR:
        prop_def->SetAttribute("default_value", prop->p_default_value.bstrVal);
        break;
    }
    std::stringstream ss;
    for (auto o_class : prop->p_class_names) { ss << o_class << ";"; }
    prop_def->SetAttribute("to_classes", ss.str().c_str());
    prop_list->InsertEndChild(prop_def);
}
void DynPropertiesManager::SavePropertiesAndValueToFile() 
{
    //stringstream ss;
    xml::XMLDocument doc = new xml::XMLDocument();
    tinyxml2::XMLElement* root = doc.NewElement("psets_info");
    /*metadata*/
    xml::XMLElement* meta = root->InsertNewChildElement("metadata");
    xml::XMLElement* project_info = meta->InsertNewChildElement("project_info");

    AcApDocument* doc_model = acDocManager->curDocument();
    const NCHAR* path = doc_model->fileName();
    auto cdoc = doc_model->cDoc();

    project_info->SetAttribute("project_name", path);

    /*properties info*/
    std::map<std::wstring, VARTYPE> props2types;
    xml::XMLElement* props_list = root->InsertNewChildElement("properties_info");
    for (auto s_dyn_property : dyn_s_props)
    {
        xml_create_property(&doc, props_list, s_dyn_property);
        OLECHAR* guidString;
        HRESULT hr = StringFromCLSID(s_dyn_property->p_guid, &guidString);
        props2types.insert({ guidString, s_dyn_property->p_valueType });
    }
    //list properties

    /*values of objects*/
    xml::XMLElement* props_values = root->InsertNewChildElement("properties_values");
    for (auto o_info : objects2properties) {
        NcDbHandle handle = o_info.first.handle();
        NCHAR buffer;
        bool ckech_succ = handle.getIntoAsciiBuffer(&buffer, 16);

        xml::XMLElement* object_def = props_values->InsertNewChildElement("object");
        object_def->SetAttribute("handle", buffer);
        for (auto o_info_value : o_info.second)
        {
            OLECHAR* guidString;
            HRESULT hr = StringFromCLSID(o_info_value.first, &guidString);
            stringstream ss;
            ss << guidString;
            VARTYPE prop_type = props2types[guidString];
            switch (prop_type) {
            case VT_I4:
                object_def->SetAttribute(ss.str().c_str(), o_info_value.second.intVal);
                break;
            case VT_R8:
                object_def->SetAttribute(ss.str().c_str(), o_info_value.second.fltVal);
                break;
            case VT_BSTR:
                object_def->SetAttribute(ss.str().c_str(), o_info_value.second.bstrVal);
                break;
            }
        }
    }


    /*saving and writing to file*/
    doc.InsertFirstChild(root);
    auto save_path = aux_functions::GetTempXmlSavePath();
    doc.SaveFile(save_path.c_str());
}