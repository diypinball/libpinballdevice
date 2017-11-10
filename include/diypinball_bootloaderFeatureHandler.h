#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

/*
 * \brief Function pointer to a boot-to-application request handler, whose implementation is platform-specific
 */
typedef void (*diypinball_bootloaderFeatureHandlerRebootHandler)(void);

/*
 * \brief Function pointer to a read flash to buffer request handler, whose implementation is platform-specific
 */
typedef uint8_t (*diypinball_bootloaderFeatureHandlerFlashReadHandler)(uint32_t baseAddress);

/*
 * \brief Function pointer to a write buffer to flash request handler, whose implementation is platform-specific
 */
typedef uint8_t (*diypinball_bootloaderFeatureHandlerFlashWriteHandler)(uint32_t baseAddress);

/*
 * \brief Function pointer to a verify buffer with flash request handler, whose implementation is platform-specific
 */
typedef uint8_t (*diypinball_bootloaderFeatureHandlerFlashVerifyHandler)(uint32_t baseAddress);

/*
 * \brief Function pointer to a check buffer CRC request handler, whose implementation is platform-specific
 */
typedef uint8_t (*diypinball_bootloaderFeatureHandlerBufferHashHandler)(uint32_t crcHash);

/*
 * \brief Function pointer to a read buffer request handler, whose implementation is platform-specific
 */
typedef void (*diypinball_bootloaderFeatureHandlerBufferReadHandler)(uint8_t offset, uint8_t *data);

/*
 * \brief Function pointer to a write buffer request handler, whose implementation is platform-specific
 */
typedef void (*diypinball_bootloaderFeatureHandlerBufferWriteHandler)(uint8_t offset, uint8_t *data);

/*
 * \struct diypinball_bootloaderFeatureHandlerInstance_t diypinball_bootloaderFeatureHandlerInstance
 * \brief Stores information relating to the instance of a BootloaderFeatureHandler feature
 */
typedef struct diypinball_bootloaderFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;         /**< featureDecoder instance for the FeatureRouter */
    uint8_t applicationVersionMajor;                                    /**< Major version number */
    uint8_t applicationVersionMinor;                                    /**< Minor version number */
    uint8_t applicationVersionPatch;                                    /**< Patch version number */
    uint32_t flashPageSize;                                             /**< Flash page size, for erase purposes */
    uint32_t flashBufferSize;                                           /**< Flash buffer size, for write purposes */
    uint32_t applicationBaseAddress;                                    /**< Application base address */
    uint32_t flashSize;                                                 /**< Device flash size, total (in bytes) */
    diypinball_bootloaderFeatureHandlerRebootHandler rebootHandler;   /**< Function pointer to reboot handler */
    diypinball_bootloaderFeatureHandlerFlashReadHandler flashReadHandler;/**< Flash read handler */
    diypinball_bootloaderFeatureHandlerFlashWriteHandler flashWriteHandler;/**< Flash write handler */
    diypinball_bootloaderFeatureHandlerFlashVerifyHandler flashVerifyHandler;/**< Flash verify handler */
    diypinball_bootloaderFeatureHandlerBufferHashHandler bufferHashHandler; /**< Buffer hash handler */
    diypinball_bootloaderFeatureHandlerBufferReadHandler bufferReadHandler; /**< Buffer read handler */
    diypinball_bootloaderFeatureHandlerBufferWriteHandler bufferWriteHandler; /**< Buffer write handler */
    uint32_t lastTick;                                                  /**< Last tick number received by millisecond tick handler */
} diypinball_bootloaderFeatureHandlerInstance_t;

/*
 * \struct diypinball_bootloaderFeatureHandlerInit_t diypinball_bootloaderFeatureHandlerInit
 * \brief Stores initialization information to set up a BootloaderFeatureHandler instance
 */
typedef struct diypinball_bootloaderFeatureHandlerInit {
    uint8_t applicationVersionMajor;                                    /**< Major version number */
    uint8_t applicationVersionMinor;                                    /**< Minor version number */
    uint8_t applicationVersionPatch;                                    /**< Patch version number */
    uint32_t flashPageSize;                                             /**< Flash page size, for erase purposes */
    uint32_t flashBufferSize;                                           /**< Flash buffer size, for write purposes */
    uint32_t applicationBaseAddress;                                    /**< Application base address */
    uint32_t flashSize;                                                 /**< Device flash size, total (in bytes) */
    diypinball_bootloaderFeatureHandlerRebootHandler rebootHandler;   /**< Function pointer to reboot handler */
    diypinball_bootloaderFeatureHandlerFlashReadHandler flashReadHandler;/**< Flash read handler */
    diypinball_bootloaderFeatureHandlerFlashWriteHandler flashWriteHandler;/**< Flash write handler */
    diypinball_bootloaderFeatureHandlerFlashVerifyHandler flashVerifyHandler;/**< Flash verify handler */
    diypinball_bootloaderFeatureHandlerBufferHashHandler bufferHashHandler; /**< Buffer hash handler */
    diypinball_bootloaderFeatureHandlerBufferReadHandler bufferReadHandler; /**< Buffer read handler */
    diypinball_bootloaderFeatureHandlerBufferWriteHandler bufferWriteHandler; /**< Buffer write handler */
    diypinball_featureRouterInstance_t *routerInstance;                 /**< FeatureRouter instance to connect to */
} diypinball_bootloaderFeatureHandlerInit_t;

/**
 * \brief Initialize the BootloaderFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  BootloaderFeatureHandler instance struct
 * \param[in] init                      BootloaderFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_bootloaderFeatureHandler_init(diypinball_bootloaderFeatureHandlerInstance_t *instance, diypinball_bootloaderFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a BootloaderFeatureHandler instance
 *
 * \param[in] instance                  BootloaderFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_bootloaderFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a BootloaderFeatureHandler instance
 *
 * \param[in] instance                  BootloaderFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_bootloaderFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the BootloaderFeatureHandler feature
 *
 * \param[in] instance                  BootloaderFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_bootloaderFeatureHandler_deinit(diypinball_bootloaderFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
