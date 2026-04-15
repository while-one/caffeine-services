/**
 * @file cfn_sal_dev_test_devices.cpp
 * @brief Verification tests for hardware device service interfaces.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "devices/cfn_sal_dev_accelerometer.h"
#include "devices/cfn_sal_dev_ambient_light.h"
#include "devices/cfn_sal_dev_battery.h"
#include "devices/cfn_sal_dev_button.h"
#include "devices/cfn_sal_dev_color.h"
#include "devices/cfn_sal_dev_composite.h"
#include "devices/cfn_sal_dev_display.h"
#include "devices/cfn_sal_dev_gnss.h"
#include "devices/cfn_sal_dev_gsm.h"
#include "devices/cfn_sal_dev_gyroscope.h"
#include "devices/cfn_sal_dev_humidity.h"
#include "devices/cfn_sal_dev_led.h"
#include "devices/cfn_sal_dev_magnetometer.h"
#include "devices/cfn_sal_dev_pressure.h"
#include "devices/cfn_sal_dev_proximity.h"
#include "devices/cfn_sal_dev_temperature.h"

// --- LED Tests ---

class LedTest : public ::testing::Test
{
  protected:
    cfn_sal_dev_led_t     driver{};
    cfn_sal_dev_led_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_SAL_DEV_TYPE_LED;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

TEST_F(LedTest, LifecycleSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_sal_dev_led_init(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(LedTest, ToggleSuccess)
{
    static bool toggled = false;
    api.toggle          = [](cfn_sal_dev_led_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        toggled = true;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_sal_dev_led_toggle(&driver), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(toggled);
}

// --- Button Tests ---

class ButtonTest : public ::testing::Test
{
  protected:
    cfn_sal_dev_button_t     driver{};
    cfn_sal_dev_button_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_SAL_DEV_TYPE_BUTTON;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

TEST_F(ButtonTest, IsPressedSuccess)
{
    api.is_pressed = [](cfn_sal_dev_button_t *d, bool *p) -> cfn_hal_error_code_t
    {
        (void) d;
        *p = true;
        return CFN_HAL_ERROR_OK;
    };
    bool pressed = false;
    EXPECT_EQ(cfn_sal_dev_button_is_pressed(&driver, &pressed), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(pressed);
}

// --- Battery Tests ---

class BatteryTest : public ::testing::Test
{
  protected:
    cfn_sal_dev_battery_t     driver{};
    cfn_sal_dev_battery_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_SAL_DEV_TYPE_BATTERY;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

TEST_F(BatteryTest, GetVoltageSuccess)
{
    api.get_voltage = [](cfn_sal_dev_battery_t *d, float *v) -> cfn_hal_error_code_t
    {
        (void) d;
        *v = 3.7f;
        return CFN_HAL_ERROR_OK;
    };
    float volts = 0;
    EXPECT_EQ(cfn_sal_dev_battery_get_voltage(&driver, &volts), CFN_HAL_ERROR_OK);
    EXPECT_NEAR(volts, 3.7f, 0.01f);
}

// --- GNSS Tests ---

class GnssTest : public ::testing::Test
{
  protected:
    cfn_sal_dev_gnss_t     driver{};
    cfn_sal_dev_gnss_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type   = CFN_SAL_DEV_TYPE_GNSS;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt    = (const struct cfn_hal_api_base_s *) &api;
        driver.api         = &api;
    }
};

TEST_F(GnssTest, GetLocationSuccess)
{
    api.get_location = [](cfn_sal_dev_gnss_t *d, cfn_sal_dev_gnss_location_t *loc) -> cfn_hal_error_code_t
    {
        (void) d;
        loc->latitude = 45.0;
        return CFN_HAL_ERROR_OK;
    };
    cfn_sal_dev_gnss_location_t loc = {};
    EXPECT_EQ(cfn_sal_dev_gnss_get_location(&driver, &loc), CFN_HAL_ERROR_OK);
    EXPECT_DOUBLE_EQ(loc.latitude, 45.0);
}
