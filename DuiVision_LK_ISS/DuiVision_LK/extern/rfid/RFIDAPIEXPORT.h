////////////////////////////////////////////////////////////////////////
// Module:  RFIDEXPORT.h
// Author:  cxm owen
// Modified: 2010年12月17日 14:23:09
// Purpose: 
////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef  _RFIDEXPORT_H
#define  _RFIDEXPORT_H
#define  STDCALL __stdcall

#ifdef RFIDAPI_EXPORTS
#define RFID_API __declspec( dllexport )
#else
#define RFID_API __declspec( dllimport )
#endif

#ifdef __cplusplus
extern "C" {
#endif


	//////////////////////////////////////////////////////////////////////////
	//1.0 API动态库版本查询
	//功能：通过参数传入，查询当前API库版本
	//参数：
	//  pHandle 			为保存打开的端口句柄
	//  copyright			版本号
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_Copyright(void** pHandle, char* copyright);


	//////////////////////////////////////////////////////////////////////////
	//1.1 TCP参数初始化
	//功能：通过参数传入，初始化TCP，为打开连接做准备
	//参数：
	//  pHandle 		为保存打开的端口句柄
	//  pHostName		本机的IP地址，仅在网络通讯模式下有效
	//  nsocketPort		网络SOCKET端口
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_TCPInit(void** pHandle,char *pHostName,int nsocketPort);


	//////////////////////////////////////////////////////////////////////////
	//1.2 COM参数初始化
	//功能：通过参数传入，初始化COM，为打开连接做准备
	//参数：
	//  pHandle 		为保存打开的端口句柄
	//  nBusAddr		为总线地址
	//  pComNum			COM1－COM4 仅在COM通讯模式下有效
	//  nBaud			串口通讯速率
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_COMInit(void** pHandle,unsigned char nBusAddr,char *pComNum,int nBaud );

	//////////////////////////////////////////////////////////////////////////
	//1.3 USB参数初始化
	//功能描述：目前读写器的USB接口应用通过是通过串口虚拟的，因此USB接口的使用也要初始化COM口，此函数通过参数传入，初始化计算机COM口，为建立建立基于USB连接做准备。
	//函数原型：
	//传入参数：
	//pHandle		为保存打开的端口句柄
	//nBusAddr		为总线地址，默认为0
	//pUSBNum		COM1－COM4 (COM1-COM4是虚拟的串口，通信时转USB协议)仅在USB通讯模式下有效
	//nBaud			USB口通讯速率,默认为152000
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API __stdcall SAAT_USBInit(void** pHandle,unsigned char nBusAddr,char * pUSBNum,int nBaud );

	//////////////////////////////////////////////////////////////////////////
	//1.4 UDP参数初始化
	//功能描述：通过参数传入，此函数用于计算机的以太网应用配置，初始化一个SOCKET，为建立基于以太网接口的UDP连接做准备。
	//函数原型：
	//bool	SAAT_UDPInit (void** pHandle,char *pHostName,int nsocketPort)
	//传入参数：
	//pHandle			为保存打开的端口句柄
	//pHostName			读写器IP地址
	//nsocketPort		读写器UDP SOCKET端口，默认为7088
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API __stdcall SAAT_UDPInit(void** pHandle,char *pHostName,int nsocketPort);

	//////////////////////////////////////////////////////////////////////////
	//1.5 打开读写器
	//功能：		创建网络连接
	//参数：
	//  pHandle 		为保存打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_Open(void* pHandle);


	//////////////////////////////////////////////////////////////////////////
	//1.6 关闭读写器
	//功能：	创建网络连接
	//参数：
	//	pHandle 	为打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_Close(void *pHandle);


	//////////////////////////////////////////////////////////////////////////
	//1.7 设置API动态库语言类型
	//功能描述：此函数用来设置API动态库语言类型。
	//函数原型：
	//bool   SAAT_SetLanguageType (void *pHandle, char* szType)
	//传入参数： 
	//pHandle 		打开的端口句柄
	//szType		语言类型,cn为简体中文,tw为繁体中文,en为英语
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_SetLanguageType (void* pHandle,char* szType);

	//////////////////////////////////////////////////////////////////////////
	//1.8  获取出错信息
	//功能：用于关闭功放,主要用在关闭读卡操作
	//参数：
	//		pHandle:	打开的端口句柄
	//		szMsg:		返回错误信息
	//      nLen:       szMsg缓冲区的长度    
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_GetErrorMessage(void *pHandle,char *szMsg, int nLen);


	//////////////////////////////////////////////////////////////////////////
	//1.9  获取出错错误代码
	//功能：获取出错错误代码
	//参数：
	//		pHandle:	打开的端口句柄
	//		pCode:		返回错误代码
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_GetErrorCode(void *pHandle,int *pCode);

	//////////////////////////////////////////////////////////////////////////
	//2.0 断开连接并重连
	//功能描述：用于断开连接并重连。
	//函数原型：
	//bool	SAAT_Reconnect(void *pHandle)
	//传入参数：
	//pHandle	为打开的端口句柄
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_Reconnect(void* pHandle);

	//////////////////////////////////////////////////////////////////////////
	//2.1 心跳指令
	//功能描述：该指令用于上位机和RFID设备之间建立基于以太网、串口、USB口等的通讯连接后检测物理接口的联通情况。上位机一般超过一定时间未接收到RFID设备的数据（如RFID设备等待读卡而当前射频范围内无卡），即可发送该指令要求RFID设备回应心跳包来检测通讯链路的连接状态和RFID设备的工作状态。
	//函数原型：
	//bool	SAAT_HeartSend (void *pHandle)
	//传入参数：
	//pHandle	为打开的端口句柄
	//返回值：true：连接正常；false：连接断开。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_HeartSend (void* pHandle);

	//////////////////////////////////////////////////////////////////////////
	//2.2 系统信息配置
	//功能：设定读写器参数
	//参数：
	//  pHandle:	打开的端口名柄
	//	nType:		系统信息配置类型
	//				0x00	    	读写器名称					8字节
	//				0x10  	    	读写器系统权限码			6字节
	//	pParm:		系统参数
	//	nLen:		系统参数长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_SysInfSet (void* pHandle, unsigned char nType, unsigned char* pParm, int nLen);

	//////////////////////////////////////////////////////////////////////////
	//2.3 系统信息查询
	//功能：查询读写器参数
	//参数：
	//	pHandle:		打开的端口句柄
	//	nType ：		要查询的参数类型
	//					nType           定义							长度
	//					0x00			读写器名称						8字节
	//					0x01			读写器产品型号					5字节
	//					0x02  			读写器出厂产品序列号			8字节
	//					0x03   			读写器处理器软件版本号			4字节
	//					0x04   			读写器解码单元软件版本号		4字节
	//					0x05  			基带电路硬件版本号				4字节
	//					0x06			射频电路硬件版本号				4字节
	//	pPara：			指向接收读写器参数数据内存的指针 
	//	pLen： 			pLen指向的内存长度
	//返回值： true为操作成功, false为操作失败
	bool RFID_API  STDCALL SAAT_SysInfQuery (void* pHandle ,unsigned char nType, unsigned char *pPara, unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	//2.4 工作模式配置
	//功能描述：此函数用于配置读写器的工作模式。
	//函数原型：
	//bool	SAAT_WorkModeSet (void* pHandle ,unsigned nType)
	//传入参数：
	//pHandle		打开的端口句柄
	//nType			要配置的工作模式
	//工作模式      定义			说明
	//0x00			主从模式    读写器在上位机的指令控制下执行标签操作；
	//0x01			预置模式    读写器根据事先预置的指令执行标签操作；
	//0x02			触发模式    读写器根据IO输入触发标签操作指令；
	//0x03			定时模式    读写器根据系统时钟定时来执行标签操作。
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_WorkModeSet (void* pHandle ,unsigned char nType);


	//////////////////////////////////////////////////////////////////////////
	//2.5 读写器系统测试
	//功能：用于测试读写器
	//参数：
	//	pHandle:		打开的端口句柄
	//	nType ：		测试模式
	//  nAntenna:		天线端口号
	//	pTestParm: 		测试模式参数
	//	nLen:			测试模式参数的长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_SysTest(void* pHandle ,
		unsigned char nType,
		unsigned char nAntenna, 
		unsigned char *pTestParm, 
		unsigned char nLen);


	//////////////////////////////////////////////////////////////////////////
	//2.6 参数表操作
	//功能：用于操作读写器的参数表，包括导入或者导出读写器的系统参数表。
	//参数：
	//	pHandle:	打开的端口句柄
	//	nType: 			参数表操作的操作模式  
	//		0			恢复系统参数表为出厂设置
	//		1			导入系统参数表
	//		2			导出系统参数表
	//	nStartAddrr:	指定操作参数表起始位置
	//	nLen:			指定操作参数表长度
	//	pData:			导出操作时的导出数据
	//	pDataLen:		导出数据长度
	//注：操作模式为0x00时，起始地址和操作长度两项参数无意义。
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_ParmOp (void* pHandle ,
		unsigned char nType, 
		unsigned char nStartAddrr, 
		unsigned char nLen,  
		unsigned char *pData, 
		unsigned char *pDataLen);


	//////////////////////////////////////////////////////////////////////////
	//2.7 通讯参数配置
	//功能：用于配置通讯参数
	//参数：
	//	pHandle : 	打开的端口句柄
	//	nType: 		配置的参数类型
	//		0x00	串行总线设备地址
	//		0x01	RS-232总线速率
	//		0x02	RS-485总线速率
	//	pPara:		读写器的串行总线参数
	//		RS-232总线速率
	//		0x00	4800bps
	//		0x01	9600 bps
	//		0x02	19200 bps
	//		0x03	38400 bps
	//		0x04	57600 bps
	//		0x05	115200 bps
	//	nLen:		读写器的串行总线参数长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_CommunicatParaSet (void* pHandle ,
		unsigned char nType, 
		unsigned char* pPara, 
		unsigned char nLen);


	//////////////////////////////////////////////////////////////////////////
	//2.8 通讯参数查询
	//功能：用于查询读写器的通讯参数
	//参数：
	//	pHandle : 	打开的端口句柄
	//	nType: 	配置的参数类型
	//	pPara:		查询的读写器总线参数
	//	pLen:		总线参数长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_CommunicatParaQuery (void* pHandle ,
		unsigned char nType, 
		unsigned char* pPara,
		unsigned char *pLen);


	//////////////////////////////////////////////////////////////////////////
	//2.9 网络参数配置
	//功能：用于配置通讯参数
	//参数：
	//	pHandle :	打开的端口句柄
	//	nType: 		配置的参数类型
	//		0x02	IP地址
	//		0x03	SOCKET端口号
	//	pPara:		配置的以太网参数数据
	//	nLen:		参数数据长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_NetParaSet (void* pHandle ,
		unsigned char nType, 
		unsigned char* pPara, 
		unsigned char nLen);


	//////////////////////////////////////////////////////////////////////////
	//3.0 网络参数查询
	//功能：用于查询通讯参数
	//参数：
	//		pHandle :	打开的端口句柄
	//		nType: 		为配置的参数类型
	//			0x01	MAC地址
	//			0x02	IP地址
	//			0x03	SOCKET端口号
	//		pPara:		读写器返回的网络参数数据
	//		pLen:		参数数据长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_NetParaQuery (void* pHandle ,
		int nType, 
		unsigned char* pPara,
		unsigned char *pLen);


	//////////////////////////////////////////////////////////////////////////
	//3.1 标签操作参数配置
	//功能：用于配置标签操作参数
	//参数：
	//		pHandle :	打开的端口句柄
	//		nType: 		“类型”为配置的参数类型，包括：
	//				区域	类型 		定义	说明
	//				所有ID	全选		0X00	要求ID值射频范围内全部标签响应查询指令
	//				基本ID	等于		0X01	要求ID值等于匹配数据的标签响应查询指令
	//				基本ID	不等于		0X02	要求ID值不等于匹配数据的标签响应查询指令
	//				基本ID	大于		0X03	要求ID值大于匹配数据的标签响应查询指令
	//				基本ID	小于		0X04	要求ID值小于匹配数据的标签响应查询指令
	//				扩展ID	等于		0X11	要求ID值等于匹配数据的标签响应查询指令
	//				扩展ID	不等于		0X12	要求ID值不等于匹配数据的标签响应查询指令
	//				扩展ID	大于		0X13	要求ID值大于匹配数据的标签响应查询指令
	//				扩展ID	小于		0X14	要求ID值小于匹配数据的标签响应查询指令
	//		pPara: 		"匹配数据"为标签的ID号
	//		nLen:		匹配数据的长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_TagOpParaSet(void* pHandle ,
		unsigned char nType, 
		unsigned char *pPara,
		unsigned char nLen);

	//////////////////////////////////////////////////////////////////////////
	//3.2 标签操作参数查询
	//功能描述：此函数用于配置读写器对标签做读写等操作的参数。
	///函数原型：
	//bool	SAAT_TagOpParaQuery (void* pHandle ,unsigned char nType, unsigned char* pPara, unsigned char *pLen)
	//传入参数：
	//pHandle		打开的端口句柄
	//nType			为配置的参数类型（定义见说明）
	//pPara			指向保存配置的参数的内存指针
	//Len			传入时为pPara长度，传出时为标签操作参数长度
	//返回值：true：操作成功；false：操作失败。
	//说明：nType 为配置的参数类型，定义与上节相同。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_TagOpParaQuery(void* pHandle ,
		unsigned char nType, unsigned char* pPara, unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	//3.3 射频端口参数查询
	//功能：用于查询读写器的参数表的全部射频端口参数
	//参数：
	//		pHandle:		打开的端口句柄
	//		szAntennaPara:	全部射频端口参数
	//			“端口使能”： 
	//			第1个字节：0x03：允许修改功放接收增益
	//			第2个字节：0x03：允许修改2401射频器件发送功率
	//			第3个字节：0x03：允许修改射频衰减器功率
	//			第4个字节：保留，0x00
	//			“输出功率”： 
	//			第1个字节（功放接收增益）：
	//			0x02：高增益；
	//			0x03：低增益；
	//			第2个字节（2401射频器件发送功率）：
	//			0x00：-18DB
	//			0x01：-12DB
	//			0x02： -6DB
	//			0x03：  0DB
	//			第3个字节（射频衰减器功率）：
	//			0：-0DB
	//			1：-1DB
	//			2：-2DB
	//			3：-3DB
	//			4：-4DB
	//			5：-5DB
	//			6：-6DB
	//			...
	//			15：-30dB
	//			第4个字节：保留，0x00
	//			“轮询时间”：长度为4个字节，依次 0x00 0x00 0x00 0x00
	//		pLen: 			参数长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_TotalAntennaParmQuery (void* pHandle,unsigned char *szAntennaPara,unsigned char *pLen);


	//////////////////////////////////////////////////////////////////////////
	//3.4 关闭功放 (关闭读卡)
	//功能：用于关闭功放,主要用在关闭读卡操作
	//参数：
	//		pHandle:		打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_PowerOff(void *pHandle);

	//////////////////////////////////////////////////////////////////////////
	//3.5 停止读卡
	//功能：用于停止读卡
	//参数：
	//		pHandle:		打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YPowerOff(void *pHandle);

	//////////////////////////////////////////////////////////////////////////
	//3.6 IO输出操作
	//功能：用于控制读写器的IO输出操作
	//参数：
	//		pHandle:	打开的端口句柄
	//		nPort:输出端口：    0    所有端口
	//							1    端口1
	//							2	 端口2
	//							3	 端口3
	//							4	 端口4
	//		nState:输出状态：   0    输出低电平
	//							1    输出高电平  
	//							2    输出正脉冲
	//							3    输出负脉冲
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_IOOperate(void* pHandle,unsigned char nPort,unsigned char nState);


	//////////////////////////////////////////////////////////////////////////
	//3.7 IO输入查询指令
	//功能：用于查询读写器扩展板的IO输入状态
	//函数原型：
	//bool SAAT_IOStateQuery(void* pHandle,unsigned char *pState)
	//参数：
	//		pHandle:	打开的端口句柄
	//		pState:		输入端口的电平状态
	//					域说明：端口状态
	//				位		Bit7	Bit 6	Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0
	//				定义	保留	保留	保留	保留	保留	保留	端口2	端口1
	//				端口状态：	0 :低电平
	//							1:高电平
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_IOStateQuery(void* pHandle,unsigned char *pState);

	//////////////////////////////////////////////////////////////////////////
	//3.8 有源发送读ID码命令
	//功能： 有源发读ID码命令
	//参数:
	//		pHandle:	为已经初始化的端口句柄
	//		nOpType:	为操作模式, 
	//		操作模式：
	//		00：对同一个标签的ID码读写器只向上位机返回一次，
	//		并要求上位机接收后给予读写器"返回数据确认"，
	//		01"：读写器将读取到的所有的标签ID码全部上传到上位机并不要求上位机接收后给予
	//				读写器"返回数据确认"，直到上位机下发"关功放"命令后才停止读标签ID码。
	//		nIDType:
	//		0x01     基本ID（BID）
	//		0x02     扩展ID（EID）
	//		0X03    标签返回基本ID和扩展ID
	//		nTagCount: 为同时预计读取标签数, 预计工作环境中一次需要同时读取的最大标签数，设计为1到255张
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YReadIDCode ( void *pHandle,
		unsigned char nOpType,
		unsigned char nIDType,
		unsigned int nTagCount);


	//////////////////////////////////////////////////////////////////////////
	//3.9 有源-发送标签主动上传命令
	//功能： 有源-发送标签主动上传命令
	//参数:
	//		pHandle 为已经初始化的端口句柄
	//		nOpType: 为操作模式, 
	//		操作模式：
	//		00：对同一个标签的ID码读写器只向上位机返回一次，
	//		并要求上位机接收后给予读写器"返回数据确认"，
	//		01"：读写器将读取到的所有的标签ID码全部上传到上位机并不要求上位机接收后给予
	//			读写器"返回数据确认"，直到上位机下发"关功放"命令后才停止读标签ID码。
	//		nIDType 要接收的ID编码
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YMakeTagUpLoadIDCode ( void *pHandle,
		unsigned char nOpType,
		unsigned char nIDType);

	//////////////////////////////////////////////////////////////////////////
	//4.0 设置EAS寄存器
	//功能： 设置EAS寄存器
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		ESA标志置位操作执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pCodeID:		ID编码
	//		nTagPwd:		标签密码
	//		nSetMode:		标签在接收到指令后，置位操作的方式
	//						0x00      置位数据与EAS寄存器做“与”操作；
	//						0x01      置位数据与EAS寄存器做“或”操作；
	//		nFlagBit:			EAS标志共有8个比特，置“1”则设置EAS比特位；置“0”则清除EAS位。
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YEASRegSet (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pCodeID,
		unsigned char *pTagPwd,
		unsigned char nSetMode,
		unsigned char nFlagBit);

	//////////////////////////////////////////////////////////////////////////
	//4.1 查询EAS寄存器
	//功能： 查询EAS寄存器
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		ESA标志置位操作执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签密码
	//		pIDType			执行EAS监控ID编码类型
	//		pEASIDCode		执行EAS监控的基本ID编码
	//		pFlagBit:		置位数据,标签EAS寄存器内的数据
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YEASRegQuery (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char *pIDType,
		unsigned char *pEASIDCode,
		unsigned char *pFlagBit);


	//////////////////////////////////////////////////////////////////////////
	//4.2 启动EAS监控
	//功能：  启动EAS监控
	//参数： 
	//		pHandle 		为已经初始化的端口句柄
	//		nOpType:		为执行模式
	//						0x00 -> 结束EAS监控;
	//						0x01 ->启动EAS监控
	//		nIDType:		ID类型
	//		nEasBit:		nEasBit

	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YEASMonitorEnable (void *pHandle,
		unsigned char nOpType, 
		unsigned char nIDType,
		unsigned char nEasBit);


	//////////////////////////////////////////////////////////////////////////
	//4.3 接收EAS监控到的数据命令
	//功能：接收EAS监控到的数据命令
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//		pIDData		为ID数据, 
	//		nIDLen		为ID数据长度
	//		nBit		为标签EAS寄存器内的数据
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	int  RFID_API  STDCALL SAAT_YRevEASIDMsg (void *pHandle, unsigned char* pIDData,
		unsigned char* pIDLen,
		unsigned char* pBit);


	//////////////////////////////////////////////////////////////////////////
	//4.4 射频端口功率配置
	//功能：用于配置读写器的指定射频端口功率
	//参数：
	//		pHandle:		打开的端口句柄
	//		rfPower:	全部射频端口参数，其值对应如下：
	//		                0：-0DB
	//		                1：-1DB
	//		                2：-2DB
	//		                3：-3DB
	//		                4：-4DB
	//		                5：-5DB
	//		                6：-6DB
	//		                ...
	//		                15：-30dB
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YAntennaPowerSet (void* pHandle,unsigned char nPower );


	//////////////////////////////////////////////////////////////////////////
	//4.5 射频端口功率查询
	//功能：用于查询读写器的参数表的射频端口功率
	//参数：
	//		pHandle:	打开的端口句柄
	//		rfPower:	全部射频端口参数，其值对应如下：
	//		                0：-0DB
	//		                1：-1DB
	//		                2：-2DB
	//		                3：-3DB
	//		                4：-4DB
	//		                5：-5DB
	//		                6：-6DB
	//		                ...
	//		                15：-30dB	                
	//返回值： true为操作成功, false为操作失败
	bool RFID_API  STDCALL SAAT_YAntennaPowerQuery (void* pHandle,unsigned char* pPower);


	//////////////////////////////////////////////////////////////////////////
	//4.6 写EID码
	//功能：用于写EID码
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//		nOpMode 	为操作模式
	//		nIDCode		为BID码
	//		pAccessPWD 	为4字节访问密码
	//		pWriteEIDData 为写入标签的EPC码的数据
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL  SAAT_YWriteEIDCode ( void* pHandle, 
		unsigned char nOpMode,
		unsigned char* pIDCode,
		unsigned char *pAccessPWD, 
		unsigned char *pWriteEIDData);

	//////////////////////////////////////////////////////////////////////////
	//4.7 读用户数据区
	//功能：读取用户数据返回
	//参数：	
	//		pHandle 	为IConnect指针
	//		nOpMode		为操作模式
	//		nIDCodeType 为编码规则
	//		pIDCode		为IDCode
	//		pTagPwd		为标签密码
	//		nBank		为标签存储区
	//		nStartAddr 	为读取标签数据区的首地址
	//		UserData	读取到的数据区数据
	//		pReadLen	实际读取到的字节数
	//		nWaitTime	超时时间
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_YReadUserData ( void* pHandle, 
		unsigned char nOpMode, 
		unsigned char nIDCodeType,
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char nBank,
		unsigned char nStartAddr,
		unsigned char *pReadData,
		int *pReadLen,
		unsigned int nWaitTime = 1000);

	//////////////////////////////////////////////////////////////////////////
	//4.8 写用户数据区
	//		pHandle 	为IConnect指针
	//		nOpMode		为操作模式
	//		nIDCodeType 为编码规则
	//		pIDCode		为IDCode
	//		pTagPwd		为标签密码
	//		nBank		为标签存储区
	//		nStartAddr 	为写标签数据区的首地址
	//		pWriteData	写入数据区的数据
	//		pRealWriteLen 返回实际写入长度
	//		nWaitTime	超时时间
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_YWriteUserData ( void* pHandle, 
		unsigned char nOpMode, 
		unsigned char nIDCodeType,
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char nBank,
		unsigned char nStartAddr,
		unsigned char *pWriteData,
		int nWriteLen,
		unsigned int nWaitTime = 1000);


	//////////////////////////////////////////////////////////////////////////
	//4.9  有源设置访问密码
	//功能：设置访问密码
	//参数：
	//		pHandle 为已经初始化的端口句柄
	//		nOpMode 为操作模式
	//		nIDType 为ID类型
	//		pIDCode 为ID编码
	//		pOrgPWD为4字节旧访问密码,默认为8个零
	//		pNewPWD为4字节新访问密码
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YAccessPWDSet (void *pHandle,
		unsigned char nOpMode, 
		unsigned char nIDType,
		unsigned char *pIDCode,
		unsigned char *pOrgPWD, 
		unsigned char *pNewPWD);


	//////////////////////////////////////////////////////////////////////////
	//5.0 有源设置配置密码
	//功能：设置配置密码
	//参数：
	//		pHandle 为已经初始化的端口句柄
	//		nOpMode 为操作模式
	//		nIDType 为ID类型
	//		pIDCode 为ID编码
	//		pOrgPWD为4字节旧配置密码,默认为8个零
	//		pNewPWD为4字节新配置密码
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YCFGPWDSet (void *pHandle,
		unsigned char nOpMode, 
		unsigned char nIDType,
		unsigned char *pIDCode,
		unsigned char *pOrgPWD, 
		unsigned char *pNewPWD);


	//////////////////////////////////////////////////////////////////////////
	//5.1 标签数据区锁操作
	//功能：锁标签数据区
	//参数：
	//		pHandle 为已经初始化的端口句柄
	//		nOpMode 为操作模式
	//		nIDType 为ID类型
	//		pIDCode 为ID编码
	//		pTagPWD 为4字节配置密码
	//		nTagOpType 为锁标签操作类型
	//		nStartAddr 为起始地址
	//		nDataLen	为数据长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YTagOp(void *pHandle,
		unsigned char nOpMode, 
		unsigned char nIDType,
		unsigned char *pIDCode,
		unsigned char *pTagPWD, 
		unsigned char nTagOpType,
		unsigned short nStartAddr,
		unsigned char nDataLen);

	//////////////////////////////////////////////////////////////////////////
	//5.2 标签射频参数配置指令
	//功能：标签射频参数配置指令
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		nAntennaPower	标识标签的当前的射频功率
	//			标识该标签的射频发送功率，定义如下：
	//			0x00     -18dBm
	//			0x01     -12dBm 
	//			0X02     -6dBm
	//			0X03     0dBm
	//			其它     保留数据位，非法参数，默认为0dBm
	//		nAntennaFrequency 射频频点	1	0x00~0xFF	标签的工作射频频点
	//			标识该标签的射频发送频点，定义如下（频道数为27个，带宽为5MHz）：
	//			0x05     2405MHz
	//			0x0A     2410MHz
	//			、        、   
	//			、        、   
	//			、        、   
	//			0X73     2450MHz 
	//			0X78     2520MHz
	//			其它     保留数据位，非法参数，默认频点2450MHz
	//		nHoppingMode:		置位数据,标签EAS寄存器内的数据
	//			跳频方式：标识该标签的跳频方式，定义如下：
	//			0x00     标签不跳频，标签工作于单频工作方式
	//			其它     保留数据位，非法参数。

	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagParmSet (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char nAntennaPower,
		unsigned char nAntennaFrequency,
		unsigned char nHoppingMode);


	//////////////////////////////////////////////////////////////////////////
	//5.3 标签射频参数查询指令
	//功能：标签射频参数查询指令
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		pAntennaPower	标识标签的当前的射频功率
	//			标识该标签的射频发送功率，定义如下：
	//			0x00     -18dBm
	//			0x01     -12dBm 
	//			0X02    -6dBm
	//			0X03     0dBm
	//			其它     保留数据位，非法参数，默认为0dBm
	//		pAntennaFrequency 射频频点	1	0x00~0xFF	标签的工作射频频点
	//			标识该标签的射频发送频点，定义如下（频道数为27个，带宽为5MHz）：
	//			0x05     2405MHz
	//			0x0A     2410MHz
	//			、        、   
	//			、        、   
	//			、        、   
	//			0X73     2450MHz 
	//			0X78     2520MHz
	//			其它     保留数据位，非法参数，默认频点2450MHz
	//		pHoppingMode:		置位数据,标签EAS寄存器内的数据
	//			跳频方式：标识该标签的跳频方式，定义如下：
	//			0x00     标签不跳频，标签工作于单频工作方式
	//			其它     保留数据位，非法参数。

	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagParmQuery (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *nTagPwd,
		unsigned char *pAntennaPower,
		unsigned char *pAntennaFrequency,
		unsigned char *pHoppingMode);


	//////////////////////////////////////////////////////////////////////////
	//5.4  标签工作模式配置指令
	//功能：标签工作模式配置指令
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		nWorkMode:		执行设置标签的工作模式
	//			0x00  自动发送模式
	//			0x01  指令查询模式
	//			0x02  激励自动发送模式
	//			0x03  激励指令查询模式
	//			0x20  射频测试模式：定频发送无调制载波
	//			0x21 射频测试模式：跳频方式发送无调制载波
	//			0x22 射频测试模式：误码率测试码接收模式
	//			0x23 射频测试模式：误码率测试码发送模式
	//			其他  保留，非法数据

	//		nUpLoadIDType:	设置标签主动工作方式下发送的ID类型
	//			0x00  标签主动工作模式下发送BID
	//			0x01  标签主动工作模式下发送UID
	//			其他  保留，非法数据

	//		nSleepTime: 睡眠时间：定义标签主动、被动的工作模式下标签睡眠时间，2字节，
	//					睡眠时间步进为100mS，最长睡眠时间为6553500mS。

	//		nSleepInterval:定义主动、被动的工作模式下标签睡眠间隔时间，
	//						标签在主动方式时，该参数定为标签主动发送数据时间，
	//						标签在被动方式时，该参数定义标签盘存接收及标签响应时间，
	//						睡眠间隔步进为1mS，最长睡眠间隔时间为255mS。

	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagWorkModeSet (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char nWorkMode,
		unsigned char nUpLoadIDType,
		unsigned short nSleepTime,
		unsigned char nSleepInterval);


	//////////////////////////////////////////////////////////////////////////
	//5.5  标签工作模式查询指令
	//功能：标签工作模式查询指令
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		*pWorkMode:		执行设置标签的工作模式
	//			0x00  自动发送模式
	//			0x01  指令查询模式
	//			0x02  激励自动发送模式
	//			0x03  激励指令查询模式
	//			0x20  射频测试模式  定频发送无调制载波
	//			0x21  射频测试模式  跳频方式发送无调制载波
	//			0x22  射频测试模式  误码率测试码接收模式
	//			0x23  射频测试模式  误码率测试码发送模式
	//			其他   保留，非法数据

	//		*pUpLoadIDType:	设置标签主动工作方式下发送的ID类型
	//			0x00  标签主动工作模式下发送BID
	//			0x01  标签主动工作模式下发送UID
	//			其他  保留，非法数据

	//		*pSleepTime: 睡眠时间：定义标签主动、被动的工作模式下标签睡眠时间，2字节，
	//					睡眠时间步进为100mS，最长睡眠时间为6553500mS。

	//		*pSleepInterval:定义主动、被动的工作模式下标签睡眠间隔时间，
	//						标签在主动方式时，该参数定为标签主动发送数据时间，
	//						标签在被动方式时，该参数定义标签盘存接收及标签响应时间，
	//						睡眠间隔步进为1mS，最长睡眠间隔时间为255mS。

	//		*pBatteryVoltage:
	//			电池电压：定义标签内置电池的电压，
	//			根据标签内置AD采集电压值返回标签当前电池电压值标签的电池电压，
	//			该数值乘以0.1V即为电池电压。
	//		*pRemainingCapacity:剩余电量：定义标签内置电池的剩余电量，以百分比表示。
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagWorkModeQuery (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char *pWorkMode,
		unsigned char *pUpLoadIDType,
		unsigned short *pSleepTime,
		unsigned char *pSleepInterval,
		unsigned char *pBatteryVoltage,
		unsigned char *pRemainingCapacity);


	//////////////////////////////////////////////////////////////////////////
	//5.6  标签出厂配置配置指令(指令字：0x75H)
	//功能：标签出厂配置配置指令(指令字：0x75H)
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		nTagType:标签类型：定义标签型号及类型。
	//			0x01  SAAT-T501标签
	//			0x02  SAAT-T502标签
	//			0x03  SAAT-T503标签
	//			0x04  SAAT-T504标签
	//			其它  未定义，保留数据
	//		nHardWareVersion:
	//			硬件版本：定义标签内置硬件电路版本号。高4位定义硬件版本主号，低字节定硬件版本次号，如版本为1.0、1.1、2.2定义如下：
	//			0x10  标签硬件版本号为1.0版本
	//			0x11  标签硬件版本号为1.1版本
	//			0x22  标签硬件版本号为2.2版本
	//		nSoftWareVersion:
	//			软件版本：定义标签内置软件版本号。高4位定义软件版本主号，低字节定软件版本次号，如版本为1.0、1.1、2.2定义如下：
	//			0x10  标签软件版本号为1.0版本
	//			0x11  标签软件版本号为1.1版本
	//			0x22  标签软件版本号为2.2版本 
	//		nBatteryConfiguration:
	//			电池配置：定义标签内置电池的型号，不同的标签配置不同电池,定义如下：
	//			0x01  SAAT-T501标签电池，电池容量750mA，型号CP224147
	//			0x02  SAAT-T502标签电池，电池容量550mA，型号CR3032
	//			0x03  SAAT-T503标签电池，电池容量450mA，型号CR2450
	//			0x04  SAAT-T504标签电池，电池容量1600mA，型号ER14505S
	//			其它  未定义，保留数据
	//		nNominalCapacity:
	//			标称容量：定义标签内置电池的标称容量。
	//电池容量定义单位为50mA，定义标签电池最大容量为12750mA.
	//如标签内配置的电池为1000mA，其值为十六进制数0x14，0x14为十进制数20
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagFatorySet (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char nTagType,
		unsigned char nHardWareVersion,
		unsigned char nSoftWareVersion,
		unsigned char nBatteryConfiguration,
		unsigned char nNominalCapacity);


	//////////////////////////////////////////////////////////////////////////
	//5.7 标签出厂配置查询指令(指令字：0x75H)
	//功能：标签出厂配置查询指令(指令字：0x75H)
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		*pTagType:标签类型：定义标签型号及类型。
	//			0x01  SAAT-T501标签
	//			0x02  SAAT-T502标签
	//			0x03  SAAT-T503标签
	//			0x04  SAAT-T504标签
	//			其它  未定义，保留数据
	//		*pHardWareVersion:
	//			硬件版本：定义标签内置硬件电路版本号。高4位定义硬件版本主号，低字节定硬件版本次号，如版本为1.0、1.1、2.2定义如下：
	//			0x10  标签硬件版本号为1.0版本
	//			0x11  标签硬件版本号为1.1版本
	//			0x22  标签硬件版本号为2.2版本
	//		*pSoftWareVersion:
	//			软件版本：定义标签内置软件版本号。高4位定义软件版本主号，低字节定软件版本次号，如版本为1.0、1.1、2.2定义如下：
	//			0x10  标签软件版本号为1.0版本
	//			0x11  标签软件版本号为1.1版本
	//			0x22  标签软件版本号为2.2版本 
	//		*pBatteryConfiguration:
	//			电池配置：定义标签内置电池的型号，不同的标签配置不同电池,定义如下：
	//			0x01  SAAT-T501标签电池，电池容量750mA，型号CP224147
	//			0x02  SAAT-T502标签电池，电池容量550mA，型号CR3032
	//			0x03  SAAT-T503标签电池，电池容量450mA，型号CR2450
	//			0x04  SAAT-T504标签电池，电池容量1600mA，型号ER14505S
	//			其它  未定义，保留数据
	//		*pNominalCapacity:
	//			标称容量：定义标签内置电池的标称容量。
	//电池容量定义单位为50mA，定义标签电池最大容量为12750mA.
	//如标签内配置的电池为1000mA，其值为十六进制数0x14，0x14为十进制数20
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagFatoryQuery (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *pTagPwd,
		unsigned char *pTagType,
		unsigned char *pHardWareVersion,
		unsigned char *pSoftWareVersion,
		unsigned char *pBatteryConfiguration,
		unsigned char *pNominalCapacity);


	//////////////////////////////////////////////////////////////////////////
	//5.8 标签初始化出厂设置指令
	//功能：5.2 标签初始化出厂设置指令
	//参数： 
	//		pHandle: 		为已经初始化的端口句柄
	//		nOpType:		执行模式
	//		nIDType:		ID类型,执行EAS标志置位的ID编码类型
	//		pIDCode:		ID编码
	//		nTagPwd:		标签配置密码
	//		指令说明：执行该指令以后，标签执行如下操作：
	//		射频参数及配置参数恢复为出厂默认值；
	//		基本ID不变；
	//		扩展ID编码恢复为全“0”；
	//		用户数据恢复为全“0”；
	//		用户数据区恢复为“无密码保护”状态及“未锁定”状态。
	//		访问密码恢复为全“0”；
	//		配置密码恢复为全“0”；
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool  RFID_API  STDCALL SAAT_YTagResetToFatory (void *pHandle, 
		unsigned char nOpType, 
		unsigned char nIDType, 
		unsigned char *pIDCode,
		unsigned char *nTagPwd);
	

	//////////////////////////////////////////////////////////////////////////
	//5.9 有源射频端口参数配置
	//功能：用于配置有源读写器的参数表的射频端口参数
	//参数：
	//		pHandle:	打开的端口句柄
	//		pPara:		配置读写器的参数表的射频端口参数 
	//		nLen: 		参数长度, 12字节
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YAntennaParmSet(void* pHandle ,unsigned char *pPara,unsigned char nLen );

	//////////////////////////////////////////////////////////////////////////
	//6.0 有源射频端口参数查询
	//功能：用于查询有源读写器的参数表的射频端口参数
	//参数：
	//		pHandle:			打开的端口句柄
	//		pPowerRecvPlus:		功放接收增益
	//				0x02:	高增益
	//				0x03:	低增益
	//		p2401FreqSendPwr:	2401射频器件发送功率
	//				0x00:	-18DB
	//				0x01:	-12DB
	//				0x02:	-6DB
	//				0x03:	0DB
	//		pAttenuatorPower:	射频衰减器功率
	//				0x00:	0DB
	//				0x01:	-1DB
	//				0x02:	-2DB
	//				0x03:	-3DB
	//				0x04:	-4DB
	//				0x05:	-5DB
	//				0x06:	-6DB
	//				0x07:	-7DB
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YAntennaParmQuery(void* pHandle,
		unsigned char *pPowerRecvPlus,
		unsigned char *p2401FreqSendPwr,
		unsigned char *pAttenuatorPower);

	//////////////////////////////////////////////////////////////////////////
	//6.1 有源读写器载波参数配置
	//功能：用于配置读写器的载波参数
	//参数：
	//	pHandle:	打开的端口句柄
	//	nType: 		设置的频率表的参数类型，为载波参数的类型，默认为0x00
	//	pPara: 		设置的频率表参数
	//		0		2405MHZ
	//		1		2410MHZ
	//		2		2415MHZ
	//		3		2420MHZ
	//		…
	//		23		2520MHZ
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YRFParaSet (void* pHandle ,
		unsigned char nType, 
		unsigned char nPara);


	//////////////////////////////////////////////////////////////////////////
	//6.2 有源读写器载波参数查询
	//功能：用于查询读写器的载波参数
	//参数：
	//	pHandle:	打开的端口句柄
	//	nType: 		设置的频率表的参数类型，为载波参数的类型，默认为0x00
	//	pPara: 		设置的频率表参数
	//		0		2405MHZ
	//		1		2410MHZ
	//		2		2415MHZ
	//		3		2420MHZ
	//		…
	//		23		2520MHZ
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YRFParaQuery (void* pHandle ,
		unsigned char nType,
		unsigned char* pPara);


	//////////////////////////////////////////////////////////////////////////
	//6.3 有源载波操作
	//功能：用于控制读写器的载波，打开或关闭载波并由指定端口输出。
	//参数：
	//		pHandle :		打开的端口句柄
	//		nType:
	//			0x00		关闭载波
	//			0x01		打开载波
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YCarrierWaveOp(void* pHandle ,unsigned char nType);


	//////////////////////////////////////////////////////////////////////////
	//6.4 有源读写器蜂鸣器操作
	//功能：用于配置读写器的内置蜂鸣器是否鸣叫
	//参数：
	//	pHandle:	打开的端口句柄
	//	nPara: 		蜂鸣器输出控制
	//		0x00	禁止蜂鸣器鸣叫
	//		0x01	打开蜂鸣器鸣叫
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YBuzzerSet (void* pHandle , unsigned char nPara);


	//////////////////////////////////////////////////////////////////////////
	//6.5 有源读写器蜂鸣器查询
	//功能：用于查询读写器的内置蜂鸣器状态
	//参数：
	//	pHandle:	打开的端口句柄
	//	pPara: 		查询蜂鸣器结果
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_YBuzzerQuery (void* pHandle ,	unsigned char* pPara);


	//////////////////////////////////////////////////////////////////////////
	//6.6 接收有源ID码命令
	//功能：用于接收ID码, ID码为8位十进制
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//		pId			为十进制int型ID, 
	//		nBit		为标签标记
	//域	保留	标签ID类型	省电标记	传感标记	按键标记	报警标记	低压标记
	//位	2	       1           1	       1         	1	       1	       1
	//返回值： 1为操作成功, 0为操作失败
	//////////////////////////////////////////////////////////////////////////
	int RFID_API  STDCALL SAAT_YRevIDMsgDec(void *pHandle, unsigned int* pId, unsigned char* nBit);


	//////////////////////////////////////////////////////////////////////////
	//6.7 接收有源ID码命令
	//功能：用于接收ID码, ID码为4位(即4字节)十六进制
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//		pIDData		为ID数据, 
	//		nIDLen		为ID数据长度
	//		nBit		为标签标记
	//域	保留	标签ID类型	省电标记	传感标记	按键标记	报警标记	低压标记
	//位	2	       1           1	       1         	1	       1	       1
	//返回值： 1为操作成功, 0为操作失败
	//////////////////////////////////////////////////////////////////////////
	int RFID_API  STDCALL SAAT_YRevIDMsgHex(void *pHandle, 
		unsigned char* pIDData,
		unsigned char* nIDLen, 
		unsigned char* nBit);



	//////////////////////////////////////////////////////////////////////////
	//6.8 设置本地语言ID码
	//功能：用于设置语言, 返回错误代码时返回适应的语言提示
	//参数:
	//		nLangId 	为已经初始化的端口句柄
	//			ZH_CN = 0,
	//			ZH_TW = 1,
	//			EN_US = 2
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_SetLangId(void *pHandle, unsigned char nLangId);

	//////////////////////////////////////////////////////////////////////////
	//6.9 读写器系统测试用函数
	//功能：直接发送底层数据(用于测试读写器对读卡垃圾数据处理能力)
	//参数：
	//	pHandle:		打开的端口句柄
	//	pSendData: 		发送数据
	//	nLen:			数据长度	
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_RawSendData(void* pHandle , unsigned char *pSendData, unsigned char nLen); 

	//////////////////////////////////////////////////////////////////////////
	//7.0 直接收取底层数据
	//功能：直接收取底层数据
	//参数：
	//	pHandle:		打开的端口句柄
	//	pTestParm: 		发送数据
	//	nLen:			数据长度	
	//	nWaitTime       超时时间
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_RawRevData(void* pHandle , unsigned char *pRecvData, unsigned char* pLen,int nWaitTime); 

	//////////////////////////////////////////////////////////////////////////
	//7.1 直接发送并收取底层数据
	//功能：直接发送并收取底层数据
	//参数：
	//	pHandle:		打开的端口句柄
	//	pSendData: 		发送数据
	//	nLen:			发送数据长度	
	//  pRecvData		接收数据
	//	pLen			接收数据长度
	//  nWaitTime       超时时间
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_RawSendAndRevData(void* pHandle , 
		unsigned char *pSendData,
		unsigned char nLen,
		unsigned char *pRecvData,
		unsigned char *pLen,
		unsigned char nWaitTime); 

	//////////////////////////////////////////////////////////////////////////
	// 7.2 读写器时间设置
	//功能：用于设置读写器时间
	//参数：
	//		pHandle :	打开的端口句柄
	//		pPara:		读写器返回的读写器时间数据
	//		pLen:		参数数据长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_SetReaderTime (void* pHandle ,unsigned char* pPara,unsigned char nLen);

	//////////////////////////////////////////////////////////////////////////
	// 7.3 读写器时间查询
	//功能：用于查询读写器时间
	//参数：
	//		pHandle :	打开的端口句柄
	//		pPara:		读写器返回的读写器时间数据
	//		pLen:		参数数据长度
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_ReaderTimeQuery (void* pHandle, unsigned char* pPara, unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	// 7.4 Flash缓存计数查询
	//功能：缓存计数查询
	//参数：
	//		pHandle :	打开的端口句柄
	//		pPara:		
	//		pLen:		总计数
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_FlashTotalCountQuery (void* pHandle ,
		unsigned char* pPara,
		unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	// 7.5 Flash缓存数据查询
	//功能：缓存计数查询
	//参数：
	//		pHandle :	打开的端口句柄
	//		nOpType :
	//		pPara:	
	//		pLen:		总计数
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  STDCALL SAAT_FlashDataQuery (void* pHandle ,
		unsigned char nOpType,
		unsigned char* pPara,
		unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	// 7.6 Flash缓存数据清除
	//功能：缓存计数查询
	//参数：
	//		pHandle :	打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_ClearFlash (void* pHandle );


	//////////////////////////////////////////////////////////////////////////
	//7.7 WIFI通讯参数配置
	//功能：用于配置WIFI模块通讯参数
	//参数：
	//	pHandle : 	打开的端口句柄
	//	nType: 	配置的参数类型
	//			“类型”为配置的参数类型，包括：
	//			0		WIFI模块串口通信波特率
	//			1		WIFI模块MAC地址
	//			2		WIFI模块IP地址
	//			3		WIFI模块TCP端口号
	//			4		WIFI模块UDP端口号 
	//	pPara:		参数
	//	nLen:		参数长度 数据定义为：
	//				0x00		4800bps
	//				0x01		9600 bps
	//				0x02		19200
	//				0x03		38400
	//				0x04		57600
	//				0x05		115200
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_WCommunicatParaSet (void* pHandle ,unsigned char nType, unsigned char* pPara, unsigned char nLen);


	//////////////////////////////////////////////////////////////////////////
	//7.8 WIFI通讯参数查询
	//功能：用于查询WIFI模块的通讯参数
	//参数：
	//	pHandle : 	打开的端口句柄
	//	nType: 	配置的参数类型
	//			“类型”为配置的参数类型，包括：
	//			0		WIFI模块串口通信波特率
	//			1		WIFI模块MAC地址
	//			2		WIFI模块IP地址
	//			3		WIFI模块TCP端口号
	//			4		WIFI模块UDP端口号 
	//	pPara:		参数
	//	nLen:		参数长度 数据定义为：
	//				0x00		4800bps
	//				0x01		9600 bps
	//				0x02		19200
	//				0x03		38400
	//				0x04		57600
	//				0x05		115200
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_WCommunicatParaQuery (void* pHandle ,unsigned char nType, unsigned char* pPara,unsigned char *pLen);

	//////////////////////////////////////////////////////////////////////////
	//7.9 重启读写器
	//功能描述：此函数用于重新启动读写器。
	//函数原型：
	//bool	SAAT_Reboot(void* pHandle,unsigned char nMode)
	//传入参数：
	//pHandle		打开的端口句柄	
	//nMode			操作模式
	//0x00  普通模式
	//0x01  BOOT重启模式，启动后RFID设备将发送BOOT指令
	//返回值：true：操作成功；false：操作失败。
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_Reboot(void* pHandle ,unsigned char nType);

	//////////////////////////////////////////////////////////////////////////
	//8.0 有源标签选择配置
	//功能： 有源标签选择配置
	//参数:
	//		pHandle: 为IConnect指针
	//      nOpEnable:LED灯 、蜂鸣器使能
	//		nMatchType: 匹配模式, 
	//		MatchData: 匹配数据
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API  __stdcall SAAT_YTagSelect ( void *pHandle, unsigned char nOpEnable, unsigned char nMatchType, unsigned char *MatchData, unsigned char nLenth);

	//////////////////////////////////////////////////////////////////////////
	//8.1 读写器配置进入透传模式用函数（WIFI设备调用）
	//功能：读写器配置进入透传模式
	//参数：
	//	pHandle:		打开的端口句柄
	//	nType: 			类型参数参数 0x00-WIFI  0xFD 复位波特率SAAT_ResetWifiBaund
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_EnterTrans(void* pHandle ,unsigned char nType);

	//////////////////////////////////////////////////////////////////////////
	//8.2 读写器WIFI配置波特率函数(WIFI设备调用)
	//功能：重新设置WIFI波特率
	//参数：
	//	pHandle:		打开的端口句柄
	//返回值： true为操作成功, false为操作失败
	//////////////////////////////////////////////////////////////////////////
	bool RFID_API STDCALL SAAT_ResetWifiBaund(void* pHandle);

	//温度标签
	//////////////////////////////////////////////////////////////////////////
	// 8.3接收有源ID码命令
	//功能：用于接收ID码, ID码为十进制
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//      nFlag		为标签标记，0x00表示普通标签；0x01表示温度标签
	//		pId			为十进制int型ID,
	//		nBit		为标签标记
	//					域	保留	标签ID类型	省电标记	传感标记	按键标记	报警标记	低压标记
	//					位	2	       1           1	       1         	1	       1	       1
	//      nHighTpt	温度标签整数
	//      nLowTpt		温度标签小数
	//返回值： 1为操作成功, 0为操作失败
	//////////////////////////////////////////////////////////////////////////
	int RFID_API STDCALL SAAT_YRevIDMsgDecTemperature(void *pHandle,unsigned char* pFlag,unsigned int* pId,unsigned char* pBit, unsigned char* pHighTpt, unsigned char* pLowTpt);

	//////////////////////////////////////////////////////////////////////////
	//8.4 接收有源ID码命令
	//功能：用于接收ID码, ID码为4位(即4字节)十六进制
	//参数:
	//		pHandle 	为已经初始化的端口句柄
	//      nFlag		为标签标记，0x00表示普通标签；0x01表示温度标签
	//		pIDData		为ID数据, 
	//		nIDLen		为ID数据长度
	//		nBit		为标签标记
	//					域	保留	标签ID类型	省电标记	传感标记	按键标记	报警标记	低压标记
	//					位	2	       1           1	       1         	1	       1	       1
	//      nHighTpt	温度标签整数
	//      nLowTpt		温度标签小数
	//返回值： 1为操作成功, 0为操作失败
	//////////////////////////////////////////////////////////////////////////
	int RFID_API  STDCALL SAAT_YRevIDMsgHexTemperature(void *pHandle, unsigned char* nFlag, unsigned char* pIDData, unsigned char* nIDLen, unsigned char* nBit, unsigned char* nHighTpt, unsigned char* nLowTpt);


	bool RFID_API STDCALL SAAT_GCommunicatParaSet (void* pHandle ,unsigned char nType, unsigned char* pPara, unsigned char nLen);

	bool RFID_API STDCALL SAAT_GCommunicatParaQuery (void* pHandle ,unsigned char nType, unsigned char* pPara,unsigned char *pLen);

	bool RFID_API STDCALL SAAT_SendTrans(void* pHandle,unsigned char nType,unsigned char* pData,unsigned char nLen);

	bool RFID_API STDCALL SAAT_RecvTrans(void* pHandle,unsigned char nType,unsigned char *pData,unsigned char* pLen);

	bool RFID_API STDCALL SAAT_ModuleReboot(void* pHandle ,unsigned char nType);


#ifdef __cplusplus
}
#endif


#endif