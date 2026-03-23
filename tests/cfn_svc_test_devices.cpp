/**
 * @file cfn_svc_test_devices.cpp
 * @brief Verification tests for hardware device service interfaces.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "devices/cfn_svc_led.h"
#include "devices/cfn_svc_button.h"
#include "devices/cfn_svc_temp_sensor.h"
#include "devices/cfn_svc_hum_sensor.h"
#include "devices/cfn_svc_accel.h"
#include "devices/cfn_svc_gsm.h"
#include "devices/cfn_svc_battery.h"
#include "devices/cfn_svc_light_sensor.h"
#include "devices/cfn_svc_pressure_sensor.h"
#include "devices/cfn_svc_gnss.h"
#include "devices/cfn_svc_display.h"
#include "network/cfn_svc_connection.h"
#include "network/cfn_svc_transport.h"
#include "utilities/cfn_svc_cli.h"
#include "utilities/cfn_svc_at_parser.h"
#include "utilities/cfn_svc_fs.h"
#include "utilities/cfn_svc_logging.h"

// --- LED Tests ---

class LedTest : public ::testing::Test
{
  protected:
    cfn_svc_led_t     driver{};
    cfn_svc_led_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_LED;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(LedTest, LifecycleSuccess)
{
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_svc_led_init(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(LedTest, ToggleSuccess)
{
    static bool toggled = false;
    api.toggle = [](cfn_svc_led_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        toggled = true;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_svc_led_toggle(&driver), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(toggled);
}

// --- Connection Tests ---

class ConnectionTest : public ::testing::Test
{
  protected:
    cfn_svc_connection_t     driver{};
    cfn_svc_connection_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_CONNECTION;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(ConnectionTest, ConnectSuccess)
{
    api.connect = [](cfn_svc_connection_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_svc_connection_connect(&driver), CFN_HAL_ERROR_OK);
}

// --- Transport Tests ---

class TransportTest : public ::testing::Test
{
  protected:
    cfn_svc_transport_t     driver{};
    cfn_svc_transport_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_TRANSPORT;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(TransportTest, SendSyncSuccess)
{
    api.send = [](cfn_svc_transport_t *d, const uint8_t *data, size_t len, uint32_t timeout) -> cfn_hal_error_code_t
    {
        (void) d;
        (void) data;
        (void) len;
        (void) timeout;
        return CFN_HAL_ERROR_OK;
    };
    uint8_t data[10] = { 0 };
    EXPECT_EQ(cfn_svc_transport_send(&driver, data, 10, 100), CFN_HAL_ERROR_OK);
}

TEST_F(TransportTest, SendAsyncSuccess)
{
    api.send_async = [](cfn_svc_transport_t *d, const uint8_t *data, size_t len) -> cfn_hal_error_code_t
    {
        (void) d;
        (void) data;
        (void) len;
        return CFN_HAL_ERROR_OK;
    };
    uint8_t data[10] = { 0 };
    EXPECT_EQ(cfn_svc_transport_send_async(&driver, data, 10), CFN_HAL_ERROR_OK);
}

// --- Button Tests ---

class ButtonTest : public ::testing::Test
{
  protected:
    cfn_svc_button_t     driver{};
    cfn_svc_button_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_BUTTON;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(ButtonTest, IsPressedSuccess)
{
    api.is_pressed = [](cfn_svc_button_t *d, bool *p) -> cfn_hal_error_code_t
    {
        (void) d;
        *p = true;
        return CFN_HAL_ERROR_OK;
    };
    bool pressed = false;
    EXPECT_EQ(cfn_svc_button_is_pressed(&driver, &pressed), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(pressed);
}

// --- Battery Tests ---

class BatteryTest : public ::testing::Test
{
  protected:
    cfn_svc_battery_t     driver{};
    cfn_svc_battery_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_BATTERY;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(BatteryTest, GetVoltageSuccess)
{
    api.get_voltage = [](cfn_svc_battery_t *d, float *v) -> cfn_hal_error_code_t
    {
        (void) d;
        *v = 3.7f;
        return CFN_HAL_ERROR_OK;
    };
    float volts = 0;
    EXPECT_EQ(cfn_svc_battery_get_voltage(&driver, &volts), CFN_HAL_ERROR_OK);
    EXPECT_NEAR(volts, 3.7f, 0.01f);
}

// --- GNSS Tests ---

class GnssTest : public ::testing::Test
{
  protected:
    cfn_svc_gnss_t     driver{};
    cfn_svc_gnss_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_GNSS;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(GnssTest, GetLocationSuccess)
{
    api.get_location = [](cfn_svc_gnss_t *d, cfn_svc_gnss_location_t *loc) -> cfn_hal_error_code_t
    {
        (void) d;
        loc->latitude = 45.0;
        return CFN_HAL_ERROR_OK;
    };
    cfn_svc_gnss_location_t loc = {};
    EXPECT_EQ(cfn_svc_gnss_get_location(&driver, &loc), CFN_HAL_ERROR_OK);
    EXPECT_DOUBLE_EQ(loc.latitude, 45.0);
}

// --- FS Tests ---

class FsTest : public ::testing::Test
{
  protected:
    cfn_svc_fs_t     driver{};
    cfn_svc_fs_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_FS;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(FsTest, MountSuccess)
{
    api.mount = [](cfn_svc_fs_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_svc_fs_mount(&driver), CFN_HAL_ERROR_OK);
}

// --- Logging Tests ---

class LoggingTest : public ::testing::Test
{
  protected:
    cfn_svc_logging_t     driver{};
    cfn_svc_logging_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_LOGGING;
        driver.base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
        driver.base.vmt = (const struct cfn_hal_api_base_s *) &api;
        driver.api = &api;
    }
};

TEST_F(LoggingTest, FlushSuccess)
{
    api.flush = [](cfn_svc_logging_t *d) -> cfn_hal_error_code_t
    {
        (void) d;
        return CFN_HAL_ERROR_OK;
    };
    EXPECT_EQ(cfn_svc_log_flush(&driver), CFN_HAL_ERROR_OK);
}
