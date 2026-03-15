/**
 * @file cfn_svc_test_advanced.cpp
 * @brief Verification tests for advanced software and device service interfaces.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "devices/cfn_svc_gnss.h"
#include "devices/cfn_svc_display.h"
#include "software/cfn_svc_cli.h"
#include "software/cfn_svc_at_parser.h"

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
        driver.api = &api;
    }
};

TEST_F(GnssTest, GetLocationSuccess)
{
    api.get_location = [](cfn_svc_gnss_t *d, cfn_svc_gnss_location_t *l) -> cfn_hal_error_code_t
    {
        l->latitude = 12.34;
        return CFN_HAL_ERROR_OK;
    };
    cfn_svc_gnss_location_t loc{};
    EXPECT_EQ(cfn_svc_gnss_get_location(&driver, &loc), CFN_HAL_ERROR_OK);
    EXPECT_DOUBLE_EQ(loc.latitude, 12.34);
}

// --- Display Tests ---

class DisplayTest : public ::testing::Test
{
  protected:
    cfn_svc_display_t     driver{};
    cfn_svc_display_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_DISPLAY;
        driver.api = &api;
    }
};

TEST_F(DisplayTest, UpdateSuccess)
{
    api.update = [](cfn_svc_display_t *d) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_display_update(&driver), CFN_HAL_ERROR_OK);
}

// --- CLI Tests ---

class CliTest : public ::testing::Test
{
  protected:
    cfn_svc_cli_t     driver{};
    cfn_svc_cli_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_CLI;
        driver.api = &api;
    }
};

TEST_F(CliTest, FeedCharSuccess)
{
    api.feed_char = [](cfn_svc_cli_t *d, char c) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_cli_feed_char(&driver, 'a'), CFN_HAL_ERROR_OK);
}

// --- AT Parser Tests ---

class AtParserTest : public ::testing::Test
{
  protected:
    cfn_svc_at_parser_t     driver{};
    cfn_svc_at_parser_api_t api{};

    void SetUp() override
    {
        memset(&driver, 0, sizeof(driver));
        memset(&api, 0, sizeof(api));
        driver.base.type = CFN_SVC_TYPE_AT_PARSER;
        driver.api = &api;
    }
};

TEST_F(AtParserTest, SendCommandSuccess)
{
    api.send_command = [](cfn_svc_at_parser_t *d, const char *c) -> cfn_hal_error_code_t { return CFN_HAL_ERROR_OK; };
    EXPECT_EQ(cfn_svc_at_send_command(&driver, "AT"), CFN_HAL_ERROR_OK);
}
