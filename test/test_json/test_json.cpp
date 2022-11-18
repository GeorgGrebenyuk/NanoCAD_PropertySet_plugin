// test_json.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

json get_property(
    std::wstring name,
    std::wstring desc,
    std::wstring type,
    std::wstring category,
    std::wstring id,
    std::wstring def_value,
    std::vector<std::wstring> object_classes)
{
    json prop_def;
    prop_def["name"] = name;
    prop_def["description"] = desc;
    prop_def["type"] = type;
    prop_def["category"] = category;
    prop_def["id"] = id;
    prop_def["default_value"] = def_value;


}
void json_work() {
    json root;
    /*meta data*/
    json meta;
    meta["project_name"] = "default";
    meta["project_name"] = "default";
    /*properties info*/
    root["metadata"] = meta;
    json props_list;
    json prop1 = get_property(L"RUS_Имя", L"", L"string", L"RUS", L"", L"");
    json prop2 = get_property(L"RUS_Тип", L"", L"string", L"RUS", L"", L"");
    json prop3 = get_property(L"RUS_Количество", L"", L"int", L"RUS", L"", L"");
    props_list.push_back(prop1);
    props_list.push_back(prop2);
    props_list.push_back(prop3);

    root["properties"] = props_list;
    /*info about assigned objects*/
    json assigned_objects;
}
int main()
{
    json_work();
    std::cout << "\nEnd!";
    return 1;
}
