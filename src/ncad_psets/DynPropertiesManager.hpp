#ifndef __DYNPROPERTIESMANAGER_H_
#define __DYNPROPERTIESMANAGER_H_

#include "rxdlinkr.h"
#include "dbents.h"
#include "accmd.h"
#include "adslib.h"
#include "resource.h"
#include "initguid.h"
#include <comdef.h>

#include "vector"
#include "map"

#include "CategorizedSingleDynProperty.hpp"
#include "CategorizedListDynProperty.hpp"

//CComModule _Module;
//
//BEGIN_OBJECT_MAP(ObjectMap)
//END_OBJECT_MAP()

class DynPropertiesManager
{
	static bool m_bInitialized;
public:
	static bool isInitialized() { return m_bInitialized; }
	//DynPropertiesManager();
	static AcRxClass* m_pClass;
	//static CComPtr<IPropertyManager>* p_prop_manager;
	static void initialize();
	static void uninitialize();
	/*�������� ������ �������� � ��������� ���������*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name, 
		/*in*/BSTR description, 
		/*in*/VARENUM type,
		/*in*/BSTR category_name);
	/*���������� ������� ������ ������� � �������*/
	static void AssignSingleDynPropertyToObject();
	/*���������� ������� �������� ��������*/
	static void AssignSingleDynPropertyValue(
		/*in*/ GUID property_id,
		/*in*/LONG_PTR objectID, 
		/*in*/VARIANT value);
	/*�������� �������� �� �������*/
	static void RemoveSingleDynProperty(
		/*in*/ GUID property_id,
		/*in*/LONG_PTR objectID);
	/*��������� ���� ������� (������������ � ��������) � �������*/
	static void GetDynProperties(
		/*in*/LONG_PTR objectID, 
		/*out*/std::map<BSTR, VARIANT> name2values);
	/*��������� ���� ��������� ������� � �������*/
	static void GetCategories(
		/*out*/std::map<BSTR, PROPCAT>* categories);
	/*��������� ������� ��������� �� ��� �����*/
	static void GetCategoryPropcatByName(
		/*in*/BSTR name, 
		/*out*/PROPCAT* propcat);

	/*������� ��� �������� ��������*/

	/*�������� ������� �� �������� ���������� �����*/
	/*
		name | description | vartype | category_name
	*/
	static void LoadPropsFromList(BSTR file_path);

	static std::map<BSTR, int> category_name2id;
	static bool SaveSinglePropertyIfNeed (CComObject<CategorizedSingleDynProperty>* _property);
	static std::map<int, std::vector<CComObject<CategorizedSingleDynProperty>*>> category2single_props;

	static void WriteDiagnostikInfo();
};
#endif