#include "stdafx.h"
#include "resource.h"
#include "DynPropertiesManager.hpp"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)

END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

//extern "C"
//BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
//{
//	if (dwReason == DLL_PROCESS_ATTACH)
//	{
//		_Module.Init(ObjectMap, hInstance);
//		DisableThreadLibraryCalls(hInstance);
//	}
//	else if (dwReason == DLL_PROCESS_DETACH)
//		_Module.Term();
//	return TRUE;    // ok
//}


const TCHAR* cstrCommandGroup = _T("NCAD_PSETS");

extern "C" __declspec(dllexport) NcRx::AppRetCode
ncrxEntryPoint(NcRx::AppMsgCode msg, void* pkt)
{
	switch (msg) {
	case AcRx::kInitAppMsg:
        //unlock the application
		acrxDynamicLinker->unlockApplication(pkt);
        acrxDynamicLinker->registerAppMDIAware(pkt);
        DynPropertiesManager::CreateSingleDynProperty(L"Test_property", L"", VARENUM::VT_I4, L"Default Category", {}, NULL);
        acedRegCmds->addCommand(cstrCommandGroup,
            _T("_NCAD_PSETS_LoadPropsFromTextFile"),
            _T("NCAD_PSETS_LoadPropsFromTextFile"),
            ACRX_CMD_MODAL,
            DynPropertiesManager::ImportPropertiesByFile);
        acedRegCmds->addCommand(cstrCommandGroup,
            _T("_NCAD_PSETS_SaveToXmlFile"),
            _T("NCAD_PSETS_SaveToXmlFile"),
            ACRX_CMD_MODAL,
            DynPropertiesManager::SavePropertiesAndValueToFile);
        acedRegCmds->addCommand(cstrCommandGroup,
            _T("_NCAD_PSETS_LoadFromXmlFile"),
            _T("NCAD_PSETS_LoadFromXmlFile"),
            ACRX_CMD_MODAL,
            DynPropertiesManager::LoadPropertiesAndValuesFromFile);
        break;
	case AcRx::kUnloadAppMsg:
        acedRegCmds->removeGroup(cstrCommandGroup);
        break;
	}
	return AcRx::kRetOK;
}

