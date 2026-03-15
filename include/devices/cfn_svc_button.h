/**
 * @file cfn_svc_button.h
 * @brief Abstract Button service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_BUTTON_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_BUTTON_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_BUTTON CFN_SVC_TYPE('B', 'U', 'T')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_BUTTON_EVENT_NONE = 0,
    CFN_SVC_BUTTON_EVENT_PRESSED = CFN_HAL_BIT(0),
    CFN_SVC_BUTTON_EVENT_RELEASED = CFN_HAL_BIT(1),
    CFN_SVC_BUTTON_EVENT_LONG_PRESS = CFN_HAL_BIT(2),
    CFN_SVC_BUTTON_EVENT_DOUBLE_CLICK = CFN_HAL_BIT(3),
} cfn_svc_button_event_t;

typedef enum
{
    CFN_SVC_BUTTON_STATE_RELEASED = 0,
    CFN_SVC_BUTTON_STATE_PRESSED
} cfn_svc_button_state_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *instance; /*!< Mapping to GPIO pin handle */
    void *user_arg;
} cfn_svc_button_phy_t;

typedef struct
{
    bool     active_low;    /*!< Logic level when pressed */
    uint32_t debounce_ms;   /*!< Software debounce time */
    uint32_t long_press_ms; /*!< Threshold for LONG_PRESS event */
    void    *custom;
} cfn_svc_button_config_t;

typedef struct cfn_svc_button_s     cfn_svc_button_t;
typedef struct cfn_svc_button_api_s cfn_svc_button_api_t;

/**
 * @brief Button callback signature.
 */
typedef void (*cfn_svc_button_callback_t)(cfn_svc_button_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief Button Virtual Method Table (VMT).
 */
struct cfn_svc_button_api_s
{
    cfn_hal_api_base_t base;

    /* Basic Operations */
    cfn_hal_error_code_t (*get_state)(cfn_svc_button_t *driver, cfn_svc_button_state_t *state_out);
    cfn_hal_error_code_t (*set_debounce_time)(cfn_svc_button_t *driver, uint32_t ms);
    cfn_hal_error_code_t (*is_pressed)(cfn_svc_button_t *driver, bool *pressed_out);
    cfn_hal_error_code_t (*is_released)(cfn_svc_button_t *driver, bool *released_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_button_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_button, cfn_svc_button_config_t, cfn_svc_button_api_t, cfn_svc_button_phy_t, cfn_svc_button_callback_t);

#define CFN_SVC_BUTTON_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                      \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_BUTTON, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_init(cfn_svc_button_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_BUTTON);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_get_state(cfn_svc_button_t *driver, cfn_svc_button_state_t *state_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, get_state, driver, error, state_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_is_pressed(cfn_svc_button_t *driver, bool *pressed_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, is_pressed, driver, error, pressed_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_BUTTON_H */
