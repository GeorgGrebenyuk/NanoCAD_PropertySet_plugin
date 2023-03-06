#include "stdafx.h"
#include "DynPropertiesManager.hpp"
#include "aux_functions.h"

std::vector<CComObject<CategorizedSingleDynProperty>*>DynPropertiesManager::dyn_s_props{ };
std::map<AcDbObjectId, std::map<GUID, _variant_t>> DynPropertiesManager::objects2properties;
std::vector<BSTR> DynPropertiesManager::categories_names;
std::map<std::wstring, std::vector<int>> DynPropertiesManager::document2properties;
int DynPropertiesManager::dyn_s_props_counter = 0;
std::string DynPropertiesManager::pDoc_name = "";
bool DynPropertiesManager::status_eventhandler_work = false;


AcRxClass* DynPropertiesManager::m_pClass = AcDbEntity::desc();

void DynPropertiesManager::CreateSingleDynProperty(
    /*in*/BSTR name,
    /*in*/BSTR description,
    /*in*/VARENUM type,
    /*in*/BSTR category_name,
    /*in*/std::vector<BSTR> class_names,
    /*in*/BSTR id,
    /*in*/int show_mode)
{
    //category_name = L"Default Category";
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
        /*если успешно, он уже преобразованный в теле запроса*/
    }
    new_property->p_name = name;
    new_property->p_description = description;
    new_property->p_valueType = type;
    new_property->p_CatName = category_name;
    new_property->p_class_names = class_names;
    new_property->show_mode = show_mode;

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
    /*ѕроверка есть ли уже такое свойство в пам€ти (по GUID'ам)
    ѕо остальным параметрам -- на усмотрение ѕользовател€
    TODO: сделать вывод сообщений об одноименных свойствах одного типа*/

    bool permit_adding = true;
    for (auto s_dyn_prop : dyn_s_props)
    {
        if (s_dyn_prop->p_guid == guid) 
        {
            permit_adding = false;
            break;
        }
    }
    if (permit_adding)
    {
        new_property->AddRef();
        _com_util::CheckError(prop_manager->AddProperty(new_property));
        

        /*‘иксируем размерность списка dyn_s_props (фактически, индекс свойства) и добавл€ем
        в document2properties*/
        std::wstring pCurDoc_title(acDocManager->curDocument()->docTitle());
        bool is_find = false;
        for (auto t : document2properties)
        {
            if (t.first == pCurDoc_title) {
                document2properties[pCurDoc_title].push_back(dyn_s_props_counter);
                is_find = true;
            }
        }
        if (!is_find)
        {
            std::vector<int> new_x;
            new_x.push_back(dyn_s_props_counter);
            document2properties.insert(std::make_pair(pCurDoc_title, new_x));
        }
        dyn_s_props_counter++;
        dyn_s_props.push_back(new_property);
    }
    else {
        new_property->Release();
    }
}

static bool operator<(const GUID& a, const GUID& b)
{
    return (a.Data1 < b.Data1);
}
bool is_data_in_objects2properties(AcDbObjectId* id) {
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

void DynPropertiesManager::SetPropertyValue(AcDbObjectId* id,
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
bool DynPropertiesManager::GetPropertyValue(AcDbObjectId* id,
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

void DynPropertiesManager::DeleteAllProperties() {
    categories_names.clear();
    objects2properties.clear();
    
    for (auto document2properties1 : document2properties)
    {
        document2properties1.second.clear();
    }
    document2properties.clear();

    CComPtr<IPropertyManager> prop_manager;
    if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
        _com_issue_error(E_FAIL);

    if (prop_manager)
    {
        for (auto prop : dyn_s_props)
        {
            if (prop)
            {
                _com_util::CheckError(prop_manager->RemoveProperty(prop));
                prop->Release();
                //__try
                //{

                //    
                //}
                //__except (EXCEPTION_CONTINUE_EXECUTION)
                //{

                //}

            }

        }
    }

}

// Document swapping functions
//

