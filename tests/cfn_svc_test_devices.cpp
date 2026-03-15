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
    api.base.init = [](cfn_hal_driver_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_led_init(&driver), CFN_HAL_ERROR_OK);
}

TEST_F(LedTest, ToggleSuccess)
{
    static bool toggled = false;
    api.toggle = [](cfn_svc_led_t *d) -> cfn_hal_error_code_t { toggled = true; return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_led_toggle(&driver), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(toggled);
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
        driver.api = &api;
    }
};

TEST_F(ButtonTest, IsPressedSuccess)
{
    api.is_pressed = [](cfn_svc_button_t *d, bool *p) -> cfn_hal_error_code_t { *p = true; return CFN_HAL_ERROR_OK; };
    bool pressed = false;
    EXPECT_EQ(cfn_svc_button_is_pressed(&driver, &pressed), CFN_HAL_ERROR_OK);
    EXPECT_TRUE(pressed);
}

// --- Sensor Tests ---

class TempSensorTest : public ::testing::Test
{
  protected:
    cfn_svc_temp_sensor_t     driver{};
    cfn_svc_temp_sensor_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_TEMP_SENSOR;
        driver.api = &api;
    }
};

TEST_F(TempSensorTest, ReadCelsiusSuccess)
{
    api.read_celsius = [](cfn_svc_temp_sensor_t *d, float *t) -> cfn_hal_error_code_t { *t = 25.5f; return CFN_HAL_ERROR_OK; };
    float temp = 0;
    EXPECT_EQ(cfn_svc_temp_sensor_read_celsius(&driver, &temp), CFN_HAL_ERROR_OK);
    EXPECT_NEAR(temp, 25.5f, 0.01f);
}

// --- GSM Tests ---

class GsmTest : public ::testing::Test
{
  protected:
    cfn_svc_gsm_t     driver{};
    cfn_svc_gsm_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_GSM;
        driver.api = &api;
    }
};

TEST_F(GsmTest, SendSmsSuccess)
{
    api.send_sms = [](cfn_svc_gsm_t *d, const char *n, const char *t) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_gsm_send_sms(&driver, "1234", "hello"), CFN_HAL_ERROR_OK);
}
