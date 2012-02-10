
// RFBeeTesterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RFBeeTester.h"
#include "RFBeeTesterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CRFBeeTesterDlg 对话框




CRFBeeTesterDlg::CRFBeeTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRFBeeTesterDlg::IDD, pParent)
	, m_threshold(0)
	, m_CF0RSSI(0)
	, m_CF1RSSI(0)
	, m_CF2RSSI(0)
	, m_CF3RSSI(0)
	, m_CF4RSSI(0)
	, m_CF5RSSI(0)
	, m_CF0State(_T(""))
	, m_CF1State(_T(""))
	, m_CF2State(_T(""))
	, m_CF3State(_T(""))
	, m_CF4State(_T(""))
	, m_CF5State(_T(""))
	, m_allState(_T(""))
	, m_serialReceiveData(_T(""))
	, m_isTimeElapsed(0)
	, m_CFState(0)
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
	DDX_Text(pDX, IDC_EDIT_CF4VALUE, m_CF4RSSI);
	DDV_MinMaxInt(pDX, m_CF4RSSI, -200, 100);
	DDX_Text(pDX, IDC_EDIT_CF5VALUE, m_CF5RSSI);
	DDV_MinMaxInt(pDX, m_CF5RSSI, -200, 100);

	DDX_Text(pDX, IDC_EDIT_CF0STATE, m_CF0State);
	DDX_Text(pDX, IDC_EDIT_CF1STATE, m_CF1State);
	DDX_Text(pDX, IDC_EDIT_CF2STATE, m_CF2State);
	DDX_Text(pDX, IDC_EDIT_CF3STATE, m_CF3State);
	DDX_Text(pDX, IDC_EDIT_CF4STATE, m_CF4State);
	DDX_Text(pDX, IDC_EDIT_CF5STATE, m_CF5State);

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
	ON_BN_CLICKED(IDC_BUTTON_TESTALL, &CRFBeeTesterDlg::OnBnClickedButtonTestAll)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF1, &CRFBeeTesterDlg::OnBnClickedButtonTestCF1)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF3, &CRFBeeTesterDlg::OnBnClickedButtonTestCF3)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF4, &CRFBeeTesterDlg::OnBnClickedButtonTestCF4)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF2, &CRFBeeTesterDlg::OnBnClickedButtonTestCF2)
	ON_BN_CLICKED(IDC_BUTTON_TESTCF5, &CRFBeeTesterDlg::OnBnClickedButtonTestCF5)
END_MESSAGE_MAP()


// CRFBeeTesterDlg 消息处理程序

BOOL CRFBeeTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	UserInitial();
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRFBeeTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRFBeeTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





BEGIN_EVENTSINK_MAP(CRFBeeTesterDlg, CDialogEx)
	ON_EVENT(CRFBeeTesterDlg, IDC_MSCOMM1, 1, CRFBeeTesterDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CRFBeeTesterDlg::OnCommMscomm1()
{
	// TODO: 在此处添加消息处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码

	if(!m_mscomm.get_PortOpen()){ //打开串口
		m_mscomm.put_InBufferSize(1024); //设置输入缓冲区的大小，Bytes

		m_mscomm.put_OutBufferSize(512); //设置输入缓冲区的大小，Bytes//


		m_mscomm.put_InputMode(0); //设置输入方式为1-二进制方式,0-文本方式


		m_mscomm.put_RThreshold(1); //为1表示有一个字符引发一个事件

		m_mscomm.put_InputLen(100);


		//set COM number
		int index = m_comNum.GetCurSel();
		m_mscomm.put_CommPort(index+1);

		//set baudrate
		index = m_baudRate.GetCurSel();
		CString baudParam[] = {_T("9600,n,8,1"),_T("19200,n,8,1"),_T("38400,n,8,1"),_T("115200,n,8,1")};

		m_mscomm.put_Settings(baudParam[index]); //设置波特率等参数

		m_mscomm.put_PortOpen(TRUE);
		if(m_mscomm.get_PortOpen()){
			//MessageBox(_T("Success!"));
			m_openCom.SetWindowTextW(_T("CloseCom"));
			SetTimer(TIMER_OPEN_COM,200,0);
		}

	}
	else
	{
		m_mscomm.put_PortOpen(FALSE);
		if(!m_mscomm.get_PortOpen()){
		m_openCom.SetWindowTextW(_T("OpenCom"));
		KillTimer(TIMER1);
		KillTimer(TIMER2);
		KillTimer(TIMER_CF0);
		KillTimer(TIMER_CF1);
		KillTimer(TIMER_CF3);
		KillTimer(TIMER_CF4);
		KillTimer(TIMER_CLOSE_COM);
		KillTimer(TIMER_OPEN_COM);
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
	// TODO: 在此添加控件通知处理程序代码
	CWnd::UpdateData(TRUE);
	//CString str;
	//str.Format(_T("%d\n"),m_threshold);
	//MessageBox((LPCTSTR)str);
}


// Send AT command to RFBee
int CRFBeeTesterDlg::sendCommand(CString cmd)
{
	int retVal = 1;

	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer

	m_mscomm.put_Output(COleVariant(cmd));//send command 

	//Delay(100);//wait 100ms for reply from RFBee
	//while(m_serialReceiveData.GetLength() < 2);
	/*SleepEx(100);//time here is critical

	if(m_serialReceiveData.Left(2) == _T("ok")){
		retVal = 0;
	}
	else{
		retVal = 1;
	//	CString msg(_T("Send "));
	//	msg += cmd;
	//	msg += CString(_T(" command error!"));
	//	MessageBox(msg);
	}
	
	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer
	*/
	//SleepEx(50);
	//return retVal;
	return 0;
}


// Send data to Remote RFBee through local RFbee
int CRFBeeTesterDlg::sendData(CString data)
{
	m_mscomm.put_OutBufferCount(0);//clear serial output buffer

	m_mscomm.put_Output(COleVariant(data));//send command 

	return 0;
}


// Check if there's reply from remote RFBee, and determine the communication quality
int CRFBeeTesterDlg::checkReplyFromRemote(int &CFRSSI, CString &CFState)
{
	//SleepEx(20);//wait for the reply from remote RFBee

	//receiving data should be
	int dataLen = m_serialReceiveData.GetLength();
	if(dataLen < 5  ){ //|| m_serialReceiveData.Find(_T("C")) == -1){
		//MessageBox(_T("No response from remote RFBee"));
		return 1;
	}
	
	int start = m_serialReceiveData.Find(_T(","));
	if(start < 1)
	{
		return 1;
	}
	//get the raw length of the first pack, format:len,srcAddr,dstAddr,rawData,RSSI,LQI
	CString tempStr(m_serialReceiveData.Right(dataLen - start +2));
	LPTSTR p = tempStr.GetBuffer(tempStr.GetLength());
	int len	= atoi((char *)p);

	if(len < 1){
		m_serialReceiveData.Empty();//clear serial data
		m_mscomm.put_InBufferCount(0);//clear serial input buffer

		return 1;
	}

	tempStr = m_serialReceiveData.Mid(start - 1 + (len+3)*2,3);//1,0,0,c,-29
	
	//((CEdit*)GetDlgItem(IDC_EDIT_CF0VALUE))->SetWindowText(tempStr); 

	p = tempStr.GetBuffer(tempStr.GetLength());
	CFRSSI = -((int)(p[1]-'0')*10 + (int)(p[2]-'0'));
	if(CFRSSI < -100 || CFRSSI > 0){
		m_serialReceiveData.Empty();//clear serial data
		m_mscomm.put_InBufferCount(0);//clear serial input buffer

		return 1;
	}
	if(CFRSSI >= m_threshold){
		CFState = _T("ok");
	}
	else{
		CFState = _T("failed");
	}

	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer


	CWnd::UpdateData(FALSE);

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
	// TODO: 在此添加控件通知处理程序代码
	m_displayReceiveData.SetSel(0,-1);
	m_displayReceiveData.Clear();
}


void CRFBeeTesterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent) {
    case TIMER1:
        {    
			Timer1Process();
			break;
        }
	case TIMER2:
        {    
			Timer2Process();
            break;
        }
	case TIMER_CF0:
		{
			KillTimer(TIMER_CF0);
			OnBnClickedButtonTestCF0();
			SetTimer(TIMER_CF1,500,0);
			break;
		}
	case TIMER_CF1:
		{
			KillTimer(TIMER_CF1);
			OnBnClickedButtonTestCF1();
			SetTimer(TIMER_CF3,500,0);
			break;
		}
	case TIMER_CF3:
		{
			KillTimer(TIMER_CF3);
			OnBnClickedButtonTestCF3();
			SetTimer(TIMER_CF4,500,0);
			break;
		}
	case TIMER_CF4:
		{
			KillTimer(TIMER_CF4);
			OnBnClickedButtonTestCF4();
			SetTimer(TIMER_CLOSE_COM,500,0);
			break;
		}
	case TIMER_CLOSE_COM:
		{
			KillTimer(TIMER_CLOSE_COM);
			if(m_mscomm.get_PortOpen()){
				m_mscomm.put_PortOpen(FALSE);
				if(!m_mscomm.get_PortOpen()){
					m_openCom.SetWindowTextW(_T("OpenCom"));
					KillTimer(TIMER1);
					KillTimer(TIMER2);
					KillTimer(TIMER_CF0);
					KillTimer(TIMER_CF1);
					KillTimer(TIMER_CF3);
					KillTimer(TIMER_CF4);
					KillTimer(TIMER_CLOSE_COM);
				}
				break;
			}
		}
	case TIMER_OPEN_COM:
		{
			KillTimer(TIMER_OPEN_COM);
			OnBnClickedButtonTestAll();
			break;
		}
    default:
        break;
    }

	CDialogEx::OnTimer(nIDEvent);
}


void CRFBeeTesterDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
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
  dfFreq = (double)litmp.QuadPart; // 获得计数器的时钟频率
  QueryPerformanceCounter(&litmp);
 
 QPart1 = litmp.QuadPart;


 do{

   //增加消息处理 删除则在此期间不响应任何消息
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


void CRFBeeTesterDlg::OnBnClickedButtonTestAll()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CF0RSSI = 0;
	m_CF1RSSI = 0;
	m_CF2RSSI = 0;
	m_CF3RSSI = 0;
	m_CF4RSSI = 0;
	m_CF5RSSI = 0;

	m_CF0State = (_T(""));
	m_CF1State = (_T(""));
	m_CF2State = (_T(""));
	m_CF3State = (_T(""));
	m_CF4State = (_T(""));
	m_CF5State = (_T(""));

	configRFBee(_T("ATRS\r"));
	
	SetTimer(TIMER_CF0,100,0);
	
}

int CRFBeeTesterDlg::configRFBee(CString cfg)
{
	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer

	m_mscomm.put_Output(COleVariant(_T("+++")));
	m_mscomm.put_Output(COleVariant(cfg));
	m_mscomm.put_Output(COleVariant(_T("ATO0\r")));
	return 0;
}

void CRFBeeTesterDlg::OnBnClickedButtonTestCF0()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	
	configRFBee(_T("ATOF3\r"));
	configRFBee(_T("ATCF0\r"));

	m_CFState = 0;
	SetTimer(TIMER2,150,0);

	
	//checkReplyFromRemote();
	//if(!sendCommand(_T("+++"))) return;
	//if(!sendCommand(_T("ATRS\r"))) return;
	//if(!sendCommand(_T("ATO0\r"))) return;
}


void CRFBeeTesterDlg::OnBnClickedButtonTestCF1()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	configRFBee(_T("ATCF1\r"));

	m_CFState = 1;
	SetTimer(TIMER2,150,0);
}

void CRFBeeTesterDlg::OnBnClickedButtonTestCF2()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	configRFBee(_T("ATCF2\r"));

	m_CFState = 2;
	SetTimer(TIMER2,150,0);
}

void CRFBeeTesterDlg::OnBnClickedButtonTestCF3()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	configRFBee(_T("ATCF3\r"));

	m_CFState = 3;
	SetTimer(TIMER2,150,0);

}


void CRFBeeTesterDlg::OnBnClickedButtonTestCF4()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	configRFBee(_T("ATCF4\r"));

	m_CFState = 4;
	SetTimer(TIMER2,150,0);
}


void CRFBeeTesterDlg::OnBnClickedButtonTestCF5()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER1);
	configRFBee(_T("ATCF5\r"));

	m_CFState = 5;
	SetTimer(TIMER2,150,0);
}


int CRFBeeTesterDlg::Timer1Process(void)
{
	switch(m_CFState){
	case 0:
		if(checkReplyFromRemote(m_CF0RSSI,m_CF0State)){
			sendData(_T("CF0"));
			return 0;
		}

		KillTimer(TIMER1);
		break;
	case 1:
		if(checkReplyFromRemote(m_CF1RSSI,m_CF1State)){
			sendData(_T("CF1"));
			return 0;
		}

		KillTimer(TIMER1);
		break;
	case 2:
		if(checkReplyFromRemote(m_CF2RSSI,m_CF2State)){
			sendData(_T("CF2"));
			return 0;
		}

		KillTimer(TIMER1);
		break;
	case 3:
		if(checkReplyFromRemote(m_CF3RSSI,m_CF3State)){
			sendData(_T("CF3"));
			return 0;
		}

		KillTimer(TIMER1);
		break;
	case 4:
		if(checkReplyFromRemote(m_CF4RSSI,m_CF4State)){
			sendData(_T("CF4"));
			return 0;
		}

		KillTimer(TIMER1);
		break;
	case 5:
		if(checkReplyFromRemote(m_CF5RSSI,m_CF5State)){
			sendData(_T("CF5"));
			return 0;
		}
		configRFBee(_T("ATRS\r"));
		KillTimer(TIMER1);
		break;
	default:
		break;
	}
	return 0;
}


int CRFBeeTesterDlg::Timer2Process(void)
{
	KillTimer(TIMER2);
	m_serialReceiveData.Empty();//clear serial data
	m_mscomm.put_InBufferCount(0);//clear serial input buffer

	CString data;
	data.Format(_T("CF%d"),m_CFState);
	sendData(data);
	SetTimer(TIMER1,250,0);
	return 0;
}
