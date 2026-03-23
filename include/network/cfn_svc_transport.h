/**
 * @file cfn_svc_transport.h
 * @brief Generic abstract interface for data transport (Streaming/Datagram).
 */

#ifndef CAFFEINE_SERVICES_NETWORK_CFN_SVC_TRANSPORT_H
#define CAFFEINE_SERVICES_NETWORK_CFN_SVC_TRANSPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_TRANSPORT CFN_SVC_TYPE('T', 'P', 'T')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_TRANSPORT_EVENT_NONE = 0,
    CFN_SVC_TRANSPORT_EVENT_TX_COMPLETE = CFN_HAL_BIT(0),
    CFN_SVC_TRANSPORT_EVENT_RX_READY = CFN_HAL_BIT(1),
    CFN_SVC_TRANSPORT_EVENT_BUFFER_OVERFLOW = CFN_HAL_BIT(2),
} cfn_svc_transport_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    size_t   tx_buffer_size;
    size_t   rx_buffer_size;
    uint32_t baudrate; /* Optional, for serial-based transports */
    void    *custom;
} cfn_svc_transport_config_t;

typedef struct cfn_svc_transport_s     cfn_svc_transport_t;
typedef struct cfn_svc_transport_api_s cfn_svc_transport_api_t;

typedef void (*cfn_svc_transport_callback_t)(
    cfn_svc_transport_t *driver, uint32_t event, uint32_t error, size_t data_len, void *user_arg);

/**
 * @brief Transport Virtual Method Table (VMT).
 */
struct cfn_svc_transport_api_s
{
    cfn_hal_api_base_t base;

    /* Synchronous Operations (Blocking) */
    cfn_hal_error_code_t (*send)(cfn_svc_transport_t *driver, const uint8_t *data, size_t len, uint32_t timeout);
    cfn_hal_error_code_t (*receive)(
        cfn_svc_transport_t *driver, uint8_t *buffer, size_t max_len, size_t *received_len, uint32_t timeout);

    /* Asynchronous Operations (Non-blocking) */
    cfn_hal_error_code_t (*send_async)(cfn_svc_transport_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*receive_async)(cfn_svc_transport_t *driver, uint8_t *buffer, size_t max_len);

    /* Maintenance */
    cfn_hal_error_code_t (*flush)(cfn_svc_transport_t *driver);
    cfn_hal_error_code_t (*flush_rx)(cfn_svc_transport_t *driver);
    cfn_hal_error_code_t (*flush_tx)(cfn_svc_transport_t *driver);

    /* Connection Oriented (Server) */
    cfn_hal_error_code_t (*listen)(cfn_svc_transport_t *driver, uint16_t port);
    cfn_hal_error_code_t (*accept)(cfn_svc_transport_t *driver, cfn_svc_transport_t **client_driver_out);

    /* Status */
    cfn_hal_error_code_t (*get_bytes_available)(cfn_svc_transport_t *driver, size_t *count_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_transport_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_transport, cfn_svc_transport_config_t, cfn_svc_transport_api_t, cfn_svc_phy_t, cfn_svc_transport_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_transport_populate(cfn_svc_transport_t              *driver,
                                               uint32_t                          peripheral_id,
                                               const cfn_svc_transport_api_t    *api,
                                               const cfn_svc_phy_t              *phy,
                                               const cfn_svc_transport_config_t *config,
                                               cfn_svc_transport_callback_t      callback,
                                               void                             *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_TRANSPORT, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_transport_construct(cfn_svc_transport_t              *driver,
                                                 const cfn_svc_transport_config_t *config,
                                                 const cfn_svc_phy_t              *phy,
                                                 cfn_svc_transport_callback_t      callback,
                                                 void                             *user_arg);
cfn_hal_error_code_t cfn_svc_transport_destruct(cfn_svc_transport_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_init(cfn_svc_transport_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_TRANSPORT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_deinit(cfn_svc_transport_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_TRANSPORT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_config_set(cfn_svc_transport_t              *driver,
                                                                 const cfn_svc_transport_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_TRANSPORT, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_config_get(cfn_svc_transport_t        *driver,
                                                                 cfn_svc_transport_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_callback_register(cfn_svc_transport_t         *driver,
                                                                        cfn_svc_transport_callback_t callback,
                                                                        void                        *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SVC_TYPE_TRANSPORT, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_power_state_set(cfn_svc_transport_t  *driver,
                                                                      cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_TRANSPORT, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_event_enable(cfn_svc_transport_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_event_disable(cfn_svc_transport_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_event_get(cfn_svc_transport_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_error_enable(cfn_svc_transport_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_TRANSPORT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_error_disable(cfn_svc_transport_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_TRANSPORT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_error_get(cfn_svc_transport_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_TRANSPORT, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_send(cfn_svc_transport_t *driver,
                                                           const uint8_t       *data,
                                                           size_t               len,
                                                           uint32_t             timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, send, driver, error, data, len, timeout);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_receive(
    cfn_svc_transport_t *driver, uint8_t *buffer, size_t max_len, size_t *received_len, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SVC_TYPE_TRANSPORT, receive, driver, error, buffer, max_len, received_len, timeout);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_send_async(cfn_svc_transport_t *driver,
                                                                 const uint8_t       *data,
                                                                 size_t               len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, send_async, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_receive_async(cfn_svc_transport_t *driver,
                                                                    uint8_t             *buffer,
                                                                    size_t               max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, receive_async, driver, error, buffer, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_flush(cfn_svc_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_TRANSPORT, flush, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_flush_rx(cfn_svc_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_TRANSPORT, flush_rx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_flush_tx(cfn_svc_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_TRANSPORT, flush_tx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_listen(cfn_svc_transport_t *driver, uint16_t port)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, listen, driver, error, port);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_accept(cfn_svc_transport_t  *driver,
                                                            cfn_svc_transport_t **client_driver_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, accept, driver, error, client_driver_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_transport_get_bytes_available(cfn_svc_transport_t *driver,
                                                                         size_t              *count_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TRANSPORT, get_bytes_available, driver, error, count_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_NETWORK_CFN_SVC_TRANSPORT_H */
