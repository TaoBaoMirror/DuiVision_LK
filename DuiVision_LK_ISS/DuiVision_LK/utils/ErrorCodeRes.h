#if !defined(ErrorCode_define)
#define ErrorCode_define

//////////////////////////////////////////////////////////////////////////
/*�û�����ģ�鷵�ش�������*/
#define ER_LIST_USERS					100001	//�г��û���Ϣʧ��
#define ER_ADD_USER						100002	//�����û�ʧ��
#define ER_OLD_PASSWORD					100003	//����˶Դ��� 
#define ER_LIST_GROUP					100004	//�г��û�ʧ��
#define ER_USER_EXISTED					100005	//�û��Ѿ����� 
#define ER_IKEY_BOUND					100006	//��iKey�Ѿ��󶨵������û�

/*���ܹ�ģ�鷵�ش�������*/
#define ER_OPEN_DEVICE				110001 	//��ikeyʧ��
#define ER_CLOSE_DEVICE				110002	//�ر�ikeyʧ��
#define ER_VERIFY_USER_PIN			110003	//�û�PIN��У��ʧ��
#define ER_HANDLE_IS_NULL			110004	//�豸���Ϊ��
#define ER_CREATE_APP_DIR			110005	//����Ӧ�ó���Ŀ¼ʧ��
#define ER_CREATE_DATA_FILE			110006	//���������ļ�ʧ��
#define ER_CREATE_MD5_HMAC_KEY		110007	//����MD5_HMAC_KEY�ļ�ʧ��
#define ER_MD5_HMAC_AUTHENTICATE	110008	//ǩ��MD5_HMACʧ��
#define ER_OPEN_DIR					110009	//��Ŀ¼ʧ��
#define ER_GET_USER					110010	//ȡ���û���ʧ��

#define ER_GET_LICENSE				111001	//��ȡ���ܹ����ʧ��
#define ER_QUERY_FEATURE			111002	//��ѯ������ʧ��
#define ER_QUERY_RESPONSE			111003	//��ѯ��Ӧ����

/*ϵͳ����ģ�鷵�ش�������*/
#define ER_NO_CONFIG_FILE	120001	//û�������ļ�
#define ER_CONFIG_FILE		120002	//�����ļ�����
#define ER_LOAD_CONFIG_FILE	120003	//���������ļ�ʧ��
#define ER_UPDATE_CONFIG	120004	//���������ļ�ʧ��

/*��������ģ�鷵�ش�������*/
#define ER_NO_PATIENT_INFO				130001	//û�л�����Ϣ
#define ER_NO_MEDICINE_INFO				130002	//û��ҩƷ��Ϣ
#define ER_SAVE_PRESCRIPTION			130003	//���洦��ʧ��
#define ER_SAVE_PATIENT					130004	//���没��ʧ��
#define ER_SAVE_PRESCRIPTION_MEDICINE	130005	//����ҩƷʧ��
#define ER_INPUT_PRESCRIPTION			130006	//¼�봦��ʧ��
#define ER_QUERY_PRESCRIPTION			130007	//������ѯʧ��
#define ER_GET_PATIENT					130008	//��ȡ������Ϣʧ��
#define ER_GET_PRESCRIPTION_MEDICINE	130009	//��ȡ����ҩƷ��Ϣʧ��
#define ER_MODIFY_PRESCRIPTION			130010	//�޸Ĵ���ʧ��
#define ER_MODIFY_PRESCRIPTION_MEDICINE	130011	//�޸Ĵ���ҩƷ��Ϣʧ��
#define ER_MODIFY_PRESCRIPTION_PATIENT	130012	//�޸Ĵ���������Ϣʧ��
#define ER_DEL_PRESCRIPTION				130013	//ɾ������ʧ��
#define ER_DEL_PRESCRIPTION_MEDICINE	130014	//ɾ������ҩƷ��Ϣʧ��
#define ER_DEL_PATIENT					130015	//ɾ��������Ϣʧ��

/*���ݿ����ģ�鷵�ش�������*/
#define ER_DBMAN_Init					140001	//���ݿ��ʼ��ʧ��
#define ER_DBMAN_Connect				140002	//���ݿ�����ʧ��
#define ER_DBMAN_SqlIsEmpty				140003	//SQL���Ϊ��
#define ER_DBMAN_ExecuteSql				140004	//ִ��SQL������
#define ER_DBMAN_DataEnd				140005  //����������һ�е���һ��
#define ER_DBMAN_NoRecordset			140006	//���ݼ�Ϊ��
#define ER_DBMAN_OpenError				140007  //���ݿ�û�д�
#define ER_DBMAN_ServerState			140008	//���ݿ����������״̬����
#define ER_UPDATE_TABLE					140012	//���±�ʧ��


/*������ӡ��ģ�鷵�ش�������*/
#define ER_PRINTER_PORT_NUM				150001	//��ӡ���˿ںŴ���
#define ER_OPEN_PRINTER_PORT			150002	//��ӡ���˿ڴ�ʧ��
#define ER_NO_BASKET_ID					150003	//û�����ӿ���
#define ER_PRINT_PRESCRIPTION			150004	//��ӡ����ʧ��
#define ER_PRESCRIPTION_ID				150005	//ͨ�������Ŵ�ӡ����ʧ��
#define ER_PRINT_HETEROMORPHISM			150006	//��ӡ����ҩƷ��Ϣʧ��

#define ER_NO_PAPER						150007	//ֽ��
#define ER_LESS_PAPER					150008	//ֽ����
#define ER_CUT					        150009	//�е�����
#define ER_HOT					        150010	//����ͷ�¶ȹ���
#define ER_NO_BASKET_NUMBER             150011  //û�������������

//CThermalPrinterStar
//#define ER_PRINTER_PORT_NUM				151001	//��ӡ���˿ںŴ���
//#define ER_OPEN_PRINTER_PORT			151002	//��ӡ���˿ڴ�ʧ��
#define WN_NOT_OPEN_PORT				151003	//��ӡ���˿�δ��
#define ER_PRINT						151004	//��ӡʧ��
#define ER_GET_PRINTER_STATE			151005	//��ȡ��ӡ��״̬ʧ��
#define ER_SEND_COMMAND					151006	//���ʹ�ӡ��ָ��ʧ��

#define ER_PORT_NUM				159001	//�������˿ںŴ���
#define ER_OPEN_PORT			159002	//�������˿ڴ�ʧ��
#define ER_WRITE_DATA			159003	//д�������˿�����ʧ��
#define ER_READ_DATA			159004	//���˿�����ʧ��
#define ER_NOT_OPEN_PORT		159005	//�������˿�û�д�
#define ER_CREATE_LISTEN_THREAD	159006	//���������������߳�ʧ��

/*PLCģ�鷵�ش�������*/
#define ER_MEDICINE_POSITION			160001	//ҩƷλ����Ϣ����
#define ER_MEDICINE_COUNT				160002	//ҩƷ��Ŀ����
#define ER_SUSPEND_LISTEN_THREAD		160003	//��ֹ�豸״̬�����߳�ʧ��
#define ER_CLOSE_LISTEN_THREAD			160004	//�ر��豸״̬�����߳̾��ʧ��
#define ER_NO_PLC						160005  //û��PLCʵ��
#define ER_NOT_OPEN_PLC					160006  //PLC����û�д�

//CPLCMitsubishi
#define ER_CREATE_INSTANCE				162001		//����COM����ʧ��
#define	ER_SET_CPUTYPE					162002		//����CPU����ʧ��
#define ER_SET_HOST_ADDRESS				162003		//����������ַʧ�� 
#define ER_SET_STATION_NUMBER			162004		//����վ��ʧ��
#define ER_OPEN							162005		//�����Ӵ���
#define ER_CLOSE						162006		//�ر����Ӵ���
#define ER_NO_DEVICENAME				162007		//û�������豸��
#define ER_READ_DEVICE					162008		//�������豸ֵ����
#define ER_WRITE_DEVICE					162009		//д�����豸ֵ����
#define ER_READ_DEVICE_BLOCK			162010		//�������豸ֵ����
#define ER_WRITE_DEVICE_BLOCK			162011		//����д�豸ֵ����
#define ER_NO_INSTANCE					162012		//plcComponentʵ��Ϊ��
#define ER_PLC_CONNECTION				162013		//����״̬����

//////////////////////////////////////////////////////////////////////////

#define FILLLIST_EMPTY                   170001		//��ҩ����Ϊ��

#define FAILED_TO_CREATE_THREAD			 180001     //�����߳�ʧ��
#define FAILED_TO_RESUME_THREAD			 180002		//�ָ��߳�ʧ��
#define FAILED_TO_SUSPEND_THREAD		 180003		//�ж��߳�ʧ��
#define FAILED_TO_CLOSE_HANDLE			 180004		//�ر��߳�ʧ��
#define NOT_ENOUGH_MEDICINE				 180005		//ҩƷ��������
#define CAN_NOT_FIND_NORMAL_ID           180006		//��ѯ�����������д���ID
#define CAN_NOT_FIND_ABNORMAL_ID         180007		//��ѯ�����쳣���д���ID

//////////////////////////////////////////////////////////////////////////
/*��ӡ��װ�෵�ش�������*/

/*���ٴ�ӡ��*/
#define ERROR_SatoPrintPack_Open         200001    //�򿪴���ʧ��
#define ERROR_SatoPrintPack_Set          200002    //���ô���ʧ��
#define ERROR_SatoPrintPack_Register     200003    //ע��ʧ��
#define ERROR_SatoPrintPack_Close        200004    //�رմ���ʧ��
#define ERROR_SatoPrintPack_Print        200005    //��ӡ�������ݴ���
#define ERROR_SatoPrintPack_PowerClose   200006    //��ӡ����Դ�ر�
#define ERROR_SatoPrintPack_HeadOpen     200007    //��ӡͷ��
#define ERROR_SatoPrintPack_RibbonEnd    200008    //��ӡ��ɫ������
#define ERROR_SatoPrintPack_PaperEnd     200009    //��ӡ����װ������
#define ERROR_SatoPrintPack_Sensor       200010    //��ӡ������������
#define ERROR_SatoPrintPack_Offline      200011    //��ӡ������
#define ERROR_SatoPrintPack_PrintStop    200012    //��ӡ��ֹͣ
#define ERROR_SatoPrintPack_HeadBad      200013    //��ӡͷ����
#define ERROR_SatoPrintPack_Other        200014    //��������
#define ERROR_SatoPrintPack_Pack         200015    //���Ӱ�װ��ʧ��
#define ERROR_SatoPrintPack_Connect      200016    //���Ӱ�װ��ʧ��

/*�����ӡ��*/
#define BPLA_OK             1000     //һ������
#define BPLA_COMERROR       1001     //ͨѶ�����δ���Ӵ�ӡ��
#define BPLA_PARAERROR      1002     //��ת��ӡ����������
#define BPLA_FILEOPENERROR  1003     //�ļ��򿪴���
#define BPLA_FILEREADERROR  1004     //�ļ�������
#define BPLA_FILEWRITEERROR 1005     //�ļ�д����
#define BPLA_FILEERROR      1006     //�ļ�����Ҫ��
#define BPLA_NUMBEROVER     1007     //ָ���Ľ�����Ϣ��������
#define BPLA_IMAGETYPEERROR 1008     //ͼ���ļ���ʽ����ȷ 
#define BPLA_NoDLL          1009     //����BPLADLL.DLLʧ��
#define BPLA_RibbonEnd      1010     //��ת��ӡ��ȱɫ��
#define BPLA_PrintBusy      1011     //��ת��ӡ��������æ
#define BPLA_Pause          1012     //��ת��ӡ����ͣ
#define BPLA_CommutError    1013     //��ת��ӡ��ͨѶ����
#define BPLA_HeadOpen       1014     //��ӡͷ̧��
#define BPLA_Headheat       1015     //��ӡͷ����
#define BPLA_CutError       1016     //��ת��ӡ���е���Ӧ��ʱ
#define BPLA_PaperEnd       1017     //��ת��ӡ��ȱֽ



/*ҩƷ�����෵�ش�������*/
#define ERROR_MedicineManage_Initialize              210001  //��ʼ������ҩƷλ��״̬ʧ��
#define ERROR_MedicineManage_PopMedicinLocation      210002  //��ҩʱ�޸�ҩƷλ��״̬ʧ��
#define ERROR_MedicineManage_PushMedicinLocation     210003  //��ҩʱ�޸�ҩƷλ��״̬ʧ��
#define ERROR_MedicineManage_AddressSearch           210004  //����ҩƷ�ĵ�ַ�б�ʧ��
#define ERROR_MedicineManage_GetMedicineCount        210005  //��ȡҩƷ��ǰ��ʣ�������ʧ��
#define ERROR_MedicineManage_FastSearch              210006  //������ٲ���ʧ��
#define ERROR_MedicineManage_ListSearch              210007  //����ҩƷ�б�ʧ��
#define ERROR_MedicineManage_LookMedicine            210008  //����ҩƷ��ϸ��Ϣʧ��
#define ERROR_MedicineManage_ModifyMedicine          210009  //�޸�ҩƷ��ϸ��Ϣʧ��
#define ERROR_MedicineManage_AddMedicine             210010  //����ҩƷ��ϸ��Ϣʧ��
#define ERROR_MedicineManage_DeleteMedicine          210011  //ɾ��ҩƷ��ϸ��Ϣʧ��
#define ERROR_MedicineManage_ClearLocation           210012  //���ҩƷ������ַ��Ϣʧ��
#define ERROR_MedicineManage_ClearAllLocation        210013  //���ҩƷ���е�ַ��Ϣʧ��
#define ERROR_MedicineManage_getMaxmedID             210014  //��ȡҩƷ���ID��ʧ��
#define ERROR_MedicineManage_getmedID                210015  //��ȡҩƷID��ʧ��
#define ERROR_MedicineManage_light_position          210016  //��ȡָʾ�ƺ�ʧ��
#define ERROR_MedicineManage_MedicineOverdue         210017  //��ǰҩƷ�Ѿ�����
#define ERROR_MedicineManage_modifyMedicinLocation   210018  //�޸�ҩƷλ��״̬����
#define ERROR_MedicineManage_QueryMedInfo_Client     210019  //��ѯ�ͻ�������ҩƷ��Ϣʧ��
#define ERROR_MedicineManage_QueryMedleavingPercent  210020  //��ѯҩƷʣ��ٷֱ�ʧ��

/*ȫ��ͳ���෵�ش�������*/
#define ERROR_Statistic_SumLayMedicinCount     220001  //�����ܰ�ҩ����ʧ��
#define ERROR_Statistic_SumPrescriptionCount   220002  //�����ܴ�������ʧ��
#define ERROR_Statistic_SumDeviceRunTime       220003  //�������豸����ʱ������ʧ��
#define ERROR_Statistic_SumLogCount            220004  //�����ܵ�¼����ʧ��
#define ERROR_Statistic_SumPrintCount          220005  //������������ӡ����ʧ��
#define ERROR_Statistic_SumSatoPrintCount      220006  //��������ת��ӡ����ʧ��
#define ERROR_Statistic_SumPackCount           220007  //�����ܰ�װ����ʧ��
#define ERROR_Statistic_AddMedicinSortd        220008  //����ҩƷ����ͳ��ʧ��
#define ERROR_Statistic_AddLocation            220009  //����λ��ͳ��ʧ��
#define ERROR_Statistic_GetMedicinSort         220010  //���ҩƷ����ͳ��ʧ��
#define ERROR_Statistic_GetLocation            220011  //�����ַ����ͳ��ʧ��
#define ERROR_Statistic_Initialize             220012  //��ʼ��ȫ��ͳ������ʧ��
#define ERROR_Statistic_SetBooking             220013  //����Ԥ����ʧ��
#define ERROR_Statistic_GetBooking             220014  //���Ԥ����ʧ��

/*���ϵͳ�෵�ش�������*/
#define ERROR_Monitor_CreatThead        230001  //�����豸����߳�ʧ��
#define ERROE_Monitor_GetCameraState    230002  //���������ʧ��
#define ERROE_Monitor_GetScannerState   230003  //����ɨ����ʧ��
#define ERROE_Monitor_GetPLCState       230004  //����PLCʧ��
#define ERROE_Monitor_GetPrintState     230005  //���Ӵ�ӡ��ʧ��

/*�������뷵�ش�������*/
#define ERROR_Import_Medicine          240001  //��������ҩƷʧ��
#define ERROR_Import_Prescription      240002  //�������봦��ʧ��

/************************************************************************/
/*					������ӡ                                        */
/************************************************************************/
#define ERROR_THERMAL_PRINT_SP_OPEN		300001	//������ӡ���򿪴���ʧ��
#define ERROR_THERMAL_PRINT_SP_RECV		300002	//������ӡ����ȡ����ʧ��
#define ERROR_THERMAL_PRINT_SP_SEND		300003	//������ӡ��д����ʧ��

//�м�
#define ERROR_PRINT_PRE_MED_INFO		300004	//��ӡ����ҩƷ�б�ʧ��
#define ERROR_THERMAL_PRINT_QUERY_EMPTY	300005	//������ӡ����ѯ��ȡ��¼Ϊ��

//Ƭ��
#define ERROR_PRINT_HETEROMORPHISM		300006	//��ӡ����ҩʧ��
#define ERROR_PRINT_COMMON				300007	//��ӡ��ͨȱҩʧ��
#define ERROR_PRINT_MANUAL				300008	//��ӡ�ֹ�ȡҩʧ��
#define ERROR_PRINT_BIG_PRE				300009	//��ӡ����ҽ��ʧ��
#define ERROR_PRINT_MED_INFO			300010	//��ӡҩƷ��Ϣʧ��
#define ERROR_PRINT_LACK_ISAS			300011	//��ӡ���ܴ�ȡȱҩ�б�ʧ��

/*����ܷ��ش�������*/
#define ERROR_LAMP_PORT_OPEN			300001	//������ܴ���ʧ��
#define ERROR_LAMP_PARAM				300002	//��������
#define ERROR_LAMP_VERIFY_SUM			300003	//У��ʹ���
#define ERROR_LAMP_SP_SEND				300004	//���ڷ���ʧ��
#define ERROR_LAMP_SP_RECV				300005	//���ڽ���ʧ��
#define	ERROR_LAMP_SP_SEND_DATA_INCONSISTENCY 300006	//���ڽ��ջ��������ֽ�������Ҫ���յ����ֽ�����һ��
#define	ERROR_LAMP_SP_RECV_DATA_INCONSISTENCY 300007	//���ڽ��ջ��������ֽ�������Ҫ���յ����ֽ�����һ��
#define ERROR_LAMP_SP_RECV_NULL			300008	//��������Ϊ��


/* PLC���ش������� */
#define ERROR_PLC_OPEN_PORT				310000	//��PLCʧ��
#define ERROR_PLC_SWITCH_SCREEN			310001	//�л���Ļʧ��
#define ERROR_PLC_BUZZER				310002	//����������ʧ��
#define ERROR_PLC_TURN_OFF_HARD_SURE_LAMP 310003 //�رա�Ӳȷ�ϡ�ָʾ��ʧ��
#define ERROR_PLC_SEND_RUN_TIER			310004	//�����߲�����ʧ��
#define ERROR_PLC_SEND_TIER_NUM			310005	//���Ͳ������ʧ��
#define ERROR_PLC_LOC_ID_BOUND			310006	//λ��IDԽ��
#define ERROR_PLC_OPEN_FRONT_DOOR		310007	//���Ϳ�ǰ������ʧ��
#define ERROR_PLC_CLOSE_FRONT_DOOR		310008	//���͹�ǰ������ʧ��
#define ERROR_PLC_OPEN_BACK_DOOR		310009	//���Ϳ���������ʧ��
#define ERROR_PLC_CLOSE_BACK_DOOR		310010	//���͹غ�������ʧ��
#define ERROR_PLC_GET_DEVICE_FUNC       310011	//��ȡGetDevice()ʧ��
#define ERROR_PLC_PARAM					310012	//��������

/************************************************************************/
/*                    ����ɨ��                                          */
/************************************************************************/
#define ERROR_BARCODE_SCANNER_SP_OPEN	320000	//����ɨ�����򿪴���ʧ��
#define ERROR_BARCODE_SCANNER_SP_RECV	320001	//����ɨ������ȡ����ʧ��
#define ERROR_BARCODE_SCANNER_SP_SEND	320002	//����ɨ����д����ʧ��
#define ERROR_BARCODE_SCANNER_SET		320003	//����ɨ��������ɨ�跽ʽʧ��

/************************************************************************/
/*                    ������                                          */
/************************************************************************/
#define ERROR_CARD_READER_SP_OPEN	330000	//�������򿪴���ʧ��
#define ERROR_CARD_READER_SP_RECV	330001	//��������ȡ����ʧ��
#define ERROR_CARD_READER_SP_SEND	330002	//������д����ʧ��
#define ERROR_CARD_READER_SP_UNPACK 330003	//��������������ʧ��


//////////////////////////////////////////////////////////////////////////

#endif