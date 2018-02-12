#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_coilEnvelopeGenerator.h"
#include "diypinball_coilFeatureHandler.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockCoilEnvelopeGeneratorHandlers {
public:
    virtual ~MockCoilEnvelopeGeneratorHandlers() {}
    MOCK_METHOD2(testCoilStateHandler, void(uint8_t, uint8_t));
};

static MockCoilEnvelopeGeneratorHandlers* CoilEnvelopeGeneratorHandlersImpl;

extern "C" {
    static void testCoilStateHandler(uint8_t coilNum, uint8_t state) {
        CoilEnvelopeGeneratorHandlersImpl->testCoilStateHandler(coilNum, state);
    }
}

class diypinball_coilEnvelopeGenerator_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CoilEnvelopeGeneratorHandlersImpl = &myCoilEnvelopeGeneratorHandlers;

        diypinball_coilEnvelopeGeneratorInit_t coilEnvelopeGeneratorInit;

        coilEnvelopeGeneratorInit.numCoils = 8;
        coilEnvelopeGeneratorInit.coilStateHandler = testCoilStateHandler;

        diypinball_coilEnvelopeGenerator_init(&coilEnvelopeGenerator, &coilEnvelopeGeneratorInit);
    }

    MockCoilEnvelopeGeneratorHandlers myCoilEnvelopeGeneratorHandlers;
    diypinball_coilEnvelopeGeneratorInstance_t coilEnvelopeGenerator;
};

TEST_F(diypinball_coilEnvelopeGenerator_test, init_zeros_structure)
{
    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastTicks[i]);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastPhases[i]);
    }

    ASSERT_TRUE(testCoilStateHandler == coilEnvelopeGenerator.coilStateHandler);
    ASSERT_EQ(8, coilEnvelopeGenerator.numCoils);
    ASSERT_EQ(0, coilEnvelopeGenerator.lastTick);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, deinit_zeros_structure)
{
    diypinball_coilEnvelopeGenerator_deinit(&coilEnvelopeGenerator);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastTicks[i]);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastPhases[i]);
    }

    ASSERT_TRUE(NULL == coilEnvelopeGenerator.coilStateHandler);
    ASSERT_EQ(0, coilEnvelopeGenerator.numCoils);
    ASSERT_EQ(0, coilEnvelopeGenerator.lastTick);
}

TEST(diypinball_coilEnvelopeGenerator_test_other, init_too_many_columns)
{
    MockCoilEnvelopeGeneratorHandlers myCoilEnvelopeGeneratorHandlers;
    diypinball_coilEnvelopeGeneratorInstance_t coilEnvelopeGenerator;
    diypinball_coilEnvelopeGeneratorInit_t coilEnvelopeGeneratorInit;

    CoilEnvelopeGeneratorHandlersImpl = &myCoilEnvelopeGeneratorHandlers;

    coilEnvelopeGeneratorInit.numCoils = 17;
    coilEnvelopeGeneratorInit.coilStateHandler = testCoilStateHandler;

    diypinball_coilEnvelopeGenerator_init(&coilEnvelopeGenerator, &coilEnvelopeGeneratorInit);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].attackDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainState);
        ASSERT_EQ(0, coilEnvelopeGenerator.coils[i].sustainDuration);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastTicks[i]);
        ASSERT_EQ(0, coilEnvelopeGenerator.lastPhases[i]);
    }

    ASSERT_TRUE(testCoilStateHandler == coilEnvelopeGenerator.coilStateHandler);
    ASSERT_EQ(16, coilEnvelopeGenerator.numCoils);
    ASSERT_EQ(0, coilEnvelopeGenerator.lastTick);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, setTick)
{
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    ASSERT_EQ(1, coilEnvelopeGenerator.lastTick);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_state_valid)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 100;
    status.sustainState = 127;
    status.sustainDuration = 0;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 0, &status);

    ASSERT_EQ(1, coilEnvelopeGenerator.lastTicks[0]);
    ASSERT_EQ(1, coilEnvelopeGenerator.lastPhases[0]);
    ASSERT_EQ(255, coilEnvelopeGenerator.coils[0].attackState);
    ASSERT_EQ(100, coilEnvelopeGenerator.coils[0].attackDuration);
    ASSERT_EQ(127, coilEnvelopeGenerator.coils[0].sustainState);
    ASSERT_EQ(0, coilEnvelopeGenerator.coils[0].sustainDuration);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_state_invalid)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 100;
    status.sustainState = 127;
    status.sustainDuration = 0;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(_, _)).Times(0);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 16, &status);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_with_sustain_forever)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 100;
    status.sustainState = 127;
    status.sustainDuration = 0;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 0, &status);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1000);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 127)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1001);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 127)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1101);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_with_sustain_bounded)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 100;
    status.sustainState = 127;
    status.sustainDuration = 10;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 0, &status);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1000);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 127)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1001);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1101);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_attack_bounded_with_no_sustain)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 100;
    status.sustainState = 0;
    status.sustainDuration = 0;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 0, &status);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1000);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1001);
}

TEST_F(diypinball_coilEnvelopeGenerator_test, set_coil_attack_forever)
{
    diypinball_coilStatus_t status;
    status.attackState = 255;
    status.attackDuration = 0;
    status.sustainState = 0;
    status.sustainDuration = 0;

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);

    diypinball_coilEnvelopeGenerator_setCoilState(&coilEnvelopeGenerator, 0, &status);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 1000);

    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(0, 255)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(1, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(2, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(3, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(4, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(5, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(6, 0)).Times(1);
    EXPECT_CALL(myCoilEnvelopeGeneratorHandlers, testCoilStateHandler(7, 0)).Times(1);

    diypinball_coilEnvelopeGenerator_millisecondTickHandler(&coilEnvelopeGenerator, 100001);
}
