#include "stdafx.h"

#include "CategorizedSingleDynProperty.hpp"
#include "DynPropertiesManager.hpp"
#include <sstream>
////////////////////////////
CategorizedSingleDynProperty::CategorizedSingleDynProperty()
    :p_Cat(1), p_CatName(L"Default Category"), p_name(L"RootProperty"), p_description(L""),
    p_valueType(VT_I4)
{

}
STDMETHODIMP CategorizedSingleDynProperty::GetGUID( /*[out]*/GUID* propGUID)
{
    *propGUID = p_guid;
    return E_NOTIMPL;
}
STDMETHODIMP CategorizedSingleDynProperty::GetDisplayName( /*[out]*/BSTR* pBstrName)
{
    if (pBstrName == NULL)
        return E_POINTER;
    *pBstrName = ::SysAllocString(p_name);
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::IsPropertyEnabled( /*[in]*/LONG_PTR objectID,
    /*[out]*/BOOL* pbEnabled)
{
    if (pbEnabled == NULL)
        return E_POINTER;
    AcDbObjectId id;
    id.setFromOldId(objectID);
    //*pbEnabled = XRecordManager::isDataPresent(id) ? TRUE : FALSE;
    *pbEnabled = TRUE;
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::IsPropertyReadOnly( /*[out]*/BOOL* pbReadonly)
{
    if (pbReadonly == NULL)
        return E_POINTER;
    *pbReadonly = FALSE;
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::GetDescription( /*[out]*/BSTR* pBstrName)
{
    if (pBstrName == NULL)
        return E_POINTER;
    *pBstrName = ::SysAllocString(p_description); //p_description
    return S_OK;
}

// *** Basic property value information ***
STDMETHODIMP CategorizedSingleDynProperty::GetCurrentValueName( /*[out]*/BSTR* pbstrName)
{
    return E_NOTIMPL;
}
STDMETHODIMP CategorizedSingleDynProperty::GetCurrentValueType( /*[out]*/VARTYPE* pVarType)
{
    if (pVarType == NULL)
        return E_POINTER;
    *pVarType = p_valueType;
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::GetCurrentValueData( /*in*/LONG_PTR objectID,
    /*[out]*/VARIANT* pVarData)
{
    if (pVarData == NULL)
        return E_POINTER;
    AcDbObjectId id;
    id.setFromOldId(objectID);
    GUID pr_id;
    this->GetGUID(&pr_id);

    _variant_t for_data;
    bool is_value_present = 
        DynPropertiesManager::GetPropertyValue(&id, &pr_id, &for_data);
    if (is_value_present)
    {
        ::VariantCopy(pVarData, &for_data);
    }
    else
    {
        VARIANT current_v;
        switch (this->p_valueType)
        {
        case VARENUM::VT_BSTR:
            current_v = _variant_t(L"test1");
            break;
        case VARENUM::VT_I4:
            current_v = _variant_t(0);
            break;
        case VARENUM::VT_R8:
            current_v = _variant_t(0.0);
            break;
        }
        ::VariantCopy(pVarData, &_variant_t(current_v));
    }

    
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::SetCurrentValueData( /*[in]*/LONG_PTR objectID,
    /*[in]*/const VARIANT varData)
{
    AcDbObjectId id;
    id.setFromOldId(objectID);

    //is_value_assigned = true;
    GUID pr_id;
    this->GetGUID(&pr_id);

    _variant_t setted_data;
    switch (this->p_valueType)
    {
    case VT_BSTR:
        setted_data = _variant_t(V_BSTR(&varData));
        break;
    case VT_I4:
        setted_data = _variant_t(V_I4(&varData));
        break;
    case VT_R8:
        setted_data = _variant_t(V_R8(&varData));
        break;
    case VT_UI4:
        setted_data = _variant_t(V_UI4(&varData));
        break;
    }

    DynPropertiesManager::SetPropertyValue(&id, &pr_id, &setted_data);
    //if the dynamic property is changed by some other
    if (m_pNotifySink != NULL)
        m_pNotifySink->OnChanged(this);
    return S_OK;
}

//*** Notifications ***
STDMETHODIMP CategorizedSingleDynProperty::Connect( /*[in]*/IDynamicPropertyNotify* pSink)
{
    if (pSink == NULL)
        return E_POINTER;
    m_pNotifySink = pSink;
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::Disconnect()
{
    m_pNotifySink.Release();
    return S_OK;
}

STDMETHODIMP CategorizedSingleDynProperty::MapPropertyToCategory(
    /* [in]  */ DISPID dispid,
    /* [out] */ PROPCAT* pPropCat)
{
    if (pPropCat == NULL)
        return E_POINTER;
    *pPropCat = p_Cat;
    return S_OK;
}
STDMETHODIMP CategorizedSingleDynProperty::GetCategoryName(
    /* [in]  */ PROPCAT propcat,
    /* [in]  */ LCID lcid,
    /* [out] */ BSTR* pBstrName)
{
    if (pBstrName == NULL)
        return E_POINTER;
    //if (propcat != 1)
    //    return E_FAIL;
    //*pBstrName = ::SysAllocString(L"Default Category");
    *pBstrName = ::SysAllocString(p_CatName);
    return S_OK;
}