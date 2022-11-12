
#include "stdafx.h"
#include "DynPropertiesManager.hpp"
#include "xrecordmanager.h"

bool DynPropertiesManager::m_bInitialized = false;
std::vector<CComObject<CategorizedSingleDynProperty>*> DynPropertiesManager::single_value_properties = { NULL };
//

AcRxClass* DynPropertiesManager::m_pClass = AcDbEntity::desc();

void DynPropertiesManager::initialize()
{
    if (m_bInitialized)
        return;
    m_bInitialized = true;
    //AcRxClass* m_pClass = AcDbEntity::desc();
    try
    {
        CComPtr<IPropertyManager> prop_manager;
        if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
            _com_issue_error(E_FAIL);
        //CComObject<SingleDynProperty>* one_prop = NULL;
        //_com_util::CheckError(CComObject<SingleDynProperty>::CreateInstance(&one_prop));
        //one_prop->AddRef();

        CComObject<CategorizedSingleDynProperty>* cat_props = NULL;
        _com_util::CheckError(CComObject<CategorizedSingleDynProperty>::CreateInstance(&cat_props));
        cat_props->set_name(L"first_property");
        cat_props->set_description("first descr");
        cat_props->set_type(VARENUM::VT_BSTR);
        cat_props->set_category(1, "DefCat");
        cat_props->AddRef();

        //LONG count;
        //prop_manager->GetDynamicPropertyCount(&count);
        //single_value_properties.push_back(&count);
        single_value_properties.push_back(cat_props);
        _com_util::CheckError(prop_manager->AddProperty(cat_props));
        
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
    //AcRxClass* m_pClass = AcDbEntity::desc();
    try
    {
        CComPtr<IPropertyManager> prop_manager;
        if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
            _com_issue_error(E_FAIL);
        for (auto single_prop : single_value_properties)
        {
            if (single_prop)
            {
                //CComPtr<IDynamicProperty> prop;
                //HRESULT res = prop_manager->GetDynamicProperty(*single_prop, &prop);
                _com_util::CheckError(prop_manager->RemoveProperty(single_prop));
                //prop->Release();
            }
        }
        /*for (auto cat_prop : list_properties)
        {
            if (cat_prop)
            {
                _com_util::CheckError(prop_manager->RemoveProperty(cat_prop));
                cat_prop->Release();
            }
        }*/
    }
    catch (const _com_error&)
    {
        acutPrintf(_T("\nSimpleDynProps: uninitialize failed!!!\n"));
    }
}

void DynPropertiesManager::CreateSingleDynProperty(BSTR name, BSTR description, 
    VARENUM type, BSTR category_name)
{
    //AcRxClass* m_pClass = AcDbEntity::desc();
    CComPtr<IPropertyManager> prop_manager;
    if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
        _com_issue_error(E_FAIL);

    CComObject<CategorizedSingleDynProperty>* new_property = NULL;
    _com_util::CheckError(CComObject<CategorizedSingleDynProperty>::CreateInstance(&new_property));

    new_property->set_name(name);
    new_property->set_description(description);
    new_property->set_type(type);
    PROPCAT cat;
    GetCategoryPropcatByName(category_name, &cat);
    new_property->set_category(cat, category_name);
    //new_property->GetGUID(property_id);
    new_property->AddRef();
    //LONG count;
    //prop_manager->GetDynamicPropertyCount(&count);
    //single_value_properties.push_back(&count);
    single_value_properties.push_back(new_property);
    _com_util::CheckError(prop_manager->AddProperty(new_property));
}

void DynPropertiesManager::AssignSingleDynPropertyToObject()
{
    //AcRxClass* m_pClass = AcDbEntity::desc();
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
    for (long i = 0; i < len; i++) {
        if (acedSSName(ss, i, ent) == RTNORM)
            if (acdbGetObjectId(id, ent) == Acad::eOk)
                XRecordManager::createDefaultData(id);
    }
    ads_ssfree(ss);
}

void DynPropertiesManager::AssignSingleDynPropertyValue(GUID property_id, LONG_PTR objectID, VARIANT value)
{
}

void DynPropertiesManager::RemoveSingleDynProperty(GUID property_id, LONG_PTR objectID)
{
}

void DynPropertiesManager::GetDynProperties(LONG_PTR objectID, std::map<BSTR, VARIANT> name2values)
{
}

void DynPropertiesManager::GetCategories(std::map<BSTR, PROPCAT>* categories)
{
    //AcRxClass* m_pClass = AcDbEntity::desc();
    CComPtr<IPropertyManager> prop_manager;
    if ((prop_manager.p = GET_OPMPROPERTY_MANAGER(m_pClass)) == NULL)
        _com_issue_error(E_FAIL);

    CComObject<CategorizedSingleDynProperty>* temp_property = NULL;
    _com_util::CheckError(CComObject<CategorizedSingleDynProperty>::CreateInstance(&temp_property));

    bool need_continue = false;
    int counter_categories = 1;
    do 
    {
        BSTR name = NULL;
        temp_property->GetCategoryName(counter_categories, NULL, &name);
        if (name != NULL) categories->insert(std::pair< BSTR, PROPCAT>(name, counter_categories));
        else need_continue = false;
        counter_categories++;
    } while (need_continue);


    _com_util::CheckError(prop_manager->RemoveProperty(temp_property));
    temp_property->Release();

}

void DynPropertiesManager::GetCategoryPropcatByName(BSTR name, PROPCAT* propcat)
{
    std::map<BSTR, PROPCAT> all_cats;
    GetCategories(&all_cats);
    bool f = false;
    for (auto cat : all_cats)
    {
        if (cat.first == name) 
        {
            *propcat = cat.second;
            f = true;
            break;
        }
    }
    if (!f) *propcat = all_cats.size() + 1;
}


#include <fstream>
#include <string>
#include <sstream>

void DynPropertiesManager::LoadPropsFromList(BSTR file_path)
{
    /*
    name | description | vartype | category_name
    */
    //C:\Users\Georg\Documents\GitHub\ncad_psets\examples\test_external_props.csv
    std::string line;
    std::ifstream file_data (file_path);
    if (file_data.is_open())
    {
        while (getline(file_data, line))
        {
            std::vector<CComBSTR> str_data;
            if (line.substr(0, 1) != "*")
            {
                int last_pos = 0;
                for (int i = 0; i < line.size(); i++)
                {
                    if (line[i] == ',')
                    {
                        std::string part = line.substr(last_pos, i - last_pos);
                        char* cstr = strcpy(new char[part.length() + 1], part.c_str());
                        str_data.push_back(cstr);
                        delete[] cstr;

                        
                        last_pos = i + 1;
                    }
                }
                std::string part = line.substr(last_pos);
                char* cstr = strcpy(new char[part.length() + 1], part.c_str());
                str_data.push_back(cstr);
                //start for each property
                //CComBSTR prop_name = str_data[0];                
                //CComBSTR prop_desc = str_data[1];
                //CComBSTR prop_cat_name = str_data[3];
  
                auto prop_type = str_data[2];
                VARENUM type = VARENUM::VT_UNKNOWN;
                if (prop_type == L"string") type = VARENUM::VT_LPWSTR;
                else if (prop_type == L"double") type = VARENUM::VT_R4;
                else if (prop_type == L"bool") type = VARENUM::VT_BOOL;
                else if (prop_type == L"datetime") type = VARENUM::VT_DATE;
                else if (prop_type == L"int") type = VARENUM::VT_UI4;
                else if (prop_type == L"long") type = VARENUM::VT_I4;

                CComBSTR prop_guid(str_data[4]);

                GUID created_id;
                //CreateSingleDynProperty(prop_name, prop_desc, type, prop_cat_name);
                CreateSingleDynProperty(str_data[0], str_data[1], type, str_data[3]);
            }
        }
    }
    file_data.close();     // закрываем файл
}
