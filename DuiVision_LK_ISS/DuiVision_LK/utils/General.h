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


//按钮的2种样式
enum
{
	CMB_OKCANCEL	= 0, //确定取消
	CMB_OK			= 1, //确定
};

namespace General{
	string integertostring(int i);
	string doubletostring(double d);
	int  AddMachineID(int rsn,int add);// 添加一个机器ID值
	int  RemoveMachineID(int rsn,int r);//移除一个机器ID值
	string PYTransform(char   *as_HzString);
	CString GetDialogExePath();
	//对话框
	int MessageBox(HWND hWnd , CString strText, CString strCaption = _T(""), UINT uType = CMB_OKCANCEL);
	//非模态对话框
	int ShowMessageBox(HWND hWnd , CString strText, CString strCaption = _T(""), UINT uType = CMB_OKCANCEL);
};


//数据库参数
typedef struct _DatabaseParam		
{
	string host;				//主机名
	string user;				//用户名
	string password;			//密码
	string db;					//数据库名
	unsigned int port;			//端口号
	string characterSet;
}DatabaseParam;
extern DatabaseParam DBparam;	//数据库连接参数
extern DatabaseParam BoxDBparam;//盒剂数据库连接参数


//操作虚拟软键盘的类
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
	static int getIntParameter(string key);		//取得配置整数值
	static void updateValueParameter(string key, string value);	//更新配置值

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
