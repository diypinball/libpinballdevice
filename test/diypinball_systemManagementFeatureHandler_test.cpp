#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_systemManagementFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockSysManHandlers {
public:
    virtual ~MockSysManHandlers() {}
    MOCK_METHOD2(testPowerStatusHandler, void(uint8_t*, uint8_t*));
};

static MockCANSend* CANSendImpl;
static MockSysManHandlers* SysManHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testPowerStatusHandler(uint8_t* voltages, uint8_t* currents) {
        SysManHandlersImpl->testPowerStatusHandler(voltages, currents);
        voltages[0] = 255;
        voltages[1] = 127;
        voltages[2] = 63;
        voltages[3] = 31;
        currents[0] = 255;
        currents[1] = 127;
        currents[2] = 63;
        currents[3] = 31;
    }
}

class diypinball_systemManagementFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        SysManHandlersImpl = &mySysManHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_systemManagementFeatureHandlerInit systemManagementFeatureHandlerInit;

        systemManagementFeatureHandlerInit.firmwareVersionMajor = 1;
        systemManagementFeatureHandlerInit.firmwareVersionMinor = 2;
        systemManagementFeatureHandlerInit.firmwareVersionPatch = 3;
        systemManagementFeatureHandlerInit.boardSerial[0] = 65536;
        systemManagementFeatureHandlerInit.boardSerial[1] = 65537;
        systemManagementFeatureHandlerInit.boardSerial[2] = 65538;
        systemManagementFeatureHandlerInit.boardSerial[3] = 65539;
        systemManagementFeatureHandlerInit.boardSignature[0] = 16777216;
        systemManagementFeatureHandlerInit.boardSignature[1] = 16777217;
        systemManagementFeatureHandlerInit.powerStatusHandler = testPowerStatusHandler;
        systemManagementFeatureHandlerInit.routerInstance = &router;

        diypinball_systemManagementFeatureHandler_init(&systemManagementFeatureHandler, &systemManagementFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_systemManagementFeatureHandlerInstance systemManagementFeatureHandler;
    MockCANSend myCANSend;
    MockSysManHandlers mySysManHandlers;
};

TEST_F(diypinball_systemManagementFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(0, systemManagementFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_EQ(1, systemManagementFeatureHandler.firmwareVersionMajor);
    ASSERT_EQ(2, systemManagementFeatureHandler.firmwareVersionMinor);
    ASSERT_EQ(3, systemManagementFeatureHandler.firmwareVersionPatch);
    ASSERT_EQ(65536, systemManagementFeatureHandler.boardSerial[0]);
    ASSERT_EQ(65537, systemManagementFeatureHandler.boardSerial[1]);
    ASSERT_EQ(65538, systemManagementFeatureHandler.boardSerial[2]);
    ASSERT_EQ(65539, systemManagementFeatureHandler.boardSerial[3]);
    ASSERT_EQ(16777216, systemManagementFeatureHandler.boardSignature[0]);
    ASSERT_EQ(16777217, systemManagementFeatureHandler.boardSignature[1]);

    ASSERT_EQ(0, systemManagementFeatureHandler.powerStatusPollingInterval);
    ASSERT_EQ(0, systemManagementFeatureHandler.lastTick);

    ASSERT_EQ(&router, systemManagementFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_TRUE(testPowerStatusHandler == systemManagementFeatureHandler.powerStatusHandler);
    ASSERT_TRUE(diypinball_systemManagementFeatureHandler_millisecondTickHandler == systemManagementFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_systemManagementFeatureHandler_messageReceivedHandler == systemManagementFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, poweron_sends_two_messages)
{
    diypinball_canMessage_t expectedCANMessage1, expectedCANMessage2;

    expectedCANMessage1.id = (0x0F << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 1;
    expectedCANMessage1.data[0] = 0;

    expectedCANMessage2.id = (0x0F << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (1 << 4) | 0;
    expectedCANMessage2.rtr = 0;
    expectedCANMessage2.dlc = 3;
    expectedCANMessage2.data[0] = 1;
    expectedCANMessage2.data[1] = 2;
    expectedCANMessage2.data[2] = 3;

    InSequence dummy;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1)));
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage2)));

    diypinball_systemManagementFeatureHandler_sendPoweronMessages(&systemManagementFeatureHandler);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_0_sends_id_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_0_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_1_sends_id_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (1 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.dlc = 3;
    expectedCANMessage.data[0] = 1;
    expectedCANMessage.data[1] = 2;
    expectedCANMessage.data[2] = 3;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_1_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_2_sends_id_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x03 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x03 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 127;
    expectedCANMessage.data[2] = 63;
    expectedCANMessage.data[3] = 31;
    expectedCANMessage.data[4] = 255;
    expectedCANMessage.data[5] = 127;
    expectedCANMessage.data[6] = 63;
    expectedCANMessage.data[7] = 31;

    EXPECT_CALL(mySysManHandlers, testPowerStatusHandler(_, _)).Times(1);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_2_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(mySysManHandlers, testPowerStatusHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_3_sends_default)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, setting_and_retrieving_function_3)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(10, systemManagementFeatureHandler.powerStatusPollingInterval);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, set_function_3_and_millisecond_tick)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(10, systemManagementFeatureHandler.powerStatusPollingInterval);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    diypinball_featureRouter_millisecondTick(&router, 0);
    diypinball_featureRouter_millisecondTick(&router, 1);
    diypinball_featureRouter_millisecondTick(&router, 2);
    diypinball_featureRouter_millisecondTick(&router, 3);
    diypinball_featureRouter_millisecondTick(&router, 4);
    diypinball_featureRouter_millisecondTick(&router, 5);
    diypinball_featureRouter_millisecondTick(&router, 6);
    diypinball_featureRouter_millisecondTick(&router, 7);
    diypinball_featureRouter_millisecondTick(&router, 8);
    diypinball_featureRouter_millisecondTick(&router, 9);

    expectedCANMessage.id = (0x0E << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 127;
    expectedCANMessage.data[2] = 63;
    expectedCANMessage.data[3] = 31;
    expectedCANMessage.data[4] = 255;
    expectedCANMessage.data[5] = 127;
    expectedCANMessage.data[6] = 63;
    expectedCANMessage.data[7] = 31;

    EXPECT_CALL(mySysManHandlers, testPowerStatusHandler(_, _)).Times(1);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 10);

    CANSendImpl = NULL;
    SysManHandlersImpl = NULL;
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_4_sends_serial_number_A_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (4 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 1;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 1;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 1;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_4_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_5_sends_serial_number_B_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (5 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 2;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 1;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 3;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 1;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_5_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_6_sends_hardware_signature)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (6 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (6 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 1;
    expectedCANMessage.data[4] = 1;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_6_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (6 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_systemManagementFeatureHandler_test, request_to_function_7_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 7; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (i << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    }
}

TEST_F(diypinball_systemManagementFeatureHandler_test, message_to_function_7_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 7; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (0 << 12) | (0 << 8) | (i << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 1;
        initiatingCANMessage.data[0] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_systemManagementFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_systemManagementFeatureHandler_deinit(&systemManagementFeatureHandler);

    ASSERT_EQ(0, systemManagementFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_EQ(0, systemManagementFeatureHandler.firmwareVersionMajor);
    ASSERT_EQ(0, systemManagementFeatureHandler.firmwareVersionMinor);
    ASSERT_EQ(0, systemManagementFeatureHandler.firmwareVersionPatch);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSerial[0]);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSerial[1]);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSerial[2]);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSerial[3]);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSignature[0]);
    ASSERT_EQ(0, systemManagementFeatureHandler.boardSignature[1]);

    ASSERT_EQ(0, systemManagementFeatureHandler.powerStatusPollingInterval);
    ASSERT_EQ(0, systemManagementFeatureHandler.lastTick);

    ASSERT_EQ(NULL, systemManagementFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, systemManagementFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, systemManagementFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_TRUE(NULL == systemManagementFeatureHandler.powerStatusHandler);
    ASSERT_TRUE(NULL == systemManagementFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == systemManagementFeatureHandler.featureHandlerInstance.messageHandler);
}
