
// RFBeeTester.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRFBeeTesterApp:
// �йش����ʵ�֣������ RFBeeTester.cpp
//

class CRFBeeTesterApp : public CWinApp
{
public:
	CRFBeeTesterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRFBeeTesterApp theApp;