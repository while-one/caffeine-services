/**
 * @file cfn_sal_utl_at_parser.h
 * @brief Abstract AT Command Parser service interface.
 */

#ifndef CAFFEINE_SAL_UTL_AT_PARSER_H
#define CAFFEINE_SAL_UTL_AT_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_UTL_TYPE_AT_PARSER CFN_SAL_TYPE('A', 'T', 'P')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_UTL_AT_PARSER_EVENT_NONE         = 0,
    CFN_SAL_UTL_AT_PARSER_EVENT_URC_RECEIVED = CFN_HAL_BIT(0), /*!< Unsolicited Result Code detected */
} cfn_sal_utl_at_parser_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *line_terminator; /*!< e.g. "\r\n" */
    uint32_t    default_timeout_ms;
    void       *custom;
} cfn_sal_utl_at_parser_config_t;

typedef struct cfn_sal_utl_at_parser_s     cfn_sal_utl_at_parser_t;
typedef struct cfn_sal_utl_at_parser_api_s cfn_sal_utl_at_parser_api_t;

typedef void (*cfn_sal_utl_at_parser_urc_callback_t)(cfn_sal_utl_at_parser_t *driver,
                                                     const char              *urc_line,
                                                     void                    *user_arg);

/**
 * @brief AT Parser Virtual Method Table (VMT).
 */
struct cfn_sal_utl_at_parser_api_s
{
    cfn_hal_api_base_t base;

    /* Basic Exchange */
    cfn_hal_error_code_t (*send_command)(cfn_sal_utl_at_parser_t *driver, const char *cmd);
    cfn_hal_error_code_t (*send_command_wait_resp)(cfn_sal_utl_at_parser_t *driver,
                                                   const char              *cmd,
                                                   const char              *expected_resp,
                                                   uint32_t                 timeout_ms);
    cfn_hal_error_code_t (*send_data)(cfn_sal_utl_at_parser_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*read_response)(cfn_sal_utl_at_parser_t *driver,
                                          char                    *buffer,
                                          size_t                   max_len,
                                          uint32_t                 timeout_ms);

    /* URC Management */
    cfn_hal_error_code_t (*register_urc)(cfn_sal_utl_at_parser_t             *driver,
                                         const char                          *prefix,
                                         cfn_sal_utl_at_parser_urc_callback_t cb,
                                         void                                *user_arg);
    cfn_hal_error_code_t (*register_urc_simple)(cfn_sal_utl_at_parser_t *driver,
                                                const char              *urc_string,
                                                void                    *callback);
    cfn_hal_error_code_t (*unregister_urc)(cfn_sal_utl_at_parser_t *driver, const char *prefix);

    /* Flow Control */
    cfn_hal_error_code_t (*set_echo)(cfn_sal_utl_at_parser_t *driver, bool enabled);
    cfn_hal_error_code_t (*flush_rx)(cfn_sal_utl_at_parser_t *driver);
    cfn_hal_error_code_t (*set_timeout)(cfn_sal_utl_at_parser_t *driver, uint32_t timeout_ms);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_utl_at_parser_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_utl_at_parser, cfn_sal_utl_at_parser_config_t, cfn_sal_utl_at_parser_api_t, cfn_sal_phy_t, cfn_hal_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_utl_at_parser_populate(cfn_sal_utl_at_parser_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   void                                 *dependency,
                                                   const cfn_sal_utl_at_parser_api_t    *api,
                                                   const cfn_sal_phy_t                  *phy,
                                                   const cfn_sal_utl_at_parser_config_t *config,
                                                   cfn_hal_callback_t                    callback,
                                                   void                                 *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_UTL_TYPE_AT_PARSER, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_utl_at_parser_construct(cfn_sal_utl_at_parser_t              *driver,
                                                     const cfn_sal_utl_at_parser_config_t *config,
                                                     const cfn_sal_phy_t                  *phy,
                                                     void                                 *dependency,
                                                     cfn_hal_callback_t                    callback,
                                                     void                                 *user_arg);
cfn_hal_error_code_t cfn_sal_utl_at_parser_destruct(cfn_sal_utl_at_parser_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_init(cfn_sal_utl_at_parser_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_deinit(cfn_sal_utl_at_parser_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_config_set(cfn_sal_utl_at_parser_t              *driver,
                                                                     const cfn_sal_utl_at_parser_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_config_get(cfn_sal_utl_at_parser_t        *driver,
                                                                     cfn_sal_utl_at_parser_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_callback_register(cfn_sal_utl_at_parser_t *driver,
                                                                            cfn_hal_callback_t       callback,
                                                                            void                    *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_power_state_set(cfn_sal_utl_at_parser_t *driver,
                                                                          cfn_hal_power_state_t    state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_event_enable(cfn_sal_utl_at_parser_t *driver,
                                                                       uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_event_disable(cfn_sal_utl_at_parser_t *driver,
                                                                        uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_event_get(cfn_sal_utl_at_parser_t *driver,
                                                                    uint32_t                *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_error_enable(cfn_sal_utl_at_parser_t *driver,
                                                                       uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_error_disable(cfn_sal_utl_at_parser_t *driver,
                                                                        uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_error_get(cfn_sal_utl_at_parser_t *driver,
                                                                    uint32_t                *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_UTL_TYPE_AT_PARSER, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_send_command(cfn_sal_utl_at_parser_t *driver, const char *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, send_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_send_data(cfn_sal_utl_at_parser_t *driver,
                                                                    const uint8_t           *data,
                                                                    size_t                   len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, send_data, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_read_response(cfn_sal_utl_at_parser_t *driver,
                                                                        char                    *buffer,
                                                                        size_t                   max_len,
                                                                        uint32_t                 timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_UTL_TYPE_AT_PARSER, read_response, driver, error, buffer, max_len, timeout_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_register_urc(cfn_sal_utl_at_parser_t             *driver,
                                                                       const char                          *prefix,
                                                                       cfn_sal_utl_at_parser_urc_callback_t cb,
                                                                       void                                *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, register_urc, driver, error, prefix, cb, user_arg);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_unregister_urc(cfn_sal_utl_at_parser_t *driver,
                                                                         const char              *prefix)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, unregister_urc, driver, error, prefix);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_set_echo(cfn_sal_utl_at_parser_t *driver, bool enabled)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, set_echo, driver, error, enabled);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_flush_rx(cfn_sal_utl_at_parser_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_UTL_TYPE_AT_PARSER, flush_rx, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_send_command_wait_resp(cfn_sal_utl_at_parser_t *driver,
                                                                                 const char              *cmd,
                                                                                 const char              *expected_resp,
                                                                                 uint32_t                 timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_UTL_TYPE_AT_PARSER, send_command_wait_resp, driver, error, cmd, expected_resp, timeout_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_register_urc_simple(cfn_sal_utl_at_parser_t *driver,
                                                                              const char              *urc_string,
                                                                              void                    *callback)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_UTL_TYPE_AT_PARSER, register_urc_simple, driver, error, urc_string, callback);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_at_parser_set_timeout(cfn_sal_utl_at_parser_t *driver,
                                                                      uint32_t                 timeout_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_AT_PARSER, set_timeout, driver, error, timeout_ms);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTL_AT_PARSER_H */
