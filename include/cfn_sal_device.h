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
 * @file cfn_sal_device.h
 * @brief Common Device API Trait for SAL components.
 */

#ifndef CAFFEINE_SAL_CFN_SAL_DEVICE_H
#define CAFFEINE_SAL_CFN_SAL_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_types.h"

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Common Device API Trait for SAL components.
 * Provides standard hardware-level device queries and interrupt handling
 * separate from the driver's primary functional interface.
 */
typedef struct
{
    cfn_hal_error_code_t (*get_id)(cfn_hal_driver_t *base, uint32_t *id_out);
    cfn_hal_error_code_t (*handle_interrupt)(cfn_hal_driver_t *base);
} cfn_sal_dev_api_t;

/**
 * @brief Compile-time check to ensure a SAL Device API struct is compatible
 * with the polymorphic device trait. All SAL device APIs must have:
 * 1. 'cfn_hal_api_base_t base' as their FIRST member (verified via HAL check).
 * 2. 'cfn_sal_dev_api_t dev' as their SECOND member.
 */
#define CFN_SAL_DEV_VMT_CHECK(api_struct_type)                                                                         \
    CFN_HAL_VMT_CHECK(api_struct_type);                                                                                \
    CFN_HAL_STATIC_ASSERT(offsetof(api_struct_type, dev) == sizeof(cfn_hal_api_base_t),                                \
                          "cfn_sal_dev_api_t must be the second member of the VMT struct")

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Generic getter for device FourCC/Hardware ID.
 * @param driver Pointer to the SAL driver instance (cast to void* or base).
 * @param id_out [out] Pointer to store the ID.
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_sal_dev_get_id(void *driver, uint32_t *id_out)
{
    cfn_hal_driver_t *base = (cfn_hal_driver_t *) driver;
    if (base && base->vmt)
    {
        /*
         * Note: Every SAL device VMT MUST have cfn_sal_dev_api_t as its second
         * member (immediately following cfn_hal_api_base_t) to support
         * this polymorphic query.
         */
        const cfn_sal_dev_api_t *dev_api =
            (const cfn_sal_dev_api_t *) ((const uint8_t *) base->vmt + sizeof(cfn_hal_api_base_t));
        if (dev_api->get_id)
        {
            return dev_api->get_id(base, id_out);
        }
    }
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Generic hardware interrupt handler for SAL devices.
 * @param driver Pointer to the SAL driver instance (cast to void* or base).
 * @return CFN_HAL_ERROR_OK on success.
 */
static inline cfn_hal_error_code_t cfn_sal_dev_handle_interrupt(void *driver)
{
    cfn_hal_driver_t *base = (cfn_hal_driver_t *) driver;
    if (base && base->vmt)
    {
        const cfn_sal_dev_api_t *dev_api =
            (const cfn_sal_dev_api_t *) ((const uint8_t *) base->vmt + sizeof(cfn_hal_api_base_t));
        if (dev_api->handle_interrupt)
        {
            return dev_api->handle_interrupt(base);
        }
    }
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_CFN_SAL_DEVICE_H */
