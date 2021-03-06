#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_lampMatrixScanner.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::SetArgPointee;

class MockLampMatrixScannerHandlers {
public:
    virtual ~MockLampMatrixScannerHandlers() {}
    MOCK_METHOD1(testSetColumnHandler, void(int8_t));
    MOCK_METHOD4(testSetRowHandler, void(uint8_t, uint8_t, uint8_t, uint8_t));
};

static MockLampMatrixScannerHandlers* LampMatrixScannerHandlersImpl;

extern "C" {
    static void testSetColumnHandler(int8_t colNum) {
        LampMatrixScannerHandlersImpl->testSetColumnHandler(colNum);
    }

    static void testSetRowHandler(uint8_t row0Val, uint8_t row1Val, uint8_t row2Val, uint8_t row3Val) {
        LampMatrixScannerHandlersImpl->testSetRowHandler(row0Val, row1Val, row2Val, row3Val);
    }
}

class diypinball_lampMatrixScanner_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        LampMatrixScannerHandlersImpl = &myLampMatrixScannerHandlers;

        diypinball_lampMatrixScannerInit_t lampMatrixScannerInit;

        lampMatrixScannerInit.numColumns = 4;
        lampMatrixScannerInit.setColumnHandler = testSetColumnHandler;
        lampMatrixScannerInit.setRowHandler = testSetRowHandler;

        diypinball_lampMatrixScanner_init(&lampMatrixScanner, &lampMatrixScannerInit);
    }

    MockLampMatrixScannerHandlers myLampMatrixScannerHandlers;
    diypinball_lampMatrixScannerInstance_t lampMatrixScanner;
};

TEST_F(diypinball_lampMatrixScanner_test, init_zeros_structure)
{
    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.numStates);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lastTick);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].currentPhase);
    }

    ASSERT_TRUE(testSetColumnHandler == lampMatrixScanner.setColumnHandler);
    ASSERT_TRUE(testSetRowHandler == lampMatrixScanner.setRowHandler);
    ASSERT_EQ(4, lampMatrixScanner.numColumns);
    ASSERT_EQ(0, lampMatrixScanner.currentColumn);
    ASSERT_EQ(0, lampMatrixScanner.lastTick);
}

TEST_F(diypinball_lampMatrixScanner_test, deinit_zeros_structure)
{
    diypinball_lampMatrixScanner_deinit(&lampMatrixScanner);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.numStates);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lastTick);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].currentPhase);
    }

    ASSERT_TRUE(NULL == lampMatrixScanner.setColumnHandler);
    ASSERT_TRUE(NULL == lampMatrixScanner.setRowHandler);
    ASSERT_EQ(0, lampMatrixScanner.numColumns);
    ASSERT_EQ(0, lampMatrixScanner.currentColumn);
    ASSERT_EQ(0, lampMatrixScanner.lastTick);
}

TEST(diypinball_lampMatrixScanner_test_other, init_too_many_columns)
{
    MockLampMatrixScannerHandlers myLampMatrixScannerHandlers;
    diypinball_lampMatrixScannerInstance_t lampMatrixScanner;
    diypinball_lampMatrixScannerInit_t lampMatrixScannerInit;

    LampMatrixScannerHandlersImpl = &myLampMatrixScannerHandlers;

    lampMatrixScannerInit.numColumns = 5;
    lampMatrixScannerInit.setColumnHandler = testSetColumnHandler;
    lampMatrixScannerInit.setRowHandler = testSetRowHandler;

    diypinball_lampMatrixScanner_init(&lampMatrixScanner, &lampMatrixScannerInit);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.numStates);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lastTick);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].currentPhase);
    }

    ASSERT_TRUE(testSetColumnHandler == lampMatrixScanner.setColumnHandler);
    ASSERT_TRUE(testSetRowHandler == lampMatrixScanner.setRowHandler);
    ASSERT_EQ(4, lampMatrixScanner.numColumns);
    ASSERT_EQ(0, lampMatrixScanner.currentColumn);
    ASSERT_EQ(0, lampMatrixScanner.lastTick);
}

TEST_F(diypinball_lampMatrixScanner_test, set_lamp_state_valid)
{
    diypinball_lampStatus_t state;

    state.state1 = 255;
    state.state1Duration = 100;
    state.state2 = 0;
    state.state2Duration = 200;
    state.state3 = 225;
    state.state3Duration = 110;
    state.numStates = 3;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    ASSERT_EQ(255, lampMatrixScanner.lamps[0].lampState.state1);
    ASSERT_EQ(100, lampMatrixScanner.lamps[0].lampState.state1Duration);
    ASSERT_EQ(0, lampMatrixScanner.lamps[0].lampState.state2);
    ASSERT_EQ(200, lampMatrixScanner.lamps[0].lampState.state2Duration);
    ASSERT_EQ(225, lampMatrixScanner.lamps[0].lampState.state3);
    ASSERT_EQ(110, lampMatrixScanner.lamps[0].lampState.state3Duration);
    ASSERT_EQ(3, lampMatrixScanner.lamps[0].lampState.numStates);
    ASSERT_EQ(0, lampMatrixScanner.lamps[0].lastTick);
    ASSERT_EQ(0, lampMatrixScanner.lamps[0].currentPhase);
}

TEST_F(diypinball_lampMatrixScanner_test, set_lamp_state_invalid)
{
    diypinball_lampStatus_t state;

    state.state1 = 255;
    state.state1Duration = 100;
    state.state2 = 0;
    state.state2Duration = 200;
    state.state3 = 225;
    state.state3Duration = 110;
    state.numStates = 3;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 16, &state);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state1Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state2Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.state3Duration);
        ASSERT_EQ(0, lampMatrixScanner.lamps[i].lampState.numStates);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, set_tick)
{
    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 4242);

    ASSERT_EQ(4242, lampMatrixScanner.lastTick);
}

TEST_F(diypinball_lampMatrixScanner_test, isr_flow) {
    ASSERT_EQ(0, lampMatrixScanner.currentColumn);

    uint8_t i;

    for(i = 0; i <= 8; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_one_state) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 0;
    state.state2 = 0;
    state.state2Duration = 0;
    state.state3 = 0;
    state.state3Duration = 0;
    state.numStates = 1;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 101);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_one_state_with_duration) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 10;
    state.state2 = 0;
    state.state2Duration = 0;
    state.state3 = 0;
    state.state3Duration = 0;
    state.numStates = 1;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 101);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_two_states) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 10;
    state.state2 = 127;
    state.state2Duration = 10;
    state.state3 = 0;
    state.state3Duration = 0;
    state.numStates = 2;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 99);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 199);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 200);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_two_states_last_infinite) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 10;
    state.state2 = 127;
    state.state2Duration = 0;
    state.state3 = 0;
    state.state3Duration = 0;
    state.numStates = 2;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 99);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 199);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 200);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_three_states) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 10;
    state.state2 = 127;
    state.state2Duration = 10;
    state.state3 = 64;
    state.state3Duration = 11;
    state.numStates = 3;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 99);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 199);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 200);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(64, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 309);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(64, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
    
    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 310);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}

TEST_F(diypinball_lampMatrixScanner_test, blink_test_three_states_last_infinite) {
    diypinball_lampStatus_t state;
    uint8_t i;

    state.state1 = 255;
    state.state1Duration = 10;
    state.state2 = 127;
    state.state2Duration = 10;
    state.state3 = 64;
    state.state3Duration = 0;
    state.numStates = 3;

    diypinball_lampMatrixScanner_setLampState(&lampMatrixScanner, 0, &state);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 99);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(255, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 100);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 199);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(127, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 200);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(64, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }

    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 309);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(64, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
    
    diypinball_lampMatrixScanner_millisecondTickHandler(&lampMatrixScanner, 310);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

    EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(64, _, _, _)).Times(1);

    diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(0, 0, 0, 0)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_RESET);

        EXPECT_CALL(myLampMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(myLampMatrixScannerHandlers, testSetRowHandler(_, _, _, _)).Times(1);

        diypinball_lampMatrixScanner_isr(&lampMatrixScanner, LAMP_INTERRUPT_MATCH);
    }
}