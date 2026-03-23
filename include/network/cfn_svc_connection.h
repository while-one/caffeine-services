/**
 * @file cfn_svc_connection.h
 * @brief Generic abstract interface for link-layer connection management.
 */

#ifndef CAFFEINE_SERVICES_NETWORK_CFN_SVC_CONNECTION_H
#define CAFFEINE_SERVICES_NETWORK_CFN_SVC_CONNECTION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_CONNECTION CFN_SVC_TYPE('C', 'O', 'N')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_CONNECTION_EVENT_NONE = 0,
    CFN_SVC_CONNECTION_EVENT_CONNECTED = CFN_HAL_BIT(0),
    CFN_SVC_CONNECTION_EVENT_DISCONNECTED = CFN_HAL_BIT(1),
    CFN_SVC_CONNECTION_EVENT_LINK_LOST = CFN_HAL_BIT(2),
} cfn_svc_connection_event_t;

typedef enum
{
    CFN_SVC_CONNECTION_STATUS_DISCONNECTED = 0,
    CFN_SVC_CONNECTION_STATUS_CONNECTING,
    CFN_SVC_CONNECTION_STATUS_CONNECTED,
    CFN_SVC_CONNECTION_STATUS_ERROR
} cfn_svc_connection_status_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    int16_t rssi_dbm;
    int8_t  snr_db;
    uint8_t link_quality_percent;
} cfn_svc_connection_info_t;

typedef struct
{
    uint32_t timeout_ms;
    bool     auto_reconnect;
    void    *custom;
} cfn_svc_connection_config_t;

typedef struct cfn_svc_connection_s     cfn_svc_connection_t;
typedef struct cfn_svc_connection_api_s cfn_svc_connection_api_t;

typedef void (*cfn_svc_connection_callback_t)(cfn_svc_connection_t *driver,
                                              uint32_t              event,
                                              uint32_t              error,
                                              void                 *user_arg);

/**
 * @brief Connection Virtual Method Table (VMT).
 */
struct cfn_svc_connection_api_s
{
    cfn_hal_api_base_t base;

    /* Control */
    cfn_hal_error_code_t (*connect)(cfn_svc_connection_t *driver);
    cfn_hal_error_code_t (*disconnect)(cfn_svc_connection_t *driver);

    /* Info */
    cfn_hal_error_code_t (*get_status)(cfn_svc_connection_t *driver, cfn_svc_connection_status_t *status_out);
    cfn_hal_error_code_t (*get_info)(cfn_svc_connection_t *driver, cfn_svc_connection_info_t *info_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_connection_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_connection,
                           cfn_svc_connection_config_t,
                           cfn_svc_connection_api_t,
                           cfn_svc_phy_t,
                           cfn_svc_connection_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_connection_populate(cfn_svc_connection_t              *driver,
                                                uint32_t                           peripheral_id,
                                                const cfn_svc_connection_api_t    *api,
                                                const cfn_svc_phy_t               *phy,
                                                const cfn_svc_connection_config_t *config,
                                                cfn_svc_connection_callback_t      callback,
                                                void                              *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_CONNECTION, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_connection_construct(cfn_svc_connection_t              *driver,
                                                  const cfn_svc_connection_config_t *config,
                                                  const cfn_svc_phy_t               *phy,
                                                  cfn_svc_connection_callback_t      callback,
                                                  void                              *user_arg);
cfn_hal_error_code_t cfn_svc_connection_destruct(cfn_svc_connection_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_init(cfn_svc_connection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_CONNECTION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_deinit(cfn_svc_connection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_CONNECTION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_config_set(cfn_svc_connection_t              *driver,
                                                                  const cfn_svc_connection_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_CONNECTION, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_config_get(cfn_svc_connection_t        *driver,
                                                                  cfn_svc_connection_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_callback_register(cfn_svc_connection_t         *driver,
                                                                         cfn_svc_connection_callback_t callback,
                                                                         void                         *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SVC_TYPE_CONNECTION, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_power_state_set(cfn_svc_connection_t *driver,
                                                                       cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_CONNECTION, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_event_enable(cfn_svc_connection_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_event_disable(cfn_svc_connection_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_event_get(cfn_svc_connection_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_CONNECTION, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_error_enable(cfn_svc_connection_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_CONNECTION, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_error_disable(cfn_svc_connection_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_CONNECTION, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_error_get(cfn_svc_connection_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_CONNECTION, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_connect(cfn_svc_connection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_CONNECTION, connect, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_disconnect(cfn_svc_connection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_CONNECTION, disconnect, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_get_status(cfn_svc_connection_t        *driver,
                                                                  cfn_svc_connection_status_t *status_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CONNECTION, get_status, driver, error, status_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_connection_get_info(cfn_svc_connection_t      *driver,
                                                                cfn_svc_connection_info_t *info_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CONNECTION, get_info, driver, error, info_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_NETWORK_CFN_SVC_CONNECTION_H */
