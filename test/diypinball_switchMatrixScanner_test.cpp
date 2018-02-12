#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_switchMatrixScanner.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::SetArgPointee;

class MockSwitchMatrixScannerHandlers {
public:
    virtual ~MockSwitchMatrixScannerHandlers() {}
    MOCK_METHOD2(testSwitchStateHandler, void(uint8_t, uint8_t));
    MOCK_METHOD1(testSetColumnHandler, void(int8_t));
    MOCK_METHOD1(testReadRowHandler, void(uint8_t*));
};

static MockSwitchMatrixScannerHandlers* SwitchMatrixScannerHandlersImpl;

extern "C" {
    static void testSwitchStateHandler(uint8_t switchNum, uint8_t state) {
        SwitchMatrixScannerHandlersImpl->testSwitchStateHandler(switchNum, state);
    }

    static void testSetColumnHandler(int8_t colNum) {
        SwitchMatrixScannerHandlersImpl->testSetColumnHandler(colNum);
    }

    static void testReadRowHandler(uint8_t *row) {
        SwitchMatrixScannerHandlersImpl->testReadRowHandler(row);
    }
}

class diypinball_switchMatrixScanner_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        SwitchMatrixScannerHandlersImpl = &mySwitchMatrixScannerHandlers;

        diypinball_switchMatrixScannerInit_t switchMatrixScannerInit;

        switchMatrixScannerInit.numColumns = 4;
        switchMatrixScannerInit.switchStateHandler = testSwitchStateHandler;
        switchMatrixScannerInit.setColumnHandler = testSetColumnHandler;
        switchMatrixScannerInit.readRowHandler = testReadRowHandler;

        diypinball_switchMatrixScanner_init(&switchMatrixScanner, &switchMatrixScannerInit);
    }

    MockSwitchMatrixScannerHandlers mySwitchMatrixScannerHandlers;
    diypinball_switchMatrixScannerInstance_t switchMatrixScanner;
};

TEST_F(diypinball_switchMatrixScanner_test, init_zeros_structure)
{
    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrixScanner.switches[i].switchState);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].debounceLimit);
    }

    ASSERT_TRUE(testSwitchStateHandler == switchMatrixScanner.switchStateHandler);
    ASSERT_TRUE(testSetColumnHandler == switchMatrixScanner.setColumnHandler);
    ASSERT_TRUE(testReadRowHandler == switchMatrixScanner.readRowHandler);
    ASSERT_EQ(4, switchMatrixScanner.numColumns);
    ASSERT_EQ(0, switchMatrixScanner.lastTick);
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);
}

TEST_F(diypinball_switchMatrixScanner_test, deinit_zeros_structure)
{
    diypinball_switchMatrixScanner_deinit(&switchMatrixScanner);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrixScanner.switches[i].switchState);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].debounceLimit);
    }

    ASSERT_TRUE(NULL == switchMatrixScanner.switchStateHandler);
    ASSERT_TRUE(NULL == switchMatrixScanner.setColumnHandler);
    ASSERT_TRUE(NULL == switchMatrixScanner.readRowHandler);
    ASSERT_EQ(0, switchMatrixScanner.numColumns);
    ASSERT_EQ(0, switchMatrixScanner.lastTick);
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);
}

TEST(diypinball_switchMatrixScanner_test_other, init_too_many_columns)
{
    MockSwitchMatrixScannerHandlers mySwitchMatrixScannerHandlers;
    diypinball_switchMatrixScannerInstance_t switchMatrixScanner;
    diypinball_switchMatrixScannerInit_t switchMatrixScannerInit;

    SwitchMatrixScannerHandlersImpl = &mySwitchMatrixScannerHandlers;

    switchMatrixScannerInit.numColumns = 5;
    switchMatrixScannerInit.switchStateHandler = testSwitchStateHandler;
    switchMatrixScannerInit.setColumnHandler = testSetColumnHandler;
    switchMatrixScannerInit.readRowHandler = testReadRowHandler;

    diypinball_switchMatrixScanner_init(&switchMatrixScanner, &switchMatrixScannerInit);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrixScanner.switches[i].switchState);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrixScanner.switches[i].debounceLimit);
    }

    ASSERT_TRUE(testSwitchStateHandler == switchMatrixScanner.switchStateHandler);
    ASSERT_TRUE(testSetColumnHandler == switchMatrixScanner.setColumnHandler);
    ASSERT_TRUE(testReadRowHandler == switchMatrixScanner.readRowHandler);
    ASSERT_EQ(4, switchMatrixScanner.numColumns);
    ASSERT_EQ(0, switchMatrixScanner.lastTick);
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);
}

TEST_F(diypinball_switchMatrixScanner_test, set_tick)
{
    diypinball_switchMatrixScanner_millisecondTickHandler(&switchMatrixScanner, 4242);

    ASSERT_EQ(4242, switchMatrixScanner.lastTick);
}

TEST_F(diypinball_switchMatrixScanner_test, set_debounce_limit_valid)
{
    diypinball_switchMatrixScanner_setDebounceLimit(&switchMatrixScanner, 0, 100);

    ASSERT_EQ(100, switchMatrixScanner.switches[0].debounceLimit);
}

TEST_F(diypinball_switchMatrixScanner_test, set_debounce_limit_invalid)
{
    diypinball_switchMatrixScanner_setDebounceLimit(&switchMatrixScanner, 17, 100);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrixScanner.switches[i].debounceLimit);
    }
}

TEST_F(diypinball_switchMatrixScanner_test, isr_flow) {
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);

    uint8_t i;

    for(i = 0; i <= 8; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }
}

TEST(diypinball_switchMatrixScanner_test_other, isr_flow_few_columns) {
    MockSwitchMatrixScannerHandlers mySwitchMatrixScannerHandlers;
    diypinball_switchMatrixScannerInstance_t switchMatrixScanner;
    diypinball_switchMatrixScannerInit_t switchMatrixScannerInit;

    SwitchMatrixScannerHandlersImpl = &mySwitchMatrixScannerHandlers;

    switchMatrixScannerInit.numColumns = 3;
    switchMatrixScannerInit.switchStateHandler = testSwitchStateHandler;
    switchMatrixScannerInit.setColumnHandler = testSetColumnHandler;
    switchMatrixScannerInit.readRowHandler = testReadRowHandler;

    diypinball_switchMatrixScanner_init(&switchMatrixScanner, &switchMatrixScannerInit);

    ASSERT_EQ(0, switchMatrixScanner.currentColumn);

    uint8_t i;

    for(i = 0; i <= 8; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 3)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }
}

TEST_F(diypinball_switchMatrixScanner_test, switch_edges) {
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);

    uint8_t i;

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x00));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }
}

TEST_F(diypinball_switchMatrixScanner_test, switch_edges_debounce) {
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);
    diypinball_switchMatrixScanner_millisecondTickHandler(&switchMatrixScanner, 5);

    diypinball_switchMatrixScanner_setDebounceLimit(&switchMatrixScanner, 3, 5);

    uint8_t i;

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x00));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    diypinball_switchMatrixScanner_millisecondTickHandler(&switchMatrixScanner, 10);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    diypinball_switchMatrixScanner_millisecondTickHandler(&switchMatrixScanner, 15);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }
}

TEST_F(diypinball_switchMatrixScanner_test, read_switch_state) {
    ASSERT_EQ(0, switchMatrixScanner.currentColumn);

    uint8_t i;
    uint8_t result;

    result = diypinball_switchMatrixScanner_readSwitchState(&switchMatrixScanner, 3);
    ASSERT_EQ(0, result);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(0)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(3, 1)).Times(1);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0x08));

    diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);

    for(i = 1; i < 4; i++) {
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(-1)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_RESET);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(i % 4)).Times(1);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(0);

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_1);

        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSwitchStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testSetColumnHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixScannerHandlers, testReadRowHandler(_)).Times(1).WillOnce(SetArgPointee<0>(0));

        diypinball_switchMatrixScanner_isr(&switchMatrixScanner, INTERRUPT_MATCH_2);
    }

    result = diypinball_switchMatrixScanner_readSwitchState(&switchMatrixScanner, 3);
    ASSERT_EQ(1, result);

    result = diypinball_switchMatrixScanner_readSwitchState(&switchMatrixScanner, 17);
    ASSERT_EQ(0, result);
}
