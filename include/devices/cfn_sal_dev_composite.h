/**
 * @file cfn_sal_dev_composite.h
 * @brief Standardized shared state for composite (multi-function) sensors.
 */

#ifndef CAFFEINE_SAL_DEV_COMPOSITE_H
#define CAFFEINE_SAL_DEV_COMPOSITE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Standardized shared state for composite (multi-function) sensors.
 * This structure should be embedded within a composite sensor struct
 * to manage shared hardware resources and initialization counts across
 * multiple polymorphic interfaces.
 */
typedef struct
{
    const cfn_sal_phy_t *phy;            /*!< Shared physical interface handle */
    uint8_t              init_ref_count; /*!< Number of active interfaces using this PHY */
    bool                 hw_initialized; /*!< Flag indicating if physical hardware is ready */
} cfn_sal_dev_composite_shared_t;

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initialize the shared composite state.
 *
 * @param state Pointer to the shared state structure.
 * @param phy   Pointer to the shared physical interface mapping.
 * @return CFN_HAL_ERROR_OK on success.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_composite_init(cfn_sal_dev_composite_shared_t *state,
                                                               const cfn_sal_phy_t            *phy)
{
    if ((state == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    state->phy            = phy;
    state->init_ref_count = 0;
    state->hw_initialized = false;

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Deinitialize the shared composite state.
 *
 * @param state Pointer to the shared state structure.
 * @return CFN_HAL_ERROR_OK on success.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_composite_deinit(cfn_sal_dev_composite_shared_t *state)
{
    if (state == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    state->phy            = NULL;
    state->init_ref_count = 0;
    state->hw_initialized = false;

    return CFN_HAL_ERROR_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_COMPOSITE_H */
