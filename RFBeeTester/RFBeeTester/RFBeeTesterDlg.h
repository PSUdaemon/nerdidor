
// RFBeeTesterDlg.h : 头文件
//

#pragma once
#include "mscomm1.h"
#include "afxwin.h"

#define TIMER1  1
#define TIMER2  2

#define TIMER_CF0  3
#define TIMER_CF1  4
#define TIMER_CF3  5
#define TIMER_CF4  6

#define TIMER_CLOSE_COM  7
#define TIMER_OPEN_COM   8

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
	int m_threshold;
	afx_msg void OnBnClickedButtonSetthreshold();
	// RSSI in 915MHz 76.8kbps
	int m_CF0RSSI;
	// RSSI in 915MHz 4.8kbps sensitivity
	int m_CF1RSSI;
	// RSSI in 915MHz 4.8kbps low current
	int m_CF2RSSI;
	// RSSI in 868MHz 76.8kbps
	int m_CF3RSSI;
	// RSSI in 868MHz 4.8kbps sensitivity
	int m_CF4RSSI;
	// RSSI in 868MHz 4.8kbps low current
	int m_CF5RSSI;
	CString m_CF0State;
	CString m_CF1State;
	CString m_CF2State;
	CString m_CF3State;
	CString m_CF4State;
	CString m_CF5State;

	CString m_allState;
	afx_msg void OnBnClickedButtonTestCF0();
	// Send AT command to RFBee
	int sendCommand(CString cmd);
	// Send data to Remote RFBee through local RFbee
	int sendData(CString data);
	// Check if there's reply from remote RFBee, and determine the communication quality
	int checkReplyFromRemote(int &CFRSSI, CString &CFState);
	// Data recieved in serial buffer
	CString m_serialReceiveData;
	// Data received needs to be displayed 
	CEdit m_displayReceiveData;
	// Display received data on the window
	int DisplayReceivedData(CString data);
	//Clear displaying window
	afx_msg void OnBnClickedButtonClearReceiveData();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	int m_isTimeElapsed;
	// Delay in millisecond
	int Delay(int millisecond);
	void SleepEx(int value);
	afx_msg void OnBnClickedButtonTestAll();
	int m_CFState;
	int configRFBee(CString cfg);
	afx_msg void OnBnClickedButtonTestCF1();
	afx_msg void OnBnClickedButtonTestCF3();
	afx_msg void OnBnClickedButtonTestCF4();
	afx_msg void OnBnClickedButtonTestCF2();
	afx_msg void OnBnClickedButtonTestCF5();
	int Timer1Process(void);
	int Timer2Process(void);
};
