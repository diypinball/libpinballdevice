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
