#include "stdafx.h"

#include "CategorizedSingleDynProperty.hpp"
#include "xrecordmanager.h"


////////////////////////////
CategorizedSingleDynProperty::CategorizedSingleDynProperty()
    :p_Cat(1), p_CatName(L"Default"), p_name(L"RootProperty"), p_description(L""),
    p_valueType(VT_BSTR), prop_id (NULL)
{
    // 

    //p_name = L"DefaultName";
    //p_description = L"111";
    //p_valueType = VT_BSTR;
    //prop_id = NULL;
}

void CategorizedSingleDynProperty::set_category(PROPCAT cat_index, BSTR cat_name)
{
    this->p_Cat = cat_index;
    this->p_CatName = cat_name;
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
    *pBstrName = ::SysAllocString(p_CatName);
    return S_OK;
}