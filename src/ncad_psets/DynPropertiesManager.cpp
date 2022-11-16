
#include "stdafx.h"
#include "DynPropertiesManager.hpp"

bool DynPropertiesManager::m_bInitialized = false;
std::vector<CComObject<CategorizedSingleDynProperty>*>DynPropertiesManager::dyn_s_props{ };
std::map<AcDbObjectId, std::map<GUID, VARIANT>> DynPropertiesManager::objects2properties;

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
        CreateSingleDynProperty(L"second_property", L"", VARENUM::VT_I4, L"Default Category");
        CreateSingleDynProperty(L"third_property", L"", VARENUM::VT_R8, L"Default Category");
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

void DynPropertiesManager::CreateSingleDynProperty(BSTR name, BSTR description, 
    VARENUM type, BSTR category_name)
{
    category_name = L"Default Category";
    //AcRxClass* m_pClass = AcDbEntity::desc();
    CComPtr<IPropertyManager> prop_manager;
    if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
        _com_issue_error(E_FAIL); 

    CComObject<CategorizedSingleDynProperty>* new_property = NULL;
    _com_util::CheckError(CComObject<CategorizedSingleDynProperty>::CreateInstance(&new_property));

    new_property->set_name(name);
    new_property->set_description(description);
    new_property->set_type(type);
    new_property->AddRef();
    _com_util::CheckError(prop_manager->AddProperty(new_property));
    dyn_s_props.push_back(new_property);
    GUID id;
    new_property->GetGUID(&id);
}

void DynPropertiesManager::AssignSingleDynPropertyToObject()
{
    if (!isInitialized())
    {
        acutPrintf(_T("\nThe dynamic properties failed to intialize. Command is disabled."));
        return;
    }
    ads_name ss;
    if (acedSSGet(NULL, NULL, NULL, NULL, ss) != RTNORM)
        return;
    long len = 0;
    acedSSLength(ss, &len);
    ads_name ent;
    AcDbObjectId id;
    for (long i = 0; i < len; i++) 
    {
        if (acedSSName(ss, i, ent) == RTNORM)
        {
            if (acdbGetObjectId(id, ent) == Acad::eOk)
            {

            }
        }
    }
    ads_ssfree(ss);
}
static bool operator<(const GUID& a, const GUID& b)
{
    return (a.Data1 < b.Data1);
}
//static bool operator<(const AcDbObjectId& a, const AcDbObjectId& b)
//{
//    return (a.asOldId() < b.asOldId());
//}
bool is_data_in_objects2properties(AcDbObjectId* id) {
    for (auto d : DynPropertiesManager::objects2properties)
    {
        if (d.first == *id) return true;
    }
    return false;
}
bool is_data_in_nested_map(GUID* id, std::map<GUID, VARIANT>* collection, VARIANT* data) {
    for (auto d : *collection)
    {
        if (d.first == *id) 
        {
            *data = d.second;
            return true;
        }
    }
    return false;
}

void DynPropertiesManager::SetPropertyValue(AcDbObjectId* id, 
    GUID* property_id, VARIANT* data)
{
    /*ѕровер€ем, имеетс€ ли в objects2properties запись дл€ данного объекта
    и если нет, создаем пустую запись. √лобальна€ цель -- получить вектор значений*/
    if (!is_data_in_objects2properties(id))
    {
        std::map<GUID, VARIANT> setting_data;
        setting_data.insert({ *property_id, *data });
        objects2properties.insert(std::make_pair(*id, setting_data));
        setting_data.clear();        
        return;
    }
    /*ѕровер€ем коллекцию значений свойств дл€ перезаписи/установки данного значени€ свойства*/
    if (is_data_in_nested_map(property_id, &objects2properties[*id], data))
    {
        //std::map<GUID, VARIANT> setting_data;
        //setting_data.insert(std::make_pair(*property_id, *data));
        objects2properties[*id].insert(std::make_pair(*property_id, * data));
    }
    else objects2properties[*id][*property_id] = *data;

}
bool DynPropertiesManager::GetPropertyValue(AcDbObjectId* id,
    GUID* property_id, VARIANT* data)
{
    if (!is_data_in_objects2properties(id)) return false;
    if (!is_data_in_nested_map(property_id, &(objects2properties[*id]), data)) return false;
    return true;
}
