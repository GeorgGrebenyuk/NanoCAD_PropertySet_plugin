#ifndef _XRECORDMANAGER_H
#define _XRECORDMANAGER_H

#include "dbents.h"
#include "dbxrecrd.h"

//this class encapsulates the manipulation
//of an xrecord object in the xtension dictionary
//of an object
class XRecordManager
{
    static const TCHAR* m_strKey;
    static Acad::ErrorStatus getXRecord(const AcDbObjectId& id,AcDbObjectId& idX);
public:
    static Acad::ErrorStatus createDefaultData(const AcDbObjectId& id, resbuf* rb);
    static Acad::ErrorStatus removeData(const AcDbObjectId& id);
    static bool isDataPresent(const AcDbObjectId& id);

    static Acad::ErrorStatus setData(const AcDbObjectId& id, long index, const resbuf& data);
    static Acad::ErrorStatus getData(const AcDbObjectId& id, long index, resbuf& data);
};

#endif