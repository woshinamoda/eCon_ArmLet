
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_BAS)
#include "ble_bas.h"
#include <string.h>
#include "ble_srv_common.h"
#include "ble_conn_state.h"

#define NRF_LOG_MODULE_NAME ble_bas
#if BLE_BAS_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       BLE_BAS_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  BLE_BAS_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR BLE_BAS_CONFIG_DEBUG_COLOR
#else // BLE_BAS_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       0
#endif // BLE_BAS_CONFIG_LOG_ENABLED
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();


#define INVALID_BATTERY_LEVEL 255


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_bas       Battery Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_bas_t * p_bas, ble_evt_t const * p_ble_evt)
{
    if (!p_bas->is_notification_supported)
    {
        return;
    }

    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (    (p_evt_write->handle == p_bas->battery_level_handles.cccd_handle)
        &&  (p_evt_write->len == 2))
    {
        if (p_bas->evt_handler == NULL)
        {
            return;
        }

        ble_bas_evt_t evt;

        if (ble_srv_is_notification_enabled(p_evt_write->data))
        {
            evt.evt_type = BLE_BAS_EVT_NOTIFICATION_ENABLED;
        }
        else
        {
            evt.evt_type = BLE_BAS_EVT_NOTIFICATION_DISABLED;
        }
        evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;

        // CCCD written, call application event handler.
        p_bas->evt_handler(p_bas, &evt);
    }
}


void ble_bas_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_bas_t * p_bas = (ble_bas_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_bas, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


//��BAS_SPEC_V10.pdf�淶��Ҫ�󣺵�ط������Ҫ�е�����	battery	Level
//battery Level���ԵĿ��������Read�Ǳ�ѡ		nority����ѡ	���඼�ǲ���Ҫ��
static ret_code_t battery_level_char_add(ble_bas_t * p_bas, const ble_bas_init_t * p_bas_init)
{
    ret_code_t             err_code;								
    ble_add_char_params_t  add_char_params;																//�����������������ṹ��
    ble_add_descr_params_t add_descr_params;															//�����������������ṹ��				
    uint8_t                initial_battery_level;													//��ʼ��ص���
    uint8_t                init_len;																			//��ʼ���ݳ���
    uint8_t                encoded_report_ref[BLE_SRV_ENCODED_REPORT_REF_LEN];

    // Add battery level characteristic
    initial_battery_level = p_bas_init->initial_batt_level;								//�����ʼ���ṹ���е�س�ֵ---��ֵinitial_battery_level

    memset(&add_char_params, 0, sizeof(add_char_params));									
    add_char_params.uuid              = BLE_UUID_BATTERY_LEVEL_CHAR;			//0x291A Battery Level characteristic��UUID
    add_char_params.max_len           = sizeof(uint8_t);
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.p_init_value      = &initial_battery_level;						//���Եĳ�ʼֵ
    add_char_params.char_props.notify = p_bas->is_notification_supported;	//��������nority
    add_char_params.char_props.read   = 1;																//��������read
		//GATT����
    add_char_params.cccd_write_access = p_bas_init->bl_cccd_wr_sec;				//��ȫ����	
    add_char_params.read_access       = p_bas_init->bl_rd_sec;

    err_code = characteristic_add(p_bas->service_handle,									//����ľ��
                                  &add_char_params,
                                  &(p_bas->battery_level_handles));				//���Եľ��
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    if (p_bas_init->p_report_ref != NULL)																	/* battery level������������*/
    {
        // ��ӱ������õ�������
        init_len = ble_srv_report_ref_encode(encoded_report_ref, p_bas_init->p_report_ref);

        memset(&add_descr_params, 0, sizeof(add_descr_params));
        add_descr_params.uuid        = BLE_UUID_REPORT_REF_DESCR;
        add_descr_params.read_access = p_bas_init->bl_report_rd_sec;
        add_descr_params.init_len    = init_len;
        add_descr_params.max_len     = add_descr_params.init_len;
        add_descr_params.p_value     = encoded_report_ref;

        err_code = descriptor_add(p_bas->battery_level_handles.value_handle,
                                  &add_descr_params,
                                  &p_bas->report_ref_handle);
        return err_code;
    }
    else
    {
        p_bas->report_ref_handle = BLE_GATT_HANDLE_INVALID;
    }

    return NRF_SUCCESS;
}


ret_code_t ble_bas_init(ble_bas_t * p_bas, const ble_bas_init_t * p_bas_init)
{//��ط����ʼ������
    if (p_bas == NULL || p_bas_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    //��ʼ������ṹ��
    p_bas->evt_handler               = p_bas_init->evt_handler;							//��ط����¼����	�����Ӧ�ó���������Ӧ�����߼� main -> static void evt_handle
		p_bas->is_notification_supported = p_bas_init->support_notification;		/*��ص���֪ͨ*/
    p_bas->battery_level_last        = INVALID_BATTERY_LEVEL;								/*��ʼ��ص���*/

    //��ӷ���
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_BATTERY_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_bas->service_handle);
    VERIFY_SUCCESS(err_code);

    //��ӵ�ص�������
    err_code = battery_level_char_add(p_bas, p_bas_init);
    return err_code;
}


//������ص���ʹ��֪ͨ
static ret_code_t battery_notification_send(ble_gatts_hvx_params_t * const p_hvx_params,
                                            uint16_t                       conn_handle)
{
    ret_code_t err_code = sd_ble_gatts_hvx(conn_handle, p_hvx_params);
    if (err_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("Battery notification has been sent using conn_handle: 0x%04X", conn_handle);
    }
    else
    {
        NRF_LOG_DEBUG("Error: 0x%08X while sending notification with conn_handle: 0x%04X",
                      err_code,
                      conn_handle);
    }
    return err_code;
}


ret_code_t ble_bas_battery_level_update(ble_bas_t * p_bas,
                                        uint8_t     battery_level,
                                        uint16_t    conn_handle)
{//���ˮƽ�ϴ�����
 //@params1����ط���ṹ��ָ��			@params2�����ˮƽ			@params3���¼����
	//ʵ�ʷ���ͨ��battery_notification_send���sd_ble_gatts_hvx��������
    if (p_bas == NULL)
    {
        return NRF_ERROR_NULL;
    }

    ret_code_t         err_code = NRF_SUCCESS;
    ble_gatts_value_t  gatts_value;

    if (battery_level != p_bas->battery_level_last)
    {//�����ص������������һ�εĵ�ص���
        memset(&gatts_value, 0, sizeof(gatts_value));

        gatts_value.len     = sizeof(uint8_t);
        gatts_value.offset  = 0;
        gatts_value.p_value = &battery_level;

        // Update database.������ȡ����ֵ
        err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID,
                                          p_bas->battery_level_handles.value_handle,
                                          &gatts_value);
        if (err_code == NRF_SUCCESS)
        {
            NRF_LOG_INFO("Battery level has been updated: %d%%", battery_level)

            // Save new battery value.
            p_bas->battery_level_last = battery_level;
        }
        else
        {
            NRF_LOG_DEBUG("Error during battery level update: 0x%08X", err_code)

            return err_code;
        }

        // Send value if connected and notifying.	����ʹ��֪ͨ���ӻ������ϴ�����
        if (p_bas->is_notification_supported)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle = p_bas->battery_level_handles.value_handle;
            hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset = gatts_value.offset;
            hvx_params.p_len  = &gatts_value.len;
            hvx_params.p_data = gatts_value.p_value;

            if (conn_handle == BLE_CONN_HANDLE_ALL)
            {
                ble_conn_state_conn_handle_list_t conn_handles = ble_conn_state_conn_handles();

                // Try sending notifications to all valid connection handles.
                for (uint32_t i = 0; i < conn_handles.len; i++)
                {
                    if (ble_conn_state_status(conn_handles.conn_handles[i]) == BLE_CONN_STATUS_CONNECTED)
                    {
                        if (err_code == NRF_SUCCESS)
                        {
                            err_code = battery_notification_send(&hvx_params,
                                                                 conn_handles.conn_handles[i]);
                        }
                        else
                        {
                            // Preserve the first non-zero error code
                            UNUSED_RETURN_VALUE(battery_notification_send(&hvx_params,
                                                                          conn_handles.conn_handles[i]));
                        }
                    }
                }
            }
            else
            {
                err_code = battery_notification_send(&hvx_params, conn_handle);
            }
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }

    return err_code;
}


ret_code_t ble_bas_battery_lvl_on_reconnection_update(ble_bas_t * p_bas,
                                                      uint16_t    conn_handle)
{
    if (p_bas == NULL)
    {
        return NRF_ERROR_NULL;
    }

    ret_code_t err_code;

    if (p_bas->is_notification_supported)
    {
        ble_gatts_hvx_params_t hvx_params;
        uint16_t               len = sizeof(uint8_t);

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_bas->battery_level_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = &p_bas->battery_level_last;

        err_code = battery_notification_send(&hvx_params, conn_handle);

        return err_code;
    }

    return NRF_ERROR_INVALID_STATE;
}


#endif // NRF_MODULE_ENABLED(BLE_BAS)
