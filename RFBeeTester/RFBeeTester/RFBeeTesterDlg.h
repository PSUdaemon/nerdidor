
// RFBeeTesterDlg.h : ͷ�ļ�
//

#pragma once
#include "mscomm1.h"
#include "afxwin.h"


// CRFBeeTesterDlg �Ի���
class CRFBeeTesterDlg : public CDialogEx
{
// ����
public:
	CRFBeeTesterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_RFBEETESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
