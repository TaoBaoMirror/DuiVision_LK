#if !defined(ErrorCode_define)
#define ErrorCode_define

//////////////////////////////////////////////////////////////////////////
/*用户管理模块返回错误类型*/
#define ER_LIST_USERS					100001	//列出用户信息失败
#define ER_ADD_USER						100002	//新增用户失败
#define ER_OLD_PASSWORD					100003	//密码核对错误 
#define ER_LIST_GROUP					100004	//列出用户失败
#define ER_USER_EXISTED					100005	//用户已经存在 
#define ER_IKEY_BOUND					100006	//此iKey已经绑定到其他用户

/*加密狗模块返回错误类型*/
#define ER_OPEN_DEVICE				110001 	//打开ikey失败
#define ER_CLOSE_DEVICE				110002	//关闭ikey失败
#define ER_VERIFY_USER_PIN			110003	//用户PIN码校验失败
#define ER_HANDLE_IS_NULL			110004	//设备句柄为空
#define ER_CREATE_APP_DIR			110005	//创建应用程序目录失败
#define ER_CREATE_DATA_FILE			110006	//创建数据文件失败
#define ER_CREATE_MD5_HMAC_KEY		110007	//创建MD5_HMAC_KEY文件失败
#define ER_MD5_HMAC_AUTHENTICATE	110008	//签名MD5_HMAC失败
#define ER_OPEN_DIR					110009	//打开目录失败
#define ER_GET_USER					110010	//取得用户名失败

#define ER_GET_LICENSE				111001	//获取加密狗许可失败
#define ER_QUERY_FEATURE			111002	//查询特征码失败
#define ER_QUERY_RESPONSE			111003	//查询响应错误

/*系统配置模块返回错误类型*/
#define ER_NO_CONFIG_FILE	120001	//没有配置文件
#define ER_CONFIG_FILE		120002	//配置文件错误
#define ER_LOAD_CONFIG_FILE	120003	//加载配置文件失败
#define ER_UPDATE_CONFIG	120004	//更新配置文件失败

/*处方管理模块返回错误类型*/
#define ER_NO_PATIENT_INFO				130001	//没有患者信息
#define ER_NO_MEDICINE_INFO				130002	//没有药品信息
#define ER_SAVE_PRESCRIPTION			130003	//保存处方失败
#define ER_SAVE_PATIENT					130004	//保存病人失败
#define ER_SAVE_PRESCRIPTION_MEDICINE	130005	//保存药品失败
#define ER_INPUT_PRESCRIPTION			130006	//录入处方失败
#define ER_QUERY_PRESCRIPTION			130007	//处方查询失败
#define ER_GET_PATIENT					130008	//获取病人信息失败
#define ER_GET_PRESCRIPTION_MEDICINE	130009	//获取处方药品信息失败
#define ER_MODIFY_PRESCRIPTION			130010	//修改处方失败
#define ER_MODIFY_PRESCRIPTION_MEDICINE	130011	//修改处方药品信息失败
#define ER_MODIFY_PRESCRIPTION_PATIENT	130012	//修改处方病人信息失败
#define ER_DEL_PRESCRIPTION				130013	//删除处方失败
#define ER_DEL_PRESCRIPTION_MEDICINE	130014	//删除处方药品信息失败
#define ER_DEL_PATIENT					130015	//删除病人信息失败

/*数据库管理模块返回错误类型*/
#define ER_DBMAN_Init					140001	//数据库初始化失败
#define ER_DBMAN_Connect				140002	//数据库连接失败
#define ER_DBMAN_SqlIsEmpty				140003	//SQL语句为空
#define ER_DBMAN_ExecuteSql				140004	//执行SQL语句错误
#define ER_DBMAN_DataEnd				140005  //结果集中最后一行的下一行
#define ER_DBMAN_NoRecordset			140006	//数据集为空
#define ER_DBMAN_OpenError				140007  //数据库没有打开
#define ER_DBMAN_ServerState			140008	//数据库服务器连接状态错误
#define ER_UPDATE_TABLE					140012	//更新表失败


/*热敏打印机模块返回错误类型*/
#define ER_PRINTER_PORT_NUM				150001	//打印机端口号错误
#define ER_OPEN_PRINTER_PORT			150002	//打印机端口打开失败
#define ER_NO_BASKET_ID					150003	//没有篮子卡号
#define ER_PRINT_PRESCRIPTION			150004	//打印处方失败
#define ER_PRESCRIPTION_ID				150005	//通过处方号打印处方失败
#define ER_PRINT_HETEROMORPHISM			150006	//打印异形药品信息失败

#define ER_NO_PAPER						150007	//纸尽
#define ER_LESS_PAPER					150008	//纸将尽
#define ER_CUT					        150009	//切刀错误
#define ER_HOT					        150010	//热敏头温度过高
#define ER_NO_BASKET_NUMBER             150011  //没有输入篮子序号

//CThermalPrinterStar
//#define ER_PRINTER_PORT_NUM				151001	//打印机端口号错误
//#define ER_OPEN_PRINTER_PORT			151002	//打印机端口打开失败
#define WN_NOT_OPEN_PORT				151003	//打印机端口未打开
#define ER_PRINT						151004	//打印失败
#define ER_GET_PRINTER_STATE			151005	//获取打印机状态失败
#define ER_SEND_COMMAND					151006	//发送打印机指令失败

#define ER_PORT_NUM				159001	//读卡器端口号错误
#define ER_OPEN_PORT			159002	//读卡器端口打开失败
#define ER_WRITE_DATA			159003	//写读卡器端口数据失败
#define ER_READ_DATA			159004	//读端口数据失败
#define ER_NOT_OPEN_PORT		159005	//读卡器端口没有打开
#define ER_CREATE_LISTEN_THREAD	159006	//创建读卡器监听线程失败

/*PLC模块返回错误类型*/
#define ER_MEDICINE_POSITION			160001	//药品位置信息错误
#define ER_MEDICINE_COUNT				160002	//药品数目错误
#define ER_SUSPEND_LISTEN_THREAD		160003	//终止设备状态监听线程失败
#define ER_CLOSE_LISTEN_THREAD			160004	//关闭设备状态监听线程句柄失败
#define ER_NO_PLC						160005  //没有PLC实例
#define ER_NOT_OPEN_PLC					160006  //PLC连接没有打开

//CPLCMitsubishi
#define ER_CREATE_INSTANCE				162001		//创建COM对象失败
#define	ER_SET_CPUTYPE					162002		//设置CPU类型失败
#define ER_SET_HOST_ADDRESS				162003		//设置主机地址失败 
#define ER_SET_STATION_NUMBER			162004		//设置站号失败
#define ER_OPEN							162005		//打开连接错误
#define ER_CLOSE						162006		//关闭连接错误
#define ER_NO_DEVICENAME				162007		//没有输入设备名
#define ER_READ_DEVICE					162008		//读单个设备值错误
#define ER_WRITE_DEVICE					162009		//写单个设备值错误
#define ER_READ_DEVICE_BLOCK			162010		//批量读设备值错误
#define ER_WRITE_DEVICE_BLOCK			162011		//批量写设备值错误
#define ER_NO_INSTANCE					162012		//plcComponent实例为空
#define ER_PLC_CONNECTION				162013		//连接状态错误

//////////////////////////////////////////////////////////////////////////

#define FILLLIST_EMPTY                   170001		//加药队列为空

#define FAILED_TO_CREATE_THREAD			 180001     //创建线程失败
#define FAILED_TO_RESUME_THREAD			 180002		//恢复线程失败
#define FAILED_TO_SUSPEND_THREAD		 180003		//中断线程失败
#define FAILED_TO_CLOSE_HANDLE			 180004		//关闭线程失败
#define NOT_ENOUGH_MEDICINE				 180005		//药品存量不够
#define CAN_NOT_FIND_NORMAL_ID           180006		//查询不到正常队列处方ID
#define CAN_NOT_FIND_ABNORMAL_ID         180007		//查询不到异常队列处方ID

//////////////////////////////////////////////////////////////////////////
/*打印包装类返回错误类型*/

/*左藤打印机*/
#define ERROR_SatoPrintPack_Open         200001    //打开串口失败
#define ERROR_SatoPrintPack_Set          200002    //设置串口失败
#define ERROR_SatoPrintPack_Register     200003    //注册失败
#define ERROR_SatoPrintPack_Close        200004    //关闭串口失败
#define ERROR_SatoPrintPack_Print        200005    //打印输入内容错误
#define ERROR_SatoPrintPack_PowerClose   200006    //打印机电源关闭
#define ERROR_SatoPrintPack_HeadOpen     200007    //打印头打开
#define ERROR_SatoPrintPack_RibbonEnd    200008    //打印机色带用完
#define ERROR_SatoPrintPack_PaperEnd     200009    //打印机包装袋用完
#define ERROR_SatoPrintPack_Sensor       200010    //打印机传感器错误
#define ERROR_SatoPrintPack_Offline      200011    //打印机离线
#define ERROR_SatoPrintPack_PrintStop    200012    //打印机停止
#define ERROR_SatoPrintPack_HeadBad      200013    //打印头坏了
#define ERROR_SatoPrintPack_Other        200014    //其它错误
#define ERROR_SatoPrintPack_Pack         200015    //连接包装机失败
#define ERROR_SatoPrintPack_Connect      200016    //连接包装机失败

/*三洋打印机*/
#define BPLA_OK             1000     //一切正常
#define BPLA_COMERROR       1001     //通讯错或者未联接打印机
#define BPLA_PARAERROR      1002     //热转打印机参数错误
#define BPLA_FILEOPENERROR  1003     //文件打开错误
#define BPLA_FILEREADERROR  1004     //文件读错误
#define BPLA_FILEWRITEERROR 1005     //文件写错误
#define BPLA_FILEERROR      1006     //文件不合要求
#define BPLA_NUMBEROVER     1007     //指定的接收信息数量过大
#define BPLA_IMAGETYPEERROR 1008     //图象文件格式不正确 
#define BPLA_NoDLL          1009     //加载BPLADLL.DLL失败
#define BPLA_RibbonEnd      1010     //热转打印机缺色带
#define BPLA_PrintBusy      1011     //热转打印机解释器忙
#define BPLA_Pause          1012     //热转打印机暂停
#define BPLA_CommutError    1013     //热转打印机通讯错误
#define BPLA_HeadOpen       1014     //打印头抬起
#define BPLA_Headheat       1015     //打印头过热
#define BPLA_CutError       1016     //热转打印机切刀响应超时
#define BPLA_PaperEnd       1017     //热转打印机缺纸



/*药品管理类返回错误类型*/
#define ERROR_MedicineManage_Initialize              210001  //初始化各类药品位置状态失败
#define ERROR_MedicineManage_PopMedicinLocation      210002  //发药时修改药品位置状态失败
#define ERROR_MedicineManage_PushMedicinLocation     210003  //加药时修改药品位置状态失败
#define ERROR_MedicineManage_AddressSearch           210004  //查找药品的地址列表失败
#define ERROR_MedicineManage_GetMedicineCount        210005  //获取药品当前所剩余的数量失败
#define ERROR_MedicineManage_FastSearch              210006  //编码快速查找失败
#define ERROR_MedicineManage_ListSearch              210007  //查找药品列表失败
#define ERROR_MedicineManage_LookMedicine            210008  //查找药品详细信息失败
#define ERROR_MedicineManage_ModifyMedicine          210009  //修改药品详细信息失败
#define ERROR_MedicineManage_AddMedicine             210010  //新增药品详细信息失败
#define ERROR_MedicineManage_DeleteMedicine          210011  //删除药品详细信息失败
#define ERROR_MedicineManage_ClearLocation           210012  //清空药品单个地址信息失败
#define ERROR_MedicineManage_ClearAllLocation        210013  //清空药品所有地址信息失败
#define ERROR_MedicineManage_getMaxmedID             210014  //获取药品最大ID号失败
#define ERROR_MedicineManage_getmedID                210015  //获取药品ID号失败
#define ERROR_MedicineManage_light_position          210016  //获取指示灯号失败
#define ERROR_MedicineManage_MedicineOverdue         210017  //当前药品已经过期
#define ERROR_MedicineManage_modifyMedicinLocation   210018  //修改药品位置状态出错
#define ERROR_MedicineManage_QueryMedInfo_Client     210019  //查询客户端需求药品信息失败
#define ERROR_MedicineManage_QueryMedleavingPercent  210020  //查询药品剩余百分比失败

/*全局统计类返回错误类型*/
#define ERROR_Statistic_SumLayMedicinCount     220001  //保存总摆药次数失败
#define ERROR_Statistic_SumPrescriptionCount   220002  //保存总处方数量失败
#define ERROR_Statistic_SumDeviceRunTime       220003  //保存总设备运行时间数据失败
#define ERROR_Statistic_SumLogCount            220004  //保存总登录次数失败
#define ERROR_Statistic_SumPrintCount          220005  //保存总热敏打印次数失败
#define ERROR_Statistic_SumSatoPrintCount      220006  //保存总热转打印次数失败
#define ERROR_Statistic_SumPackCount           220007  //保存总包装次数失败
#define ERROR_Statistic_AddMedicinSortd        220008  //保存药品分类统计失败
#define ERROR_Statistic_AddLocation            220009  //保存位置统计失败
#define ERROR_Statistic_GetMedicinSort         220010  //输出药品分类统计失败
#define ERROR_Statistic_GetLocation            220011  //输出地址分类统计失败
#define ERROR_Statistic_Initialize             220012  //初始化全局统计数据失败
#define ERROR_Statistic_SetBooking             220013  //设置预警量失败
#define ERROR_Statistic_GetBooking             220014  //输出预警量失败

/*监控系统类返回错误类型*/
#define ERROR_Monitor_CreatThead        230001  //创建设备监控线程失败
#define ERROE_Monitor_GetCameraState    230002  //连接摄像机失败
#define ERROE_Monitor_GetScannerState   230003  //连接扫描仪失败
#define ERROE_Monitor_GetPLCState       230004  //连接PLC失败
#define ERROE_Monitor_GetPrintState     230005  //连接打印机失败

/*批量导入返回错误类型*/
#define ERROR_Import_Medicine          240001  //批量导入药品失败
#define ERROR_Import_Prescription      240002  //批量导入处方失败

/************************************************************************/
/*					热敏打印                                        */
/************************************************************************/
#define ERROR_THERMAL_PRINT_SP_OPEN		300001	//热敏打印机打开串口失败
#define ERROR_THERMAL_PRINT_SP_RECV		300002	//热敏打印机读取串口失败
#define ERROR_THERMAL_PRINT_SP_SEND		300003	//热敏打印机写串口失败

//盒剂
#define ERROR_PRINT_PRE_MED_INFO		300004	//打印处方药品列表失败
#define ERROR_THERMAL_PRINT_QUERY_EMPTY	300005	//热敏打印机查询获取记录为空

//片剂
#define ERROR_PRINT_HETEROMORPHISM		300006	//打印异形药失败
#define ERROR_PRINT_COMMON				300007	//打印普通缺药失败
#define ERROR_PRINT_MANUAL				300008	//打印手工取药失败
#define ERROR_PRINT_BIG_PRE				300009	//打印超大医嘱失败
#define ERROR_PRINT_MED_INFO			300010	//打印药品信息失败
#define ERROR_PRINT_LACK_ISAS			300011	//打印智能存取缺药列表失败

/*数码管返回错误类型*/
#define ERROR_LAMP_PORT_OPEN			300001	//打开数码管串口失败
#define ERROR_LAMP_PARAM				300002	//参数错误
#define ERROR_LAMP_VERIFY_SUM			300003	//校验和错误
#define ERROR_LAMP_SP_SEND				300004	//串口发送失败
#define ERROR_LAMP_SP_RECV				300005	//串口接收失败
#define	ERROR_LAMP_SP_SEND_DATA_INCONSISTENCY 300006	//串口接收缓存区的字节数和需要接收到的字节数不一致
#define	ERROR_LAMP_SP_RECV_DATA_INCONSISTENCY 300007	//串口接收缓存区的字节数和需要接收到的字节数不一致
#define ERROR_LAMP_SP_RECV_NULL			300008	//接收数据为空


/* PLC返回错误类型 */
#define ERROR_PLC_OPEN_PORT				310000	//打开PLC失败
#define ERROR_PLC_SWITCH_SCREEN			310001	//切换屏幕失败
#define ERROR_PLC_BUZZER				310002	//蜂鸣器操作失败
#define ERROR_PLC_TURN_OFF_HARD_SURE_LAMP 310003 //关闭“硬确认”指示灯失败
#define ERROR_PLC_SEND_RUN_TIER			310004	//发送走层命令失败
#define ERROR_PLC_SEND_TIER_NUM			310005	//发送层号命令失败
#define ERROR_PLC_LOC_ID_BOUND			310006	//位置ID越界
#define ERROR_PLC_OPEN_FRONT_DOOR		310007	//发送开前门命令失败
#define ERROR_PLC_CLOSE_FRONT_DOOR		310008	//发送关前门命令失败
#define ERROR_PLC_OPEN_BACK_DOOR		310009	//发送开后门命令失败
#define ERROR_PLC_CLOSE_BACK_DOOR		310010	//发送关后门命令失败
#define ERROR_PLC_GET_DEVICE_FUNC       310011	//读取GetDevice()失败
#define ERROR_PLC_PARAM					310012	//参数错误

/************************************************************************/
/*                    条码扫描                                          */
/************************************************************************/
#define ERROR_BARCODE_SCANNER_SP_OPEN	320000	//条码扫描器打开串口失败
#define ERROR_BARCODE_SCANNER_SP_RECV	320001	//条码扫描器读取串口失败
#define ERROR_BARCODE_SCANNER_SP_SEND	320002	//条码扫描器写串口失败
#define ERROR_BARCODE_SCANNER_SET		320003	//条码扫描器设置扫描方式失败

/************************************************************************/
/*                    读卡器                                          */
/************************************************************************/
#define ERROR_CARD_READER_SP_OPEN	330000	//读卡器打开串口失败
#define ERROR_CARD_READER_SP_RECV	330001	//读卡器读取串口失败
#define ERROR_CARD_READER_SP_SEND	330002	//读卡器写串口失败
#define ERROR_CARD_READER_SP_UNPACK 330003	//读卡器解析数据失败


//////////////////////////////////////////////////////////////////////////

#endif