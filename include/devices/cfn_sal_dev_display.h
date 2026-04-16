/**
 * @file cfn_sal_dev_display.h
 * @brief Abstract Display service interface.
 */

#ifndef CAFFEINE_SAL_DEV_DISPLAY_H
#define CAFFEINE_SAL_DEV_DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_DISPLAY CFN_SAL_TYPE('D', 'S', 'P')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_DISPLAY_EVENT_NONE         = 0,
    CFN_SAL_DEV_DISPLAY_EVENT_REFRESH_DONE = CFN_HAL_BIT(0),
} cfn_sal_dev_display_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t  color_depth;
    void    *custom;
} cfn_sal_dev_display_config_t;

typedef struct cfn_sal_dev_display_s     cfn_sal_dev_display_t;
typedef struct cfn_sal_dev_display_api_s cfn_sal_dev_display_api_t;

typedef void (*cfn_sal_dev_display_callback_t)(cfn_sal_dev_display_t *driver,
                                               uint32_t               event,
                                               uint32_t               error,
                                               void                  *user_arg);

/**
 * @brief Display Virtual Method Table (VMT).
 */
struct cfn_sal_dev_display_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Power & Control */
    cfn_hal_error_code_t (*turn_on)(cfn_sal_dev_display_t *driver);
    cfn_hal_error_code_t (*turn_off)(cfn_sal_dev_display_t *driver);
    cfn_hal_error_code_t (*set_brightness)(cfn_sal_dev_display_t *driver, uint8_t percent);
    cfn_hal_error_code_t (*set_backlight)(cfn_sal_dev_display_t *driver, uint8_t percent);

    /* Drawing Operations */
    cfn_hal_error_code_t (*draw_pixel)(cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint32_t color);
    cfn_hal_error_code_t (*draw_line)(
        cfn_sal_dev_display_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
    cfn_hal_error_code_t (*draw_rect)(
        cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
    cfn_hal_error_code_t (*fill_rect)(
        cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
    cfn_hal_error_code_t (*draw_bitmap)(
        cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);
    cfn_hal_error_code_t (*clear_window)(cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    cfn_hal_error_code_t (*set_cursor)(cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y);

    /* Buffer Management */
    cfn_hal_error_code_t (*clear)(cfn_sal_dev_display_t *driver);
    cfn_hal_error_code_t (*update)(cfn_sal_dev_display_t *driver);

    /* Low-level Communication */
    cfn_hal_error_code_t (*write_data)(cfn_sal_dev_display_t *driver, const uint8_t *data, size_t len);
    cfn_hal_error_code_t (*write_command)(cfn_sal_dev_display_t *driver, uint8_t cmd);

    /* Configuration */
    cfn_hal_error_code_t (*set_orientation)(cfn_sal_dev_display_t *driver, uint8_t orientation);
};

CFN_SAL_DEV_VMT_CHECK(struct cfn_sal_dev_display_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_display,
                           cfn_sal_dev_display_config_t,
                           cfn_sal_dev_display_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_display_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_display_populate(cfn_sal_dev_display_t              *driver,
                                                 uint32_t                            peripheral_id,
                                                 void                               *dependency,
                                                 const cfn_sal_dev_display_api_t    *api,
                                                 const cfn_sal_phy_t                *phy,
                                                 const cfn_sal_dev_display_config_t *config,
                                                 cfn_sal_dev_display_callback_t      callback,
                                                 void                               *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_DISPLAY, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_display_construct(cfn_sal_dev_display_t              *driver,
                                                   const cfn_sal_dev_display_config_t *config,
                                                   const cfn_sal_phy_t                *phy,
                                                   void                               *dependency,
                                                   cfn_sal_dev_display_callback_t      callback,
                                                   void                               *user_arg);
cfn_hal_error_code_t cfn_sal_dev_display_destruct(cfn_sal_dev_display_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_init(cfn_sal_dev_display_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_deinit(cfn_sal_dev_display_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_config_set(cfn_sal_dev_display_t              *driver,
                                                                   const cfn_sal_dev_display_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_config_get(cfn_sal_dev_display_t        *driver,
                                                                   cfn_sal_dev_display_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_callback_register(cfn_sal_dev_display_t         *driver,
                                                                          cfn_sal_dev_display_callback_t callback,
                                                                          void                          *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_DISPLAY, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_power_state_set(cfn_sal_dev_display_t *driver,
                                                                        cfn_hal_power_state_t  state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_event_enable(cfn_sal_dev_display_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_event_disable(cfn_sal_dev_display_t *driver,
                                                                      uint32_t               event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_event_get(cfn_sal_dev_display_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_error_enable(cfn_sal_dev_display_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_error_disable(cfn_sal_dev_display_t *driver,
                                                                      uint32_t               error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_error_get(cfn_sal_dev_display_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_DISPLAY, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_turn_on(cfn_sal_dev_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_DISPLAY, turn_on, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_turn_off(cfn_sal_dev_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_DISPLAY, turn_off, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_set_brightness(cfn_sal_dev_display_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, set_brightness, driver, error, percent);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_draw_pixel(cfn_sal_dev_display_t *driver,
                                                                   uint16_t               x,
                                                                   uint16_t               y,
                                                                   uint32_t               color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, draw_pixel, driver, error, x, y, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_fill_rect(
    cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, fill_rect, driver, error, x, y, w, h, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_draw_bitmap(
    cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, draw_bitmap, driver, error, x, y, w, h, data);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_clear(cfn_sal_dev_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_DISPLAY, clear, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_update(cfn_sal_dev_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_DISPLAY, update, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_set_backlight(cfn_sal_dev_display_t *driver, uint8_t percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, set_backlight, driver, error, percent);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_draw_line(
    cfn_sal_dev_display_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, draw_line, driver, error, x0, y0, x1, y1, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_draw_rect(
    cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, draw_rect, driver, error, x, y, w, h, color);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_sal_dev_display_clear_window(cfn_sal_dev_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, clear_window, driver, error, x, y, w, h);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_set_cursor(cfn_sal_dev_display_t *driver,
                                                                   uint16_t               x,
                                                                   uint16_t               y)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, set_cursor, driver, error, x, y);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_write_data(cfn_sal_dev_display_t *driver,
                                                                   const uint8_t         *data,
                                                                   size_t                 len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, write_data, driver, error, data, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_write_command(cfn_sal_dev_display_t *driver, uint8_t cmd)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, write_command, driver, error, cmd);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_display_set_orientation(cfn_sal_dev_display_t *driver,
                                                                        uint8_t                orientation)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_DISPLAY, set_orientation, driver, error, orientation);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_DISPLAY_H */
