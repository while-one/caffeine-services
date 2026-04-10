/**
 * @file cfn_sal_led.h
 * @brief Abstract LED service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_LED_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_LED CFN_SAL_TYPE('L', 'E', 'D')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_LED_STATE_OFF = 0,
    CFN_SAL_LED_STATE_ON
} cfn_sal_led_state_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} cfn_sal_led_color_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    bool  active_low; /*!< Invert logic if LED is sink-driven */
    void *custom;
} cfn_sal_led_config_t;

typedef struct cfn_sal_led_s     cfn_sal_led_t;
typedef struct cfn_sal_led_api_s cfn_sal_led_api_t;

typedef void (*cfn_sal_led_callback_t)(cfn_sal_led_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief LED Virtual Method Table (VMT).
 */
struct cfn_sal_led_api_s
{
    cfn_hal_api_base_t base;

    /* Basic Operations */
    cfn_hal_error_code_t (*set_state)(cfn_sal_led_t *driver, cfn_sal_led_state_t state);
    cfn_hal_error_code_t (*get_state)(cfn_sal_led_t *driver, cfn_sal_led_state_t *state_out);
    cfn_hal_error_code_t (*toggle)(cfn_sal_led_t *driver);

    /* Dimming (PWM-capable) */
    cfn_hal_error_code_t (*set_brightness)(cfn_sal_led_t *driver, uint8_t percent);
    cfn_hal_error_code_t (*get_brightness)(cfn_sal_led_t *driver, uint8_t *percent_out);

    /* Color (RGB-capable) */
    cfn_hal_error_code_t (*set_color)(cfn_sal_led_t *driver, cfn_sal_led_color_t color);

    /* Effects */
    cfn_hal_error_code_t (*blink_start)(cfn_sal_led_t *driver, uint32_t period_ms);
    cfn_hal_error_code_t (*blink_stop)(cfn_sal_led_t *driver);
    cfn_hal_error_code_t (*set_blink)(cfn_sal_led_t *driver, uint32_t interval_ms, uint32_t count);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_led_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_led, cfn_sal_led_config_t, cfn_sal_led_api_t, cfn_sal_phy_t, cfn_sal_led_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_led_populate(cfn_sal_led_t              *driver,
                                         uint32_t                    peripheral_id,
                                         void                       *dependency,
                                         const cfn_sal_led_api_t    *api,
                                         const cfn_sal_phy_t        *phy,
                                         const cfn_sal_led_config_t *config,
                                         cfn_sal_led_callback_t      callback,
                                         void                       *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_LED, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_led_construct(cfn_sal_led_t              *driver,
                                           const cfn_sal_led_config_t *config,
                                           const cfn_sal_phy_t        *phy,
                                           void                       *dependency,
                                           cfn_sal_led_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_sal_led_destruct(cfn_sal_led_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_init(cfn_sal_led_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_LED);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_deinit(cfn_sal_led_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_LED);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_config_set(cfn_sal_led_t *driver, const cfn_sal_led_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_LED, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_config_get(cfn_sal_led_t *driver, cfn_sal_led_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_callback_register(cfn_sal_led_t         *driver,
                                                                  cfn_sal_led_callback_t callback,
                                                                  void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SAL_TYPE_LED, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_power_state_set(cfn_sal_led_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_LED, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_event_enable(cfn_sal_led_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_LED, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_event_disable(cfn_sal_led_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_LED, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_event_get(cfn_sal_led_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_LED, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_error_enable(cfn_sal_led_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_LED, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_error_disable(cfn_sal_led_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_LED, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_error_get(cfn_sal_led_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_LED, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_set_state(cfn_sal_led_t *driver, cfn_sal_led_state_t state)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, set_state, driver, error, state);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_get_state(cfn_sal_led_t *driver, cfn_sal_led_state_t *state_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, get_state, driver, error, state_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_toggle(cfn_sal_led_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_LED, toggle, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_set_brightness(cfn_sal_led_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, set_brightness, driver, error, percent);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_get_brightness(cfn_sal_led_t *driver, uint8_t *percent_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, get_brightness, driver, error, percent_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_set_color(cfn_sal_led_t *driver, cfn_sal_led_color_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, set_color, driver, error, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_blink_start(cfn_sal_led_t *driver, uint32_t period_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, blink_start, driver, error, period_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_blink_stop(cfn_sal_led_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_LED, blink_stop, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_led_set_blink(cfn_sal_led_t *driver, uint32_t interval_ms, uint32_t count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LED, set_blink, driver, error, interval_ms, count);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_LED_H */
