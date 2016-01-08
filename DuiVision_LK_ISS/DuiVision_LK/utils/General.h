#if !defined(General_define)
#define General_define

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;


//��ť��2����ʽ
enum
{
	CMB_OKCANCEL	= 0, //ȷ��ȡ��
	CMB_OK			= 1, //ȷ��
};

namespace General{
	string integertostring(int i);
	string doubletostring(double d);
	int  AddMachineID(int rsn,int add);// ���һ������IDֵ
	int  RemoveMachineID(int rsn,int r);//�Ƴ�һ������IDֵ
	string PYTransform(char   *as_HzString);
	CString GetDialogExePath();
	//�Ի���
	int MessageBox(HWND hWnd , CString strText, CString strCaption = _T(""), UINT uType = CMB_OKCANCEL);
	//��ģ̬�Ի���
	int ShowMessageBox(HWND hWnd , CString strText, CString strCaption = _T(""), UINT uType = CMB_OKCANCEL);
};


//���ݿ����
typedef struct _DatabaseParam		
{
	string host;				//������
	string user;				//�û���
	string password;			//����
	string db;					//���ݿ���
	unsigned int port;			//�˿ں�
	string characterSet;
}DatabaseParam;
extern DatabaseParam DBparam;	//���ݿ����Ӳ���
extern DatabaseParam BoxDBparam;//�м����ݿ����Ӳ���


//������������̵���
class COpenSoftKeyboard
{
public:
	COpenSoftKeyboard();
	~COpenSoftKeyboard();
	void setCWnd(CWnd* pParentWnd);
	void setIdd(int idd);
	void setNextIdd(int idd);
	void openSoftKeyboard();
	void closeSoftKeyboard();
	DWORD FindProcess(char *strProcessName);
private:
	static SHELLEXECUTEINFO m_oskname;
	int m_oskType;

	CWnd* m_pParentWnd ;
	bool m_openFlag;
};


class ConfigParameter{
public:
	static map<string,string> m_mapParameter;
public:
	static string getValueParameter(string key);
	static void setValueParameter(string key, string value);
	static int getIntParameter(string key);		//ȡ����������ֵ
	static void updateValueParameter(string key, string value);	//��������ֵ

	ConfigParameter(){
		
	};

	~ConfigParameter(){};

};

static ConfigParameter configPar;


/*

class ConfigureParameter
{
private:
	static map<string,string> m_mapParameter;
public:
	static string getValueParameter(string key);
	
	static void setValueParameter(string key, string value);
	
	static int getIntParameter(string key);						
	
	ConfigureParameter(){
		
	};
	
	~ConfigureParameter(){};
	
};

static ConfigureParameter configPar ;

*/
#endif
