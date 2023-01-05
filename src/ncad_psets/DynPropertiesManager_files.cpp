#include "stdafx.h"
#include "resource.h"
#include "DynPropertiesManager.hpp"

#include <fstream>
#include <sstream>
#include <string>
//#include <codecvt>

#include <tinyxml2.h>
namespace xml = tinyxml2;

#include "aux_functions.h"
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
//static const std::locale ru_loc = std::locale("ru_RU.UTF-8");
//static const std::locale en_loc = std::locale("en_US.UTF-8");
//static void get_type(VARENUM* type, std::string str)
//{
//    if (str == "string") *type = VARENUM::VT_BSTR;
//    else if (str == "double") *type = VARENUM::VT_R8;
//    //else if (prop_type == L"bool") type = VARENUM::VT_BOOL;
//    //else if (prop_type == L"datetime") type = VARENUM::VT_DATE;
//    else if (str == "int") *type = VARENUM::VT_I4;
//    //else if (prop_type == L"long") type = VARENUM::VT_UI4;
//}
void DynPropertiesManager::ImportPropertiesByFile() {
	ACHAR file_path[512];
	int ret;
	ret = acedGetString(1, L"\nУкажите абсолютный файловый путь: ", file_path);
    std::string s_file_path = aux_functions::ToStringFromWString(file_path, ru_loc);
	if (ret == RTNORM)
	{
        auto lines = aux_functions::ReadFileByPath(s_file_path.c_str());
        for (auto line : lines)
        {
            std::vector<std::string> str_data;
            int last_pos = 0;
            for (int i = 0; i < line.size(); i++)
            {
                if (line[i] == ',')
                {
                    std::string part = line.substr(last_pos, i - last_pos);
                    //BSTR bstr_str = aux_functions::ToBSTRFromString(part);
                    str_data.push_back(part);
                    //delete[] cstr;
                    last_pos = i + 1;
                }
            }
            std::string part = line.substr(last_pos);
            //BSTR bstr_str2 = aux_functions::ToBSTRFromString(part);
            str_data.push_back(part);
            //char* cstr = strcpy(new char[part.length() + 1], part.c_str());

            auto prop_type = str_data[2];
            VARENUM type = VARENUM::VT_UNKNOWN;
            if (prop_type == "string") type = VARENUM::VT_BSTR;
            else if (prop_type == "double") type = VARENUM::VT_R8;
            else if (prop_type == "int") type = VARENUM::VT_I4;

            //CComBSTR prop_guid(str_data[4]);
            std::vector<BSTR> classes;
            if (!str_data[5].empty())
            {
                auto splited_data = aux_functions::StringSplit(str_data[5], ';');
                for (auto i : splited_data) { classes.push_back(aux_functions::ToBSTRFromString(i)); }
            }

            //GUID created_id;
            //CreateSingleDynProperty(prop_name, prop_desc, type, prop_cat_name);
            CreateSingleDynProperty(
                aux_functions::ToBSTRFromString(str_data[0]),
                aux_functions::ToBSTRFromString(str_data[1]),
                type,
                aux_functions::ToBSTRFromString(str_data[3]),
                classes,
                aux_functions::ToBSTRFromString(str_data[4]));
        }

	}
}
void DynPropertiesManager::WritePropsToConsole()
{
}
//////////////////////////////////////////////////////////////////////////////////////
/*Для загрузки свойств из файла*/
//////////////////////////////////////////////////////////////////////////////////////
const char* xml_file_root_name = "psets_info";
const char* xml_file_metadata_name = "metadata";
const char* xml_file_project_info_name = "project_info";
const char* xml_file_properties_info_name = "properties_info";
const char* xml_file_properties_values_name = "properties_values";

void DynPropertiesManager::LoadPropertiesAndValuesFromFile() 
{
    /*TODO: сделать preload с просмотром metadata в форме
    в рамках загрузчика таких файлов из Истории (папки ncad_psets)*/

    ACHAR file_path[512];
    int ret;
    ret = acedGetString(1, L"\nУкажите абсолютный файловый путь до файла конфигурации: ", file_path);
    std::string s_file_path = aux_functions::ToStringFromWString(file_path, ru_loc);

    if (ret != RTNONE)
    {
        AcDbDatabase* pCurDb = acDocManager->curDocument()->database();
        std::map<std::string, AcDbObjectId> current_data = aux_functions::GetDrawingHandles(pCurDb);
        xml::XMLDocument xml_doc;
        //Необходимо чтобы XML был в ANSI-кодировке. Из-под C# я это могу сохранить, тут нет
        auto check_openning = xml_doc.LoadFile(s_file_path.c_str()); // 

        if (check_openning == xml::XMLError::XML_SUCCESS)
        {
            xml::XMLElement* root = xml_doc.RootElement();
            std::map<std::string, VARTYPE> props2types;

            /*Регистрация новых свойств*/
            xml::XMLElement* props_list = root->FirstChildElement(xml_file_properties_info_name);
            for (xml::XMLElement* child = props_list->FirstChildElement(); child != NULL; 
                child = child->NextSiblingElement())
            {
                /*Для одного свойства*/
                std::string p_name = child->FindAttribute("name")->Value();
                BSTR name = aux_functions::ToBSTRFromString(p_name);
                auto description = aux_functions::ToBSTRFromString(child->FindAttribute("description")->Value());
                std::string s_type = child->FindAttribute("type")->Value();
                VARENUM type = VARENUM::VT_UNKNOWN;
                if (s_type == "8") type = VARENUM::VT_BSTR;
                else if (s_type == "5") type = VARENUM::VT_R8;
                else if (s_type == "3") type = VARENUM::VT_I4;
                
                auto category = aux_functions::ToBSTRFromString(child->FindAttribute("category")->Value());

                std::string data_guid = child->FindAttribute("id")->Value();
                auto w_data_guid = aux_functions::ToWStringFromString(data_guid);
                GUID represented_guid;
                HRESULT hr2 = CLSIDFromString(w_data_guid.c_str(), (LPCLSID)&represented_guid);

                if (hr2 == S_OK)
                {
                    std::string current_guid = aux_functions::ToStringFromGuid(represented_guid);
                    auto id = aux_functions::ToBSTRFromString(current_guid);

                    /*classes*/
                    std::string to_classes_data = child->FindAttribute("to_classes")->Value();
                    std::vector<BSTR> classes;
                    if (!to_classes_data.empty())
                    {
                        auto splited_data = aux_functions::StringSplit(to_classes_data, ';');
                        for (auto i : splited_data) { classes.push_back(aux_functions::ToBSTRFromString(i)); }
                    }
                    props2types.insert({ current_guid, type });
                    CreateSingleDynProperty(name, description, type, category, classes, id);
                }
            }
            /*assign properties values by reading handle*/
            xml::XMLElement* props_values_list = root->FirstChildElement(xml_file_properties_values_name);
            for (xml::XMLElement* child = props_values_list->FirstChildElement(); child != NULL;
                child = child->NextSiblingElement())
            {
                /*Для одного объекта*/
                std::string s_handle = child->FindAttribute("handle")->Value();
                auto w_handle = aux_functions::ToWStringFromString(s_handle);
                AcDbObjectId drawing_object_id = AcDbObjectId::kNull;
                try
                {

                    AcDbHandle drawing_handle(w_handle.c_str());
                    if (pCurDb->getAcDbObjectId(drawing_object_id, false, drawing_handle) == Acad::eOk)
                    {
                        drawing_object_id = current_data[s_handle];
                    }
                }
                catch (int err) {}

                if (!drawing_object_id.isNull())
                {
                    for (xml::XMLElement* prop_value = child->FirstChildElement(); prop_value != NULL;
                        prop_value = prop_value->NextSiblingElement())
                    {
                        auto attr_value = prop_value->FindAttribute("value");
                        auto s_value = attr_value->Value();

                        std::string data_guid = prop_value->FindAttribute("id")->Value();
                        //data_guid = data_guid.replace(data_guid.find("{"), 1, "");
                        //data_guid = data_guid.replace(data_guid.find("}"), 1, "");

                        auto w_data_guid = aux_functions::ToWStringFromString(data_guid);
                        GUID represented_guid;
                        HRESULT hr2 = CLSIDFromString(w_data_guid.c_str(), (LPCLSID) &represented_guid);
                        if (hr2 == S_OK)
                        {
                            std::string current_guid = aux_functions::ToStringFromGuid(represented_guid);
                            VARTYPE prop_type = props2types[current_guid];

                            VARIANT current_v = _variant_t();
                            switch (prop_type)
                            {
                            case VARENUM::VT_BSTR:
                            {
                                auto str_representation = aux_functions::ToBSTRFromString(s_value);
                                current_v = _variant_t(str_representation);
                                break;
                            }

                            case VARENUM::VT_I4:
                                current_v = _variant_t(atoi(s_value));
                                break;
                            case VARENUM::VT_R8:
                                current_v = _variant_t(atof(s_value), VARENUM::VT_R8);
                                break;
                            }
                            DynPropertiesManager::SetPropertyValue(&drawing_object_id,
                                &represented_guid, &_variant_t(current_v));
                        }
                    }
                }
            }
            int for_breakpoint_1 = 0;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////
/*Для сохранения свойств в файл*/
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
    
    std::vector<std::string> classes_names;
    for (auto o_class : prop->p_class_names) { classes_names.push_back(aux_functions::ToStringFromBSTR(o_class)); }
    std::string j_classes_names;
    if (!prop->p_class_names.empty()) j_classes_names = aux_functions::ToJoinedStringFromVectorString(classes_names, ';');
    else j_classes_names = "";
    prop_def->SetAttribute("to_classes", j_classes_names.c_str());
    prop_list->InsertEndChild(prop_def);
}
void DynPropertiesManager::SavePropertiesAndValueToFile() 
{
    xml::XMLDocument doc = new xml::XMLDocument();
    //doc.InsertFirstChild(doc.NewDeclaration());
    tinyxml2::XMLElement* root = doc.NewElement(xml_file_root_name);

    /*metadata*/
    xml::XMLElement* meta = root->InsertNewChildElement(xml_file_metadata_name);
    xml::XMLElement* project_info = meta->InsertNewChildElement(xml_file_project_info_name);

    AcApDocument* doc_model = acDocManager->curDocument();
    const NCHAR* project_path = doc_model->fileName();
    project_info->SetAttribute("project_path", aux_functions::ToStringFromWString(project_path, ru_loc).c_str());

    /*properties info*/
    std::map<std::string, VARTYPE> props2types;
    xml::XMLElement* props_list = root->InsertNewChildElement(xml_file_properties_info_name);
    for (auto s_dyn_property : dyn_s_props)
    {
        xml_create_property(&doc, props_list, s_dyn_property);
        props2types.insert({ aux_functions::ToStringFromGuid(s_dyn_property->p_guid), s_dyn_property->p_valueType });
    }
    //list properties

    /*values of objects*/
    xml::XMLElement* props_values = root->InsertNewChildElement(xml_file_properties_values_name);
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
    auto save_path = aux_functions::GetTempSavePath();
    //doc.SetBOM(true);

    doc.SaveFile(save_path.c_str());

    // Need convert to ANSI
    //aux_functions::ConvertFileToAnsiFromUTF8(save_path);
}