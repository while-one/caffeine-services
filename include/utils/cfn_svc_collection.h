/**
 * @file cfn_svc_collection.h
 * @brief Unified polymorphic interface for data collections (Ring Buffers, Lists, Queues).
 */

#ifndef CAFFEINE_SERVICES_UTILS_CFN_SVC_COLLECTION_H
#define CAFFEINE_SERVICES_UTILS_CFN_SVC_COLLECTION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

/**
 * @brief FourCC type for Collections.
 */
#define CFN_SVC_TYPE_COLLECTION CFN_SVC_TYPE('C', 'O', 'L')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Collection physical mapping (e.g. underlying buffer and its size).
 */
typedef struct
{
    void  *buffer;   /*!< Pointer to the static memory allocated for storage */
    size_t size;     /*!< Size of the buffer in bytes or elements (implementation-defined) */
    void  *user_arg; /*!< implementation-specific metadata */
} cfn_svc_collection_phy_t;

/**
 * @brief Collection configuration (e.g. item size).
 */
typedef struct
{
    size_t item_size; /*!< Size of a single element in the collection */
    void  *custom;    /*!< Vendor-specific custom configuration */
} cfn_svc_collection_config_t;

typedef struct cfn_svc_collection_s     cfn_svc_collection_t;
typedef struct cfn_svc_collection_api_s cfn_svc_collection_api_t;

/**
 * @brief Collection callback signature.
 */
typedef void (*cfn_svc_collection_callback_t)(cfn_svc_collection_t *driver,
                                               uint32_t              event_mask,
                                               uint32_t              error_mask,
                                               void                 *user_arg);

/**
 * @brief Collection Virtual Method Table (VMT).
 */
struct cfn_svc_collection_api_s
{
    cfn_hal_api_base_t base;

    /* Write Operations */
    cfn_hal_error_code_t (*push_back)(cfn_svc_collection_t *driver, const void *item);
    cfn_hal_error_code_t (*push_front)(cfn_svc_collection_t *driver, const void *item);
    cfn_hal_error_code_t (*insert_at)(cfn_svc_collection_t *driver, size_t index, const void *item);

    /* Read/Remove Operations */
    cfn_hal_error_code_t (*pop_back)(cfn_svc_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*pop_front)(cfn_svc_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*remove_at)(cfn_svc_collection_t *driver, size_t index, void *item_out);

    /* Inspection Operations */
    cfn_hal_error_code_t (*peek_back)(cfn_svc_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*peek_front)(cfn_svc_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*peek_at)(cfn_svc_collection_t *driver, size_t index, void *item_out);

    /* State Operations */
    cfn_hal_error_code_t (*get_size)(cfn_svc_collection_t *driver, size_t *size_out);
    cfn_hal_error_code_t (*get_capacity)(cfn_svc_collection_t *driver, size_t *capacity_out);
    cfn_hal_error_code_t (*is_empty)(cfn_svc_collection_t *driver, bool *is_empty_out);
    cfn_hal_error_code_t (*is_full)(cfn_svc_collection_t *driver, bool *is_full_out);
    cfn_hal_error_code_t (*clear)(cfn_svc_collection_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_collection_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_collection,
                           cfn_svc_collection_config_t,
                           cfn_svc_collection_api_t,
                           cfn_svc_collection_phy_t,
                           cfn_svc_collection_callback_t);

#define CFN_SVC_COLLECTION_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                   \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_COLLECTION, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_init(cfn_svc_collection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_COLLECTION);
}

/**
 * @brief Deinitializes the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_deinit(cfn_svc_collection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_COLLECTION);
}

/**
 * @brief Adds an item to the end of the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_push_back(cfn_svc_collection_t *driver, const void *item)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_COLLECTION, push_back, driver, error, item);
    return error;
}

/**
 * @brief Adds an item to the front of the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_push_front(cfn_svc_collection_t *driver, const void *item)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_COLLECTION, push_front, driver, error, item);
    return error;
}

/**
 * @brief Removes and retrieves an item from the back.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_pop_back(cfn_svc_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_COLLECTION, pop_back, driver, error, item_out);
    return error;
}

/**
 * @brief Removes and retrieves an item from the front.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_pop_front(cfn_svc_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_COLLECTION, pop_front, driver, error, item_out);
    return error;
}

/**
 * @brief Retrieves the current number of items.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_get_size(cfn_svc_collection_t *driver, size_t *size_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_COLLECTION, get_size, driver, error, size_out);
    return error;
}

/**
 * @brief Clears all items from the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_collection_clear(cfn_svc_collection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_COLLECTION, clear, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_UTILS_CFN_SVC_COLLECTION_H */
