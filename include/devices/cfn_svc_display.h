/**
 * @file cfn_svc_display.h
 * @brief Abstract Display service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_DISPLAY_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_DISPLAY CFN_SVC_TYPE('D', 'S', 'P')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_DISPLAY_EVENT_NONE = 0,
    CFN_SVC_DISPLAY_EVENT_REFRESH_DONE = CFN_HAL_BIT(0),
} cfn_svc_display_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t  color_depth;
    void    *custom;
} cfn_svc_display_config_t;

typedef struct cfn_svc_display_s     cfn_svc_display_t;
typedef struct cfn_svc_display_api_s cfn_svc_display_api_t;

typedef void (*cfn_svc_display_callback_t)(cfn_svc_display_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Display Virtual Method Table (VMT).
 */
struct cfn_svc_display_api_s
{
    cfn_hal_api_base_t base;

    /* Power & Control */
    cfn_hal_error_code_t (*turn_on)(cfn_svc_display_t *driver);
    cfn_hal_error_code_t (*turn_off)(cfn_svc_display_t *driver);
    cfn_hal_error_code_t (*set_brightness)(cfn_svc_display_t *driver, uint8_t percent);
    cfn_hal_error_code_t (*set_backlight)(cfn_svc_display_t *driver, uint8_t percent);

    /* Drawing Operations */
    cfn_hal_error_code_t (*draw_pixel)(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint32_t color);
    cfn_hal_error_code_t (*draw_line)(
        cfn_svc_display_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
    cfn_hal_error_code_t (*draw_rect)(
        cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
    cfn_hal_error_code_t (*fill_rect)(
        cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
    cfn_hal_error_code_t (*draw_bitmap)(
        cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);
    cfn_hal_error_code_t (*clear_window)(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    cfn_hal_error_code_t (*set_cursor)(cfn_svc_display_t *driver, uint16_t x, uint16_t y);

    /* Buffer Management */
    cfn_hal_error_code_t (*clear)(cfn_svc_display_t *driver);
    cfn_hal_error_code_t (*update)(cfn_svc_display_t *driver);

    /* Low-level Communication */
    cfn_hal_error_code_t (*write_data)(cfn_svc_display_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*write_command)(cfn_svc_display_t *driver, uint8_t cmd);

    /* Configuration */
    cfn_hal_error_code_t (*set_orientation)(cfn_svc_display_t *driver, uint8_t orientation);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_display_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_display, cfn_svc_display_config_t, cfn_svc_display_api_t, cfn_svc_phy_t, cfn_svc_display_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_display_populate(cfn_svc_display_t              *driver,
                                             uint32_t                        peripheral_id,
                                             const cfn_svc_display_api_t    *api,
                                             const cfn_svc_phy_t            *phy,
                                             const cfn_svc_display_config_t *config,
                                             cfn_svc_display_callback_t      callback,
                                             void                           *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_DISPLAY, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_display_construct(cfn_svc_display_t              *driver,
                                               const cfn_svc_display_config_t *config,
                                               const cfn_svc_phy_t            *phy,
                                               cfn_svc_display_callback_t      callback,
                                               void                           *user_arg);
cfn_hal_error_code_t cfn_svc_display_destruct(cfn_svc_display_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_init(cfn_svc_display_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_DISPLAY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_deinit(cfn_svc_display_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_DISPLAY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_config_set(cfn_svc_display_t              *driver,
                                                               const cfn_svc_display_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_DISPLAY, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_config_get(cfn_svc_display_t        *driver,
                                                               cfn_svc_display_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_callback_register(cfn_svc_display_t         *driver,
                                                                      cfn_svc_display_callback_t callback,
                                                                      void                      *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_DISPLAY, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_power_state_set(cfn_svc_display_t    *driver,
                                                                    cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_DISPLAY, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_event_enable(cfn_svc_display_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_event_disable(cfn_svc_display_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_event_get(cfn_svc_display_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_error_enable(cfn_svc_display_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_DISPLAY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_error_disable(cfn_svc_display_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_DISPLAY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_error_get(cfn_svc_display_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_DISPLAY, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_turn_on(cfn_svc_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_DISPLAY, turn_on, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_turn_off(cfn_svc_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_DISPLAY, turn_off, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_set_brightness(cfn_svc_display_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, set_brightness, driver, error, percent);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_draw_pixel(cfn_svc_display_t *driver,
                                                               uint16_t           x,
                                                               uint16_t           y,
                                                               uint32_t           color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, draw_pixel, driver, error, x, y, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_display_fill_rect(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, fill_rect, driver, error, x, y, w, h, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_draw_bitmap(
    cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, draw_bitmap, driver, error, x, y, w, h, data);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_clear(cfn_svc_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_DISPLAY, clear, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_update(cfn_svc_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_DISPLAY, update, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_set_backlight(cfn_svc_display_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, set_backlight, driver, error, percent);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_draw_line(
    cfn_svc_display_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, draw_line, driver, error, x0, y0, x1, y1, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_display_draw_rect(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, draw_rect, driver, error, x, y, w, h, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_display_clear_window(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, clear_window, driver, error, x, y, w, h);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_set_cursor(cfn_svc_display_t *driver, uint16_t x, uint16_t y)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, set_cursor, driver, error, x, y);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_write_data(cfn_svc_display_t *driver,
                                                              const uint8_t     *data,
                                                              size_t             len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, write_data, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_write_command(cfn_svc_display_t *driver, uint8_t cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, write_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_set_orientation(cfn_svc_display_t *driver, uint8_t orientation)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_DISPLAY, set_orientation, driver, error, orientation);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_DISPLAY_H */
