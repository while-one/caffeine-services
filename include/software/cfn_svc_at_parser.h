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
#include "cfn_svc_types.h"

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
    void *instance; /*!< Mapping to UART handle */
    void *user_arg;
} cfn_svc_at_phy_t;

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
    cfn_hal_error_code_t (*send_data)(cfn_svc_at_parser_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*read_response)(cfn_svc_at_parser_t *driver, char *buffer, size_t max_len, uint32_t timeout_ms);

    /* URC Management */
    cfn_hal_error_code_t (*register_urc)(cfn_svc_at_parser_t *driver, const char *prefix, cfn_svc_at_urc_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*unregister_urc)(cfn_svc_at_parser_t *driver, const char *prefix);

    /* Flow Control */
    cfn_hal_error_code_t (*set_echo)(cfn_svc_at_parser_t *driver, bool enabled);
    cfn_hal_error_code_t (*flush_rx)(cfn_svc_at_parser_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_at_parser_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_at_parser, cfn_svc_at_config_t, cfn_svc_at_parser_api_t, cfn_svc_at_phy_t, cfn_hal_callback_t);

#define CFN_SVC_AT_PARSER_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                   \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_AT_PARSER, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_init(cfn_svc_at_parser_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_AT_PARSER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_at_send_command(cfn_svc_at_parser_t *driver, const char *cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_AT_PARSER, send_command, driver, error, cmd);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_AT_PARSER_H */
