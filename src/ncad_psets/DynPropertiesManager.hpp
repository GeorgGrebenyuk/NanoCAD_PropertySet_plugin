#ifndef __DYNPROPERTIESMANAGER_H_
#define __DYNPROPERTIESMANAGER_H_

//#include "rxdlinkr.h"
//#include "dbents.h"
//#include "accmd.h"
//#include "adslib.h"
#include "resource.h"
//#include "initguid.h"
#include <comdef.h>

#include "vector"
#include "map"

#include "CategorizedSingleDynProperty.hpp"

class DynPropertiesManager
{
public:
	//DynPropertiesManager();
	static AcRxClass* m_pClass;
	//static CComPtr<IPropertyManager>* p_prop_manager;
	//static CComPtr<IPropertyManager>* p_prop_manager;


	/*�������� ������ �������� � ��������� ���������*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name,
		/*in*/BSTR description,
		/*in*/VARENUM type,
		/*in*/BSTR category_name,
		/*in*/std::vector<BSTR> class_names,
		/*in*/BSTR id);

	//////////////////////////////////////////////////////////////////////////////////////
	/*����������*/
	//////////////////////////////////////////////////////////////////////////////////////
	static std::vector<BSTR> categories_names;

	static std::vector<CComObject<CategorizedSingleDynProperty>*> dyn_s_props;
	//static std::map<GUID, int> props_id2index;
	/*���������� ��������� �������� � �������� ������ � ��������*/
	static std::map<AcDbObjectId, std::map<GUID, _variant_t>> objects2properties;
	/*������ �������� �������� ������� � objects2properties*/
	static void SetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);
	/*�������� �������� �������� ������� �� objects2properties*/
	static bool GetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);

	//////////////////////////////////////////////////////////////////////////////////////
	/*������� �������*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*�������� ������� � � �� �������� �� �������� �����*/
	static void LoadPropertiesAndValuesFromFile();

	/*������ ������� � �� �������� � ����*/
	static void SavePropertiesAndValueToFile();

	/*������ ����������� ������� �� ���������� ����� (���������� �����)*/
	static void ImportPropertiesByFile();

};
#endif