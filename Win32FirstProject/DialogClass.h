#pragma once


// диалоговое окно DialogClass

class DialogClass : public CDialogEx
{
	DECLARE_DYNAMIC(DialogClass)

public:
	DialogClass(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~DialogClass();

// Данные диалогового окна
	enum { IDD = IDD_ABOUT_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
};
