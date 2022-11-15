
#include "stdafx.h"
#include "DynPropertiesManager.hpp"
#include "xrecordmanager.h"

bool DynPropertiesManager::m_bInitialized = false;
std::vector<CComObject<CategorizedSingleDynProperty>*>DynPropertiesManager::dyn_s_props{ };

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
                resbuf rb;
                DynPropertiesManager::SetResbuf(&rb);
                XRecordManager::createDefaultData(id, &rb);
            }
        }
    }
    ads_ssfree(ss);
}
void DynPropertiesManager::SetResbuf(resbuf* rb)
{
    rb = acutBuildList(AcDb::kDxfXTextString, L"First value", AcDb::kDxfInt32, 2, AcDb::kDxfReal, 3.34);
    /*for (int i = dyn_s_props.size(); i <= 0; i--)
    {
        auto pr = dyn_s_props[i];
        if (pr != NULL)
        {
            switch (pr->p_valueType)
            {
            case VARENUM::VT_BSTR:
                rb->rbnext = acutBuildList(AcDb::kDxfXTextString, L"Test");
                break;
            case VARENUM::VT_I4:
                rb->rbnext = acutBuildList(AcDb::kDxfInt32, 1);
                break;
            case VARENUM::VT_R8:
                rb->rbnext = acutBuildList(AcDb::kDxfReal, 1.2);
                break;
            case VARENUM::VT_UI4:
                rb->rbnext = acutBuildList(AcDb::kDxfInt64, 24);
                break;
            }
        }
    }*/
}
