
// RFBeeTesterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RFBeeTester.h"
#include "RFBeeTesterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRFBeeTesterDlg �Ի���




CRFBeeTesterDlg::CRFBeeTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRFBeeTesterDlg::IDD, pParent)
	, m_threshold(0)
	, m_CF0RSSI(0)
	, m_CF1RSSI(0)
	, m_CF2RSSI(0)
	, m_CF3RSSI(0)
	, m_CF0State(_T(""))
	, m_CF1State(_T(""))
	, m_CF2State(_T(""))
	, m_CF3State(_T(""))
	, m_allState(_T(""))
	, m_serialReceiveData(_T(""))
	, m_isTimeElapsed(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRFBeeTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_mscomm);
	DDX_Control(pDX, IDC_COMBO_COMNUM, m_comNum);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_baudRate);
	DDX_Control(pDX, IDC_BUTTON_OPENCOM, m_openCom);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_threshold);
	DDV_MinMaxInt(pDX, m_threshold, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF0VALUE, m_CF0RSSI);
	DDV_MinMaxInt(pDX, m_CF0RSSI, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF1VALUE, m_CF1RSSI);
	DDV_MinMaxInt(pDX, m_CF1RSSI, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF2VALUE, m_CF2RSSI);
	DDV_MinMaxInt(pDX, m_CF2RSSI, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF3VALUE, m_CF3RSSI);
	DDV_MinMaxInt(pDX, m_CF3RSSI, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF0STATE, m_CF0State);
	DDX_Text(pDX, IDC_EDIT_CF1STATE, m_CF1State);
	DDX_Text(pDX, IDC_EDIT_CF2STATE, m_CF2State);
	DDX_Text(pDX, IDC_EDIT_CF3STATE, m_CF3State);
	DDX_Text(pDX, IDC_EDIT_ALLSTATE, m_allState);
	DDX_Control(pDX, IDC_EDIT_RECEIVEDATA, m_displayReceiveData);
}

BEGIN_MESSAGE_MAP(CRFBeeTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CRFBeeTesterDlg::OnBnClickedButton1)
	//ON_BN_CLICKED(IDC_BUTTON2, &CRFBeeTesterDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_OPENCOM, &CRFBeeTesterDlg::OnBnClickedButtonOpencom)
	ON_BN_CLICKED(IDC_BUTTON_SETTHRESHOLD, &CRFBeeTesterDlg::OnBnClickedButtonSetthreshold)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF0, &CRFBeeTesterDlg::OnBnClickedButtonTestCF0)
	ON_BN_CLICKED(IDC_BUTTON_CLEARRECEIVEDATA, &CRFBeeTesterDlg::OnBnClickedButtonClearReceiveData)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CRFBeeTesterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRFBeeTesterDlg ��Ϣ�������

BOOL CRFBeeTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	UserInitial();
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRFBeeTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRFBeeTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRFBeeTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





BEGIN_EVENTSINK_MAP(CRFBeeTesterDlg, CDialogEx)
	ON_EVENT(CRFBeeTesterDlg, IDC_MSCOMM1, 1, CRFBeeTesterDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CRFBeeTesterDlg::OnCommMscomm1()
{
	// TODO: �ڴ˴������Ϣ����������
	CString str;
	switch(m_mscomm.get_CommEvent())
	{
	case 2:
		str = CString(m_mscomm.get_Input().bstrVal);
		//MessageBox(str);
		m_serialReceiveData += str; 
		
		//display the received data on the window
		DisplayReceivedData(str);
		break;
	default:
		break;
	}
}


void CRFBeeTesterDlg::OnBnClickedButtonOpencom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if(!m_mscomm.get_PortOpen()){ //�򿪴���
		m_mscomm.put_InBufferSize(1024); //�������뻺�����Ĵ�С��Bytes

		m_mscomm.put_OutBufferSize(512); //�������뻺�����Ĵ�С��Bytes//


		m_mscomm.put_InputMode(0); //�������뷽ʽΪ1-�����Ʒ�ʽ,0-�ı���ʽ


		m_mscomm.put_RThreshold(1); //Ϊ1��ʾ��һ���ַ�����һ���¼�

		m_mscomm.put_InputLen(100);


		//set COM number
		int index = m_comNum.GetCurSel();
		m_mscomm.put_CommPort(index+1);

		//set baudrate
		index = m_baudRate.GetCurSel();
		CString baudParam[] = {_T("9600,n,8,1"),_T("19200,n,8,1"),_T("38400,n,8,1"),_T("115200,n,8,1")};

		m_mscomm.put_Settings(baudParam[index]); //���ò����ʵȲ���

		m_mscomm.put_PortOpen(TRUE);
		if(m_mscomm.get_PortOpen()){
			MessageBox(_T("Success!"));
			m_openCom.SetWindowTextW(_T("CloseCom"));
		}

	}
	else
	{
		m_mscomm.put_PortOpen(FALSE);
		if(!m_mscomm.get_PortOpen()){
		m_openCom.SetWindowTextW(_T("OpenCom"));
		}
	}  
}


// Do some intialization
int CRFBeeTesterDlg::UserInitial(void)
{
	m_openCom.SetWindowTextW(_T("OpenCom"));

	m_comNum.SetCurSel(0);
	m_baudRate.SetCurSel(0);
	
	m_threshold = -50;
	CWnd::UpdateData(FALSE);
	
	//SetTimer(TIMER1,1,0);

	return 0;
}


void CRFBeeTesterDlg::OnBnClickedButtonSetthreshold()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd::UpdateData(TRUE);
	//CString str;
	//str.Format(_T("%d\n"),m_threshold);
	//MessageBox((LPCTSTR)str);
}


void CRFBeeTesterDlg::OnBnClickedButtonTestCF0()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!sendCommand(_T("+++"))) return;
	if(!sendCommand(_T("ATRS\r"))) return;
	if(!sendCommand(_T("ATCF0\r"))) return;
	if(!sendCommand(_T("ATOF3\r"))) return;
	if(!sendCommand(_T("ATO0\r"))) return;
	sendData(_T("hello 1234567890 ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	checkReplyFromRemote();
	if(!sendCommand(_T("+++"))) return;
	if(!sendCommand(_T("ATRS\r"))) return;
	if(!sendCommand(_T("ATO0\r"))) return;
}


// Send AT command to RFBee
int CRFBeeTesterDlg::sendCommand(CString cmd)
{
	int retVal = 0;

	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer

	m_mscomm.put_Output(COleVariant(cmd));//send command 

	//Delay(100);//wait 100ms for reply from RFBee
	//while(m_serialReceiveData.GetLength() < 2);
	SleepEx(20);//time here is critical

	if(m_serialReceiveData.Left(2) == _T("ok")){
		retVal = 1;
	}
	else{
		retVal = 0;
		CString msg(_T("Send "));
		msg += cmd;
		msg += CString(_T(" command error!"));
		MessageBox(msg);
	}
	
	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer

	SleepEx(50);
	return retVal;
}


// Send data to Remote RFBee through local RFbee
int CRFBeeTesterDlg::sendData(CString data)
{
	return 0;
}


// Check if there's reply from remote RFBee, and determine the communication quality
int CRFBeeTesterDlg::checkReplyFromRemote(void)
{
	return 0;
}


// Display received data on the window.
int CRFBeeTesterDlg::DisplayReceivedData(CString data)
{

	int textLen = m_displayReceiveData.GetWindowTextLength();

	m_displayReceiveData.SetSel(textLen,textLen);
	m_displayReceiveData.ReplaceSel(data);


	return 0;
}

//Clear displaying window
void CRFBeeTesterDlg::OnBnClickedButtonClearReceiveData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_displayReceiveData.SetSel(0,-1);
	m_displayReceiveData.Clear();
}


void CRFBeeTesterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent) {
    case TIMER1:
        {    
            m_isTimeElapsed = 1;
			//Delay(0);
            break;
        }
    default:
        break;
    }
	CDialogEx::OnTimer(nIDEvent);
}


void CRFBeeTesterDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(TIMER1);
	CDialogEx::OnOK();
}


// Delay in millisecond
int CRFBeeTesterDlg::Delay(int millisecond)
{
	/*if( 0 == m_isTimeElapsed){
		SetTimer(TIMER1,millisecond,0);
		return 0;
	}

	m_isTimeElapsed = 0;

	KillTimer(TIMER1);
	*/
	long time = 0;
	for(int i = 0; i < millisecond; i++){
		for(int j = 0; j < 10000;j++){
			for(int k = 0; k < 10000;k++){
				time++;
			}
		}
	}

	return 0;
}


void CRFBeeTesterDlg::SleepEx(int value)
{
	 LARGE_INTEGER  litmp; 
 LONGLONG       QPart1,QPart2;
 double         dfMinus, dfFreq, dfTim; 
  QueryPerformanceFrequency(&litmp); 
  dfFreq = (double)litmp.QuadPart; // ��ü�������ʱ��Ƶ��
  QueryPerformanceCounter(&litmp);
 
 QPart1 = litmp.QuadPart;


 do{

   //������Ϣ���� ɾ�����ڴ��ڼ䲻��Ӧ�κ���Ϣ
   MSG  msg;  
   GetMessage(&msg,NULL,0,0);  
   TranslateMessage(&msg); 
   DispatchMessage(&msg); 


  QueryPerformanceCounter(&litmp); 
  QPart2 = litmp.QuadPart;
  dfMinus = (double)(QPart2-QPart1); 
  dfTim = dfMinus / dfFreq;
 }while(dfTim<0.001*1000/value);
}
