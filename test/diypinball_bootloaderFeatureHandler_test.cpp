#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_bootloaderFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::ElementsAreArray;
using ::testing::Args;
using ::testing::SetArrayArgument;

class MockBootloaderHandlers {
public:
    virtual ~MockBootloaderHandlers() {}
    MOCK_METHOD0(testRebootHandler, void(void));
    MOCK_METHOD1(testFlashReadHandler, uint8_t(uint32_t));
    MOCK_METHOD1(testFlashWriteHandler, uint8_t(uint32_t));
    MOCK_METHOD1(testFlashVerifyHandler, uint8_t(uint32_t));
    MOCK_METHOD1(testBufferHashHandler, uint8_t(uint32_t));
    MOCK_METHOD2(testBufferReadHandler, void(uint8_t, uint8_t*));
    MOCK_METHOD2(testBufferWriteHandler, void(uint8_t, uint8_t*));
};

static MockCANSend* CANSendImpl;
static MockBootloaderHandlers* BootloaderHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testRebootHandler(void) {
        BootloaderHandlersImpl->testRebootHandler();
    }

    static uint8_t testFlashReadHandler(uint32_t baseAddress) {
        return BootloaderHandlersImpl->testFlashReadHandler(baseAddress);
    }

    static uint8_t testFlashWriteHandler(uint32_t baseAddress) {
        return BootloaderHandlersImpl->testFlashWriteHandler(baseAddress);
    }

    static uint8_t testFlashVerifyHandler(uint32_t baseAddress) {
        return BootloaderHandlersImpl->testFlashVerifyHandler(baseAddress);
    }

    static uint8_t testBufferHashHandler(uint32_t crcHash) {
        return BootloaderHandlersImpl->testBufferHashHandler(crcHash);
    }

    static void testBufferReadHandler(uint8_t offset, uint8_t *data) {
        BootloaderHandlersImpl->testBufferReadHandler(offset, data);
    }

    static void testBufferWriteHandler(uint8_t offset, uint8_t *data) {
        BootloaderHandlersImpl->testBufferWriteHandler(offset, data);
    }
}

MATCHER_P(BootBufferEqual, status, "") {
    uint8_t fieldFlag = (memcmp(arg, status, 8));

    return !(fieldFlag);
}

class diypinball_bootloaderFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        BootloaderHandlersImpl = &myBootloaderHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_bootloaderFeatureHandlerInit bootloaderFeatureHandlerInit;

        bootloaderFeatureHandlerInit.applicationVersionMajor = 1;
        bootloaderFeatureHandlerInit.applicationVersionMinor = 2;
        bootloaderFeatureHandlerInit.applicationVersionPatch = 3;
        bootloaderFeatureHandlerInit.flashPageSize = 2048;
        bootloaderFeatureHandlerInit.flashBufferSize = 1024;
        bootloaderFeatureHandlerInit.applicationBaseAddress = 0x02002000;
        bootloaderFeatureHandlerInit.flashSize = 131072;
        bootloaderFeatureHandlerInit.rebootHandler = testRebootHandler;
        bootloaderFeatureHandlerInit.flashReadHandler = testFlashReadHandler;
        bootloaderFeatureHandlerInit.flashWriteHandler = testFlashWriteHandler;
        bootloaderFeatureHandlerInit.flashVerifyHandler = testFlashVerifyHandler;
        bootloaderFeatureHandlerInit.bufferHashHandler = testBufferHashHandler;
        bootloaderFeatureHandlerInit.bufferReadHandler = testBufferReadHandler;
        bootloaderFeatureHandlerInit.bufferWriteHandler = testBufferWriteHandler;
        bootloaderFeatureHandlerInit.routerInstance = &router;

        diypinball_bootloaderFeatureHandler_init(&bootloaderFeatureHandler, &bootloaderFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_bootloaderFeatureHandlerInstance bootloaderFeatureHandler;
    MockCANSend myCANSend;
    MockBootloaderHandlers myBootloaderHandlers;
};

TEST_F(diypinball_bootloaderFeatureHandler_test, init_sets_up_structure)
{
    ASSERT_EQ(7, bootloaderFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_EQ(1, bootloaderFeatureHandler.applicationVersionMajor);
    ASSERT_EQ(2, bootloaderFeatureHandler.applicationVersionMinor);
    ASSERT_EQ(3, bootloaderFeatureHandler.applicationVersionPatch);
    ASSERT_EQ(2048, bootloaderFeatureHandler.flashPageSize);
    ASSERT_EQ(1024, bootloaderFeatureHandler.flashBufferSize);
    ASSERT_EQ(0x02002000, bootloaderFeatureHandler.applicationBaseAddress);
    ASSERT_EQ(131072, bootloaderFeatureHandler.flashSize);

    ASSERT_EQ(&router, bootloaderFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_TRUE(testRebootHandler == bootloaderFeatureHandler.rebootHandler);
    ASSERT_TRUE(testFlashReadHandler == bootloaderFeatureHandler.flashReadHandler);
    ASSERT_TRUE(testFlashWriteHandler == bootloaderFeatureHandler.flashWriteHandler);
    ASSERT_TRUE(testFlashVerifyHandler == bootloaderFeatureHandler.flashVerifyHandler);
    ASSERT_TRUE(testBufferHashHandler == bootloaderFeatureHandler.bufferHashHandler);
    ASSERT_TRUE(testBufferReadHandler == bootloaderFeatureHandler.bufferReadHandler);
    ASSERT_TRUE(testBufferWriteHandler == bootloaderFeatureHandler.bufferWriteHandler);
    ASSERT_TRUE(diypinball_bootloaderFeatureHandler_millisecondTickHandler == bootloaderFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_bootloaderFeatureHandler_messageReceivedHandler == bootloaderFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_bootloaderFeatureHandler_deinit(&bootloaderFeatureHandler);

    ASSERT_EQ(0, bootloaderFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_EQ(0, bootloaderFeatureHandler.applicationVersionMajor);
    ASSERT_EQ(0, bootloaderFeatureHandler.applicationVersionMinor);
    ASSERT_EQ(0, bootloaderFeatureHandler.applicationVersionPatch);
    ASSERT_EQ(0, bootloaderFeatureHandler.flashPageSize);
    ASSERT_EQ(0, bootloaderFeatureHandler.flashBufferSize);
    ASSERT_EQ(0, bootloaderFeatureHandler.applicationBaseAddress);
    ASSERT_EQ(0, bootloaderFeatureHandler.flashSize);

    ASSERT_EQ(NULL, bootloaderFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, bootloaderFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.rebootHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.flashReadHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.flashWriteHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.flashVerifyHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.bufferHashHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.bufferReadHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.bufferWriteHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == bootloaderFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, application_version_request_gets_version)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 3;
    expectedCANMessage.data[0] = 1;
    expectedCANMessage.data[1] = 2;
    expectedCANMessage.data[2] = 3;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, application_version_message_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_page_size_request_gets_data)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 0x00;
    expectedCANMessage.data[1] = 0x08;
    expectedCANMessage.data[2] = 0x00;
    expectedCANMessage.data[3] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_page_size_message_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_buffer_size_request_gets_data)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 0x00;
    expectedCANMessage.data[1] = 0x04;
    expectedCANMessage.data[2] = 0x00;
    expectedCANMessage.data[3] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_buffer_size_message_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, application_base_address_request_gets_data)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 0x00;
    expectedCANMessage.data[1] = 0x20;
    expectedCANMessage.data[2] = 0x00;
    expectedCANMessage.data[3] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, application_base_address_message_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_size_request_gets_data)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (4 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (4 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 0x00;
    expectedCANMessage.data[1] = 0x00;
    expectedCANMessage.data[2] = 0x02;
    expectedCANMessage.data[3] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_size_message_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (4 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, reboot_request_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, reboot_message_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, reboot_message_with_wrong_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, reboot_message_with_right_data_reboots)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x42;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_read_to_buffer_with_address_calls_function)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;
    initiatingCANMessage.data[3] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(0x00002000)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, flash_read_to_buffer_with_not_enough_data_fails)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_write_to_flash_with_address_calls_function)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;
    initiatingCANMessage.data[3] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(0x00002000)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, write_flash_to_buffer_with_not_enough_data_fails)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (1 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_verify_against_flash_with_address_calls_function)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;
    initiatingCANMessage.data[3] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(0x00002000)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_verify_against_flash_with_not_enough_data_fails)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (2 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_check_crc_with_address_calls_function)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;
    initiatingCANMessage.data[3] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0x00;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(0x00002000)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_check_crc_with_not_enough_data_fails)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 0x00;
    initiatingCANMessage.data[1] = 0x20;
    initiatingCANMessage.data[2] = 0x00;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (3 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
    EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_write_passes_data)
{
    diypinball_canMessage_t initiatingCANMessage;

    uint8_t hi, lo;
    uint8_t expectedArray[8];

    for(uint8_t i = 0; i < 128; i++) {
        hi = (i & 0x78) >> 3;
        lo = ((i & 0x07) << 1) | 0x01;

        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (hi << 8) | (lo << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 8;
        initiatingCANMessage.data[0] = 0x00;
        initiatingCANMessage.data[1] = 0x20;
        initiatingCANMessage.data[2] = 0x00;
        initiatingCANMessage.data[3] = 0x40;
        initiatingCANMessage.data[4] = 0x00;
        initiatingCANMessage.data[5] = 0x80;
        initiatingCANMessage.data[6] = 0x00;
        initiatingCANMessage.data[7] = 0x10;

        expectedArray[0] = 0x00;
        expectedArray[1] = 0x20;
        expectedArray[2] = 0x00;
        expectedArray[3] = 0x40;
        expectedArray[4] = 0x00;
        expectedArray[5] = 0x80;
        expectedArray[6] = 0x00;
        expectedArray[7] = 0x10;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(i, BootBufferEqual(expectedArray))).Times(1);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_write_pads_data)
{
    diypinball_canMessage_t initiatingCANMessage;

    uint8_t hi, lo;
    uint8_t expectedArray[8];

    for(uint8_t i = 0; i < 128; i++) {
        hi = (i & 0x78) >> 3;
        lo = ((i & 0x07) << 1) | 0x01;

        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (hi << 8) | (lo << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 6;
        initiatingCANMessage.data[0] = 0x00;
        initiatingCANMessage.data[1] = 0x20;
        initiatingCANMessage.data[2] = 0x00;
        initiatingCANMessage.data[3] = 0x40;
        initiatingCANMessage.data[4] = 0x00;
        initiatingCANMessage.data[5] = 0x80;

        expectedArray[0] = 0x00;
        expectedArray[1] = 0x20;
        expectedArray[2] = 0x00;
        expectedArray[3] = 0x40;
        expectedArray[4] = 0x00;
        expectedArray[5] = 0x80;
        expectedArray[6] = 0xff;
        expectedArray[7] = 0xff;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(_, _)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(i, BootBufferEqual(expectedArray))).Times(1);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_bootloaderFeatureHandler_test, buffer_read_gets_data)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    uint8_t hi, lo;
    uint8_t expectedArray[8];

    for(uint8_t i = 0; i < 128; i++) {
        hi = (i & 0x78) >> 3;
        lo = ((i & 0x07) << 1) | 0x01;

        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (hi << 8) | (lo << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        expectedArray[0] = 0x20;
        expectedArray[1] = 0x00;
        expectedArray[2] = 0x40;
        expectedArray[3] = 0x00;
        expectedArray[4] = 0x80;
        expectedArray[5] = 0x00;
        expectedArray[6] = 0x10;
        expectedArray[7] = 0x00;

        expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (7 << 12) | (hi << 8) | (lo << 4) | 0;
        expectedCANMessage.rtr = 0;
        expectedCANMessage.dlc = 8;
        expectedCANMessage.data[0] = 0x20;
        expectedCANMessage.data[1] = 0x00;
        expectedCANMessage.data[2] = 0x40;
        expectedCANMessage.data[3] = 0x00;
        expectedCANMessage.data[4] = 0x80;
        expectedCANMessage.data[5] = 0x00;
        expectedCANMessage.data[6] = 0x10;
        expectedCANMessage.data[7] = 0x00;

        EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
        EXPECT_CALL(myBootloaderHandlers, testRebootHandler()).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashReadHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashWriteHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testFlashVerifyHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferHashHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderHandlers, testBufferReadHandler(i, _)).Times(1).WillOnce(SetArrayArgument<1>(expectedArray, expectedArray+8));
        EXPECT_CALL(myBootloaderHandlers, testBufferWriteHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}
