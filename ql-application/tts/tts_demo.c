/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tts_demo.h"
#include "ql_api_osi.h"
#include "ql_api_tts.h"
#include "ql_log.h"
#include "ql_osi_def.h"
#include "ql_audio.h"
#include "ql_fs.h"

//用户通过预置中文TTS资源文件,或英文TTS资源文件,来决定播放中文TTS还是英文TTS。默认预置中文TTS，中文TTS和英文TTS资源文件只能二选一
//若同时预置两个资源文件，则也是使用中文库; 
//英文TTS资源文件和中文TTS资源文件均在components\ql-config\download\prepack下，
//中文TTS资源文件为quectel_pcm_resource.bin，英文TTS资源文件为quectel_pcm_english.bin，用户通过修改prepack脚本选择预置哪一个资源文件，默认预置中文TTS
//英文TTS播放时，需要1.45M的RAM空间，因此选择使用英文tts资源文件时，要注意RAM空间是否充足； 选择中文TTS资源文件时，只需要620k的RAM空间
#define	QL_TTS_LANGUAGE_ENGLISH			0

#define QL_TTS_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_TTS_LOG(msg, ...)			QL_LOG(QL_TTS_LOG_LEVEL, "ql_app_tts", msg, ##__VA_ARGS__)
#define QL_TTS_LOG_PUSH(msg, ...)		QL_LOG_PUSH("ql_app_tts", msg, ##__VA_ARGS__)

#if !defined(tts_demo_no_err)
	#define tts_demo_no_err(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x != 0)                                                                        				\
				{                                                                                               \
					QL_TTS_LOG(str);																			\
					{action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif


/*===========================================================================
 * Variate
 ===========================================================================*/

PCM_HANDLE_T tts_player = NULL;
ql_task_t ql_tts_demo_task = NULL;

/*===========================================================================
 * Functions
 ===========================================================================*/

int userCallback(void *param, int param1, int param2, int param3, int data_len, const void *pcm_data)
{
	int err;

	err = ql_pcm_write(tts_player, (void *)pcm_data, data_len);
	if(err <= 0)
	{
		QL_TTS_LOG("write data to PCM player failed");
		return -1;
	}	
    return 0;
}


void ql_tts_thread(void *param)
{
    int err = 0, cnt = 0, len=0;
	QL_PCM_CONFIG_T config = {1, 16000, 0};
	
	ql_set_audio_path_earphone();
	ql_aud_set_volume(QL_AUDIO_PLAY_TYPE_LOCAL, AUDIOHAL_SPK_VOL_11);

	tts_player = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG);
	tts_demo_no_err(!tts_player, goto exit, "create pcm_player failed");

	err = ql_tts_engine_init(userCallback);
	tts_demo_no_err(err, goto exit, "tts session begain failed");
		
	while(1)
	{	
#if  !QL_TTS_LANGUAGE_ENGLISH
		if(cnt == 0)  //play utf8 encoding
		{
			char *tts_str = "֧支付宝收款123456789.000元";        //utf-8 encoding
			len = strlen(tts_str);
			ql_tts_set_config_param(QL_TTS_CONFIG_ENCODING,QL_TTS_UTF8);

			err = ql_tts_start((const char *)tts_str, len);
			tts_demo_no_err(err, goto exit, "tts start failed");
		}
		else if(cnt == 1) //play ucs2 encoding
		{
			uint16 tts_str[8] = {0x6B22, 0x8FCE, 0x4F7F, 0x7528, 0x79FB, 0x8FDC, 0x6A21, 0x5757}; //欢迎使用移远模块
			len = sizeof(tts_str);
			ql_tts_set_config_param(QL_TTS_CONFIG_ENCODING,QL_TTS_UCS2);

			err = ql_tts_start((const char *)tts_str, len);
			tts_demo_no_err(err, goto exit, "tts start failed");
		}
#else
		if(cnt == 2)
		{
			char *str_eng = "The price of the shirt is $50, and the price of the computer is $1200";
			len = strlen(str_eng);
			
			err = ql_tts_start((const char *)str_eng, len);
			tts_demo_no_err(err, goto exit, "tts start failed");
		}
#endif

		ql_rtos_task_sleep_ms(1000);							
		if(cnt == 2){
			goto exit;
		}
		else{
			cnt++;
			continue;
		}
	}

	exit:
		ql_aud_data_done();
		ql_aud_wait_play_finish(QL_WAIT_FOREVER);
		ql_tts_end();
	
		if(tts_player){
			ql_pcm_close(tts_player);
			tts_player = NULL;
		}		
		QL_TTS_LOG("tts done");
		ql_rtos_task_delete(NULL);
}



void ql_tts_demo1_init(void)
{
    uint8_t err = QL_OSI_SUCCESS;
      
    err = ql_rtos_task_create(&ql_tts_demo_task, QL_TTS_TASK_STACK, QL_TTS_TASK_PRIO, "ql_tts_task", ql_tts_thread, NULL, 1);
    if (err != QL_OSI_SUCCESS)
    {
        QL_TTS_LOG("TTS demo task created failed");
    }
}

