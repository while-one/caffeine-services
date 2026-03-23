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
typedef void (*cfn_svc_button_callback_t)(cfn_svc_button_t *driver,
                                          uint32_t          event_mask,
                                          uint32_t          error_mask,
                                          void             *user_arg);

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
    cfn_hal_error_code_t (*get_click_count)(cfn_svc_button_t *driver, uint8_t *count_out);
    cfn_hal_error_code_t (*get_hold_time_ms)(cfn_svc_button_t *driver, uint32_t *time_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_button_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_button, cfn_svc_button_config_t, cfn_svc_button_api_t, cfn_svc_phy_t, cfn_svc_button_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_button_populate(cfn_svc_button_t              *driver,
                                            uint32_t                       peripheral_id,
                                            const cfn_svc_button_api_t    *api,
                                            const cfn_svc_phy_t           *phy,
                                            const cfn_svc_button_config_t *config,
                                            cfn_svc_button_callback_t      callback,
                                            void                          *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_BUTTON, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_button_construct(cfn_svc_button_t              *driver,
                                              const cfn_svc_button_config_t *config,
                                              const cfn_svc_phy_t           *phy,
                                              cfn_svc_button_callback_t      callback,
                                              void                          *user_arg);
cfn_hal_error_code_t cfn_svc_button_destruct(cfn_svc_button_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_init(cfn_svc_button_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_BUTTON);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_deinit(cfn_svc_button_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_BUTTON);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_config_set(cfn_svc_button_t              *driver,
                                                              const cfn_svc_button_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_BUTTON, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_config_get(cfn_svc_button_t *driver, cfn_svc_button_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_callback_register(cfn_svc_button_t         *driver,
                                                                     cfn_svc_button_callback_t callback,
                                                                     void                     *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_BUTTON, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_power_state_set(cfn_svc_button_t     *driver,
                                                                   cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_BUTTON, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_event_enable(cfn_svc_button_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_event_disable(cfn_svc_button_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_event_get(cfn_svc_button_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_BUTTON, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_error_enable(cfn_svc_button_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_BUTTON, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_error_disable(cfn_svc_button_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_BUTTON, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_error_get(cfn_svc_button_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_BUTTON, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_get_state(cfn_svc_button_t       *driver,
                                                             cfn_svc_button_state_t *state_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, get_state, driver, error, state_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_set_debounce_time(cfn_svc_button_t *driver, uint32_t ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, set_debounce_time, driver, error, ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_is_pressed(cfn_svc_button_t *driver, bool *pressed_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, is_pressed, driver, error, pressed_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_is_released(cfn_svc_button_t *driver, bool *released_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, is_released, driver, error, released_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_get_click_count(cfn_svc_button_t *driver, uint8_t *count_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, get_click_count, driver, error, count_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_button_get_hold_time_ms(cfn_svc_button_t *driver, uint32_t *time_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_BUTTON, get_hold_time_ms, driver, error, time_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_BUTTON_H */
