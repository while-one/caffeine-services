/**
 * @file cfn_sal_cli.h
 * @brief Abstract Command Line Interface (CLI) service interface.
 */

#ifndef CAFFEINE_SAL_SOFTWARE_CFN_SAL_CLI_H
#define CAFFEINE_SAL_SOFTWARE_CFN_SAL_CLI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_CLI CFN_SAL_TYPE('C', 'L', 'I')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_CLI_EVENT_NONE      = 0,
    CFN_SAL_CLI_EVENT_CMD_START = CFN_HAL_BIT(0),
    CFN_SAL_CLI_EVENT_CMD_DONE  = CFN_HAL_BIT(1),
} cfn_sal_cli_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *name;
    const char *help;
    int (*handler)(int argc, char **argv);
} cfn_sal_cli_cmd_t;

typedef struct
{
    const char *prompt;
    const char *welcome_msg;
    bool        echo_enabled;
    void       *custom;
} cfn_sal_cli_config_t;

typedef struct cfn_sal_cli_s     cfn_sal_cli_t;
typedef struct cfn_sal_cli_api_s cfn_sal_cli_api_t;

typedef void (*cfn_sal_cli_callback_t)(cfn_sal_cli_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief CLI Virtual Method Table (VMT).
 */
struct cfn_sal_cli_api_s
{
    cfn_hal_api_base_t base;

    /* Input/Output */
    cfn_hal_error_code_t (*feed_char)(cfn_sal_cli_t *driver, char c);
    cfn_hal_error_code_t (*print_string)(cfn_sal_cli_t *driver, const char *str);
    cfn_hal_error_code_t (*print_line)(cfn_sal_cli_t *driver, const char *str);

    /* Command Management */
    cfn_hal_error_code_t (*register_command)(cfn_sal_cli_t *driver, const cfn_sal_cli_cmd_t *cmd);
    cfn_hal_error_code_t (*unregister_command)(cfn_sal_cli_t *driver, const char *name);
    cfn_hal_error_code_t (*register_command_simple)(cfn_sal_cli_t *driver, const char *cmd, void *callback);
    cfn_hal_error_code_t (*print_help)(cfn_sal_cli_t *driver);

    /* UI Control */
    cfn_hal_error_code_t (*set_prompt)(cfn_sal_cli_t *driver, const char *prompt);
    cfn_hal_error_code_t (*clear_screen)(cfn_sal_cli_t *driver);

    /* History */
    cfn_hal_error_code_t (*history_prev)(cfn_sal_cli_t *driver);
    cfn_hal_error_code_t (*history_next)(cfn_sal_cli_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_cli_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_cli, cfn_sal_cli_config_t, cfn_sal_cli_api_t, cfn_sal_phy_t, cfn_sal_cli_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_cli_populate(cfn_sal_cli_t              *driver,
                                         uint32_t                    peripheral_id,
                                         void                       *dependency,
                                         const cfn_sal_cli_api_t    *api,
                                         const cfn_sal_phy_t        *phy,
                                         const cfn_sal_cli_config_t *config,
                                         cfn_sal_cli_callback_t      callback,
                                         void                       *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_CLI, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_cli_construct(cfn_sal_cli_t              *driver,
                                           const cfn_sal_cli_config_t *config,
                                           const cfn_sal_phy_t        *phy,
                                           void                       *dependency,
                                           cfn_sal_cli_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_sal_cli_destruct(cfn_sal_cli_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_init(cfn_sal_cli_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_CLI);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_deinit(cfn_sal_cli_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_CLI);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_config_set(cfn_sal_cli_t *driver, const cfn_sal_cli_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_CLI, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_config_get(cfn_sal_cli_t *driver, cfn_sal_cli_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_callback_register(cfn_sal_cli_t         *driver,
                                                                  cfn_sal_cli_callback_t callback,
                                                                  void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SAL_TYPE_CLI, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_power_state_set(cfn_sal_cli_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_CLI, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_event_enable(cfn_sal_cli_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_event_disable(cfn_sal_cli_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_event_get(cfn_sal_cli_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_CLI, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_error_enable(cfn_sal_cli_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_CLI, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_error_disable(cfn_sal_cli_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_CLI, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_error_get(cfn_sal_cli_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_CLI, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_feed_char(cfn_sal_cli_t *driver, char c)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, feed_char, driver, error, c);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_print(cfn_sal_cli_t *driver, const char *str)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, print_string, driver, error, str);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_print_line(cfn_sal_cli_t *driver, const char *str)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, print_line, driver, error, str);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_register_command(cfn_sal_cli_t *driver, const cfn_sal_cli_cmd_t *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, register_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_unregister_command(cfn_sal_cli_t *driver, const char *name)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, unregister_command, driver, error, name);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_set_prompt(cfn_sal_cli_t *driver, const char *prompt)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, set_prompt, driver, error, prompt);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_clear_screen(cfn_sal_cli_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CLI, clear_screen, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_register_command_simple(cfn_sal_cli_t *driver,
                                                                        const char    *cmd,
                                                                        void          *callback)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_CLI, register_command_simple, driver, error, cmd, callback);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_print_help(cfn_sal_cli_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CLI, print_help, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_history_prev(cfn_sal_cli_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CLI, history_prev, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_cli_history_next(cfn_sal_cli_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_CLI, history_next, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_SOFTWARE_CFN_SAL_CLI_H */
