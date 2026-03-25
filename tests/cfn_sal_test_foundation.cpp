/**
 * @file cfn_sal_test_foundation.cpp
 * @brief Baseline tests to verify the foundational macros and types in Caffeine SAL.
 */

#include <gtest/gtest.h>
#include "cfn_sal.h"

// Define a dummy service API to test the macros
struct dummy_sal_api_s
{
    cfn_hal_api_base_t base;
};
CFN_HAL_VMT_CHECK(struct dummy_sal_api_s);

typedef struct
{
    void *dummy;
} dummy_sal_config_t;

typedef void (*dummy_sal_callback_t)(void *driver, uint32_t event, uint32_t error, void *arg);

CFN_SAL_CREATE_DRIVER_TYPE(dummy_sal, dummy_sal_config_t, struct dummy_sal_api_s, cfn_sal_phy_t, dummy_sal_callback_t);

CFN_HAL_INLINE void cfn_dummy_sal_populate(cfn_dummy_sal_t              *driver,
                                           uint32_t                      peripheral_id,
                                           const struct dummy_sal_api_s *api,
                                           const cfn_sal_phy_t          *phy,
                                           const dummy_sal_config_t     *config,
                                           dummy_sal_callback_t          callback,
                                           void                         *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, peripheral_id, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

TEST(FoundationTest, FourCCGeneration)
{
    uint32_t type = CFN_SAL_TYPE('D', 'U', 'M');
    // Ensure the prefix is correctly packed as the lowest byte (Little Endian representation of the string)
    EXPECT_EQ(type & 0xFF, CFN_SAL_PREFIX);
    EXPECT_EQ((type >> 8) & 0xFF, 'D');
    EXPECT_EQ((type >> 16) & 0xFF, 'U');
    EXPECT_EQ((type >> 24) & 0xFF, 'M');
}

TEST(FoundationTest, DriverPopulate)
{
    struct dummy_sal_api_s api = {};
    cfn_sal_phy_t          phy = {};
    dummy_sal_config_t     config = {};

    cfn_dummy_sal_t driver{};
    cfn_dummy_sal_populate(&driver, CFN_SAL_TYPE('D', 'U', 'M'), &api, &phy, &config, NULL, NULL);

    EXPECT_EQ(driver.base.type, CFN_SAL_TYPE('D', 'U', 'M'));
    EXPECT_EQ(driver.base.status, CFN_HAL_DRIVER_STATUS_CONSTRUCTED);
    EXPECT_EQ(driver.api, &api);
    EXPECT_EQ(driver.phy, &phy);
    EXPECT_EQ(driver.config, &config);
    EXPECT_EQ(driver.cb, nullptr);
}

TEST(FoundationTest, ErrorBaseOffset)
{
    // Make sure our specific error space starts after typical HAL errors
    EXPECT_GE(CFN_SAL_ERROR_BASE, 0x600);
}
