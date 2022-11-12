#include "SimpleDynProperty.hpp"
#include "xrecordmanager.h"

SingleDynProperty::SingleDynProperty() :
    p_name(L"DefaultName"),
    p_description(L"111"),
    p_valueType(VT_BSTR)
{
    this->prop_id = NULL;
}
STDMETHODIMP SingleDynProperty::GetGUID( /*[out]*/GUID* propGUID)
{
    if (this->prop_id != NULL)
    {
        GUID guid;
        HRESULT hr = CLSIDFromString(this->prop_id, (LPCLSID)&guid);
        if (hr != S_OK) {
            // bad GUID string...
        }
        else *propGUID = guid;;
    }
    return E_NOTIMPL;
}
STDMETHODIMP SingleDynProperty::GetDisplayName( /*[out]*/BSTR* pBstrName)
{
    if (pBstrName == NULL)
        return E_POINTER;
    *pBstrName = ::SysAllocString(p_name);
    return S_OK;
}
STDMETHODIMP SingleDynProperty::IsPropertyEnabled( /*[in]*/LONG_PTR objectID,
    /*[out]*/BOOL* pbEnabled)
{
    if (pbEnabled == NULL)
        return E_POINTER;
    AcDbObjectId id;
    id.setFromOldId(objectID);
    *pbEnabled = XRecordManager::isDataPresent(id) ? TRUE : FALSE;
    return S_OK;
}
STDMETHODIMP SingleDynProperty::IsPropertyReadOnly( /*[out]*/BOOL* pbReadonly)
{
    if (pbReadonly == NULL)
        return E_POINTER;
    *pbReadonly = FALSE;
    return S_OK;
}
STDMETHODIMP SingleDynProperty::GetDescription( /*[out]*/BSTR* pBstrName)
{
    if (pBstrName == NULL)
        return E_POINTER;
    *pBstrName = ::SysAllocString(p_description);
    return S_OK;
}

// *** Basic property value information ***
STDMETHODIMP SingleDynProperty::GetCurrentValueName( /*[out]*/BSTR* pbstrName)
{
    return E_NOTIMPL;
}
STDMETHODIMP SingleDynProperty::GetCurrentValueType( /*[out]*/VARTYPE* pVarType)
{
    if (pVarType == NULL)
        return E_POINTER;
    *pVarType = p_valueType;
    return S_OK;
}
STDMETHODIMP SingleDynProperty::GetCurrentValueData( /*in*/LONG_PTR objectID,
    /*[out]*/VARIANT* pVarData)
{
    if (pVarData == NULL)
        return E_POINTER;
    AcDbObjectId id;
    id.setFromOldId(objectID);
    resbuf data;
    if (XRecordManager::getData(id, 0, data) != Acad::eOk)
        return E_FAIL;
    switch (this->p_valueType)
    {
    case VARENUM::VT_R8:
        ::VariantCopy(pVarData, &_variant_t(data.resval.rreal));
        break;
        case VARENUM::VT_BOOL |
        VARENUM::VT_BOOL:
            ::VariantCopy(pVarData, &_variant_t(data.resval.rint, this->p_valueType));
            break;
    }

    return S_OK;
}
STDMETHODIMP SingleDynProperty::SetCurrentValueData( /*[in]*/LONG_PTR objectID,
    /*[in]*/const VARIANT varData)
{
    AcDbObjectId id;
    id.setFromOldId(objectID);
    resbuf data;

    switch (this->p_valueType)
    {
    case VARENUM::VT_R8:
        data.restype = AcDb::kDxfReal;
        data.resval.rreal = V_R8(&varData);
        break;
        case VARENUM::VT_BOOL |
        VARENUM::VT_BOOL:
            data.restype = AcDb::kDxfBool;
            data.resval.rint = V_BOOL(&varData);
            break;
        case VARENUM::VT_BSTR | VARENUM::VT_BSTR_BLOB:
            data.restype = AcDb::kDxfText;
            data.resval.rstring = V_BSTR(&varData);
            break;
    }
    if (XRecordManager::setData(id, 0, data) != Acad::eOk)
        return E_FAIL;
    //if the dynamic property is changed by some other
    if (m_pNotifySink != NULL)
        m_pNotifySink->OnChanged(this);
    return S_OK;
}

//*** Notifications ***
STDMETHODIMP SingleDynProperty::Connect( /*[in]*/IDynamicPropertyNotify* pSink)
{
    if (pSink == NULL)
        return E_POINTER;
    m_pNotifySink = pSink;
    return S_OK;
}
STDMETHODIMP SingleDynProperty::Disconnect()
{
    m_pNotifySink.Release();
    return S_OK;
}
void SingleDynProperty::set_name(bstr_t name)
{
    this->p_name = name;

}
void SingleDynProperty::set_type(VARENUM type)
{
    this->p_valueType = type;
}
void SingleDynProperty::set_guid(BSTR guid)
{
    this->prop_id = guid;
}
void SingleDynProperty::set_description(bstr_t desk)
{
    this->p_description = desk;
}