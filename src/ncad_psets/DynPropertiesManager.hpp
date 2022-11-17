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
	static CComPtr<IPropertyManager>* p_prop_manager;
	//static CComPtr<IPropertyManager>* p_prop_manager;
	static void initialize();
	static void uninitialize();
	/*�������� ������ �������� � ��������� ���������*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name, 
		/*in*/BSTR description, 
		/*in*/VARENUM type,
		/*in*/BSTR category_name, 
		/*in*/BSTR id = NULL);

	//////////////
	/*����������*/
	//////////////
	static std::vector<BSTR> categories_names;

	static std::vector<CComObject<CategorizedSingleDynProperty>*> dyn_s_props;
	//static std::map<GUID, int> props_id2index;
	/*���������� ��������� �������� � �������� ������ � ��������*/
	static std::map<AcDbObjectId, std::map<GUID, _variant_t>> objects2properties;
	/*������ �������� �������� ������� � objects2properties*/
	static void SetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);
	/*�������� �������� �������� ������� �� objects2properties*/
	static bool GetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);

	/*������� �������*/

	/*������ ����������� ������� �� ���������� ����� (���������� �����)*/
	static void ImportPropertiesByFile();
};
#endif