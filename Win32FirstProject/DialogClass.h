#pragma once


// ���������� ���� DialogClass

class DialogClass : public CDialogEx
{
	DECLARE_DYNAMIC(DialogClass)

public:
	DialogClass(CWnd* pParent = NULL);   // ����������� �����������
	virtual ~DialogClass();

// ������ ����������� ����
	enum { IDD = IDD_ABOUT_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	DECLARE_MESSAGE_MAP()
};
