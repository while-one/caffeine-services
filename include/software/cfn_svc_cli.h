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
#include "cfn_svc_types.h"

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
    void *instance; /*!< Mapping to UART handle or terminal driver */
    void *user_arg;
} cfn_svc_cli_phy_t;

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

CFN_SVC_CREATE_DRIVER_TYPE(svc_cli, cfn_svc_cli_config_t, cfn_svc_cli_api_t, cfn_svc_cli_phy_t, cfn_svc_cli_callback_t);

#define CFN_SVC_CLI_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                       \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_CLI, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_cli_init(cfn_svc_cli_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_CLI);
}

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

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_CLI_H */
