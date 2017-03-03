#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "diypinball.h"

using ::testing::Return;
using ::testing::_; // Matcher for parameters

class MockCANSend {
public:
    virtual ~MockCANSend() {}
    MOCK_METHOD1(testCanSendHandler, void(diypinball_canMessage_t*));
};

MATCHER_P(PinballMessageEqual, message, "") {
    uint8_t fieldFlag = (arg->priority == message.priority) &&
        (arg->unitSpecific == message.unitSpecific) &&
        (arg->boardAddress == message.boardAddress) &&
        (arg->featureType == message.featureType) &&
        (arg->featureNum == message.featureNum) &&
        (arg->function == message.function) &&
        (arg->reserved == message.reserved) &&
        (arg->messageType == message.messageType) &&
        (arg->dataLength == message.dataLength);

    uint8_t i;
    if(fieldFlag) {
        for(i=0; i < arg->dataLength; i++) {
            if(arg->data[i] != message.data[i]) {
                fieldFlag = 0;
                break;
            }
        }
    }
    return !(!fieldFlag);
}

MATCHER_P(CanMessageEqual, message, "") {
    uint8_t fieldFlag = (arg->id == message.id) &&
        (arg->rtr == message.rtr) &&
        (arg->dlc == message.dlc);

    uint8_t i;
    if(fieldFlag) {
        for(i=0; i < arg->dlc; i++) {
            if(arg->data[i] != message.data[i]) {
            	printf("Expected data byte %d: 0x%02x\r\n", i, message.data[i]);
            	printf("  Actual data byte %d: 0x%02x\r\n", i, arg->data[i]);
                fieldFlag = 0;
                break;
            }
        }
    } else {
    	printf("Expected id: 0x%08x, rtr: %d, dlc: %d\r\n", message.id, message.rtr, message.dlc);
    	printf("  Actual id: 0x%08x, rtr: %d, dlc: %d\r\n", arg->id, arg->rtr, arg->dlc);
    }
    return !(!fieldFlag);
}
