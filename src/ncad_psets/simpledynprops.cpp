//
// (C) Copyright 1998-2006 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
// SimpleDynProp.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f SimpleDynPropps.mk in the project directory.
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


//assign the xrecord to the object
//the xrecord stores the value for the properties
//for now it looks like this:
//double,double,long
//
const TCHAR* cstrCommandGroup = _T("ASDK_SIMPLEDYNPROPS");

void create_props() 
{
    const char* path = "C:\\Users\\Georg\\Documents\\GitHub\\ncad_psets\\examples\\test_external_props2.csv";
    CComBSTR path_bstr(path);
    DynPropertiesManager::LoadPropsFromList(path_bstr);
}
extern "C" __declspec(dllexport) NcRx::AppRetCode
ncrxEntryPoint(NcRx::AppMsgCode msg, void* pkt)
{
	switch (msg) {
	case AcRx::kInitAppMsg:
        //unlock the application
		acrxDynamicLinker->unlockApplication(pkt);
        acrxDynamicLinker->registerAppMDIAware(pkt);
        
        DynPropertiesManager::initialize();
        acedRegCmds->addCommand(cstrCommandGroup,_T("_add_to_object"),_T("add_to_object"), ACRX_CMD_MODAL, DynPropertiesManager::AssignSingleDynPropertyToObject);
        acedRegCmds->addCommand(cstrCommandGroup, _T("_create_props"), _T("create_props"), ACRX_CMD_MODAL, create_props);
        acedRegCmds->addCommand(cstrCommandGroup, _T("_getting_info"), _T("getting_info"), ACRX_CMD_MODAL, DynPropertiesManager::WriteDiagnostikInfo);
		break;
	case AcRx::kUnloadAppMsg:
        acedRegCmds->removeGroup(cstrCommandGroup);
        DynPropertiesManager::uninitialize();
        break;
	}
	return AcRx::kRetOK;
}

