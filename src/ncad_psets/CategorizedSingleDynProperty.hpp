#ifndef __CATEGORIZEDSINGLEDYNPROPERTY_H_
#define __CATEGORIZEDSINGLEDYNPROPERTY_H_
#include "stdafx.h"


#include "resource.h"       // main symbols
#include "SimpleDynProperty.hpp"
#include "dynprops.h"

/// <summary>
/// Class for descripte dynamic properties that contains in category
/// </summary>
class ATL_NO_VTABLE CategorizedSingleDynProperty :
	public SingleDynProperty,
	public ICategorizeProperties
{
public:
	CategorizedSingleDynProperty();

	BEGIN_COM_MAP(CategorizedSingleDynProperty)
		COM_INTERFACE_ENTRY(ICategorizeProperties)
		COM_INTERFACE_ENTRY_CHAIN(SingleDynProperty)
	END_COM_MAP()
public:
    STDMETHOD(MapPropertyToCategory)(
        /* [in]  */ DISPID dispid,
        /* [out] */ PROPCAT* ppropcat);
    STDMETHOD(GetCategoryName)(
        /* [in]  */ PROPCAT propcat,
        /* [in]  */ LCID lcid,
        /* [out] */ BSTR* pbstrName);

    //edit property
    PROPCAT p_Cat;
    _bstr_t p_CatName;
    void set_category(PROPCAT cat_index, bstr_t cat_name);
};
#endif

