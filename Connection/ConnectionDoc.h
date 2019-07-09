#pragma once


class CConnectionDoc : public CDocument
{
protected: // 仅从序列化创建
	CConnectionDoc();
	DECLARE_DYNCREATE(CConnectionDoc)

// 属性
public:

public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CConnectionDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


