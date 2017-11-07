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

TEST(diypinball_bootloaderFeatureHandler_test, init_zeros_structure)
{
    diypinball_featureRouterInstance router;
    diypinball_featureRouterInit routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_bootloaderFeatureHandlerInstance bootloaderFeatureHandler;
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
