
#ifndef BLE_NUS_H__
#define BLE_NUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ble_link_ctx_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_nus instance.
 *
 * @param     _name            Name of the instance.
 * @param[in] _nus_max_clients Maximum number of NUS clients connected at a time.
 * @hideinitializer
 */
#define BLE_NUS_DEF(_name, _nus_max_clients)                      \
    BLE_LINK_CTX_MANAGER_DEF(CONCAT_2(_name, _link_ctx_storage),  \
                             (_nus_max_clients),                  \
                             sizeof(ble_nus_client_context_t));   \
    static ble_nus_t _name =                                      \
    {                                                             \
        .p_link_ctx_storage = &CONCAT_2(_name, _link_ctx_storage) \
    };                                                            \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,                           \
                         BLE_NUS_BLE_OBSERVER_PRIO,               \
                         ble_nus_on_ble_evt,                      \
                         &_name)

#define BLE_UUID_NUS_SERVICE 0xCCF0 /**< The UUID of the Nordic UART Service. */

#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_NUS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
    #define BLE_NUS_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
    #warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif


/**@brief   Nordic UART Service event types. */
typedef enum
{
    BLE_NUS_EVT_RX_DATA,      /**< Data received. */
    BLE_NUS_EVT_TX_RDY,       /**< Service is ready to accept new data to be transmitted. */
    BLE_NUS_EVT_COMM_STARTED, /**< Notification has been enabled. */
    BLE_NUS_EVT_COMM_STOPPED, /**< Notification has been disabled. */
} ble_nus_evt_type_t;


/* Forward declaration of the ble_nus_t type. */
typedef struct ble_nus_s ble_nus_t;


/**@brief   Nordic UART Service @ref BLE_NUS_EVT_RX_DATA event data.
 *
 * @details This structure is passed to an event when @ref BLE_NUS_EVT_RX_DATA occurs.
 */
typedef struct
{
    uint8_t const * p_data; /**< A pointer to the buffer with received data. */
    uint16_t        length; /**< Length of received data. */
} ble_nus_evt_rx_data_t;


/**@brief Nordic UART Service client context structure.
 *
 * @details This structure contains state context related to hosts.
 */
typedef struct
{
    bool is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
} ble_nus_client_context_t;



typedef struct
{
    ble_nus_evt_type_t         type;        //串口事件类型
    ble_nus_t                * p_nus;       //事例指针
    uint16_t                   conn_handle; //连接句柄
    ble_nus_client_context_t * p_link_ctx;  //上下文链接
    union
    {
        ble_nus_evt_rx_data_t rx_data; 			//参数BLE_NUS_EVT_RX_DATA事件的数据
    } params;
} ble_nus_evt_t;


/**@brief Nordic UART Service event handler type. */
typedef void (* ble_nus_data_handler_t) (ble_nus_evt_t * p_evt);

//ble_nus_init_t用于初始化参数
typedef struct
{
    ble_nus_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_nus_init_t;


//ble_nus_s服务类型结构
struct ble_nus_s
{
    uint8_t                         uuid_type;          //UUID类型
    uint16_t                        service_handle;     //服务句柄
		ble_gatts_char_handles_t        tx_handles;         //发送句柄
    ble_gatts_char_handles_t        rx_handles;         //接受句柄
    blcm_link_ctx_storage_t * const p_link_ctx_storage; //存储连接空间
    ble_nus_data_handler_t          data_handler;       //传输数据句柄
};


uint32_t ble_nus_init(ble_nus_t * p_nus, ble_nus_init_t const * p_nus_init);
void ble_nus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
uint32_t ble_nus_data_send(ble_nus_t * p_nus,
                           uint8_t   * p_data,
                           uint16_t  * p_length,
                           uint16_t    conn_handle);


#ifdef __cplusplus
}
#endif

#endif // BLE_NUS_H__

/** @} */
