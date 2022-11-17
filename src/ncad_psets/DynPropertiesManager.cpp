
#include "stdafx.h"
#include "DynPropertiesManager.hpp"

bool DynPropertiesManager::m_bInitialized = false;
std::vector<CComObject<CategorizedSingleDynProperty>*>DynPropertiesManager::dyn_s_props{ };
std::map<long long, std::map<GUID, _variant_t>> DynPropertiesManager::objects2properties;
std::vector<BSTR> DynPropertiesManager::categories_names;

AcRxClass* DynPropertiesManager::m_pClass = AcDbEntity::desc();

void DynPropertiesManager::initialize()
{
    if (m_bInitialized)
        return;
    m_bInitialized = true;
    try
    {
        CComPtr<IPropertyManager> prop_manager;
        if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
            _com_issue_error(E_FAIL);
        CreateSingleDynProperty(L"first_property", L"", VARENUM::VT_BSTR, L"Default Category");
        //CreateSingleDynProperty(L"second_property", L"", VARENUM::VT_I4, L"Default Category 2");
        //CreateSingleDynProperty(L"third_property", L"", VARENUM::VT_R8, L"Default Category");
    }
    catch (const _com_error&)
    {
        uninitialize();
        acutPrintf(_T("\nSimpleDynProps: initialize failed!!!\n"));
        return;
    }
}
void DynPropertiesManager::uninitialize()
{
    if (!m_bInitialized)
        return;
    m_bInitialized = false;
    try
    {
        CComPtr<IPropertyManager> prop_manager;
        if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
            _com_issue_error(E_FAIL);
        for (auto cat_props : dyn_s_props)
        {
            if (cat_props)
            {
                _com_util::CheckError(prop_manager->RemoveProperty(cat_props));
                cat_props->Release();
            } 
        }
    }
    catch (const _com_error&)
    {
        acutPrintf(_T("\nSimpleDynProps: uninitialize failed!!!\n"));
    }
}

void DynPropertiesManager::CreateSingleDynProperty(
    /*in*/BSTR name,
    /*in*/BSTR description,
    /*in*/VARENUM type,
    /*in*/BSTR category_name,
    /*in*/BSTR id)
{
    CComPtr<IPropertyManager> prop_manager;
    if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
        _com_issue_error(E_FAIL); 

    CComObject<CategorizedSingleDynProperty>* new_property = NULL;
    _com_util::CheckError(CComObject<CategorizedSingleDynProperty>::CreateInstance(&new_property));

    GUID guid;
    HRESULT hr1 = CoCreateGuid(&guid);
    if (id == NULL){
        new_property->p_guid = guid;
    }
    else {
        HRESULT hr2 = CLSIDFromString(id, (LPCLSID)&(new_property->p_guid));
        if (hr2 != S_OK) {
            new_property->p_guid = guid;
        }
    }
    new_property->p_name = name;
    new_property->p_description = description;
    new_property->p_valueType = type;
    new_property->p_CatName = category_name;

    int cat_index = 1;
    bool is_a = false;
    for (int i = 0; i < categories_names.size(); i++)
    {
        if (0 == wcscmp(categories_names[i], category_name)) 
        {
            cat_index = i + 1;
            is_a = true;
            break;
        }
    }
    if (!is_a) 
    {
        categories_names.push_back(category_name);
        cat_index = categories_names.size();
    }

    new_property->p_Cat = cat_index;
    new_property->AddRef();
    _com_util::CheckError(prop_manager->AddProperty(new_property));
    dyn_s_props.push_back(new_property);
}

static bool operator<(const GUID& a, const GUID& b)
{
    return (a.Data1 < b.Data1);
}
bool is_data_in_objects2properties(long long* id) {
    for (auto d : DynPropertiesManager::objects2properties)
    {
        if (d.first == *id) return true;
    }
    return false;
}
bool is_data_in_nested_map(GUID* id, std::map<GUID, _variant_t>* collection) {
    for (auto d : *collection)
    {
        if (d.first == *id) 
        {
            //*data = d.second;
            return true;
        }
    }
    return false;
}

void DynPropertiesManager::SetPropertyValue(long long* id,
    GUID* property_id, _variant_t* data)
{
   /* VARIANT to_set;
    switch (type)
    {
    case VT_BSTR:

    }*/
    /*ѕровер€ем, имеетс€ ли в objects2properties запись дл€ данного объекта
    и если нет, создаем пустую запись. √лобальна€ цель -- получить вектор значений*/
    if (!is_data_in_objects2properties(id))
    {
        std::map<GUID, _variant_t> setting_data;
        setting_data.insert({ *property_id, *data });
        objects2properties.insert(std::make_pair(*id, setting_data));
        setting_data.clear();        
        return;
    }
    /*ѕровер€ем коллекцию значений свойств дл€ перезаписи/установки данного значени€ свойства*/
    if (is_data_in_nested_map(property_id, &objects2properties[*id]))
    {
        //ќбновл€ем значени€ при наличии прежней записи
        objects2properties[*id][*property_id] = *data;
    }
    //”станавливаем значение, это нова€ запись
    else objects2properties[*id].insert(std::make_pair(*property_id, *data));

}
bool DynPropertiesManager::GetPropertyValue(long long* id,
    GUID* property_id, _variant_t* data)
{
    if (!is_data_in_objects2properties(id)) return false;
    if (!is_data_in_nested_map(property_id, &(objects2properties[*id]))) return false;
    else
    {
        *data = objects2properties[*id][*property_id];
    }
    return true;
}
