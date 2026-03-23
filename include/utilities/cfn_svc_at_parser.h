/**
 * @file cfn_svc_at_parser.h
 * @brief Abstract AT Command Parser service interface.
 */

#ifndef CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_AT_PARSER_H
#define CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_AT_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_AT_PARSER CFN_SVC_TYPE('A', 'T', 'P')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_AT_EVENT_NONE = 0,
    CFN_SVC_AT_EVENT_URC_RECEIVED = CFN_HAL_BIT(0), /*!< Unsolicited Result Code detected */
} cfn_svc_at_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *line_terminator; /*!< e.g. "\r\n" */
    uint32_t    default_timeout_ms;
    void       *custom;
} cfn_svc_at_config_t;

typedef struct cfn_svc_at_parser_s     cfn_svc_at_parser_t;
typedef struct cfn_svc_at_parser_api_s cfn_svc_at_parser_api_t;

typedef void (*cfn_svc_at_urc_callback_t)(cfn_svc_at_parser_t *driver, const char *urc_line, void *user_arg);

/**
 * @brief AT Parser Virtual Method Table (VMT).
 */
struct cfn_svc_at_parser_api_s
{
    cfn_hal_api_base_t base;

    /* Basic Exchange */
    cfn_hal_error_code_t (*send_command)(cfn_svc_at_parser_t *driver, const char *cmd);
    cfn_hal_error_code_t (*send_command_wait_resp)(cfn_svc_at_parser_t *driver,
                                                   const char         *cmd,
                                                   const char         *expected_resp,
                                                   uint32_t            timeout_ms);
    cfn_hal_error_code_t (*send_data)(cfn_svc_at_parser_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*read_response)(cfn_svc_at_parser_t *driver,
                                          char                *buffer,
                                          size_t               max_len,
                                          uint32_t             timeout_ms);

    /* URC Management */
    cfn_hal_error_code_t (*register_urc)(cfn_svc_at_parser_t      *driver,
                                         const char               *prefix,
                                         cfn_svc_at_urc_callback_t cb,
                                         void                     *user_arg);
    cfn_hal_error_code_t (*register_urc_simple)(cfn_svc_at_parser_t *driver,
                                                const char         *urc_string,
                                                void               *callback);
    cfn_hal_error_code_t (*unregister_urc)(cfn_svc_at_parser_t *driver, const char *prefix);

    /* Flow Control */
    cfn_hal_error_code_t (*set_echo)(cfn_svc_at_parser_t *driver, bool enabled);
    cfn_hal_error_code_t (*flush_rx)(cfn_svc_at_parser_t *driver);
    cfn_hal_error_code_t (*set_timeout)(cfn_svc_at_parser_t *driver, uint32_t timeout_ms);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_at_parser_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_at_parser, cfn_svc_at_config_t, cfn_svc_at_parser_api_t, cfn_svc_phy_t, cfn_hal_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_at_parser_populate(cfn_svc_at_parser_t           *driver,
                                               uint32_t                       peripheral_id,
                                               const cfn_svc_at_parser_api_t *api,
                                               const cfn_svc_phy_t           *phy,
                                               const cfn_svc_at_config_t     *config,
                                               cfn_hal_callback_t             callback,
                                               void                          *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_AT_PARSER, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_at_parser_construct(cfn_svc_at_parser_t       *driver,
                                                 const cfn_svc_at_config_t *config,
                                                 const cfn_svc_phy_t       *phy,
                                                 cfn_hal_callback_t         callback,
                                                 void                      *user_arg);
cfn_hal_error_code_t cfn_svc_at_parser_destruct(cfn_svc_at_parser_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_init(cfn_svc_at_parser_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_AT_PARSER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_deinit(cfn_svc_at_parser_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_AT_PARSER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_config_set(cfn_svc_at_parser_t       *driver,
                                                          const cfn_svc_at_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_AT_PARSER, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_config_get(cfn_svc_at_parser_t *driver, cfn_svc_at_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_callback_register(cfn_svc_at_parser_t *driver,
                                                                 cfn_hal_callback_t   callback,
                                                                 void                *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_AT_PARSER, callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_power_state_set(cfn_svc_at_parser_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_AT_PARSER, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_event_enable(cfn_svc_at_parser_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_event_disable(cfn_svc_at_parser_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_event_get(cfn_svc_at_parser_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_error_enable(cfn_svc_at_parser_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_AT_PARSER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_error_disable(cfn_svc_at_parser_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_AT_PARSER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_error_get(cfn_svc_at_parser_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_AT_PARSER, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_send_command(cfn_svc_at_parser_t *driver, const char *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, send_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_send_data(cfn_svc_at_parser_t *driver, const uint8_t *data, size_t len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, send_data, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_read_response(cfn_svc_at_parser_t *driver,
                                                             char                *buffer,
                                                             size_t               max_len,
                                                             uint32_t             timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, read_response, driver, error, buffer, max_len, timeout_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_register_urc(cfn_svc_at_parser_t      *driver,
                                                            const char               *prefix,
                                                            cfn_svc_at_urc_callback_t cb,
                                                            void                     *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, register_urc, driver, error, prefix, cb, user_arg);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_unregister_urc(cfn_svc_at_parser_t *driver, const char *prefix)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, unregister_urc, driver, error, prefix);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_set_echo(cfn_svc_at_parser_t *driver, bool enabled)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, set_echo, driver, error, enabled);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_flush_rx(cfn_svc_at_parser_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_AT_PARSER, flush_rx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_send_command_wait_resp(cfn_svc_at_parser_t *driver,
                                                                     const char         *cmd,
                                                                     const char         *expected_resp,
                                                                     uint32_t            timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SVC_TYPE_AT_PARSER, send_command_wait_resp, driver, error, cmd, expected_resp, timeout_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_at_register_urc_simple(cfn_svc_at_parser_t *driver, const char *urc_string, void *callback)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, register_urc_simple, driver, error, urc_string, callback);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_set_timeout(cfn_svc_at_parser_t *driver, uint32_t timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, set_timeout, driver, error, timeout_ms);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_AT_PARSER_H */
