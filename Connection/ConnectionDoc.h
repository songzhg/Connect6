#pragma once


class CConnectionDoc : public CDocument
{
protected: // �������л�����
	CConnectionDoc();
	DECLARE_DYNCREATE(CConnectionDoc)

// ����
public:

public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CConnectionDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


