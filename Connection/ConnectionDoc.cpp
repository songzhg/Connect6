
// SXU_Connect6Doc.cpp : CConnectionDoc 类的实现
//
#include "stdafx.h"
#include "Connection.h"
#include "ConnectionDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConnectionDoc

IMPLEMENT_DYNCREATE(CConnectionDoc, CDocument)

BEGIN_MESSAGE_MAP(CConnectionDoc, CDocument)
END_MESSAGE_MAP()


CConnectionDoc::CConnectionDoc()
{

}

CConnectionDoc::~CConnectionDoc()
{
}

BOOL CConnectionDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



void CConnectionDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}


#ifdef _DEBUG
void CConnectionDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CConnectionDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

