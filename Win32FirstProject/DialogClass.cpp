// DialogClass.cpp: ���� ����������
//

#include "stdafx.h"
#include "DialogClass.h"
#include "afxdialogex.h"


// ���������� ���� DialogClass

IMPLEMENT_DYNAMIC(DialogClass, CDialogEx)

DialogClass::DialogClass(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogClass::IDD, pParent)
{

}

DialogClass::~DialogClass()
{
}

void DialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogClass, CDialogEx)
END_MESSAGE_MAP()


// ����������� ��������� DialogClass
