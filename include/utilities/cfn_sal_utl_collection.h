/**
 * @file cfn_sal_utl_collection.h
 * @brief Unified polymorphic interface for data collections (Ring Buffers, Lists, Queues).
 */

#ifndef CAFFEINE_SAL_UTL_COLLECTION_H
#define CAFFEINE_SAL_UTL_COLLECTION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"
#include "cfn_sal_device.h"

/* Defines ----------------------------------------------------------*/

/**
 * @brief FourCC type for Collections.
 */
#define CFN_SAL_UTL_TYPE_COLLECTION CFN_SAL_TYPE('C', 'O', 'L')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Collection configuration (e.g. item size).
 */
typedef struct
{
    size_t item_size; /*!< Size of a single element in the collection */
    void  *custom;    /*!< Vendor-specific custom configuration */
} cfn_sal_utl_collection_config_t;

typedef struct cfn_sal_utl_collection_s     cfn_sal_utl_collection_t;
typedef struct cfn_sal_utl_collection_api_s cfn_sal_utl_collection_api_t;

/**
 * @brief Collection callback signature.
 */
typedef void (*cfn_sal_utl_collection_callback_t)(cfn_sal_utl_collection_t *driver,
                                                  uint32_t                  event_mask,
                                                  uint32_t                  error_mask,
                                                  void                     *user_arg);

/**
 * @brief Collection Virtual Method Table (VMT).
 */
struct cfn_sal_utl_collection_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Write Operations */
    cfn_hal_error_code_t (*push_back)(cfn_sal_utl_collection_t *driver, const void *item);
    cfn_hal_error_code_t (*push_front)(cfn_sal_utl_collection_t *driver, const void *item);
    cfn_hal_error_code_t (*insert_at)(cfn_sal_utl_collection_t *driver, size_t index, const void *item);

    /* Read/Remove Operations */
    cfn_hal_error_code_t (*pop_back)(cfn_sal_utl_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*pop_front)(cfn_sal_utl_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*remove_at)(cfn_sal_utl_collection_t *driver, size_t index, void *item_out);

    /* Inspection Operations */
    cfn_hal_error_code_t (*peek_back)(cfn_sal_utl_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*peek_front)(cfn_sal_utl_collection_t *driver, void *item_out);
    cfn_hal_error_code_t (*peek_at)(cfn_sal_utl_collection_t *driver, size_t index, void *item_out);
    cfn_hal_error_code_t (*find)(cfn_sal_utl_collection_t *driver, const void *item, size_t *index_out);

    /* State Operations */
    cfn_hal_error_code_t (*get_size)(cfn_sal_utl_collection_t *driver, size_t *size_out);
    cfn_hal_error_code_t (*get_capacity)(cfn_sal_utl_collection_t *driver, size_t *capacity_out);
    cfn_hal_error_code_t (*is_empty)(cfn_sal_utl_collection_t *driver, bool *is_empty_out);
    cfn_hal_error_code_t (*is_full)(cfn_sal_utl_collection_t *driver, bool *is_full_out);
    cfn_hal_error_code_t (*clear)(cfn_sal_utl_collection_t *driver);
    cfn_hal_error_code_t (*sort)(cfn_sal_utl_collection_t *driver, void *compare_func);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_utl_collection_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_utl_collection,
                           cfn_sal_utl_collection_config_t,
                           cfn_sal_utl_collection_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_utl_collection_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_utl_collection_populate(cfn_sal_utl_collection_t              *driver,
                                                    uint32_t                               peripheral_id,
                                                    void                                  *dependency,
                                                    const cfn_sal_utl_collection_api_t    *api,
                                                    const cfn_sal_phy_t                   *phy,
                                                    const cfn_sal_utl_collection_config_t *config,
                                                    cfn_sal_utl_collection_callback_t      callback,
                                                    void                                  *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_UTL_TYPE_COLLECTION, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_utl_collection_construct(cfn_sal_utl_collection_t              *driver,
                                                      const cfn_sal_utl_collection_config_t *config,
                                                      const cfn_sal_phy_t                   *phy,
                                                      void                                  *dependency,
                                                      cfn_sal_utl_collection_callback_t      callback,
                                                      void                                  *user_arg);
cfn_hal_error_code_t cfn_sal_utl_collection_destruct(cfn_sal_utl_collection_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_config_validate(
    const cfn_sal_utl_collection_t *driver, const cfn_sal_utl_collection_config_t *config)
{
    if (!driver || !config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_config_validate(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, config);
}

/**
 * @brief Initializes the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_init(cfn_sal_utl_collection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_sal_utl_collection_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION);
}

/**
 * @brief Deinitializes the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_deinit(cfn_sal_utl_collection_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION);
}

/**
 * @brief Sets the collection configuration.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_config_set(cfn_sal_utl_collection_t              *driver,
                                                                      const cfn_sal_utl_collection_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_sal_utl_collection_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, (const void *) config);
}

/**
 * @brief Gets the current collection configuration.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_config_get(cfn_sal_utl_collection_t        *driver,
                                                                      cfn_sal_utl_collection_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for collection events and errors.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_callback_register(cfn_sal_utl_collection_t         *driver,
                                                                             cfn_sal_utl_collection_callback_t callback,
                                                                             void                             *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_UTL_TYPE_COLLECTION, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the collection power state.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_power_state_set(cfn_sal_utl_collection_t *driver,
                                                                           cfn_hal_power_state_t     state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, state);
}

/**
 * @brief Enables one or more collection nominal events.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_event_enable(cfn_sal_utl_collection_t *driver,
                                                                        uint32_t                  event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, event_mask);
}

/**
 * @brief Disables one or more collection nominal events.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_event_disable(cfn_sal_utl_collection_t *driver,
                                                                         uint32_t                  event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, event_mask);
}

/**
 * @brief Retrieves the current collection nominal event status.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_event_get(cfn_sal_utl_collection_t *driver,
                                                                     uint32_t                 *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, event_mask);
}

/**
 * @brief Enables one or more collection exception errors.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_error_enable(cfn_sal_utl_collection_t *driver,
                                                                        uint32_t                  error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, error_mask);
}

/**
 * @brief Disables one or more collection exception errors.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_error_disable(cfn_sal_utl_collection_t *driver,
                                                                         uint32_t                  error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, error_mask);
}

/**
 * @brief Retrieves the current collection exception error status.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_error_get(cfn_sal_utl_collection_t *driver,
                                                                     uint32_t                 *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_UTL_TYPE_COLLECTION, error_mask);
}

/* Service Specific Functions --------------------------------------- */

/**
 * @brief Adds an item to the end of the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_push_back(cfn_sal_utl_collection_t *driver, const void *item)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, push_back, driver, error, item);
    return error;
}

/**
 * @brief Adds an item to the front of the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_push_front(cfn_sal_utl_collection_t *driver,
                                                                      const void               *item)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, push_front, driver, error, item);
    return error;
}

/**
 * @brief Inserts an item at a specific index.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_insert_at(cfn_sal_utl_collection_t *driver,
                                                                     size_t                    index,
                                                                     const void               *item)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, insert_at, driver, error, index, item);
    return error;
}

/**
 * @brief Removes and retrieves an item from the back.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_pop_back(cfn_sal_utl_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, pop_back, driver, error, item_out);
    return error;
}

/**
 * @brief Removes and retrieves an item from the front.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_pop_front(cfn_sal_utl_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, pop_front, driver, error, item_out);
    return error;
}

/**
 * @brief Removes and retrieves an item at a specific index.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_remove_at(cfn_sal_utl_collection_t *driver,
                                                                     size_t                    index,
                                                                     void                     *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, remove_at, driver, error, index, item_out);
    return error;
}

/**
 * @brief Peeks at an item from the back.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_peek_back(cfn_sal_utl_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, peek_back, driver, error, item_out);
    return error;
}

/**
 * @brief Peeks at an item from the front.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_peek_front(cfn_sal_utl_collection_t *driver, void *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, peek_front, driver, error, item_out);
    return error;
}

/**
 * @brief Peeks at an item at a specific index.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_peek_at(cfn_sal_utl_collection_t *driver,
                                                                   size_t                    index,
                                                                   void                     *item_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, peek_at, driver, error, index, item_out);
    return error;
}

/**
 * @brief Retrieves the current number of items.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_get_size(cfn_sal_utl_collection_t *driver, size_t *size_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, get_size, driver, error, size_out);
    return error;
}

/**
 * @brief Retrieves the capacity of the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_get_capacity(cfn_sal_utl_collection_t *driver,
                                                                        size_t                   *capacity_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, get_capacity, driver, error, capacity_out);
    return error;
}

/**
 * @brief Checks if the collection is empty.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_is_empty(cfn_sal_utl_collection_t *driver,
                                                                    bool                     *is_empty_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, is_empty, driver, error, is_empty_out);
    return error;
}

/**
 * @brief Checks if the collection is full.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_is_full(cfn_sal_utl_collection_t *driver, bool *is_full_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, is_full, driver, error, is_full_out);
    return error;
}

/**
 * @brief Clears all items from the collection.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_clear(cfn_sal_utl_collection_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_UTL_TYPE_COLLECTION, clear, driver, error);
    return error;
}

/**
 * @brief Finds an item in the collection and retrieves its index.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_find(cfn_sal_utl_collection_t *driver,
                                                                const void               *item,
                                                                size_t                   *index_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, find, driver, error, item, index_out);
    return error;
}

/**
 * @brief Sorts the collection using the provided comparison function.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_collection_sort(cfn_sal_utl_collection_t *driver, void *compare_func)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_COLLECTION, sort, driver, error, compare_func);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTL_COLLECTION_H */
