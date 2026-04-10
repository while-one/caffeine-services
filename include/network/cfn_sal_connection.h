/**
 * @file cfn_sal_connection.h
 * @brief Generic abstract interface for link-layer connection management.
 */

#ifndef CAFFEINE_SAL_NETWORK_CFN_SAL_CONNECTION_H
#define CAFFEINE_SAL_NETWORK_CFN_SAL_CONNECTION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_CONNECTION CFN_SAL_TYPE('C', 'O', 'N')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_CONNECTION_EVENT_NONE         = 0,
    CFN_SAL_CONNECTION_EVENT_CONNECTED    = CFN_HAL_BIT(0),
    CFN_SAL_CONNECTION_EVENT_DISCONNECTED = CFN_HAL_BIT(1),
    CFN_SAL_CONNECTION_EVENT_LINK_LOST    = CFN_HAL_BIT(2),
} cfn_sal_connection_event_t;

typedef enum
{
    CFN_SAL_CONNECTION_STATUS_DISCONNECTED = 0,
    CFN_SAL_CONNECTION_STATUS_CONNECTING,
    CFN_SAL_CONNECTION_STATUS_CONNECTED,
    CFN_SAL_CONNECTION_STATUS_ERROR
} cfn_sal_connection_status_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    int16_t rssi_dbm;
    int8_t  snr_db;
    uint8_t link_quality_percent;
} cfn_sal_connection_info_t;

typedef struct
{
    uint32_t timeout_ms;
    bool     auto_reconnect;
    void    *custom;
} cfn_sal_connection_config_t;

typedef struct cfn_sal_connection_s     cfn_sal_connection_t;
typedef struct cfn_sal_connection_api_s cfn_sal_connection_api_t;

typedef void (*cfn_sal_connection_callback_t)(cfn_sal_connection_t *driver,
                                              uint32_t              event,
                                              uint32_t              error,
                                              void                 *user_arg);

/**
 * @brief Connection Virtual Method Table (VMT).
 */
struct cfn_sal_connection_api_s
{
    cfn_hal_api_base_t base;

    /* Control */
    cfn_hal_error_code_t (*connect)(cfn_sal_connection_t *driver);
    cfn_hal_error_code_t (*disconnect)(cfn_sal_connection_t *driver);

    /* Info */
    cfn_hal_error_code_t (*get_status)(cfn_sal_connection_t *driver, cfn_sal_connection_status_t *status_out);
    cfn_hal_error_code_t (*get_info)(cfn_sal_connection_t *driver, cfn_sal_connection_info_t *info_out);

    /* Extended Info & Config */
    cfn_hal_error_code_t (*scan_networks)(cfn_sal_connection_t *driver, void *results_array, size_t max_count);
    cfn_hal_error_code_t (*get_mac_address)(cfn_sal_connection_t *driver, uint8_t *mac_buf);
    cfn_hal_error_code_t (*get_ip_address)(cfn_sal_connection_t *driver, uint8_t *ip_buf);
    cfn_hal_error_code_t (*get_gateway)(cfn_sal_connection_t *driver, uint8_t *ip_buf);
    cfn_hal_error_code_t (*get_dns)(cfn_sal_connection_t *driver, uint8_t *ip_buf);
    cfn_hal_error_code_t (*set_static_ip)(cfn_sal_connection_t *driver,
                                          const uint8_t        *ip,
                                          const uint8_t        *mask,
                                          const uint8_t        *gateway);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_connection_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_connection,
                           cfn_sal_connection_config_t,
                           cfn_sal_connection_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_connection_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_connection_populate(cfn_sal_connection_t              *driver,
                                                uint32_t                           peripheral_id,
                                                void                              *dependency,
                                                const cfn_sal_connection_api_t    *api,
                                                const cfn_sal_phy_t               *phy,
                                                const cfn_sal_connection_config_t *config,
                                                cfn_sal_connection_callback_t      callback,
                                                void                              *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_CONNECTION, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_connection_construct(cfn_sal_connection_t              *driver,
                                                  const cfn_sal_connection_config_t *config,
                                                  const cfn_sal_phy_t               *phy,
                                                  void                              *dependency,
                                                  cfn_sal_connection_callback_t      callback,
                                                  void                              *user_arg);
cfn_hal_error_code_t cfn_sal_connection_destruct(cfn_sal_connection_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_init(cfn_sal_connection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_CONNECTION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_deinit(cfn_sal_connection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_CONNECTION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_config_set(cfn_sal_connection_t              *driver,
                                                                  const cfn_sal_connection_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_CONNECTION, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_config_get(cfn_sal_connection_t        *driver,
                                                                  cfn_sal_connection_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_callback_register(cfn_sal_connection_t         *driver,
                                                                         cfn_sal_connection_callback_t callback,
                                                                         void                         *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_TYPE_CONNECTION, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_power_state_set(cfn_sal_connection_t *driver,
                                                                       cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_CONNECTION, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_event_enable(cfn_sal_connection_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_event_disable(cfn_sal_connection_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_event_get(cfn_sal_connection_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_error_enable(cfn_sal_connection_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_CONNECTION, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_error_disable(cfn_sal_connection_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_CONNECTION, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_error_get(cfn_sal_connection_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_CONNECTION, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_connect(cfn_sal_connection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CONNECTION, connect, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_disconnect(cfn_sal_connection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CONNECTION, disconnect, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_status(cfn_sal_connection_t        *driver,
                                                                  cfn_sal_connection_status_t *status_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_status, driver, error, status_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_info(cfn_sal_connection_t      *driver,
                                                                cfn_sal_connection_info_t *info_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_info, driver, error, info_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_scan_networks(cfn_sal_connection_t *driver,
                                                                     void                 *results_array,
                                                                     size_t                max_count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, scan_networks, driver, error, results_array, max_count);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_mac_address(cfn_sal_connection_t *driver, uint8_t *mac_buf)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_mac_address, driver, error, mac_buf);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_ip_address(cfn_sal_connection_t *driver, uint8_t *ip_buf)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_ip_address, driver, error, ip_buf);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_gateway(cfn_sal_connection_t *driver, uint8_t *ip_buf)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_gateway, driver, error, ip_buf);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_get_dns(cfn_sal_connection_t *driver, uint8_t *ip_buf)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, get_dns, driver, error, ip_buf);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_connection_set_static_ip(cfn_sal_connection_t *driver,
                                                                     const uint8_t        *ip,
                                                                     const uint8_t        *mask,
                                                                     const uint8_t        *gateway)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CONNECTION, set_static_ip, driver, error, ip, mask, gateway);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_NETWORK_CFN_SAL_CONNECTION_H */
