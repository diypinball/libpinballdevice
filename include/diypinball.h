/*
libpinballdevice
Copyright (C) 2018 Randy Glenn <randy.glenn@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdint.h>
#include <stddef.h>

/*
 * \struct diypinball_canMessage
 * \brief Stores an entire CAN message to be sent or having been received
 */
typedef struct diypinball_canMessage {
    uint32_t id;                                /**< CAN packet arbitration field */
    uint8_t rtr;                                /**< Remote transfer request flag */
    uint8_t dlc;                                /**< Data length */
    uint8_t data[8];                            /**< Packet data */
} diypinball_canMessage_t;

/*
 * \brief Pinball message type
 */
typedef enum diypinball_pinballMessageType {
    MESSAGE_RESPONSE,                           /**< Response to a received request */
    MESSAGE_COMMAND,                            /**< Command sent to a board */
    MESSAGE_REQUEST                             /**< Request for a response */
} diypinball_pinballMessageType_t;

/*
 * \struct diypinball_pinballMessage
 * \brief Stores an entire pinball packet, to be encoded to or decoded from a diypinball_canMessage_t
 */
typedef struct diypinball_pinballMessage {
    uint8_t priority;                           /**< Packet priority, 0-15 */
    uint8_t unitSpecific;                       /**< Unit-specific packet (1) or broadcast (0) */
    uint8_t boardAddress;                       /**< Board address, 0-255 */
    uint8_t featureType;                        /**< Feature type, 0-15 */
    uint8_t featureNum;                         /**< Feature number, 0-15 */
    uint8_t function;                           /**< Function, 0-15 */
    uint8_t reserved;                           /**< Reserved, 0-15 */
    diypinball_pinballMessageType_t messageType;/**< Message type */
    uint8_t dataLength;                         /**< Data length */
    uint8_t data[8];                            /**< Packet data */
} diypinball_pinballMessage_t;

/*
 * \brief Pinball function result
 */
typedef enum diypinball_result {
    RESULT_SUCCESS,                             /**< Function succeeded */
    RESULT_FAIL_INVALID_PARAMETER               /**< Failed - Provided parameter was invalid */
} diypinball_result_t;

/*
 * \brief Function pointer to a message send handler, implemented by the user for a given platform
 */
typedef void (*diypinball_canMessageSendHandler)(diypinball_canMessage_t *message);

#ifdef __cplusplus
}
#endif