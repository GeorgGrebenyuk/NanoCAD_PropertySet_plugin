#ifndef __CATEGORIZEDSINGLEDYNPROPERTY_H_
#define __CATEGORIZEDSINGLEDYNPROPERTY_H_
#include "stdafx.h"


#include "resource.h"       // main symbols
//#include "SimpleDynProperty.hpp"
#include "dynprops.h"

/// <summary>
/// Class for descripte dynamic properties that contains in category
/// </summary>
class ATL_NO_VTABLE CategorizedSingleDynProperty :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IDynamicProperty,
	public ICategorizeProperties
{
public:
	CategorizedSingleDynProperty();

	BEGIN_COM_MAP(CategorizedSingleDynProperty)
        COM_INTERFACE_ENTRY(IDynamicProperty)
		COM_INTERFACE_ENTRY(ICategorizeProperties)
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

    
    STDMETHOD(MapPropertyToCategory)(
        /* [in]  */ DISPID dispid,
        /* [out] */ PROPCAT* ppropcat);
    STDMETHOD(GetCategoryName)(
        /* [in]  */ PROPCAT propcat,
        /* [in]  */ LCID lcid,
        /* [out] */ BSTR* pbstrName);

    //*** Notifications ***
    STDMETHOD(Connect)( /*[in]*/IDynamicPropertyNotify* pSink);
    STDMETHOD(Disconnect)();

    CComPtr<IDynamicPropertyNotify> m_pNotifySink;

    BSTR p_name;
    void set_name(BSTR name);

    BSTR p_description;
    void set_description(BSTR desk);

    VARTYPE p_valueType;
    void set_type(VARENUM type);

    BSTR prop_id;
    void set_guid(BSTR guid);

    //edit property
    PROPCAT p_Cat;
    _bstr_t p_CatName;
    void set_category(PROPCAT cat_index, BSTR cat_name);

};
#endif

