#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_scoreFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockScoreFeatureHandlerHandlers {
public:
    virtual ~MockScoreFeatureHandlerHandlers() {}
    MOCK_METHOD1(testDisplayChangedHandler, void(char*));
    MOCK_METHOD1(testBrightnessChangedHandler, void(uint8_t));
};

static MockCANSend* CANSendImpl;
static MockScoreFeatureHandlerHandlers* ScoreFeatureHandlerHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testDisplayChangedHandler(char* display) {
        ScoreFeatureHandlerHandlersImpl->testDisplayChangedHandler(display);
    }

    static void testBrightnessChangedHandler(uint8_t brightness) {
        ScoreFeatureHandlerHandlersImpl->testBrightnessChangedHandler(brightness);
    }
}

MATCHER_P(DisplayBufferEqual, status, "") {
    uint8_t fieldFlag = (memcmp(arg, status, 8));

    return !(fieldFlag);
}

class diypinball_scoreFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        ScoreFeatureHandlerHandlersImpl = &myScoreFeatureHandlerHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_scoreFeatureHandlerInit_t scoreFeatureHandlerInit;

        scoreFeatureHandlerInit.displayChangedHandler = testDisplayChangedHandler;
        scoreFeatureHandlerInit.brightnessChangedHandler = testBrightnessChangedHandler;
        scoreFeatureHandlerInit.routerInstance = &router;

        diypinball_scoreFeatureHandler_init(&scoreFeatureHandler, &scoreFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_scoreFeatureHandlerInstance_t scoreFeatureHandler;
    MockCANSend myCANSend;
    MockScoreFeatureHandlerHandlers myScoreFeatureHandlerHandlers;
};

TEST_F(diypinball_scoreFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(4, scoreFeatureHandler.featureHandlerInstance.featureType);

    char expectedArray[] = {0, 0, 0, 0, 0, 0, 0, 0};

    ASSERT_EQ(&router, scoreFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&scoreFeatureHandler, scoreFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_TRUE(testDisplayChangedHandler == scoreFeatureHandler.displayChangedHandler);
    ASSERT_TRUE(testBrightnessChangedHandler == scoreFeatureHandler.brightnessChangedHandler);
    ASSERT_EQ(0, scoreFeatureHandler.brightness);
    ASSERT_TRUE( 0 == memcmp( expectedArray, scoreFeatureHandler.display, sizeof( expectedArray ) ) );
    ASSERT_TRUE(diypinball_scoreFeatureHandler_millisecondTickHandler == scoreFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_scoreFeatureHandler_messageReceivedHandler == scoreFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_scoreFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_scoreFeatureHandler_deinit(&scoreFeatureHandler);

    char expectedArray[] = {0, 0, 0, 0, 0, 0, 0, 0};

    ASSERT_EQ(0, scoreFeatureHandler.featureHandlerInstance.featureType);

    ASSERT_EQ(NULL, scoreFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, scoreFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, scoreFeatureHandler.brightness);
    ASSERT_TRUE(NULL == scoreFeatureHandler.displayChangedHandler);
    ASSERT_TRUE(NULL == scoreFeatureHandler.brightnessChangedHandler);
    ASSERT_TRUE( 0 == memcmp( expectedArray, scoreFeatureHandler.display, sizeof( expectedArray ) ) );
    ASSERT_TRUE(NULL == scoreFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == scoreFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_scoreFeatureHandler_test, request_to_feature_0_gets_display_blank)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;
    uint8_t i;

    for(i=0; i<16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        expectedCANMessage.rtr = 0;
        expectedCANMessage.dlc = 8;
        expectedCANMessage.data[0] = 0;
        expectedCANMessage.data[1] = 0;
        expectedCANMessage.data[2] = 0;
        expectedCANMessage.data[3] = 0;
        expectedCANMessage.data[4] = 0;
        expectedCANMessage.data[5] = 0;
        expectedCANMessage.data[6] = 0;
        expectedCANMessage.data[7] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_scoreFeatureHandler_test, message_to_feature_0_sets_display_full)
{
    diypinball_canMessage_t initiatingCANMessage;
    uint8_t i;

    char expectedDisplay[] = {15, 16, 32, 64, 14, 18, 19, 42};

    for(i=0; i<16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 8;
        initiatingCANMessage.data[0] = 15;
        initiatingCANMessage.data[1] = 16;
        initiatingCANMessage.data[2] = 32;
        initiatingCANMessage.data[3] = 64;
        initiatingCANMessage.data[4] = 14;
        initiatingCANMessage.data[5] = 18;
        initiatingCANMessage.data[6] = 19;
        initiatingCANMessage.data[7] = 42;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(DisplayBufferEqual(expectedDisplay))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_scoreFeatureHandler_test, message_to_feature_0_sets_display_incomplete)
{
    diypinball_canMessage_t initiatingCANMessage;
    uint8_t i;

    char expectedDisplay[] = {15, 16, 32, 64, 14, 0, 0, 0};

    for(i=0; i<16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 5;
        initiatingCANMessage.data[0] = 15;
        initiatingCANMessage.data[1] = 16;
        initiatingCANMessage.data[2] = 32;
        initiatingCANMessage.data[3] = 64;
        initiatingCANMessage.data[4] = 14;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(DisplayBufferEqual(expectedDisplay))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_scoreFeatureHandler_test, request_to_feature_0_gets_display_when_set_full)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;
    uint8_t i;

    char expectedDisplay[] = {15, 16, 32, 64, 14, 18, 19, 42};

    for(i=0; i<16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 8;
        initiatingCANMessage.data[0] = 15;
        initiatingCANMessage.data[1] = 16;
        initiatingCANMessage.data[2] = 32;
        initiatingCANMessage.data[3] = 64;
        initiatingCANMessage.data[4] = 14;
        initiatingCANMessage.data[5] = 18;
        initiatingCANMessage.data[6] = 19;
        initiatingCANMessage.data[7] = 42;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(DisplayBufferEqual(expectedDisplay))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        expectedCANMessage.rtr = 0;
        expectedCANMessage.dlc = 8;
        expectedCANMessage.data[0] = 15;
        expectedCANMessage.data[1] = 16;
        expectedCANMessage.data[2] = 32;
        expectedCANMessage.data[3] = 64;
        expectedCANMessage.data[4] = 14;
        expectedCANMessage.data[5] = 18;
        expectedCANMessage.data[6] = 19;
        expectedCANMessage.data[7] = 42;

        EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_scoreFeatureHandler_test, request_to_feature_0_gets_display_when_set_incomplete)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;
    uint8_t i;

    char expectedDisplay[] = {15, 16, 32, 64, 14, 0, 0, 0};

    for(i=0; i<16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 5;
        initiatingCANMessage.data[0] = 15;
        initiatingCANMessage.data[1] = 16;
        initiatingCANMessage.data[2] = 32;
        initiatingCANMessage.data[3] = 64;
        initiatingCANMessage.data[4] = 14;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(DisplayBufferEqual(expectedDisplay))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (0 << 4) | 0;
        expectedCANMessage.rtr = 0;
        expectedCANMessage.dlc = 8;
        expectedCANMessage.data[0] = 15;
        expectedCANMessage.data[1] = 16;
        expectedCANMessage.data[2] = 32;
        expectedCANMessage.data[3] = 64;
        expectedCANMessage.data[4] = 14;
        expectedCANMessage.data[5] = 0;
        expectedCANMessage.data[6] = 0;
        expectedCANMessage.data[7] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testDisplayChangedHandler(_)).Times(0);
        EXPECT_CALL(myScoreFeatureHandlerHandlers, testBrightnessChangedHandler(_)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}
