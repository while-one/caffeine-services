/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_sal_dev_gpio_expander.h
 * @brief Abstract GPIO Expander service interface.
 */

#ifndef CAFFEINE_SAL_DEV_GPIO_EXPANDER_H
#define CAFFEINE_SAL_DEV_GPIO_EXPANDER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_GPIO_EXPANDER CFN_SAL_TYPE('G', 'X', 'P')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *custom;
} cfn_sal_dev_gpio_expander_config_t;

typedef struct cfn_sal_dev_gpio_expander_s     cfn_sal_dev_gpio_expander_t;
typedef struct cfn_sal_dev_gpio_expander_api_s cfn_sal_dev_gpio_expander_api_t;

typedef void (*cfn_sal_dev_gpio_expander_callback_t)(cfn_sal_dev_gpio_expander_t *driver,
                                                     uint32_t                     event,
                                                     uint32_t                     error,
                                                     void                        *user_arg);

/**
 * @brief GPIO Expander Virtual Method Table (VMT).
 */
struct cfn_sal_dev_gpio_expander_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;
};

CFN_HAL_VMT_CHECK(struct cfn_sal_dev_gpio_expander_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_gpio_expander,
                           cfn_sal_dev_gpio_expander_config_t,
                           cfn_sal_dev_gpio_expander_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_gpio_expander_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_gpio_expander_populate(cfn_sal_dev_gpio_expander_t              *driver,
                                                       uint32_t                                  peripheral_id,
                                                       void                                     *dependency,
                                                       const cfn_sal_dev_gpio_expander_api_t    *api,
                                                       const cfn_sal_phy_t                      *phy,
                                                       const cfn_sal_dev_gpio_expander_config_t *config,
                                                       cfn_sal_dev_gpio_expander_callback_t      callback,
                                                       void                                     *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_GPIO_EXPANDER, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_gpio_expander_construct(cfn_sal_dev_gpio_expander_t              *driver,
                                                         const cfn_sal_dev_gpio_expander_config_t *config,
                                                         const cfn_sal_phy_t                      *phy,
                                                         void                                     *dependency,
                                                         cfn_sal_dev_gpio_expander_callback_t      callback,
                                                         void                                     *user_arg);

cfn_hal_error_code_t cfn_sal_dev_gpio_expander_destruct(cfn_sal_dev_gpio_expander_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gpio_expander_init(cfn_sal_dev_gpio_expander_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_GPIO_EXPANDER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gpio_expander_deinit(cfn_sal_dev_gpio_expander_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_GPIO_EXPANDER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gpio_expander_handle_interrupt(cfn_sal_dev_gpio_expander_t *driver)
{
    return cfn_sal_dev_handle_interrupt((void *) driver);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gpio_expander_get_id(cfn_sal_dev_gpio_expander_t *driver,
                                                                     uint32_t                    *id_out)
{
    return cfn_sal_dev_get_id((void *) driver, id_out);
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_GPIO_EXPANDER_H */
