
#include "stdafx.h"
#include "DynPropertiesManager.hpp"
#include "xrecordmanager.h"

bool DynPropertiesManager::m_bInitialized = false;
//std::vector<CComObject<CategorizedSingleDynProperty>*> DynPropertiesManager::single_value_properties = { NULL };
//
std::map<int, std::vector<CComObject<CategorizedSingleDynProperty>*>> DynPropertiesManager::category2single_props;
std::map<BSTR, int> DynPropertiesManager::category_name2id;

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
        cat_props->set_description(L"first descr");
        cat_props->set_type(VARENUM::VT_BSTR);
        cat_props->set_category(1, L"DefCat");
        cat_props->AddRef();

        
        //LONG count;
        //prop_manager->GetDynamicPropertyCount(&count);
        //single_value_properties.push_back(&count);
        if (SaveSinglePropertyIfNeed(cat_props))
        {
            category_name2id.insert(std::pair<BSTR, int>(L"DefCat", 1));
            _com_util::CheckError(prop_manager->AddProperty(cat_props));
        }
        else {
            _com_util::CheckError(prop_manager->RemoveProperty(cat_props));
            cat_props->Release();

        }        
        
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
        for (auto cat_props : category2single_props)
        {
            for (auto s_props : cat_props.second)
            {
                if (s_props)
                {
                    //CComPtr<IDynamicProperty> prop;
                    //HRESULT res = prop_manager->GetDynamicProperty(*single_prop, &prop);
                    _com_util::CheckError(prop_manager->RemoveProperty(s_props));
                    s_props->Release();
                    //prop->Release();
                }
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
    bool need_ad = SaveSinglePropertyIfNeed(new_property);
    if (need_ad)
    {
        _com_util::CheckError(prop_manager->AddProperty(new_property));
    }
    else {
        prop_manager->RemoveProperty(new_property);
        new_property->Release();
    }
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
    /*int counter_categories = 1;
    do 
    {
        BSTR name = NULL;
        temp_property->GetCategoryName(counter_categories, NULL, &name);
        if (name != NULL) categories->insert(std::pair< BSTR, PROPCAT>(name, counter_categories));
        else need_continue = false;
        counter_categories++;
    } while (need_continue);*/
    LONG count;
    prop_manager->GetDynamicPropertyCount(&count);
    for (int i = 0; i < count; i++)
    {
        BSTR name = NULL;
        temp_property->GetCategoryName(i, NULL, &name);
        if (name != NULL) categories->insert(std::pair< BSTR, PROPCAT>(name, i));
    }


    _com_util::CheckError(prop_manager->RemoveProperty(temp_property));
    temp_property->Release();

}

void DynPropertiesManager::GetCategoryPropcatByName(BSTR name, PROPCAT* propcat)
{
    //std::map<BSTR, PROPCAT> all_cats;
    //GetCategories(&all_cats);
    //category_name2id.insert(std::pair<BSTR, int>(L"DefCat", 1));
    //bool f = false;
    for (auto cat : category_name2id)
    {
        if (wcscmp(cat.first, name) == 0)
        {
            *propcat = cat.second;
            return;
        }
    }
    int size = category_name2id.size() + 1;
    category_name2id.insert(std::pair<BSTR, int>(name, size));
    *propcat = size;
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

bool DynPropertiesManager::SaveSinglePropertyIfNeed(CComObject<CategorizedSingleDynProperty>* _property)
{
    if (category2single_props.empty()) 
    {
        category2single_props.insert(std::pair<int, std::vector<CComObject<CategorizedSingleDynProperty>*>> {_property->p_Cat, { _property }});
        return true;
    }
    for (auto p : category2single_props)
    {
        if (p.first == _property->p_Cat)
        {
            for (auto pr : p.second)
            {
                if (wcscmp(pr->p_name, _property->p_name) == 0)
                    return false;
            }
            p.second.push_back(_property);
            return true;
        }
    }
    //Если такового свойства не было найдено
    category2single_props.insert(std::pair<int, std::vector<CComObject<CategorizedSingleDynProperty>*>> {_property->p_Cat, { _property }});
    return true;

}
static char* bstr_to_str(BSTR source) {
    //source = L"lol2inside";
    _bstr_t wrapped_bstr = _bstr_t(source);
    int length = wrapped_bstr.length();
    char* char_array = new char[length];
    strcpy_s(char_array, length + 1, wrapped_bstr);
    return char_array;
}
void DynPropertiesManager::WriteDiagnostikInfo() {
    acutPrintf(_T("\nInfo about categories - names and PROPCAT"));
    for (auto a : category_name2id)
    {
        stringstream ss;
        ss << "\n" << bstr_to_str(a.first) << " " << a.second;
        CComBSTR str(ss.str().c_str());
        acutPrintf(str);
        ss.clear();
    }
    acutPrintf(_T("\nInfo about categories - and linked properties"));
    for (auto a : category2single_props)
    {

        stringstream ss;
        ss << "\n Info for category = " << a.first;
        CComBSTR str(ss.str().c_str());
        acutPrintf(str);
        ss.clear();
        for (auto pr : a.second)
        {
            stringstream ss2;
            ss2 << "\n Property info: " << 
                bstr_to_str(pr->p_name) << " " << 
                bstr_to_str(pr->p_description) << " " << 
                pr->p_Cat << " " << 
                bstr_to_str(pr->p_CatName) << " " <<
                pr->p_valueType;
            CComBSTR str2(ss2.str().c_str());
            acutPrintf(str2);
            ss2.clear();
        }
    }
}

