/*
tuya_cloud_com_defs.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef TUYA_CLOUD_COM_DEFS_H
#define TUYA_CLOUD_COM_DEFS_H

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

// tuya sdk constants definitions
#define ENCRYPT_KEY_LEN         16      // max string length of storage encrypt
#define GW_UUID_LEN             25      // max string length of UUID
#define AUTH_KEY_LEN            32      // max string length of AUTH_KEY
#define PSK_KEY_LEN             64      // max string length of PSK_KEY
#define PRODUCT_KEY_LEN         16      // max string length of PRODUCT_KEY
#define SW_VER_LEN              10      // max string length of VERSION

#define BT_HID_LEN              19      // max string length of HID
#define BT_MAC_LEN              12      // max string length of MAC

#define TUYA_PSK_LEN            32
#define TUYA_PSK21_LEN          44
#define TUYA_IMEI_LEN           18
#define TUYA_SN_LEN             20

#define SYS_ENV_LEN             20
#define LOG_SEQ_PATH_LEN        32
#define GW_ID_LEN               25      // max string length of GW_ID
#define DEV_UUID_LEN            25      // max string length of DEV UUID
#define DEV_ID_LEN              25      // max string length of DEV_ID
#define SCHEMA_ID_LEN           16      // max string length of SCHEMA
#define SCHEMA_VER_LEN          16      // max string length of SCHEMA VERSION
#define BIND_TOKEN_LEN          16      // max string length of BIND_TOKEN
#define TOKEN_LEN               8       // max string length of TOKEN
#define REGION_LEN              2       // max string length of REGIN IN TOKEN
#define REGIST_KEY_LEN          4       // max string length of REGIST_KEY IN TOKEN
#define HTTP_URL_LMT            128     // max string length of URL
#define MQ_URL_LMT              128     // max string length of MQTT_URL
#define TIME_ZONE_LEN           10      // max string length of TIME_ZONE
#define SUMMER_TM_ZONE_LEN      256     // max string length of SUMMER_TIME_ZONE
#define COUNTRY_CODE_LEN        8       // max string length of COUNTRY_CODE
#define COUNTRY_CODE_LEN        8       // max string length of COUNTRY_CODE
#define IPV4_LEN                15      // max string length of IPV4_LEN 255.255.255.255

#define SEC_KEY_LEN             16      // max string length of SECURITY_KEY
#define LOCAL_KEY_LEN           16      // max string length of LOCAL_KEY

#define WXAPPID_LEN             32      // max string length of wechat_appid
#define WXUUID_LEN              32      // max string length of wechat_uuid

#define GRP_ID_LEN              5       // max string length of group id range:1-65535
#define SCENE_ID_LEN            3       // max string length of scene id range:1-255

#define NET_MODE_LEN            32       // max string length of NET MODE


#define LC_GW_SLEEP_HB_LMT_S 12*3600 // 12h
//group and scene error code
#define M_SCE_SUCCESS 0        //成功
#define M_SCE_ERR_EXCEED 1     //空间不足
#define M_SCE_ERR_TIMEOUT 2    //通讯超时
#define M_SCE_ERR_PARAM 3      //参数超出范围
#define M_SCE_WRITE_FILE 4     //写数据库文件错误
#define M_SCE_ERR_OTHER 5      //其他错误




// Product Function Specification
typedef BYTE_T GW_ABI;
#define GW_VIRTUAL 0x00 // product doesn't have subdevices functions,it only has device function. For example: SOC or MCU
#define GW_ENTITY_ATTH_DEV 0x01 // Product has subdevices functions and it also has device function.
#define GW_ENTITY 0x02 // Product only has subdevices functions.

// Product Network Connection
typedef BYTE_T GW_NW_STAT_T;
#define GNS_UNVALID 0    // product is offline in LAN and WAN
#define GNS_LAN_VALID 1  // product is online in LAN but offline in WAN
#define GNS_WAN_VALID 2  // product is online in LAN and WAN

// Product Extend State
typedef BYTE_T GW_EXT_STAT_T;
#define EXT_UNAUTH 0
#define EXT_PROD_TEST 1
#define EXT_NORMAL_S 2
#define EXT_GW_UPGRD 3
#define EXT_DEV_UPGRD 4
#define EXT_DEV_ADD 5
#define EXT_REG_FAIL 6
#define EXT_NET_FAIL 7
#define EXT_CONFIG_BACK_UP 8
#define EXT_CONFIG_RESTORE 9

//固件类型与云端保持一致
//为了保持API接口一致性，复用原DEV_TYPE_T表示固件类型及对应的OTA通道
typedef BYTE_T DEV_TYPE_T;
#define DEV_NM_ATH_SNGL     0   //主联网模块固件
#define DEV_BLE_SNGL        1   //BLE固件
#define DEV_ZB_SNGL         3   //ZigBee固件
#define DEV_NM_NOT_ATH_SNGL 9   //MCU固件
//使用attach模块固件type时，需和平台上添加的attach固件type一一对应
#define DEV_ATTACH_MOD_1 10
#define DEV_ATTACH_MOD_2 11
#define DEV_ATTACH_MOD_3 12
#define DEV_ATTACH_MOD_4 13
#define DEV_ATTACH_MOD_5 14
#define DEV_ATTACH_MOD_6 15
#define DEV_ATTACH_MOD_7 16
#define DEV_ATTACH_MOD_8 17
#define DEV_ATTACH_MOD_9 18
#define DEV_ATTACH_MOD_10 19

// Device sub type ,can be different type of devices.
typedef BYTE_T DEV_SUB_TYPE_T;
#define NONE_DEV_SUB_TYPE   0
#define BLE_SUB_TP_MESH     0
#define BLE_SUB_TP_SINGLE   1

typedef BYTE_T DEV_ABI_E;
#define ABI_DEV_MESH            0x8000 //bit 15
#define ABI_DEV_SINGLE_BLE      0x400   //bit10

#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
typedef BYTE_T IOT_GW_NET_TYPE_T;
#define IOT_GW_NET_WIRED       0   //只支持有线, only support wried
#define IOT_GW_NET_WIFI        1   //只支持无线, only support wifi
#define IOT_GW_NET_WIRED_WIFI  2   //有线无线都支持, support wired and wifi

typedef BYTE_T CUR_NET_STATUS_T;
#define NET_WIRED 0
#define NET_WIFI  1
#endif

// sub-device detail type
typedef UINT_T USER_DEV_DTL_DEF_T; // user detial type define

typedef BYTE_T GW_PERMIT_DEV_TP_T;
#define GP_DEV_DEF 0xFF //不区分协议类型
#define GP_DEV_ZB DEV_ZB_SNGL
#define GP_DEV_BLE DEV_BLE_SNGL
#define GP_DEV_MCU  DEV_NM_NOT_ATH_SNGL
#define GP_DEV_ATH_1 DEV_ATTACH_MOD_1
#define GP_DEV_ATH_2 DEV_ATTACH_MOD_2
#define GP_DEV_ATH_3 DEV_ATTACH_MOD_3
#define GP_DEV_ATH_4 DEV_ATTACH_MOD_4
#define GP_DEV_ATH_5 DEV_ATTACH_MOD_5
#define GP_DEV_ATH_6 DEV_ATTACH_MOD_6
#define GP_DEV_ATH_7 DEV_ATTACH_MOD_7
#define GP_DEV_ATH_8 DEV_ATTACH_MOD_8
#define GP_DEV_ATH_9 DEV_ATTACH_MOD_9
#define GP_DEV_ATH_10 DEV_ATTACH_MOD_10

#define GP_DEV_SUPPORT_MAX GP_DEV_ATH_10

typedef struct {
    BOOL_T init_db;
    CHAR_T sys_env[SYS_ENV_LEN];
    CHAR_T log_seq_path[LOG_SEQ_PATH_LEN];
}TY_INIT_PARAMS_S;

typedef struct {
    GW_PERMIT_DEV_TP_T tp;
    CHAR_T ver[SW_VER_LEN+1]; // xx.xx.xx
}GW_ATTACH_ATTR_T;

#define CH_NAME_LMT 15
#define CH_CODE_LMT 20
#define CH_SN_LMT 20
#define CH_REPORT_CODE_LMT 20
#define CH_MANU_ID_LMT 10
#define CH_VERSION_LMT 10
#define CH_ENCRYPT_KEY_LMT 20

typedef struct{
    CHAR_T ch_name[CH_NAME_LMT+1];    
    CHAR_T ch_code[CH_CODE_LMT+1];
    CHAR_T ch_sn[CH_SN_LMT+1];
    CHAR_T ch_report_code[CH_REPORT_CODE_LMT+1];
    CHAR_T ch_manu_id[CH_MANU_ID_LMT+1];
    CHAR_T ch_version[CH_VERSION_LMT+1];
    CHAR_T ch_encrype_key[CH_ENCRYPT_KEY_LMT+1];
}CH_CODE_ST;

typedef struct{
    USHORT_T sNodeID;   //short nodeID
    USHORT_T fNodeID;   //father short nodeID
    CHAR_T   rssi;
    UCHAR_T  lqi;
    CHAR_T   fid[DEV_ID_LEN+1];//father node ID str
}DEV_QOS_ST;   // Device Quaility of Service

#define GW_ATTACH_ATTR_LMT 4

// dp report type
typedef BYTE_T DP_REPT_TYPE_E;
#define T_OBJ_REPT      0           // dp is value,str,enum,bool,bitmap
#define T_RAW_REPT      1           // raw type
#define T_STAT_REPT     2           // stat type
#define T_RE_TRANS_REPT 10          // 重发上报，前面数值预留

// dp type
typedef BYTE_T DP_TYPE_E;
#define T_OBJ           0           // dp is value,str,enum,bool,bitmap
#define T_RAW           1           // raw type
#define T_FILE          2           // file type

// dp mode
typedef BYTE_T DP_MODE_E;
#define M_RW            0          // cloud/app can read and write
#define M_WR            1          // cloud/app can only write to device
#define M_RO            2          // cloud/app can only read from device

// dp schema type
typedef BYTE_T DP_PROP_TP_E;
#define PROP_BOOL 0
#define PROP_VALUE 1
#define PROP_STR 2
#define PROP_ENUM 3
#define PROP_BITMAP 4

typedef BYTE_T UP_MAP_TYPE_E;
#define UP_CLEANER_MAP   0
#define UP_CLEANER_PATH  1
#define UP_CLEANER_MAP_PATH 2

typedef BYTE_T UPGRADE_TYPE_T;
#define UPGRADE_TYPE_NORMAL 0
#define UPGRADE_TYPE_SILENT 1

// dp prop
typedef struct {
    INT_T min;
    INT_T max;
    SHORT_T step;
    USHORT_T scale;
    INT_T value;
}DP_PROP_VAL_S;

typedef struct {
    INT_T cnt;
    CHAR_T **pp_enum;
    INT_T value;
}DP_PROP_ENUM_S;

typedef struct {
    INT_T max_len;
    INT_T cur_len;//当前分配的长度
    void* dp_str_mutex;
    CHAR_T *value;
}DP_PROP_STR_S;

typedef struct {
    BOOL_T value;
}DP_BOOL_S;

typedef struct {
    UINT_T max_len;
    UINT_T value;
}DP_PROP_BITMAP;

typedef union {
    DP_PROP_VAL_S prop_value;
    DP_PROP_ENUM_S prop_enum;
    DP_PROP_STR_S prop_str;
    DP_BOOL_S prop_bool;
    DP_PROP_BITMAP prop_bitmap;
}DP_PROP_VALUE_U;

typedef BYTE_T DP_TRIG_T_E;
#define TRIG_PULSE      0   // upload when value changes
#define TRIG_DIRECT     1   // upload without value change check

/* tuya sdk dp positive upload policy */
typedef BYTE_T DP_PSV_E;
#define PSV_FALSE 0    /* disabled */
#define PSV_TRUE 1     /* app hasn't query yet */
#define PSV_F_ONCE 2   /* app has queryed, shoulb report */

// dp statistics type
typedef BYTE_T DP_STAT_TP_T;
#define DST_NONE 0 // no need statistics
#define DST_INC 1 // dp statistics increase
#define DST_TOTAL 2 // dp statistics total

typedef struct {
    BYTE_T dp_id;
    DP_MODE_E mode;
    DP_PSV_E passive;  /* true false once */
    DP_TYPE_E type;   /* obj raw file */
    DP_PROP_TP_E prop_tp;
    DP_TRIG_T_E trig_t;
    DP_STAT_TP_T stat; /* none total increase */
}DP_DESC_IF_S;

/* tuya sdk dp cmd type */
typedef BYTE_T DP_CMD_TYPE_E;
#define DP_CMD_LAN      0       // cmd from LAN
#define DP_CMD_MQ       1       // cmd from MQTT
#define DP_CMD_TIMER    2       // cmd from Local Timer
#define DP_CMD_SCENE_LINKAGE 3  // cmd from scene linkage
#define DP_CMD_RELIABLE_TRANSFER 4 // cmd from reliable transfer
#define DP_CMD_BT       5      // cmd from bt
#define DP_CMD_SCENE_LINKAGE_LAN 6  // cmd from lan scene linkage


typedef BYTE_T DP_TRANS_TYPE_T;
#define DTT_SCT_UNC     0       // unicast
#define DTT_SCT_BNC     1       // boardcast
#define DTT_SCT_MNC     2       // multicast
#define DTT_SCT_SCENE   3       // scene

/* tuya sdk dp value union */
typedef union {
    INT_T dp_value;             // valid when dp type is value
    UINT_T dp_enum;             // valid when dp type is enum
    CHAR_T *dp_str;             // valid when dp type is str
    BOOL_T dp_bool;             // valid when dp type is bool
    UINT_T dp_bitmap;           // valid when dp type is bitmap
}TY_OBJ_DP_VALUE_U;

/* tuya sdk obj dp manage info */
typedef struct {
    BYTE_T dpid;                // dp id
    DP_PROP_TP_E type;          // dp type
    TY_OBJ_DP_VALUE_U value;    // dp value
    UINT_T time_stamp;          // dp happen time. if 0, mean now
}TY_OBJ_DP_S;

typedef struct {
    DP_CMD_TYPE_E cmd_tp;
    DP_TRANS_TYPE_T dtt_tp;
    CHAR_T *cid; // if(NULL == cid) then then the cid represents gwid
    CHAR_T *mb_id;
    UINT_T dps_cnt;
    TY_OBJ_DP_S dps[0];
}TY_RECV_OBJ_DP_S;

typedef struct {
    DP_CMD_TYPE_E cmd_tp;
    DP_TRANS_TYPE_T dtt_tp;
    CHAR_T *cid; // if(NULL == cid) then then the cid represents gwid
    BYTE_T dpid;
    CHAR_T *mb_id;
    UINT_T len;
    BYTE_T data[0];
}TY_RECV_RAW_DP_S;

typedef struct {
    CHAR_T *cid; // if(NULL == cid) then then the cid represents gwid
    UINT_T cnt; // dpid cnt if(0 == cnt) then query all object dp
    BYTE_T dpid[0]; // dpid
}TY_DP_QUERY_S;

//add by sunkz
#define DP_REPT_USER_REG_MAX_NUM  5
typedef struct{
    DP_REPT_TYPE_E  dp_rept_type;
    VOID_T*         dp_data;//obj类型为TY_OBJ_DP_REPT_S，stat类型dp为TY_STAT_DP_REPT_S，RAW类型dp为TY_RAW_DP_REPT_S
    CHAR_T*         dp_data_json;//DP上报后处理数据，为CHAR_T*类型(cjson格式)
    BOOL_T          is_query;
}DP_REPT_CB_DATA;

//DP上报前处理
typedef OPERATE_RET (*DP_REPT_PRE_HANDLE)(IN CONST DP_REPT_CB_DATA* dp_data);
//DP上报后处理
typedef OPERATE_RET (*DP_REPT_POST_HANDLE)(IN CONST OPERATE_RET dp_rslt, IN CONST DP_REPT_CB_DATA* dp_data);
//用户DP处理注册接口
typedef struct{
    DP_REPT_PRE_HANDLE  dp_rept_pre_cb;
    DP_REPT_POST_HANDLE dp_rept_post_cb;
    BOOL_T need_dp_force;
}DP_REPT_HADLE_CB_S;
//add by sunkz

#define FW_URL_LEN      255       // max length of firmware download url
#define FW_MD5_LEN      32        // max length of firmware md5
#define FW_HMAC_LEN     64        // max length of firmware hmac

/* tuya sdk ota firmware info */
typedef struct {
    DEV_TYPE_T tp;      // firmware type
    UPGRADE_TYPE_T type;
    CHAR_T fw_url[FW_URL_LEN+1];  // firmware download url
    CHAR_T sw_ver[SW_VER_LEN+1];  // firmware version
    UINT_T file_size;             // firmware size in BYTE
    CHAR_T fw_hmac[FW_HMAC_LEN+1];  // firmware hmac
    BOOL_T diff_ota;
}FW_UG_S;

typedef enum {
    GWDEV_DELTYPE_MQTT = 0,
    GWDEV_DELTYPE_SYNC = 1,
}GW_DELDEV_TYPE;





/***********************************************************
*  callback function: DEV_HEARTBEAT_SEND_CB
*  Desc:    send heartbeat to subdevice callback
*  Input:   dev_id: subdevice id
***********************************************************/
typedef VOID (*DEV_HEARTBEAT_SEND_CB)(IN CONST CHAR_T *dev_id);

/* tuya sdk gateway status info */
typedef BYTE_T GW_STATUS_E;
#define GW_RESET            0 // gw reset
#define GW_ACTIVED          1 // gw actived
#define GW_FIRST_START      2 // start tuya-sdk in the first time.
#define GW_NORMAL           3 // tuya-sdk is actived and started

/***********************************************************
*  callback function: GW_STATUS_CHANGED_CB
*  Desc:    gw status changed callback
*  Input:   status: current status
***********************************************************/
typedef VOID (*GW_STATUS_CHANGED_CB)(IN CONST GW_STATUS_E status);

/***********************************************************
*  callback function: DEV_OBJ_DP_CMD_CB
*  Desc:    obj dp info cmd callback
*  Input:   dp: obj dp info
***********************************************************/
typedef VOID (*DEV_OBJ_DP_CMD_CB)(IN CONST TY_RECV_OBJ_DP_S *dp);

/***********************************************************
*  callback function: DEV_RAW_DP_CMD_CB
*  Desc:    raw dp info cmd callback
*  Input:   dp: raw dp info
***********************************************************/
typedef VOID (*DEV_RAW_DP_CMD_CB)(IN CONST TY_RECV_RAW_DP_S *dp);

/***********************************************************
*  callback function: DEV_DP_QUERY_CB
*  Desc:    dp info query callback
*  Input:   dp_qry: query info
***********************************************************/
typedef VOID (*DEV_DP_QUERY_CB)(IN CONST TY_DP_QUERY_S *dp_qry);

/***********************************************************
*  callback function: GW_UG_INFORM_CB
*  Desc:    gateway ota firmware available nofity callback.
*  Input:   fw: firmware info
***********************************************************/
typedef int (*GW_UG_INFORM_CB)(IN CONST FW_UG_S *fw);


/* tuya sdk gateway reset type */
typedef enum {
    GW_LOCAL_RESET_FACTORY = 0,
    GW_REMOTE_UNACTIVE,
    GW_LOCAL_UNACTIVE,
    GW_REMOTE_RESET_FACTORY,
    GW_RESET_DATA_FACTORY, //need clear local data when active
}GW_RESET_TYPE_E;
/***********************************************************
*  callback function: GW_RESET_IFM_CB
*  Desc:    gateway restart callback
***********************************************************/
typedef VOID (*GW_RESET_IFM_CB)(GW_RESET_TYPE_E type);

/***********************************************************
*  callback function: DEV_UG_INFORM_CB
*  Desc:    subdevice ota firmware available nofity callback.
*  Input:   dev_id: subdevice id
*  Input:   fw: firmware info
***********************************************************/
typedef int (*DEV_UG_INFORM_CB)(IN CONST CHAR_T *dev_id,IN CONST FW_UG_S *fw);

typedef enum {
    DEV_REMOTE_RESET_FACTORY,
    DEV_RESET_DATA_FACTORY, //need clear local data when bind
}DEV_RESET_TYPE_E;
/***********************************************************
*  callback function: DEV_RESET_IFM_CB
*  Desc:    subdevice reset callback
***********************************************************/
typedef VOID (*DEV_RESET_IFM_CB)(IN CONST CHAR_T *dev_id,IN DEV_RESET_TYPE_E type);

#if defined(TUYA_GW_OPERATOR) && (TUYA_GW_OPERATOR==1)
/***********************************************************
*  callback function: OPE_HTTPC_GET_CHCODE_CB
*  Desc:    subdevice reset callback
***********************************************************/
typedef OPERATE_RET (*OPE_HTTPC_GET_CHCODE_CB)(IN CONST BOOL_T is_gw, IN CONST CHAR_T *devid, INOUT CH_CODE_ST *ch_code);
#endif



/***********************************************************
*  callback function: GET_FILE_DATA_CB
*  Desc:    firmware download content process callback.
*  Input:   fw: firmware info
*  Input:   total_len: firmware total size
*  Input:   offset: offset of this download package
*  Input:   data && len: this download package
*  Output:  remain_len: the size left to process in next cb.
*  Input:   pri_data: private data
*  Return:  OPRT_OK: success  Other: fail
***********************************************************/
typedef OPERATE_RET (*GET_FILE_DATA_CB)(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len,IN CONST UINT_T offset,\
                                                IN CONST BYTE_T *data,IN CONST UINT_T len,OUT UINT_T *remain_len, IN PVOID_T pri_data);

/***********************************************************
*  callback function: ACTIVE_SHORTURL_CB
*  Desc:    device active short url.
***********************************************************/
typedef VOID (*ACTIVE_SHORTURL_CB)(OUT CONST CHAR_T *shorturl);


/***********************************************************
*  callback function: UPGRADE_NOTIFY_CB
*  Desc:    firmware download finish result callback.
*  Input:   fw: firmware info
*  Input:   download_result: 0 means download succes. other means fail
*  Input:   pri_data: private data
***********************************************************/
typedef VOID (*UPGRADE_NOTIFY_CB)(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data);


/* tuya-sdk gateway callback functions */
typedef struct {
    GW_STATUS_CHANGED_CB gw_status_cb;
    GW_UG_INFORM_CB gw_ug_cb;
    GW_RESET_IFM_CB gw_reset_cb;
    DEV_OBJ_DP_CMD_CB dev_obj_dp_cb;
    DEV_RAW_DP_CMD_CB dev_raw_dp_cb;
    DEV_DP_QUERY_CB dev_dp_query_cb;
    DEV_UG_INFORM_CB dev_ug_cb;
    DEV_RESET_IFM_CB dev_reset_cb;
#if defined(TUYA_GW_OPERATOR) && (TUYA_GW_OPERATOR==1)
    OPE_HTTPC_GET_CHCODE_CB ope_get_chcode_cb;
#endif

#if defined(QRCODE_ACTIVE_MODE) && (QRCODE_ACTIVE_MODE==1)
    ACTIVE_SHORTURL_CB active_shorturl;
#endif
    GW_UG_INFORM_CB pre_gw_ug_cb;
    DEV_UG_INFORM_CB pre_dev_ug_cb;
}TY_IOT_CBS_S;


#if (ENABLE_CLOUD_STREAM == 1)
typedef OPERATE_RET (*GW_CLOUD_STREAM_MQTT_CB)(IN struct ty_cJSON *root_json);
#endif


/* tuya-sdk gateway subdevices management callback functions */


// mqtt media data interface
typedef BYTE_T TRAN_STEP_T;
#define TS_START 0
#define TS_TRANSFER 1
#define TS_END 2

typedef BYTE_T CLEAR_TYPE_T;
#define CLEAR_CONTINUE_ADD 0
#define CLEAR_REMOVE_MAP   1

typedef BYTE_T DATA_TYPE_T;
#define DATA_INCREMENT_MAP 0
#define DATA_COMPLETE_MAP  1	

#pragma pack(1)
typedef struct {
    USHORT_T id;
    UINT_T posix;
    TRAN_STEP_T step;
    UINT_T offset;
    USHORT_T len;
    BYTE_T data[0];
}FLOW_BODY_ST;

typedef struct {
    USHORT_T id;
	BYTE_T map_id;
	BYTE_T clear_type;
	BYTE_T data_type;		
    UINT_T posix;
    UINT_T offset;
    USHORT_T len;
    BYTE_T data[0];
}FLOW_BODY_V2_ST;

#pragma pack()


typedef struct {
    CHAR_T region[REGION_LEN + 1];
    CHAR_T time_zone[TIME_ZONE_LEN+1];
}TY_IOT_REGION_INFO_S;


typedef enum {
    CFG_DEVICE = 1,
    CFG_PRODUCT = 2,
    CFG_INVALID,
}TY_CUSTOM_CFG_E;


typedef struct {
    CHAR_T country_code[COUNTRY_CODE_LEN + 1];
    CHAR_T ip[IPV4_LEN + 1];
}TY_LOCATION_INFO_S;


typedef VOID (*GW_APP_LOG_PATH_CB)(OUT CHAR_T *path, IN CONST INT_T len);

typedef struct {
    GW_APP_LOG_PATH_CB gw_app_log_path_cb;
}TY_IOT_APP_CBS_S;


#ifdef __cplusplus
}
#endif

#endif
