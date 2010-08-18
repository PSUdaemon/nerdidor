
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



void CRFBeeTesterDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_mscomm.put_CommPort(2);//ѡ��COM?
	
	m_mscomm.put_InBufferSize(1024); //�������뻺�����Ĵ�С��Bytes

	m_mscomm.put_OutBufferSize(512); //�������뻺�����Ĵ�С��Bytes//


	m_mscomm.put_InputMode(0); //�������뷽ʽΪ1-�����Ʒ�ʽ,0-�ı���ʽ

	m_mscomm.put_Settings(_T("9600,n,8,1")); //���ò����ʵȲ���



	m_mscomm.put_RThreshold(1); //Ϊ1��ʾ��һ���ַ�����һ���¼�

	m_mscomm.put_InputLen(100);

	if(!m_mscomm.get_PortOpen()){ //�򿪴���
		m_mscomm.put_PortOpen(TRUE);
	}
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
		MessageBox(str);
		break;
	default:
		break;
	}
}
