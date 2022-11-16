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
	//static CComPtr<IPropertyManager>* p_prop_manager;
	static void initialize();
	static void uninitialize();
	/*Создание нового свойства с одиночным значением*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name, 
		/*in*/BSTR description, 
		/*in*/VARENUM type,
		/*in*/BSTR category_name);
	/*Присвоение объекту модели свойств с выбором*/
	static void AssignSingleDynPropertyToObject();
	//////////////
	/*Контейнеры*/
	//////////////
	static std::vector<CComObject<CategorizedSingleDynProperty>*> dyn_s_props;
	//static std::map<GUID, int> props_id2index;
	/*Сохранение объектных значений в процессе работы с чертежом*/
	static std::map<AcDbObjectId, std::map<GUID, VARIANT>> objects2properties;
	/*Запись значения свойства объекта в objects2properties*/
	static void SetPropertyValue(AcDbObjectId* id, GUID* property_id, VARIANT* data);
	/*Получает значение свойства объекта из objects2properties*/
	static bool GetPropertyValue(AcDbObjectId* id, GUID* property_id, VARIANT* data);
};
#endif