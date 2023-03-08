#include "DocumentReactor.hpp"
//ACRX_CONS_DEFINE_MEMBERS(DocumentReactor, AcApDocManagerReactor, 1)

//void DocumentReactor::beginDwgOpen(NCHAR* name)
//{
//    //парсинг psets_folder и прогрузка на него xml-файла на этот документ при наличии
//    DynPropertiesManager::pDoc_id = aux_functions::ToStringFromWString(name, ru_loc).c_str();
//    DynPropertiesManager::status_eventhandler_work = true;
//    DynPropertiesManager::LoadPropertiesAndValuesFromFile();
//    DynPropertiesManager::status_eventhandler_work = false;
//    //DynPropertiesManager::pDoc_pointer = NULL;
//    DynPropertiesManager::pDoc_name = "";
//}
void DocumentReactor2::beginDocClose(NcDbDatabase* pDwg)
{
    //DynPropertiesManager::pDoc_pointer = pDoc;
    DynPropertiesManager::SavePropertiesAndValueToFile();
    //DynPropertiesManager::pDoc_pointer = NULL;
}