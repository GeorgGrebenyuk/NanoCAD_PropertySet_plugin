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

class DynPropertiesManager : public AcApDocManagerReactor
{
public:
	/*Версия данного исходного кода (для будущих включений в другой код)*/
	const BSTR version = L"1.0.0";
	static AcRxClass* m_pClass;
	//////////////////////////////////////////////////////////////////////////////////////
	/*Функции со стороны пользователя (остальное позже спрятать в protected)*/
	//////////////////////////////////////////////////////////////////////////////////////
	static void CreateIntegerDynProperty(std::string Name, std::string Description, 
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, int DefaultValue);
	static void CreateRealDynProperty(std::string Name, std::string Description,
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, double DefaultValue);
	static void CreateStringDynProperty(std::string Name, std::string Description,
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, std::string DefaultValue);
	//////////////////////////////////////////////////////////////////////////////////////
	/*Внутренние функции*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*Создание нового свойства с одиночным значением*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name,
		/*in*/BSTR description,
		/*in*/VARENUM type,
		/*in*/BSTR category_name,
		/*in*/std::vector<BSTR> class_names,
		/*in*/BSTR id,
		/*in*/int show_mode);
	/*Удаление регистрации всех свойств из PropertyManager*/
	static void RemoveAllProperties();
	//////////////////////////////////////////////////////////////////////////////////////
	/*Контейнеры*/
	//////////////////////////////////////////////////////////////////////////////////////
	static std::vector<BSTR> categories_names;
	/*Эти параметры (свойств) регистрируются на всю сессию, а не чертёж*/
	static int dyn_s_props_counter;
	static std::vector<CComObject<CategorizedSingleDynProperty>*> dyn_s_props;
	/* В силу того, что свойства регистрируются на сессию приложения, необходимо
	*  переопределить, в каком документе они будут отображаться, а в каком -- не будут.
	* Словарь далее хранит текущую БД документа, а аргумент -- номера свойств из dyn_s_props,
	* которые для документа разрешены
	*/
	static std::map<std::wstring, std::vector<int>> document2properties;

	//static std::map<GUID, int> props_id2index;
	/*Сохранение объектных значений в процессе работы с чертежом*/
	static std::map<AcDbObjectId, std::map<GUID, _variant_t>> objects2properties;
	/*Запись значения свойства объекта в objects2properties*/
	static void SetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);
	/*Получает значение свойства объекта из objects2properties*/
	static bool GetPropertyValue(AcDbObjectId* id, GUID* property_id, _variant_t* data);

	//////////////////////////////////////////////////////////////////////////////////////
	/*Функции импорта-экспорта*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*Загрузка свойств и и их значений из внешнего файла*/
	static void LoadPropertiesAndValuesFromFile();

	/*Запись свойств и их значений в файл*/
	static void SavePropertiesAndValueToFile();

	/*Импорт определений свойств из текстового файла (ОТЛАДОЧНЫЙ МЕТОД)*/
	static void ImportPropertiesByFile();

	static void WritePropsToConsole();

	static void DeleteAllProperties();
	/*Nandlers of document*/
	//static AcApDocument* pDoc_pointer;
	static std::string pDoc_name;
	static bool status_eventhandler_work;


};
#endif