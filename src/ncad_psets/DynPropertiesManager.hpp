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
	/*Создание нового свойства с одиночным значением*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name, 
		/*in*/BSTR description, 
		/*in*/VARENUM type,
		/*in*/BSTR category_name);
	/*Присвоение объекту модели свойств с выбором*/
	static void AssignSingleDynPropertyToObject();
	/*Назначение объекту значения свойства*/
	static void AssignSingleDynPropertyValue(
		/*in*/ GUID property_id,
		/*in*/LONG_PTR objectID, 
		/*in*/VARIANT value);
	/*Удаление свойства из проекта*/
	static void RemoveSingleDynProperty(
		/*in*/ GUID property_id,
		/*in*/LONG_PTR objectID);
	/*Получение всех свойств (наименований и значений) у объекта*/
	static void GetDynProperties(
		/*in*/LONG_PTR objectID, 
		/*out*/std::map<BSTR, VARIANT> name2values);
	/*Получение всех категорий свойств в проекте*/
	static void GetCategories(
		/*out*/std::map<BSTR, PROPCAT>* categories);
	/*Получение индекса категории по его имени*/
	static void GetCategoryPropcatByName(
		/*in*/BSTR name, 
		/*out*/PROPCAT* propcat);

	/*Функции для массовых операций*/

	/*Создание свойств из внешнего текстового файла*/
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