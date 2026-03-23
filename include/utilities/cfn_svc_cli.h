/**
 * @file cfn_svc_cli.h
 * @brief Abstract Command Line Interface (CLI) service interface.
 */

#ifndef CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_CLI_H
#define CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_CLI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_CLI CFN_SVC_TYPE('C', 'L', 'I')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_CLI_EVENT_NONE = 0,
    CFN_SVC_CLI_EVENT_CMD_START = CFN_HAL_BIT(0),
    CFN_SVC_CLI_EVENT_CMD_DONE = CFN_HAL_BIT(1),
} cfn_svc_cli_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *name;
    const char *help;
    int (*handler)(int argc, char **argv);
} cfn_svc_cli_cmd_t;

typedef struct
{
    const char *prompt;
    const char *welcome_msg;
    bool        echo_enabled;
    void       *custom;
} cfn_svc_cli_config_t;

typedef struct cfn_svc_cli_s     cfn_svc_cli_t;
typedef struct cfn_svc_cli_api_s cfn_svc_cli_api_t;

typedef void (*cfn_svc_cli_callback_t)(cfn_svc_cli_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief CLI Virtual Method Table (VMT).
 */
struct cfn_svc_cli_api_s
{
    cfn_hal_api_base_t base;

    /* Input/Output */
    cfn_hal_error_code_t (*feed_char)(cfn_svc_cli_t *driver, char c);
    cfn_hal_error_code_t (*print_string)(cfn_svc_cli_t *driver, const char *str);
    cfn_hal_error_code_t (*print_line)(cfn_svc_cli_t *driver, const char *str);

    /* Command Management */
    cfn_hal_error_code_t (*register_command)(cfn_svc_cli_t *driver, const cfn_svc_cli_cmd_t *cmd);
    cfn_hal_error_code_t (*unregister_command)(cfn_svc_cli_t *driver, const char *name);

    /* UI Control */
    cfn_hal_error_code_t (*set_prompt)(cfn_svc_cli_t *driver, const char *prompt);
    cfn_hal_error_code_t (*clear_screen)(cfn_svc_cli_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_cli_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_cli, cfn_svc_cli_config_t, cfn_svc_cli_api_t, cfn_svc_phy_t, cfn_svc_cli_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_cli_populate(cfn_svc_cli_t              *driver,
                                         uint32_t                    peripheral_id,
                                         const cfn_svc_cli_api_t    *api,
                                         const cfn_svc_phy_t        *phy,
                                         const cfn_svc_cli_config_t *config,
                                         cfn_svc_cli_callback_t      callback,
                                         void                       *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_CLI, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_cli_construct(cfn_svc_cli_t              *driver,
                                           const cfn_svc_cli_config_t *config,
                                           const cfn_svc_phy_t        *phy,
                                           cfn_svc_cli_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_svc_cli_destruct(cfn_svc_cli_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_init(cfn_svc_cli_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_CLI);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_deinit(cfn_svc_cli_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_CLI);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_config_set(cfn_svc_cli_t *driver, const cfn_svc_cli_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_CLI, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_config_get(cfn_svc_cli_t *driver, cfn_svc_cli_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_callback_register(cfn_svc_cli_t         *driver,
                                                                  cfn_svc_cli_callback_t callback,
                                                                  void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_CLI, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_power_state_set(cfn_svc_cli_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_CLI, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_event_enable(cfn_svc_cli_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_event_disable(cfn_svc_cli_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_event_get(cfn_svc_cli_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_error_enable(cfn_svc_cli_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_CLI, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_error_disable(cfn_svc_cli_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_CLI, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_error_get(cfn_svc_cli_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_CLI, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_feed_char(cfn_svc_cli_t *driver, char c)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, feed_char, driver, error, c);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_print(cfn_svc_cli_t *driver, const char *str)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, print_string, driver, error, str);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_print_line(cfn_svc_cli_t *driver, const char *str)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, print_line, driver, error, str);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_register_command(cfn_svc_cli_t *driver, const cfn_svc_cli_cmd_t *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, register_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_unregister_command(cfn_svc_cli_t *driver, const char *name)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, unregister_command, driver, error, name);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_set_prompt(cfn_svc_cli_t *driver, const char *prompt)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_CLI, set_prompt, driver, error, prompt);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_clear_screen(cfn_svc_cli_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_CLI, clear_screen, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_CLI_H */
