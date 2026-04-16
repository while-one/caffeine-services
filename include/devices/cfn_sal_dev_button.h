/**
 * @file cfn_sal_dev_button.h
 * @brief Abstract Button service interface.
 */

#ifndef CAFFEINE_SAL_DEV_BUTTON_H
#define CAFFEINE_SAL_DEV_BUTTON_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_BUTTON CFN_SAL_TYPE('B', 'T', 'N')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_BUTTON_EVENT_NONE         = 0,
    CFN_SAL_DEV_BUTTON_EVENT_PRESSED      = CFN_HAL_BIT(0),
    CFN_SAL_DEV_BUTTON_EVENT_RELEASED     = CFN_HAL_BIT(1),
    CFN_SAL_DEV_BUTTON_EVENT_LONG_PRESS   = CFN_HAL_BIT(2),
    CFN_SAL_DEV_BUTTON_EVENT_DOUBLE_CLICK = CFN_HAL_BIT(3),
} cfn_sal_dev_button_event_t;

typedef enum
{
    CFN_SAL_DEV_BUTTON_STATE_RELEASED = 0,
    CFN_SAL_DEV_BUTTON_STATE_PRESSED,

    CFN_SAL_DEV_BUTTON_STATE_MAX
} cfn_sal_dev_button_state_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    bool     active_low;    /*!< Logic level when pressed */
    uint32_t debounce_ms;   /*!< Software debounce time */
    uint32_t long_press_ms; /*!< Threshold for LONG_PRESS event */
    void    *custom;
} cfn_sal_dev_button_config_t;

typedef struct cfn_sal_dev_button_s     cfn_sal_dev_button_t;
typedef struct cfn_sal_dev_button_api_s cfn_sal_dev_button_api_t;

/**
 * @brief Button callback signature.
 */
typedef void (*cfn_sal_dev_button_callback_t)(cfn_sal_dev_button_t *driver,
                                              uint32_t              event_mask,
                                              uint32_t              error_mask,
                                              void                 *user_arg);

/**
 * @brief Button Virtual Method Table (VMT).
 */
struct cfn_sal_dev_button_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Basic Operations */
    cfn_hal_error_code_t (*get_state)(cfn_sal_dev_button_t *driver, cfn_sal_dev_button_state_t *state_out);
    cfn_hal_error_code_t (*set_debounce_time)(cfn_sal_dev_button_t *driver, uint32_t ms);
    cfn_hal_error_code_t (*is_pressed)(cfn_sal_dev_button_t *driver, bool *pressed_out);
    cfn_hal_error_code_t (*is_released)(cfn_sal_dev_button_t *driver, bool *released_out);
    cfn_hal_error_code_t (*get_click_count)(cfn_sal_dev_button_t *driver, uint8_t *count_out);
    cfn_hal_error_code_t (*get_hold_time_ms)(cfn_sal_dev_button_t *driver, uint32_t *time_out);
};

CFN_SAL_DEV_VMT_CHECK(struct cfn_sal_dev_button_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_button,
                           cfn_sal_dev_button_config_t,
                           cfn_sal_dev_button_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_button_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_button_populate(cfn_sal_dev_button_t              *driver,
                                                uint32_t                           peripheral_id,
                                                void                              *dependency,
                                                const cfn_sal_dev_button_api_t    *api,
                                                const cfn_sal_phy_t               *phy,
                                                const cfn_sal_dev_button_config_t *config,
                                                cfn_sal_dev_button_callback_t      callback,
                                                void                              *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_BUTTON, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_button_construct(cfn_sal_dev_button_t              *driver,
                                                  const cfn_sal_dev_button_config_t *config,
                                                  const cfn_sal_phy_t               *phy,
                                                  void                              *dependency,
                                                  cfn_sal_dev_button_callback_t      callback,
                                                  void                              *user_arg);
cfn_hal_error_code_t cfn_sal_dev_button_destruct(cfn_sal_dev_button_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_config_validate(
    const cfn_sal_dev_button_t *driver, const cfn_sal_dev_button_config_t *config)
{
    if (!driver || !config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_config_validate(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_init(cfn_sal_dev_button_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_sal_dev_button_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_BUTTON);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_deinit(cfn_sal_dev_button_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_BUTTON);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_config_set(cfn_sal_dev_button_t              *driver,
                                                                  const cfn_sal_dev_button_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_sal_dev_button_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_config_get(cfn_sal_dev_button_t        *driver,
                                                                  cfn_sal_dev_button_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_callback_register(cfn_sal_dev_button_t         *driver,
                                                                         cfn_sal_dev_button_callback_t callback,
                                                                         void                         *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_BUTTON, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_power_state_set(cfn_sal_dev_button_t *driver,
                                                                       cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_event_enable(cfn_sal_dev_button_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_event_disable(cfn_sal_dev_button_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_event_get(cfn_sal_dev_button_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_error_enable(cfn_sal_dev_button_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_error_disable(cfn_sal_dev_button_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_error_get(cfn_sal_dev_button_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_BUTTON, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_get_state(cfn_sal_dev_button_t       *driver,
                                                                 cfn_sal_dev_button_state_t *state_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, get_state, driver, error, state_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_set_debounce_time(cfn_sal_dev_button_t *driver, uint32_t ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, set_debounce_time, driver, error, ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_is_pressed(cfn_sal_dev_button_t *driver, bool *pressed_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, is_pressed, driver, error, pressed_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_is_released(cfn_sal_dev_button_t *driver, bool *released_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, is_released, driver, error, released_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_get_click_count(cfn_sal_dev_button_t *driver, uint8_t *count_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, get_click_count, driver, error, count_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_button_get_hold_time_ms(cfn_sal_dev_button_t *driver,
                                                                        uint32_t             *time_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BUTTON, get_hold_time_ms, driver, error, time_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_BUTTON_H */
