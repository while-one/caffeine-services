/**
 * @file cfn_svc_led.h
 * @brief Abstract LED service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_LED_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_LED CFN_SVC_TYPE('L', 'E', 'D')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_LED_STATE_OFF = 0,
    CFN_SVC_LED_STATE_ON
} cfn_svc_led_state_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} cfn_svc_led_color_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *instance; /*!< Mapping to GPIO pin handle or PWM driver */
    void *user_arg;
} cfn_svc_led_phy_t;

typedef struct
{
    bool active_low; /*!< Invert logic if LED is sink-driven */
    void *custom;
} cfn_svc_led_config_t;

typedef struct cfn_svc_led_s     cfn_svc_led_t;
typedef struct cfn_svc_led_api_s cfn_svc_led_api_t;

typedef void (*cfn_svc_led_callback_t)(cfn_svc_led_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief LED Virtual Method Table (VMT).
 */
struct cfn_svc_led_api_s
{
    cfn_hal_api_base_t base;

    /* Basic Operations */
    cfn_hal_error_code_t (*set_state)(cfn_svc_led_t *driver, cfn_svc_led_state_t state);
    cfn_hal_error_code_t (*get_state)(cfn_svc_led_t *driver, cfn_svc_led_state_t *state_out);
    cfn_hal_error_code_t (*toggle)(cfn_svc_led_t *driver);

    /* Dimming (PWM-capable) */
    cfn_hal_error_code_t (*set_brightness)(cfn_svc_led_t *driver, uint8_t percent);
    cfn_hal_error_code_t (*get_brightness)(cfn_svc_led_t *driver, uint8_t *percent_out);

    /* Color (RGB-capable) */
    cfn_hal_error_code_t (*set_color)(cfn_svc_led_t *driver, cfn_svc_led_color_t color);

    /* Effects */
    cfn_hal_error_code_t (*blink_start)(cfn_svc_led_t *driver, uint32_t period_ms);
    cfn_hal_error_code_t (*blink_stop)(cfn_svc_led_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_led_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_led, cfn_svc_led_config_t, cfn_svc_led_api_t, cfn_svc_led_phy_t, cfn_svc_led_callback_t);

#define CFN_SVC_LED_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                         \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_LED, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_led_init(cfn_svc_led_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_LED);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_led_set_state(cfn_svc_led_t *driver, cfn_svc_led_state_t state)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_LED, set_state, driver, error, state);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_led_toggle(cfn_svc_led_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_LED, toggle, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_led_set_brightness(cfn_svc_led_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_LED, set_brightness, driver, error, percent);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_LED_H */
