#include "DocumentReactor.hpp"
//ACRX_CONS_DEFINE_MEMBERS(DocumentReactor, AcApDocManagerReactor, 1)

void DocumentReactor::beginDwgOpen(NCHAR* name)
{
    //������� psets_folder � ��������� �� ���� xml-����� �� ���� �������� ��� �������
    DynPropertiesManager::pDoc_name = aux_functions::ToStringFromWString(name, ru_loc).c_str();
    DynPropertiesManager::status_eventhandler_work = true;
    DynPropertiesManager::LoadPropertiesAndValuesFromFile();
    DynPropertiesManager::status_eventhandler_work = false;
    //DynPropertiesManager::pDoc_pointer = NULL;
    DynPropertiesManager::pDoc_name = "";
}
void DocumentReactor2::beginDocClose(NcDbDatabase* pDwg)
{
    //���������� ������� � ���� � psets_folder
    NCHAR* title_info;
    auto wait_getting = pDwg->getProjectName(title_info);
    DynPropertiesManager::pDoc_name = aux_functions::ToStringFromWString(title_info, ru_loc).c_str();

    //DynPropertiesManager::pDoc_pointer = pDoc;
    DynPropertiesManager::SavePropertiesAndValueToFile();
    DynPropertiesManager::pDoc_name = "";
    //DynPropertiesManager::pDoc_pointer = NULL;
}