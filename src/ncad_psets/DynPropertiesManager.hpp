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
	/*������ ������� ��������� ���� (��� ������� ��������� � ������ ���)*/
	const BSTR version = L"1.0.0";
	static AcRxClass* m_pClass;
	//////////////////////////////////////////////////////////////////////////////////////
	/*������� �� ������� ������������ (��������� ����� �������� � protected)*/
	//////////////////////////////////////////////////////////////////////////////////////
	static void CreateIntegerDynProperty(std::string Name, std::string Description, 
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, int DefaultValue);
	static void CreateRealDynProperty(std::string Name, std::string Description,
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, double DefaultValue);
	static void CreateStringDynProperty(std::string Name, std::string Description,
		std::string CategoryName, std::vector<std::string> class_names, std::string Guid, std::string DefaultValue);
	//////////////////////////////////////////////////////////////////////////////////////
	/*���������� �������*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*�������� ������ �������� � ��������� ���������*/
	static void CreateSingleDynProperty(
		/*in*/BSTR name,
		/*in*/BSTR description,
		/*in*/VARENUM type,
		/*in*/BSTR category_name,
		/*in*/std::vector<BSTR> class_names,
		/*in*/BSTR id);
	/*�������� ����������� ���� ������� �� PropertyManager*/
	static void RemoveAllProperties();
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
	/*������� �������-��������*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*�������� ������� � � �� �������� �� �������� �����*/
	static void LoadPropertiesAndValuesFromFile();

	/*������ ������� � �� �������� � ����*/
	static void SavePropertiesAndValueToFile();

	/*������ ����������� ������� �� ���������� ����� (���������� �����)*/
	static void ImportPropertiesByFile();

	static void WritePropsToConsole();

	static void DeleteAllProperties();

};
#endif