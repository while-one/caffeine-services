/**
 * @file cfn_sal_test_collection.cpp
 * @brief Unit tests for the unified Collection service interface.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "utilities/cfn_sal_utl_collection.h"

class CollectionTest : public ::testing::Test
{
  protected:
    cfn_sal_utl_collection_t     driver{};
    cfn_sal_utl_collection_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_SAL_UTL_TYPE_COLLECTION;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

// --- Big 4 Negative Tests ---

TEST_F(CollectionTest, NullDriverReturnsBadParam)
{
    EXPECT_EQ(cfn_sal_utl_collection_init(nullptr), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_sal_utl_collection_deinit(nullptr), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CollectionTest, WrongPeripheralTypeReturnsBadParam)
{
    driver.base.type = CFN_HAL_PERIPHERAL_TYPE_UNKNOWN;
    EXPECT_EQ(cfn_sal_utl_collection_init(&driver), CFN_HAL_ERROR_BAD_PARAM);
    EXPECT_EQ(cfn_sal_utl_collection_deinit(&driver), CFN_HAL_ERROR_BAD_PARAM);
}

TEST_F(CollectionTest, UnimplementedApiReturnsNotSupported)
{
    driver.base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    // api.push_back is null by default
    EXPECT_EQ(cfn_sal_utl_collection_push_back(&driver, nullptr), CFN_HAL_ERROR_NOT_SUPPORTED);
}

TEST_F(CollectionTest, OnConfigFailureAbortsInit)
{
    driver.base.on_config = [](cfn_hal_driver_t *b, void *arg, cfn_hal_config_phase_t phase) -> cfn_hal_error_code_t
    {
        (void) b;
        (void) arg;
        return (phase == CFN_HAL_CONFIG_PHASE_INIT) ? CFN_HAL_ERROR_FAIL : CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_sal_utl_collection_init(&driver), CFN_HAL_ERROR_FAIL);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
}

// --- Lifecycle Tests ---

TEST_F(CollectionTest, InitSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_sal_utl_collection_init(&driver), CFN_HAL_ERROR_OK);
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_INITIALIZED);
}

// --- Functional Tests (Mocks) ---

TEST_F(CollectionTest, PushPopSizeSuccess)
{
    static int mock_val = 0;

    api.push_back       = [](cfn_sal_utl_collection_t *d, const void *item) -> cfn_hal_error_code_t
    {
        (void) d;
        mock_val = *(const int *) item;
        return CFN_HAL_ERROR_OK;
    };

    api.pop_front = [](cfn_sal_utl_collection_t *d, void *item_out) -> cfn_hal_error_code_t
    {
        (void) d;
        *(int *) item_out = mock_val;
        return CFN_HAL_ERROR_OK;
    };

    api.get_size = [](cfn_sal_utl_collection_t *d, size_t *s) -> cfn_hal_error_code_t
    {
        (void) d;
        *s = 1;
        return CFN_HAL_ERROR_OK;
    };

    int    input  = 42;
    int    output = 0;
    size_t size   = 0;

    EXPECT_EQ(cfn_sal_utl_collection_push_back(&driver, &input), CFN_HAL_ERROR_OK);
    EXPECT_EQ(cfn_sal_utl_collection_get_size(&driver, &size), CFN_HAL_ERROR_OK);
    EXPECT_EQ(size, 1);
    EXPECT_EQ(cfn_sal_utl_collection_pop_front(&driver, &output), CFN_HAL_ERROR_OK);
    EXPECT_EQ(output, 42);
}

TEST_F(CollectionTest, ClearSuccess)
{
    api.clear = [](cfn_sal_utl_collection_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_sal_utl_collection_clear(&driver), CFN_HAL_ERROR_OK);
}
