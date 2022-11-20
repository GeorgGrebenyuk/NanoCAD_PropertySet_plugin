// test_json.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <tinyxml2.h>
#include <fstream>
#include "vector"
#include "sstream"
#include "string"
//using json = nlohmann::json;
using namespace tinyxml2;

/// <summary>
/// Сохранение в файл данных о динамичесском свойстве
/// </summary>
void create_property(XMLDocument* doc, XMLElement* prop_list,
    std::string name,
    std::string desc,
    std::string type,
    std::string category,
    std::string id,
    std::string def_value,
    std::vector<std::string> object_classes)
{
    XMLElement* prop_def = doc->NewElement("property");
    
    prop_def->SetAttribute("name", name.c_str());
    prop_def->SetAttribute("description", desc.c_str());
    prop_def->SetAttribute("type", type.c_str());
    prop_def->SetAttribute("category", category.c_str());
    prop_def->SetAttribute("id", id.c_str());
    prop_def->SetAttribute("default_value", def_value.c_str());

    std::stringstream ss;
    for (auto o_class : object_classes) { ss << o_class << ";"; }
    prop_def->SetAttribute("to_classes", ss.str().c_str());

    //prop_list->InsertNewChildElement("property");
    prop_list->InsertEndChild(prop_def);
}

void xml_work() {

    XMLDocument doc = new XMLDocument();
    XMLElement* root = doc.NewElement("psets_info");
    
    /*meta data*/
    XMLElement* meta = root->InsertNewChildElement("metadata");
    XMLElement* project_info = meta->InsertNewChildElement("project_info");
    project_info->SetAttribute("project_name", "Drawing1");
    //project_info->SetAttribute("save_time", "19.11.2022 18:46");
    //project_info->SetAttribute("hash_sum", "6050C47ADB62DFE5");
    /*properties info*/
    XMLElement* props_list = root->InsertNewChildElement("properties_info");
    create_property(&doc, props_list, "RUS_Имя", "", "string", "RUS", "", "", { "NcDbPoint", "NcDbLine" });
    create_property(&doc, props_list, "RUS_Количество", "", "int", "RUS", "", "0", { "NcDbPoint", "NcDbLine" });
    
    doc.InsertFirstChild(root);
    doc.SaveFile("test_psets.xml");
}
int main()
{
    setlocale(LC_ALL, "");
    xml_work();
    std::cout << "\nEnd!";
    return 1;
}
