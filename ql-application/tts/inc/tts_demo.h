/**  @file
  audio_demo.h

  @brief
  This file is used to define audio demo for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

#ifndef TTS_DEMO_H
#define TTS_DEMO_H


#ifdef __cplusplus
extern "C" {
#endif

#include "ql_api_tts.h"

/*========================================================================
 *  Variable Definition
 *========================================================================*/
#define QL_TTS_TASK_PRIO     25
#define QL_TTS_TASK_STACK    8*1024

/*========================================================================
 *  function Definition
 *========================================================================*/

void ql_tts_demo1_init(void);
void ql_tts_demo2_init(void);
int ql_tts_init(pUserCallback mCallback);
int ql_tts_deinit(void);
int ql_tts_play(ql_tts_encoding_e mode, const char* string, uint len);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* AUDIO_DEMO_H */


