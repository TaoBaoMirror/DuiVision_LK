#include "stdafx.h"
#include "General.h"
#include "KoKoonDBManage.h"

#include "Psapi.h"

#pragma comment(lib,"Psapi.lib")

DatabaseParam DBparam;
DatabaseParam BoxDBparam;
SHELLEXECUTEINFO COpenSoftKeyboard::m_oskname;
map<string,string> ConfigParameter::m_mapParameter;

string General::integertostring(int i)
{
	stringstream ss;
    ss << i;
    return ss.str();
}

string General::doubletostring(double d)
{
	//double 转 string 除小数点后的零
	char Temp[1024] = {0};
	sprintf(Temp,"%f",d);
	string strTemp = Temp;
	
	string::size_type nStart = strTemp.find_first_of(".",0);
	if(nStart != string::npos)
	{
		int nLength = strlen(strTemp.c_str());
		while(nLength>0)
		{
			string::size_type nEnd = strTemp.find_last_of("0");
			
			if(nEnd == nLength-1)
			{
				strTemp = strTemp.substr(0,strlen(strTemp.c_str())-1);
			}
			else
			{
				break;
			}
			nLength = strlen(strTemp.c_str());
		}		
	}
	
	nStart = strTemp.find_first_of(".",0);
	int nLength = strlen(strTemp.c_str());
	if(nStart == nLength-1 && nStart>0)
	{
		strTemp = strTemp.substr(0,nLength-1);
	}	
	return strTemp;	
}

int General::AddMachineID(int rsn,int add)
{
	const int iTimes=sizeof(int) * 8; 	
	int iMask=1; 	
	int x2[iTimes]; 
	int i = 0;
	for(i=0 ; i<iTimes ; i++ )
	{ 
		x2[i]=rsn & iMask; 
		x2[i]=x2[i]?1:0;
		iMask = iMask << 1;  
	}
	x2[add-1]=1;
	int num=0;
	iMask=1;
	for (i=0;i<=iTimes-1;i++)
	{
		num += (x2[i])*iMask;
		iMask*=2;
	}
	return num;
}

int General::RemoveMachineID(int rsn,int r)
{
	const int iTimes=sizeof(int) * 8; 	
	int iMask=1; 	
	int x2[iTimes]; 
	int i=0;
	for(i=0 ; i<iTimes ; i++ )
	{ 
		x2[i]=rsn & iMask; 
		x2[i]=x2[i]?1:0;
		iMask = iMask << 1;  
	}
	x2[r-1]=0;
	int num=0;
	iMask=1;
	for (i=0;i<=iTimes-1;i++)
	{
		num += (x2[i])*iMask;
		iMask*=2;
	}
	return num;
}

string   General::PYTransform(char   *as_HzString) 
{ 
  static   int   li_SecPosValue[]={1601,1637,1833,2078,2274,2302,2433,2594,2787,3106,3212,3472,3635,3722,3730,3858,4027,4086,4390,4558,4684,4925,5249}; 
  static   char*   lc_FirstLetter[]     =   { "A",   "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "W", "X", "Y", "Z"}; 
  static   char*   ls_SecondSecTable   = 
"CJWGNSPGCGNE[Y[BTYYZDXYKYGT[JNNJQMBSGZSCYJSYY[PGKBZGY[YWJKGKLJYWKPJQHY[W[DZLSGMRYPYWWCCKZNKYYGTTNJJNYKKZYTCJNMCYLQLYPYQFQRPZSLWBTGKJFYXJWZLTBNCXJJJJTXDTTSQZYCDXXHGCK[PHFFSS[YBGXLPPBYLL[HLXS[ZM[JHSOJNGHDZQYKLGJHSGQZHXQGKEZZWYSCSCJXYEYXADZPMDSSMZJZQJYZC[J[WQJBYZPXGZNZCPWHKXHQKMWFBPBYDTJZZKQHY " 
"LYGXFPTYJYYZPSZLFCHMQSHGMXXSXJ[[DCSBBQBEFSJYHXWGZKPYLQBGLDLCCTNMAYDDKSSNGYCSGXLYZAYBNPTSDKDYLHGYMYLCXPY[JNDQJWXQXFYYFJLEJPZRXCCQWQQSBNKYMGPLBMJRQCFLNYMYQMSQYRBCJTHZTQFRXQHXMJJCJLXQGJMSHZKBSWYEMYLTXFSYDSWLYCJQXSJNQBSCTYHBFTDCYZDJWYGHQFRXWCKQKXEBPTLPXJZSRMEBWHJLBJSLYYSMDXLCLQKXLHXJRZJMFQHXHWY " 
"WSBHTRXXGLHQHFNM[YKLDYXZPYLGG[MTCFPAJJZYLJTYANJGBJPLQGDZYQYAXBKYSECJSZNSLYZHSXLZCGHPXZHZNYTDSBCJKDLZAYFMYDLEBBGQYZKXGLDNDNYSKJSHDLYXBCGHXYPKDJMMZNGMMCLGWZSZXZJFZNMLZZTHCSYDBDLLSCDDNLKJYKJSYCJLKWHQASDKNHCSGANHDAASHTCPLCPQYBSDMPJLPZJOQLCDHJJYSPRCHN[NNLHLYYQYHWZPTCZGWWMZFFJQQQQYXACLBHKDJXDGMMY " 
"DJXZLLSYGXGKJRYWZWYCLZMSSJZLDBYD[FCXYHLXCHYZJQ[[QAGMNYXPFRKSSBJLYXYSYGLNSCMHZWWMNZJJLXXHCHSY[[TTXRYCYXBYHCSMXJSZNPWGPXXTAYBGAJCXLY[DCCWZOCWKCCSBNHCPDYZNFCYYTYCKXKYBSQKKYTQQXFCWCHCYKELZQBSQYJQCCLMTHSYWHMKTLKJLYCXWHEQQHTQH[PQ[QSCFYMNDMGBWHWLGSLLYSDLMLXPTHMJHWLJZYHZJXHTXJLHXRSWLWZJCBXMHZQXSDZP " 
"MGFCSGLSXYMJSHXPJXWMYQKSMYPLRTHBXFTPMHYXLCHLHLZYLXGSSSSTCLSLDCLRPBHZHXYYFHB[GDMYCNQQWLQHJJ[YWJZYEJJDHPBLQXTQKWHLCHQXAGTLXLJXMSL[HTZKZJECXJCJNMFBY[SFYWYBJZGNYSDZSQYRSLJPCLPWXSDWEJBJCBCNAYTWGMPAPCLYQPCLZXSBNMSGGFNZJJBZSFZYNDXHPLQKZCZWALSBCCJX[YZGWKYPSGXFZFCDKHJGXDLQFSGDSLQWZKXTMHSBGZMJZRGLYJB " 
"PMLMSXLZJQQHZYJCZYDJWBMYKLDDPMJEGXYHYLXHLQYQHKYCWCJMYYXNATJHYCCXZPCQLBZWWYTWBQCMLPMYRJCCCXFPZNZZLJPLXXYZTZLGDLDCKLYRZZGQTGJHHGJLJAXFGFJZSLCFDQZLCLGJDJCSNZLLJPJQDCCLCJXMYZFTSXGCGSBRZXJQQCTZHGYQTJQQLZXJYLYLBCYAMCSTYLPDJBYREGKLZYZHLYSZQLZNWCZCLLWJQJJJKDGJZOLBBZPPGLGHTGZXYGHZMYCNQSYCYHBHGXKAMTX " 
"YXNBSKYZZGJZLQJDFCJXDYGJQJJPMGWGJJJPKQSBGBMMCJSSCLPQPDXCDYYKY[CJDDYYGYWRHJRTGZNYQLDKLJSZZGZQZJGDYKSHPZMTLCPWNJAFYZDJCNMWESCYGLBTZCGMSSLLYXQSXSBSJSBBSGGHFJLYPMZJNLYYWDQSHZXTYYWHMZYHYWDBXBTLMSYYYFSXJC[DXXLHJHF[SXZQHFZMZCZTQCXZXRTTDJHNNYZQQMNQDMMG[YDXMJGDHCDYZBFFALLZTDLTFXMXQZDNGWQDBDCZJDXBZGS " 
"QQDDJCMBKZFFXMKDMDSYYSZCMLJDSYNSBRSKMKMPCKLGDBQTFZSWTFGGLYPLLJZHGJ[GYPZLTCSMCNBTJBQFKTHBYZGKPBBYMTDSSXTBNPDKLEYCJNYDDYKZDDHQHSDZSCTARLLTKZLGECLLKJLQJAQNBDKKGHPJTZQKSECSHALQFMMGJNLYJBBTMLYZXDCJPLDLPCQDHZYCBZSCZBZMSLJFLKRZJSNFRGJHXPDHYJYBZGDLQCSEZGXLBLGYXTWMABCHECMWYJYZLLJJYHLG[DJLSLYGKDZPZXJ " 
"YYZLWCXSZFGWYYDLYHCLJSCMBJHBLYZLYCBLYDPDQYSXQZBYTDKYXJY[CNRJMPDJGKLCLJBCTBJDDBBLBLCZQRPPXJCJLZCSHLTOLJNMDDDLNGKAQHQHJGYKHEZNMSHRP[QQJCHGMFPRXHJGDYCHGHLYRZQLCYQJNZSQTKQJYMSZSWLCFQQQXYFGGYPTQWLMCRNFKKFSYYLQBMQAMMMYXCTPSHCPTXXZZSMPHPSHMCLMLDQFYQXSZYYDYJZZHQPDSZGLSTJBCKBXYQZJSGPSXQZQZRQTBDKYXZK " 
"HHGFLBCSMDLDGDZDBLZYYCXNNCSYBZBFGLZZXSWMSCCMQNJQSBDQSJTXXMBLTXZCLZSHZCXRQJGJYLXZFJPHYMZQQYDFQJJLZZNZJCDGZYGCTXMZYSCTLKPHTXHTLBJXJLXSCDQXCBBTJFQZFSLTJBTKQBXXJJLJCHCZDBZJDCZJDCPRNPQCJPFCZLCLZXZDMXMPHJSGZGSZZQLYLWTJPFSYASMCJBTZKYCWMYTCSJJLJCQLWZMALBXYFBPNLSFHTGJWEJJXXGLLJSTGSHJQLZFKCGNNNSZFDEQ " 
"FHBSAQTGYLBXMMYGSZLDYDQMJJRGBJTKGDHGKBLQKBDMBYLXWCXYTTYBKMRTJZXQJBHLMHMJJZMQASLDCYXYQDLQCAFYWYXQHZ "; 
  string   result   =   ""; 
  int   H,L,W; 
  unsigned   stringlen   =   strlen(as_HzString); 
  for(int     i   =   0;   i   <   stringlen;   i++   )   
  { 
        H   =   (unsigned   char)(as_HzString[i]); 
        L   =   (unsigned   char)(as_HzString[i+1]); 
        if(H   <   0xA1   ||   L   <   0xA1){ 
			if (H < 0x80)
			{
				result += as_HzString[i];
				continue;
			}else 
			{
				i++;
				continue;
			}
        } 
        else   W   =   (H   -   160)   *   100   +   L   -   160; 
        if(W   >   1600   &&   W   <   5590)   
		{ 
              for(int j = 22; j >= 0;   j--) 
                      if(W   >=   li_SecPosValue[j])   { 
                          result   +=   lc_FirstLetter[j]; 
                          i++; 
                          break; 
                      } 
              continue; 
        } 
        else     
		{ 
          i++; 
          W   =   (   H   -   160   -   56   )*94   +   L   -   161; 
          if(W   >=   0   &&   W   <=   3007) 
                result   +=   ls_SecondSecTable[W]; 
          else   { 
              result   +=   (char)H; 
              result   +=   (char)L; 
          } 
		} 
  } 
  return   result; 
} 


int General::MessageBox(HWND hWnd , CString strText, CString strCaption /*= _T("")*/, UINT uType /*= CMB_OKCANCEL*/)
{
//	CLKMessageBox messageBox;
//	messageBox.InitParam(hWnd, strText, strCaption, uType);	
//	return messageBox.DoModal();
	return 0;
}

int General::ShowMessageBox(HWND hWnd , CString strText, CString strCaption /*= _T("")*/, UINT uType /*= CMB_OKCANCEL*/)
{
// 	CLKMessageBox *messageBox;
// 	messageBox=new CLKMessageBox(NULL);
// 	messageBox->InitParam(hWnd, strText, strCaption, uType);
// 	messageBox->Create(IDD_MESSAGE_BOX,NULL);	
// 	messageBox->ShowWindow(SW_SHOW) ;
	//messageBox->SetWindowPos(NULL,500,200,500,245,SWP_SHOWWINDOW);
	return 1 ;
}

CString General::GetDialogExePath()
{
	char path[MAX_PATH];
	memset(path, 0, MAX_PATH);
	GetModuleFileName(NULL, path, MAX_PATH);

	CString strPath = path; 
	int index = strPath.ReverseFind('\\');   
	strPath = strPath.Left(index+1); 	
	return strPath;
}

COpenSoftKeyboard::COpenSoftKeyboard()
{
    memset(&m_oskname,0,sizeof(m_oskname)); 
    m_oskname.cbSize=sizeof(m_oskname);  
    m_oskname.hwnd=NULL;  
    m_oskname.lpFile="osk.exe";  
    m_oskname.nShow=SW_SHOWNORMAL;  
    m_oskname.fMask=SEE_MASK_NOCLOSEPROCESS;
    m_oskType = configPar.getIntParameter("Openosk");
	m_openFlag = false ;
}

COpenSoftKeyboard::~COpenSoftKeyboard()
{

}

void COpenSoftKeyboard::openSoftKeyboard()
{
    if (m_oskType ==1) 
    {
		if(FindProcess("osk.exe")==0)
			ShellExecuteEx(&m_oskname);
    }
// 	else if (m_oskType ==2 )
// 	{
// 		if(!m_openFlag )
// 		{
// 			ckb.Create(IDD_OSKeyboard, m_pParentWnd);
// 			ckb.setCPWnd(m_pParentWnd);
// 			m_openFlag = true ;
// 		}
// 		ckb.ShowWindow(SW_SHOW);
// 	}
}

void COpenSoftKeyboard::closeSoftKeyboard()
{
	if (m_oskType ==1)
	{
		if(FindProcess("osk.exe")!=0 )
			TerminateProcess(m_oskname.hProcess,0);
	}
	else if (m_oskType ==2)
	{
// 		if(m_openFlag)
// 			ckb.ShowWindow(SW_HIDE);		
	}
}

DWORD COpenSoftKeyboard::FindProcess(char *strProcessName)
{
    DWORD aProcesses[1024], cbNeeded, cbMNeeded;
    HMODULE hMods[1024];
    HANDLE hProcess;
    char szProcessName[MAX_PATH];
	
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) ) 
		return 0;
    for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); i++)
    {
        hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
        EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
        GetModuleFileNameEx( hProcess, hMods[0], szProcessName,sizeof(szProcessName));
		
        if(strstr(szProcessName, strProcessName))
        {
            return(aProcesses[i]);
        }
    }
    return 0;
}

void COpenSoftKeyboard::setIdd(int idd)
{
//	ckb.setIdd(idd);
}

void COpenSoftKeyboard::setNextIdd(int idd)
{
//	ckb.setNextIdd(idd);
}

void COpenSoftKeyboard::setCWnd(CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
}


string ConfigParameter::getValueParameter(string key)
{
	map<string,string>::iterator iter;
	
	iter = m_mapParameter.find(key);
	if(iter != m_mapParameter.end())
	{
		return iter->second;  
	}else{
		return ""; 
	}
}

void ConfigParameter::setValueParameter(string key, string value)
{
	m_mapParameter.insert(pair<string,string>(key,value));
	return;
}

int ConfigParameter::getIntParameter(string key)
{
	return atoi(getValueParameter(key).c_str());
}

void ConfigParameter::updateValueParameter(string key, string value) 
{
	map<string,string>::iterator iter;

	//如果键存在，则更新值，不存在，则插入键值
	iter = m_mapParameter.find(key);
	if(iter != m_mapParameter.end())
	{
		iter->second = value;   
	}else{
		m_mapParameter.insert(pair<string,string>(key,value));
	}
}