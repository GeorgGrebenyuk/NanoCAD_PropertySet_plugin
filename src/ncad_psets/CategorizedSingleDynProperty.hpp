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
    STDMETHOD(GetGUID)( /*[out]*/GUID* propGUID) override;
    STDMETHOD(GetDisplayName)( /*[out]*/BSTR* bstrName) override;
    STDMETHOD(IsPropertyEnabled)( /*[in]*/LONG_PTR objectID, /*[out]*/BOOL* pbEnabled) override;
    STDMETHOD(IsPropertyReadOnly)( /*[out]*/BOOL* pbReadonly) override;
    STDMETHOD(GetDescription)( /*[out]*/BSTR* bstrName) override;
    STDMETHOD(GetCurrentValueName)( /*[out]*/BSTR* pbstrName) override;
    STDMETHOD(GetCurrentValueType)( /*[out]*/VARTYPE* pVarType) override;
    STDMETHOD(GetCurrentValueData)( /*in*/LONG_PTR objectID, /*[out]*/VARIANT* pvarData) override;
    STDMETHOD(SetCurrentValueData)( /*[in]*/LONG_PTR objectID,/*[in]*/const VARIANT varData) override;
    //*** Notifications ***
    STDMETHOD(Connect)( /*[in]*/IDynamicPropertyNotify* pSink) override;
    STDMETHOD(Disconnect)() override;
    
    STDMETHOD(MapPropertyToCategory)(
        /* [in]  */ DISPID dispid,
        /* [out] */ PROPCAT* ppropcat);
    STDMETHOD(GetCategoryName)(
        /* [in]  */ PROPCAT propcat,
        /* [in]  */ LCID lcid,
        /* [out] */ BSTR* pbstrName);
    CComPtr<IDynamicPropertyNotify> m_pNotifySink;

    GUID p_guid;
    BSTR p_name;
    BSTR p_description;
    VARTYPE p_valueType;
    PROPCAT p_Cat;
    BSTR p_CatName;
    std::vector<BSTR> p_class_names;
    /*mode= 0: �������� ������������ ��� ���������� � ���� ������� 
    � ����������� �������� ����������� �������;
      mode= 1: �������� ������������ ������ � ��������, ������� �������� 
    (������ ��� ������� ������� ������, ��������, ���)*/
    int show_mode = 0;
};
#endif

