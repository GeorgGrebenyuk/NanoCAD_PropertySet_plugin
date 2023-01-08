#ifndef __DOCUMENTREACTOR_H_
#define __DOCUMENTREACTOR_H_
#include "stdafx.h"
#include "dbmain.h"
#include "DynPropertiesManager.hpp"
#include "aux_functions.h"

class DocumentReactor : public AcEditorReactor
{
public:
	//ACRX_DECLARE_MEMBERS(DocumentReactor);
public:
	//virtual void documentToBeActivated(AcApDocument* pDoc);
	virtual void beginDwgOpen(NCHAR* name);
	//virtual void beginQuit();
};
class DocumentReactor2 : public AcEditorReactor2
{
public:
	//ACRX_DECLARE_MEMBERS(DocumentReactor);
public:
	virtual void beginDocClose(NcDbDatabase* pDwg);
};
#endif

