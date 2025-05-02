/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _QUEC_BOOT_VFS_H_
#define _QUEC_BOOT_VFS_H_

#include "osi_compiler.h"
#include "sffs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "quec_boot_pff.h"

#define QUEC_BOOT_VFS_ECHO  0

#define QUEC_BOOT_LEVEL OSI_LOG_LEVEL_INFO
#define QUEC_BOOT_LOG_TAG OSI_MAKE_LOG_TAG('Q', 'U', 'E', 'V')
#define QUEC_BOOT_PRINTF(level, fmt, ...)                                        \
    do                                                                          \
    {                                                                           \
        if (QUEC_BOOT_LEVEL >= level)                                            \
            osiTracePrintf((level << 28) | (QUEC_BOOT_LOG_TAG), fmt, ##__VA_ARGS__); \
    } while (0)
			
#define custom_boot_log(user_str, msg, ...)				QUEC_BOOT_PRINTF(QUEC_BOOT_LEVEL, "["user_str"][%s, %d] "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define QUEC_BOOT_VFS_LOG(msg, ...)                custom_boot_log("boot vfs", msg, ##__VA_ARGS__)

OSI_EXTERN_C_BEGIN

int quec_bootVfsOpen(const char *path);
int quec_bootVfsRead(int fd, void *data, size_t size);
bool quec_bootVfsClose(int fd);
int quec_bootVfsFileSize(int fd,int *size);

OSI_EXTERN_C_END
#endif
