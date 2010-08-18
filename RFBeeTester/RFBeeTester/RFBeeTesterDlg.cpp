
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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRFBeeTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_mscomm);
}

BEGIN_MESSAGE_MAP(CRFBeeTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRFBeeTesterDlg::OnBnClickedButton1)
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



void CRFBeeTesterDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_mscomm.put_CommPort(2);//选择COM?
	
	m_mscomm.put_InBufferSize(1024); //设置输入缓冲区的大小，Bytes

	m_mscomm.put_OutBufferSize(512); //设置输入缓冲区的大小，Bytes//


	m_mscomm.put_InputMode(0); //设置输入方式为1-二进制方式,0-文本方式

	m_mscomm.put_Settings(_T("9600,n,8,1")); //设置波特率等参数



	m_mscomm.put_RThreshold(1); //为1表示有一个字符引发一个事件

	m_mscomm.put_InputLen(100);

	if(!m_mscomm.get_PortOpen()){ //打开串口
		m_mscomm.put_PortOpen(TRUE);
	}
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
		MessageBox(str);
		break;
	default:
		break;
	}
}
