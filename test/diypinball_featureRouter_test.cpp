#include "gtest/gtest.h"

#include <stdint.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        return;
    }
}

TEST(diypinball_featureRouter_test, init_zeros_structure)
{
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    for(uint8_t i=0; i<16; i++) {
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ(instance.boardAddress, init.boardAddress);
    ASSERT_EQ(instance.canSendHandler, testCanSendHandler);
}
