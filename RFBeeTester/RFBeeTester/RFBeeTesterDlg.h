
// RFBeeTesterDlg.h : 头文件
//

#pragma once
#include "mscomm1.h"
#include "afxwin.h"


// CRFBeeTesterDlg 对话框
class CRFBeeTesterDlg : public CDialogEx
{
// 构造
public:
	CRFBeeTesterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RFBEETESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMscomm1 m_mscomm;
	//afx_msg void OnBnClickedButton1();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	//afx_msg void OnBnClickedButton2();
	CComboBox m_comNum;
	CComboBox m_baudRate;
	afx_msg void OnBnClickedButtonOpencom();
	CButton m_openCom;
	// Do some intialization
	int UserInitial(void);
};
