#ifndef __SIMPLEDYNPROPERTY_H_
#define __SIMPLEDYNPROPERTY_H_

#include "stdafx.h"
#include "rxclass.h"
#include "dynprops.h"
#include <comdef.h>
#include "resource.h"       // main symbols

/// <summary>
/// Class for descripte dynamic properties, non categorized
/// </summary>
class ATL_NO_VTABLE SingleDynProperty :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IDynamicProperty
{
public:
    SingleDynProperty();

    BEGIN_COM_MAP(SingleDynProperty)
        COM_INTERFACE_ENTRY(IDynamicProperty)
    END_COM_MAP()
public:
    STDMETHOD(GetGUID)( /*[out]*/GUID* propGUID);
    STDMETHOD(GetDisplayName)( /*[out]*/BSTR* bstrName);
    STDMETHOD(IsPropertyEnabled)( /*[in]*/LONG_PTR objectID, /*[out]*/BOOL* pbEnabled);
    STDMETHOD(IsPropertyReadOnly)( /*[out]*/BOOL* pbReadonly);
    STDMETHOD(GetDescription)( /*[out]*/BSTR* bstrName);
    STDMETHOD(GetCurrentValueName)( /*[out]*/BSTR* pbstrName);
    STDMETHOD(GetCurrentValueType)( /*[out]*/VARTYPE* pVarType);
    STDMETHOD(GetCurrentValueData)( /*in*/LONG_PTR objectID, /*[out]*/VARIANT* pvarData);
    STDMETHOD(SetCurrentValueData)( /*[in]*/LONG_PTR objectID,/*[in]*/const VARIANT varData);

    //*** Notifications ***
    STDMETHOD(Connect)( /*[in]*/IDynamicPropertyNotify* pSink);
    STDMETHOD(Disconnect)();

    CComPtr<IDynamicPropertyNotify> m_pNotifySink;

    BSTR p_name;
    void set_name(bstr_t name);
    
    BSTR p_description;
    void set_description(bstr_t desk);
    
    VARTYPE p_valueType;
    void set_type(VARENUM type);

    BSTR prop_id;
    void set_guid(BSTR guid);
};
#endif

