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
#include "cfn_svc_types.h"

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
    void *instance; /*!< Mapping to I2C/SPI driver or Parallel interface */
    void *user_arg;
} cfn_svc_display_phy_t;

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

    /* Drawing Operations */
    cfn_hal_error_code_t (*draw_pixel)(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint32_t color);
    cfn_hal_error_code_t (*fill_rect)(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
    cfn_hal_error_code_t (*draw_bitmap)(cfn_svc_display_t *driver, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);

    /* Buffer Management */
    cfn_hal_error_code_t (*clear)(cfn_svc_display_t *driver);
    cfn_hal_error_code_t (*update)(cfn_svc_display_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_display_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_display, cfn_svc_display_config_t, cfn_svc_display_api_t, cfn_svc_display_phy_t, cfn_svc_display_callback_t);

#define CFN_SVC_DISPLAY_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                   \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_DISPLAY, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_init(cfn_svc_display_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_DISPLAY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_display_update(cfn_svc_display_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_DISPLAY, update, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_DISPLAY_H */
