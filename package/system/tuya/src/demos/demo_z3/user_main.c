#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "tuya_gw_infra_api.h"
#include "tuya_gw_misc_api.h"
#include "tuya_gw_z3_api.h"
#include "tuya_gw_dp_api.h"

/* must apply for uuid, authkey and product key from tuya iot develop platform */
#define UUID         "tuya461dbc63aeeb991f"
#define AUTHKEY      "c8X4PR4wx1gMFaQlaZu5dfgVvVRwB8Ug"
#define PRODUCT_KEY  "fljmamufiym5fktz"

#define Z3_PROFILE_ID_HA           0x0104

#define ZCL_BASIC_CLUSTER_ID       0x0000
#define ZCL_ON_OFF_CLUSTER_ID      0x0006

#define Z3_CMD_TYPE_GLOBAL         0x00
#define Z3_CMD_TYPE_PRIVATE        0x01

#define ZCL_OFF_COMMAND_ID         0x00
#define ZCL_ON_COMMAND_ID          0x01

#define ZCL_READ_ATTRIBUTES_COMMAND_ID      0x00
#define READ_ATTRIBUTES_RESPONSE_COMMAND_ID 0x01
#define REPORT_ATTRIBUTES_COMMAND_ID        0x0A
#define READ_ATTRIBUTER_RESPONSE_HEADER     3 /* Attributer ID: 2 Bytes, Status: 1 Btye */
#define REPORT_ATTRIBUTES_HEADER            2 /* Attributer ID: 2 Bytes */

TY_Z3_DEV_S my_z3_dev[] = {
    { "TUYATEC-nPGIPl5D", "TS0001" },
    { "TUYATEC-1xAC7DHQ", "TZ3000" },
    /* { "...", "..." } */
};

TY_Z3_DEVLIST_S my_z3_devlist = {
    .devs = my_z3_dev,
    .dev_num = CNTSOF(my_z3_dev),
};

static void usage(void)
{
    printf("usage:\r\n");
    printf("  u: unactive gw unittest\r\n");
    printf("  p: permit device join unittest\r\n");
    printf("  s: network scan unittest\r\n");
    printf("  g: get radio channel unittest\r\n");
    printf("  q: exit \r\n");
    printf("\r\n"); 
}

static void test_switch_onoff(const char *id, uint8_t dst_ep, int on)
{
    int ret = 0;
    TY_Z3_APS_FRAME_S frame = {0};

    memset(&frame, 0x00, sizeof(TY_Z3_APS_FRAME_S));
    log_debug("test_switch_onoff");
    strncpy(frame.id, id, sizeof(frame.id));
    frame.profile_id = Z3_PROFILE_ID_HA;
    frame.cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame.cmd_type = Z3_CMD_TYPE_PRIVATE;
    frame.src_endpoint = 0x01;
    frame.dst_endpoint = dst_ep;

    frame.msg_length = 0;

    if (on)
        frame.cmd_id = ZCL_ON_COMMAND_ID;
    else 
        frame.cmd_id = ZCL_OFF_COMMAND_ID;

    ret = tuya_user_iot_zig_send(&frame);
    if (ret != 0) {
        log_err("tuya_user_iot_zig_send err: %d", ret);
        return;
    }

    return ;
}

static void __test_switch_read_attr(const char *dev_id)
{
    OPERATE_RET op_ret = OPRT_OK;
    TY_Z3_APS_FRAME_S frame = {0};
    USHORT_T atrr_buf[5] = { 0x0000, 0x4001, 0x4002, 0x8001, 0x5000 };
    log_debug("__test_switch_read_attr");
    memset(&frame, 0x00, sizeof(TY_Z3_APS_FRAME_S));

    strncpy(frame.id, dev_id, SIZEOF(frame.id));
    frame.profile_id = Z3_PROFILE_ID_HA;
    frame.cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame.cmd_type = Z3_CMD_TYPE_GLOBAL;
    frame.src_endpoint = 0x01;
    frame.dst_endpoint = 0xff; // all endpoints
    frame.cmd_id = ZCL_READ_ATTRIBUTES_COMMAND_ID;

    frame.msg_length = SIZEOF(atrr_buf);
    frame.message = (UCHAR_T *)atrr_buf;

    op_ret = tuya_user_iot_zig_send(&frame);
    if (op_ret != OPRT_OK) {
        log_err("tuya_user_iot_zig_send err: %d", op_ret);
        return;
    }
}

static void __test_switch_read_ver(const char *dev_id)
{
    OPERATE_RET op_ret = OPRT_OK;
    TY_Z3_APS_FRAME_S frame = {0};
    USHORT_T atrr_buf[1] = { 0x0001 };
    log_debug("__test_switch_read_ver");
    strncpy(frame.id, dev_id, SIZEOF(frame.id));
    frame.profile_id = Z3_PROFILE_ID_HA;
    frame.cluster_id = ZCL_BASIC_CLUSTER_ID;
    frame.cmd_type = Z3_CMD_TYPE_GLOBAL;
    frame.src_endpoint = 0x01;
    frame.dst_endpoint = 0x01;
    frame.cmd_id = ZCL_READ_ATTRIBUTES_COMMAND_ID;

    frame.msg_length = SIZEOF(atrr_buf);
    frame.message = (UCHAR_T *)atrr_buf;

    op_ret = tuya_user_iot_zig_send(&frame);
    if (op_ret != OPRT_OK) {
        log_err("tuya_user_iot_zig_send err: %d", op_ret);
        return;
    }
}

static int _iot_get_uuid_authkey_cb(char *uuid, int uuid_size, char *authkey, int authkey_size)
{
    strncpy(uuid, UUID, uuid_size);
    strncpy(authkey, AUTHKEY, authkey_size);

    return 0;
}

static int _iot_get_product_key_cb(char *pk, int pk_size)
{
    strncpy(pk, PRODUCT_KEY, pk_size);

    return 0;
}

static int _gw_active_status_changed_cb(ty_gw_status_t status)
{
    log_debug("active status changed, status: %d", status);

    return 0;
}

static int _gw_online_status_changed_cb(bool online)
{
    log_debug("online status changed, registerd: %d, online: %d", online);

    return 0;
}

static int _gw_fetch_local_log_cb(char *path, int path_len)
{
    char cmd[256] = {0};

    log_debug("gw fetch local log callback");
    /* USER TODO */

    /* snprintf(path, path_len, "/tmp/tuya.log"); */

    return 0;
}

static void _gw_reboot_cb(void)
{
    log_debug("gw reboot callback");
    /* USER TODO */

    exit(0);

    return;
}

static void _gw_reset_cb(void)
{
    log_debug("gw reset callback");
    /* USER TODO */

    return;
}

static void _gw_upgrade_cb(const char *img)
{
    log_debug("gw upgrade callback");
    /* USER TODO */

    return;
}

static int _dev_obj_cmd_cb(const ty_obj_cmd_s *dp)
{
    int i = 0;
    DEV_DESC_IF_S *dev_if = NULL;

    log_debug("device obj cmd callback");
    log_debug("cmd_tp: %d, dtt_tp: %d, dps_cnt: %u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);

    for (i = 0; i < dp->dps_cnt; i++) {
        log_debug("dpid: %d", dp->dps[i].dpid);
        switch (dp->dps[i].type) {
        case DP_TYPE_BOOL:
            log_debug("dp_bool value: %d", dp->dps[i].value.dp_bool);
            break;
        case DP_TYPE_VALUE:
            log_debug("dp_value value: %d", dp->dps[i].value.dp_value);
            break;
        case DP_TYPE_ENUM:
            log_debug("dp_enum value: %d", dp->dps[i].value.dp_enum);
            break;
        case DP_TYPE_STR:
            log_debug("dp_str value: %s", dp->dps[i].value.dp_str);
            break;
        }
    }
    /* USER TODO */

    if (dp->cid != NULL) {
        dev_if = tuya_user_iot_misc_dev_desc_get(dp->cid);
        if (dev_if == NULL) {
            log_err("dev_id %s is not found");
            return -1;
        }

        if (dev_if->tp == DEV_TP_ZIGBEE) {
            /* test on smart switch */
            test_switch_onoff(dp->cid, 0x01, dp->dps[0].value.dp_bool);
        } else {
            // other device protocol
            // USER TODO
        }
	} else {
        // dp for gateway
        // USER TODO
    }

    return 0; 
}

static int _dev_raw_cmd_cb(const ty_raw_cmd_s *dp)
{
    int i = 0;

    log_debug("device raw cmd callback");
    log_debug("cmd_tp: %d, dtt_tp: %d, dpid: %d, len: %u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);

    log_debug("data: ");
    for (i = 0; i < dp->len; i++)
        printf("%02x ", dp->data[i]);

    printf("\n");
    /* USER TODO */

    return 0;
}

static void _misc_dev_add_cb(bool permit, uint32_t timeout)
{
    log_debug("misc device add callback");
    log_debug("permit: %d, timeout: %d", permit, timeout);
    /* USER TODO */

    return;
}

static void _misc_dev_del_cb(const char *dev_id)
{
    int ret = 0;
    DEV_DESC_IF_S *dev_if = NULL;

    log_debug("misc device del callback");
    log_debug("dev_id: %s", dev_id);

    dev_if = tuya_user_iot_misc_dev_desc_get(dev_id);
    if (dev_if == NULL) {
        log_err("dev_id %s is not found");
        return;
    }

    if (dev_if->tp == DEV_TP_ZIGBEE) {
        ret = tuya_user_iot_zig_del(dev_id);
        if (ret != 0) {
            log_err("tuya_user_iot_zig_del err: %d", ret);
            return;
        }
    } else {
        // other device protocol
        // USER TODO
    }

    return;
}

static void _misc_dev_bind_ifm_cb(const char *dev_id, int result)
{
    int ret = 0;
    DEV_DESC_IF_S *dev_if = NULL;

    log_debug("misc device bind inform callback");
    log_debug("dev_id: %s, result: %d", dev_id, result);

    if (result != 0) {
        log_err("bind dev to gateway error");
        return;
    }

    dev_if = tuya_user_iot_misc_dev_desc_get(dev_id);
    if (dev_if == NULL) {
        log_err("dev_id %s is not found");
        return;
    }

    if (dev_if->tp == DEV_TP_ZIGBEE) {
        ret = tuya_user_iot_misc_dev_hb_cfg(dev_id, 120, 3, false);
        if (ret != 0) {
            log_err("tuya_user_iot_misc_dev_hb_fresh err: %d", ret);
            return;
        }

        ret = tuya_user_iot_misc_dev_hb_fresh(dev_id);
        if (ret != 0) {
            log_err("tuya_user_iot_misc_dev_hb_fresh err: %d", ret);
            return;
        }

        __test_switch_read_attr(dev_id);
    } else {
        // other device protocol
        // USER TODO
    }

    return;
}

static void _misc_dev_upgrade_cb(const char *dev_id, const char *img)
{
    DEV_DESC_IF_S *dev_if = NULL;

    log_debug("misc device upgrade callback");
    log_debug("dev_id: %s, image: %s", dev_id, img);

    dev_if = tuya_user_iot_misc_dev_desc_get(dev_id);
    if (dev_if == NULL) {
        log_err("dev_id %s is not found");
        return;
    }

    if (dev_if->tp == DEV_TP_ZIGBEE) {
        tuya_user_iot_zig_upgrade(dev_id, img);
    } else {
        // other device protocol
        // USER TODO
    }

    return;
}

static void _misc_dev_reset_cb(const char *dev_id)
{
    int ret = 0;
    DEV_DESC_IF_S *dev_if = NULL;

    log_debug("misc device reset callback");
    log_debug("dev_id: %s", dev_id);

    dev_if = tuya_user_iot_misc_dev_desc_get(dev_id);
    if (dev_if == NULL) {
        log_err("dev_id %s is not found");
        return;
    }

    if (dev_if->tp == DEV_TP_ZIGBEE) {
        ret = tuya_user_iot_zig_del(dev_id);
        if (ret != 0) {
            log_err("tuya_user_iot_zig_del err: %d", ret);
            return;
        }
    } else {
        // other device protocol
        // USER TODO
    }

    return;
}

static void _misc_dev_hb_cb(const char *dev_id)
{
    DEV_DESC_IF_S *dev_if = NULL;
     log_debug("_misc_dev_hb_cb");
    dev_if = tuya_user_iot_misc_dev_desc_get(dev_id);
    if (dev_if == NULL) {
        return;
    }

    if (dev_if->tp == DEV_TP_ZIGBEE) {
        __test_switch_read_ver(dev_id);
    } else {
        // other device protocol
        // USER TODO
    }
}

static void __my_z3_dev_join(TY_Z3_DESC_S *dev)
{
    OPERATE_RET op_ret = OPRT_OK;
    CONST CHAR_T *pid = "nPGIPl5D";
    CONST CHAR_T *ver = "1.0.7";
    USER_DEV_DTL_DEF_T uddd = 0x8000200;

    if (dev == NULL) {
        log_err("invalid param");
        return;
    }

    log_debug("device join callback, dev_id: %s", dev->id);
    log_debug("     node_id: 0x%04x", dev->node_id);
    log_debug("   manu_name: %s", dev->manu_name);
    log_debug("    model_id: %s", dev->model_id);

    op_ret = tuya_user_iot_misc_dev_bind(DEV_TP_ZIGBEE, uddd, dev->id, pid, ver);
    if (op_ret != OPRT_OK) {
        log_err("tuya_user_iot_dev_bind err: %d", op_ret);
        return;
    }
}

static void __my_z3_dev_leave(CONST CHAR_T *dev_id)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (dev_id == NULL) {
        log_err("invalid param");
        return;
    }

    log_debug("device leave callback, dev_id: %s", dev_id);

    op_ret = tuya_user_iot_misc_dev_unbind(dev_id);
    if (op_ret != OPRT_OK) {
        log_err("tuya_user_iot_dev_unbind err: %d", op_ret);
        return;
    }
}

static void __my_z3_dev_report(TY_Z3_APS_FRAME_S *frame)
{
    INT_T i = 0;
    TY_OBJ_DP_S dp_data = {0};
    OPERATE_RET op_ret = OPRT_OK;

    if (frame == NULL) {
        log_err("invalid param");
        return;
    }

    log_debug("device zcl report callback, dev_id: %s", frame->id);
    log_debug("profile_id: 0x%04x", frame->profile_id);
    log_debug("cluster_id: 0x%04x", frame->cluster_id);
    log_debug("   node_id: 0x%04x", frame->node_id);
    log_debug("    src_ep: %d", frame->src_endpoint);
    log_debug("    dst_ep: %d", frame->dst_endpoint);
    log_debug("  group_id: %d", frame->group_id);
    log_debug("  cmd_type: %d", frame->cmd_type);
    log_debug("   command: 0x%02x", frame->cmd_id);
    log_debug("frame_type: %d", frame->frame_type);
    log_debug("   msg_len: %d", frame->msg_length);
    log_debug("       msg: ");
    for (i = 0; i < frame->msg_length; i++)
        printf("%02x ", frame->message[i]);
    
    printf("\n");

    op_ret = tuya_user_iot_misc_dev_hb_fresh(frame->id);
    if (op_ret != OPRT_OK) {
        log_warn("tuya_user_iot_misc_dev_hb_fresh err: %d", op_ret);
    }

    dp_data.dpid = 0x01;
    dp_data.type = PROP_BOOL;

    // only test one switch in here
    if ((frame->profile_id == Z3_PROFILE_ID_HA) && \
        (frame->cluster_id == ZCL_ON_OFF_CLUSTER_ID)) {
        if (frame->cmd_id == REPORT_ATTRIBUTES_COMMAND_ID) {
            dp_data.value.dp_bool = frame->message[REPORT_ATTRIBUTES_HEADER+1];
        } else if (frame->cmd_id == READ_ATTRIBUTES_RESPONSE_COMMAND_ID) {
            dp_data.value.dp_bool = frame->message[READ_ATTRIBUTER_RESPONSE_HEADER+1];
        } else {
            return;
        }

        op_ret = tuya_user_iot_report_obj_dp(frame->id, &dp_data, 1);
        if (op_ret != OPRT_OK) {
            log_err("tuya_user_iot_report_obj_dp err: %d", op_ret);
            return;
        }
    }
}

static void __my_z3_dev_notify(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    TY_Z3_APS_FRAME_S frame = {0};
    DEV_DESC_IF_S *dev_if = NULL;
    VOID *iter = NULL;

    log_debug("dev notify");

    dev_if = tuya_user_iot_misc_dev_traversal(&iter);
    while (dev_if) {
        if (dev_if->tp == DEV_TP_ZIGBEE) {
            __test_switch_read_attr(dev_if->id);

            op_ret = tuya_user_iot_misc_dev_hb_cfg(dev_if->id, 120, 3, FALSE);
            if (op_ret != OPRT_OK) {
                log_warn("tuya_user_iot_misc_dev_hb_cfg err: %d", op_ret);
            }
        }

        dev_if = tuya_user_iot_misc_dev_traversal(&iter);
    }
}

static void __my_z3_dev_upgrade_end(CONST CHAR_T *dev_id, INT_T rc, UCHAR_T version)
{
    log_debug("dev upgrade end, dev_id: %s, result: %d, version: %d", dev_id, rc, version);

    // update new version
    tuya_user_iot_misc_dev_ver_update(dev_id, "8.8.8");

    return;
}

static void __z3_network_scan_result_cb(TY_Z3_ACTIVE_SCAN_RESULT_S *active_results, UCHAR_T active_num, \
                                        TY_Z3_ENERGY_SCAN_RESULT_S *energy_results, UCHAR_T energy_num)
{
    int i = 0;

    if ((active_results == NULL) && (energy_results == NULL)) {
        log_err("network scan error");
        return;
    }

    if (active_results != NULL) {
        log_debug("active scan results");
        for (i = 0; i < active_num; i++) {
            log_debug("panid: 0x%04x, channel: 0x%02x, rssi: %d, lqi: %d", \
                      active_results[i].panid, \
                      active_results[i].channel, \
                      active_results[i].rssi, \
                      active_results[i].lqi);
        }
    }

    if (energy_results != NULL) {
        log_debug("energy scan results");
        for (i = 0; i < energy_num; i++) {
            log_debug("channel: 0x%02x, rssi: %d", energy_results[i].channel, energy_results[i].rssi);
        }
    }

    return;
}

static void test_tuya_user_iot_unactive_gw(void)
{
    tuya_user_iot_unactive_gw();

    return;
}

static void test_tuya_user_iot_permit_join(void)
{
    static bool permit = false;
    int ret = 0;

    permit ^= 1;

    ret = tuya_user_iot_permit_join(permit, 256);
    if (ret != 0) {
        log_err("tuya_user_iot_permit_join error, ret: %d", ret);
        return;
    }

    return;
}

static void test_tuya_user_iot_network_scan(void)
{
    int ret = 0;
    
    ret = tuya_user_iot_zig_network_scan(0x07FFF800UL, __z3_network_scan_result_cb);
    if (ret != 0) {
        log_err("tuya_user_iot_zig_network_scan error, ret: %d", ret);
    }

    return;
}

static void test_tuya_user_iot_channel_get(void)
{
    int ret = 0;
    char channel = 0;

    ret = tuya_user_iot_zig_channel_get(&channel);
    if (ret != 0) {
        log_err("tuya_user_iot_zig_channel_get error, ret: %d", ret);
        return;
    }
    log_debug("channel: %d", channel);

    return;
}
static void my_gateway_device_init(void) {
    system("uci set wireless.@wifi-device[0].disabled=0");
    system("uci set network.wwan=interface");
    system("uci set network.wwan.proto=dhcp");
    system("uci set wireless.radio0.channel=11");
    system("uci set wireless.@wifi-iface[0].network=wwan");
    system("uci set wireless.@wifi-iface[0].mode=ap");
    system("uci set wireless.@wifi-iface[0].ssid=SampleAP");
    system("uci set wireless.@wifi-iface[0].encryption=psk2");
    system("uci set wireless.@wifi-iface[0].key=12345678");
    system("uci commit wireless");
    system("uci set network.lan.ipaddr=192.168.10.1");
    system("uci set network.lan.gateway=192.168.3.1");
    system("uci set network.lan.dns=8.8.8.8");
    system("uci commit network");
    system("uci set dhcp.lan.ignore=0");
    system("uci set dhcp.lan.ra_management=1");
    system("uci commit network");
    system("uci commit dhcp");
    system("uci commit wireless");
    system("wifi down");
    system("wifi up");
}
int main(int argc, char **argv)
{
    int ret = 0;
    bool debug = true;
    char line[256] = {0};

    FILE* fp=NULL;
    char buf[100] = {0};    
    char str1[100];
    char *q = NULL;
    my_gateway_device_init();
    system("mkdir tuya");
    ty_gw_attr_s gw_attr = {
        .storage_path = "./tuya",
        .cache_path   = "/tmp/",
        .tty_device   = "/dev/ttyS1",
        .tty_baudrate = 57600,
        .ssid = "SampleAP",
        .password = "12345678",
        .wifi_mode =  TY_CONN_MODE_EZ_ONLY,
        .ver          = "1.0.0",
        .log_level    = TY_LOG_DEBUG
    };

    ty_gw_infra_cbs_s gw_infra_cbs = {
        .get_uuid_authkey_cb = _iot_get_uuid_authkey_cb,
        .get_product_key_cb = _iot_get_product_key_cb,
        .gw_fetch_local_log_cb = _gw_fetch_local_log_cb,
        .gw_reboot_cb = _gw_reboot_cb,
        .gw_reset_cb = _gw_reset_cb,
        .gw_upgrade_cb = _gw_upgrade_cb,
        .gw_active_status_changed_cb = _gw_active_status_changed_cb,
        .gw_online_status_changed_cb = _gw_online_status_changed_cb,
    };

    ty_dev_cmd_cbs_s dev_cmd_cbs = {
        .dev_obj_cmd_cb = _dev_obj_cmd_cb,
        .dev_raw_cmd_cb = _dev_raw_cmd_cb,
    };
 
     ty_misc_dev_cbs_s misc_dev_cbs = {
        .misc_dev_add_cb       = _misc_dev_add_cb,
        .misc_dev_del_cb       = _misc_dev_del_cb,
        .misc_dev_bind_ifm_cb  = _misc_dev_bind_ifm_cb,
        .misc_dev_upgrade_cb   = _misc_dev_upgrade_cb,
        .misc_dev_reset_cb     = _misc_dev_reset_cb,
        .misc_dev_heartbeat_cb = _misc_dev_hb_cb,
    };

    TY_Z3_DEV_CBS_S z3_dev_cbs = {
        .join        = __my_z3_dev_join,
        .leave       = __my_z3_dev_leave,
        .report      = __my_z3_dev_report,
        .notify      = __my_z3_dev_notify,
        .upgrade_end = __my_z3_dev_upgrade_end,
    };

    tuya_user_iot_pre_init();

    ret = tuya_user_iot_reg_dev_cmd_cb(&dev_cmd_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_reg_dev_cmd_cb failed, ret: %d", ret);
        return ret;
    }

    ret = tuya_user_iot_reg_misc_dev_cb(&misc_dev_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_reg_misc_dev_cb failed, ret: %d", ret);
        return ret;
    }

    ret = tuya_user_iot_zig_mgr_register(&my_z3_devlist, &z3_dev_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_zig_mgr_register err: %d", ret);
        return ret;
    }

    ret = tuya_user_iot_init(&gw_attr, &gw_infra_cbs);
    if (ret != 0) {
        log_err("tuya_user_iot_init failed");
        return ret;
    }

    while (1) {
        // if(access("/tmp/button",F_OK)==0)
        // {
        //     if((fp = popen("cat /tmp/button | grep button:", "r")) != NULL)
        //     {
        //         printf("popen command success\n");
        //         fgets(buf, sizeof(buf), fp);
        //         {
        //             q = strstr(buf, "button:");
        //             if (q != NULL)
        //             {
        //                 sscanf(q, "button:%[^\"]", str1);
        //                 log_err("button:%s",str1);
        //                 q = NULL;
        //             }
        //         }
        //         system("rm /tmp/button");
        //         pclose(fp);
        //     }
        // }
        
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), stdin);
        printf("Your input: %c\r\n", line[0]);
        switch (line[0]) {
        case 'u':
            test_tuya_user_iot_unactive_gw();
            break;
        case 'p':
            test_tuya_user_iot_permit_join();
            break;
        case 's':
            test_tuya_user_iot_network_scan();
            break;
        case 'g':
            test_tuya_user_iot_channel_get();
            break;
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        default:
            usage();
            break;
        }
        
    }

    return 0;
}
