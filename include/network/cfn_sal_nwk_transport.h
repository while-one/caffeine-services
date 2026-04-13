/**
 * @file cfn_sal_nwk_transport.h
 * @brief Generic abstract interface for data transport (Streaming/Datagram).
 */

#ifndef CAFFEINE_SAL_NWK_TRANSPORT_H
#define CAFFEINE_SAL_NWK_TRANSPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_NWK_TYPE_TRANSPORT CFN_SAL_TYPE('T', 'P', 'T')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_NWK_TRANSPORT_EVENT_NONE            = 0,
    CFN_SAL_NWK_TRANSPORT_EVENT_TX_COMPLETE     = CFN_HAL_BIT(0),
    CFN_SAL_NWK_TRANSPORT_EVENT_RX_READY        = CFN_HAL_BIT(1),
    CFN_SAL_NWK_TRANSPORT_EVENT_BUFFER_OVERFLOW = CFN_HAL_BIT(2),
} cfn_sal_nwk_transport_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    size_t   tx_buffer_size;
    size_t   rx_buffer_size;
    uint32_t baudrate; /* Optional, for serial-based transports */
    void    *custom;
} cfn_sal_nwk_transport_config_t;

typedef struct cfn_sal_nwk_transport_s     cfn_sal_nwk_transport_t;
typedef struct cfn_sal_nwk_transport_api_s cfn_sal_nwk_transport_api_t;

typedef void (*cfn_sal_nwk_transport_callback_t)(
    cfn_sal_nwk_transport_t *driver, uint32_t event, uint32_t error, size_t data_len, void *user_arg);

/**
 * @brief Transport Virtual Method Table (VMT).
 */
struct cfn_sal_nwk_transport_api_s
{
    cfn_hal_api_base_t base;

    /* Synchronous Operations (Blocking) */
    cfn_hal_error_code_t (*send)(cfn_sal_nwk_transport_t *driver, const uint8_t *data, size_t len, uint32_t timeout);
    cfn_hal_error_code_t (*receive)(
        cfn_sal_nwk_transport_t *driver, uint8_t *buffer, size_t max_len, size_t *received_len, uint32_t timeout);

    /* Asynchronous Operations (Non-blocking) */
    cfn_hal_error_code_t (*send_async)(cfn_sal_nwk_transport_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*receive_async)(cfn_sal_nwk_transport_t *driver, uint8_t *buffer, size_t max_len);

    /* Maintenance */
    cfn_hal_error_code_t (*flush)(cfn_sal_nwk_transport_t *driver);
    cfn_hal_error_code_t (*flush_rx)(cfn_sal_nwk_transport_t *driver);
    cfn_hal_error_code_t (*flush_tx)(cfn_sal_nwk_transport_t *driver);

    /* Connection Oriented (Server) */
    cfn_hal_error_code_t (*listen)(cfn_sal_nwk_transport_t *driver, uint16_t port);
    cfn_hal_error_code_t (*accept)(cfn_sal_nwk_transport_t *driver, cfn_sal_nwk_transport_t **client_driver_out);

    /* Status */
    cfn_hal_error_code_t (*get_bytes_available)(cfn_sal_nwk_transport_t *driver, size_t *count_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_nwk_transport_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_nwk_transport,
                           cfn_sal_nwk_transport_config_t,
                           cfn_sal_nwk_transport_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_nwk_transport_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_nwk_transport_populate(cfn_sal_nwk_transport_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   void                                 *dependency,
                                                   const cfn_sal_nwk_transport_api_t    *api,
                                                   const cfn_sal_phy_t                  *phy,
                                                   const cfn_sal_nwk_transport_config_t *config,
                                                   cfn_sal_nwk_transport_callback_t      callback,
                                                   void                                 *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_NWK_TYPE_TRANSPORT, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_nwk_transport_construct(cfn_sal_nwk_transport_t              *driver,
                                                     const cfn_sal_nwk_transport_config_t *config,
                                                     const cfn_sal_phy_t                  *phy,
                                                     void                                 *dependency,
                                                     cfn_sal_nwk_transport_callback_t      callback,
                                                     void                                 *user_arg);
cfn_hal_error_code_t cfn_sal_nwk_transport_destruct(cfn_sal_nwk_transport_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_init(cfn_sal_nwk_transport_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_deinit(cfn_sal_nwk_transport_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_config_set(cfn_sal_nwk_transport_t              *driver,
                                                                     const cfn_sal_nwk_transport_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_config_get(cfn_sal_nwk_transport_t        *driver,
                                                                     cfn_sal_nwk_transport_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_callback_register(cfn_sal_nwk_transport_t         *driver,
                                                                            cfn_sal_nwk_transport_callback_t callback,
                                                                            void                            *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_power_state_set(cfn_sal_nwk_transport_t *driver,
                                                                          cfn_hal_power_state_t    state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_event_enable(cfn_sal_nwk_transport_t *driver,
                                                                       uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_event_disable(cfn_sal_nwk_transport_t *driver,
                                                                        uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_event_get(cfn_sal_nwk_transport_t *driver,
                                                                    uint32_t                *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_error_enable(cfn_sal_nwk_transport_t *driver,
                                                                       uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_error_disable(cfn_sal_nwk_transport_t *driver,
                                                                        uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_error_get(cfn_sal_nwk_transport_t *driver,
                                                                    uint32_t                *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_NWK_TYPE_TRANSPORT, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_send(cfn_sal_nwk_transport_t *driver,
                                                               const uint8_t           *data,
                                                               size_t                   len,
                                                               uint32_t                 timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, send, driver, error, data, len, timeout);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_receive(
    cfn_sal_nwk_transport_t *driver, uint8_t *buffer, size_t max_len, size_t *received_len, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_NWK_TYPE_TRANSPORT, receive, driver, error, buffer, max_len, received_len, timeout);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_send_async(cfn_sal_nwk_transport_t *driver,
                                                                     const uint8_t           *data,
                                                                     size_t                   len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, send_async, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_receive_async(cfn_sal_nwk_transport_t *driver,
                                                                        uint8_t                 *buffer,
                                                                        size_t                   max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, receive_async, driver, error, buffer, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_flush(cfn_sal_nwk_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_NWK_TYPE_TRANSPORT, flush, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_flush_rx(cfn_sal_nwk_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_NWK_TYPE_TRANSPORT, flush_rx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_flush_tx(cfn_sal_nwk_transport_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_NWK_TYPE_TRANSPORT, flush_tx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_listen(cfn_sal_nwk_transport_t *driver, uint16_t port)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, listen, driver, error, port);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_accept(cfn_sal_nwk_transport_t  *driver,
                                                                 cfn_sal_nwk_transport_t **client_driver_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, accept, driver, error, client_driver_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_nwk_transport_get_bytes_available(cfn_sal_nwk_transport_t *driver,
                                                                              size_t                  *count_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_NWK_TYPE_TRANSPORT, get_bytes_available, driver, error, count_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_NWK_TRANSPORT_H */
