/**
 * @file cfn_svc_test_foundation.cpp
 * @brief Baseline tests to verify the foundational macros and types in Caffeine Services.
 */

#include <gtest/gtest.h>
#include "cfn_svc.h"

// Define a dummy service API to test the macros
struct dummy_svc_api_s
{
    cfn_hal_api_base_t base;
};
CFN_HAL_VMT_CHECK(struct dummy_svc_api_s);

typedef struct
{
    void *dummy;
} dummy_svc_config_t;

typedef struct
{
    void *instance;
} dummy_svc_phy_t;

typedef void (*dummy_svc_callback_t)(void *driver, uint32_t event, uint32_t error, void *arg);

CFN_HAL_CREATE_DRIVER_TYPE(dummy_svc, dummy_svc_config_t, struct dummy_svc_api_s, dummy_svc_phy_t, dummy_svc_callback_t);

TEST(FoundationTest, FourCCGeneration)
{
    uint32_t type = CFN_SVC_TYPE('D', 'U', 'M');
    // Ensure the prefix is correctly packed as the lowest byte (Little Endian representation of the string)
    EXPECT_EQ(type & 0xFF, CFN_SVC_PREFIX);
    EXPECT_EQ((type >> 8) & 0xFF, 'D');
    EXPECT_EQ((type >> 16) & 0xFF, 'U');
    EXPECT_EQ((type >> 24) & 0xFF, 'M');
}

TEST(FoundationTest, DriverInitializer)
{
    struct dummy_svc_api_s api = {};
    dummy_svc_phy_t phy = {};
    dummy_svc_config_t config = {};

    cfn_hal_dummy_svc_t driver = CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE('D', 'U', 'M'), &api, &phy, &config);

    EXPECT_EQ(driver.base.type, CFN_SVC_TYPE('D', 'U', 'M'));
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
    EXPECT_EQ(driver.api, &api);
    EXPECT_EQ(driver.phy, &phy);
    EXPECT_EQ(driver.config, &config);
    EXPECT_EQ(driver.cb, nullptr);
}

TEST(FoundationTest, ErrorBaseOffset)
{
    // Make sure our specific error space starts after typical HAL errors
    EXPECT_GE(CFN_SVC_ERROR_BASE, 0x600);
}
