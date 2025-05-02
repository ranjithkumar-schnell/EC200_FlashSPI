/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

/************************************************************************************************************
  此demo将tts的api，与audio的播放函数一起进行封装，封装后的ql_tts_play，ql_tts_init，ql_tts_deinit与我司
  ASR系列的TTS播放函数类似，直接调用ql_tts_init+ql_tts_play即可以开始播放，播放完成后，调用ql_tts_deinit
  函数释放TTS资源。用户可参考tts_demo.c中的方法，也可使用本demo中的方法进行TTS播放
  注意：由于audio的播放器只有一个，因此此demo与tts_demo.c同一时间只能有一个运行

  用户如需将TTS库放到内核，则在target.config中将 CONFIG_QUEC_PROJECT_FEATURE_TTS_IN_KERNEL 置为 y,
  并修改分区，将内核分区增大250k左右，其余的内容不变
*************************************************************************************************************/

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
#include "ql_api_dev.h"

//用户通过预置中文TTS资源文件,或英文TTS资源文件,来决定播放中文TTS还是英文TTS。默认预置中文TTS，中文TTS和英文TTS资源文件只能二选一
//若同时预置两个资源文件，则也是使用中文库; 
//英文TTS资源文件和中文TTS资源文件均在components\ql-config\download\prepack下，
//中文TTS资源文件为quectel_pcm_resource.bin，英文TTS资源文件为quectel_pcm_english.bin，用户通过修改prepack脚本选择预置哪一个资源文件，默认预置中文TTS
//英文TTS播放时，需要1.45M的RAM空间，因此选择使用英文tts资源文件时，要注意RAM空间是否充足； 选择中文TTS资源文件时，只需要620k的RAM空间
#define	QL_TTS_LANGUAGE_USE_ENGLISH			0

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

typedef struct
{
	ql_tts_encoding_e encode;
	char 			  *str;
	uint			  len;
}tts_demo_play_info_t;

/*===========================================================================
 * Variate
 ===========================================================================*/

PCM_HANDLE_T tts_decoder = NULL;
ql_task_t  ql_tts_demo_task2 = NULL;
ql_queue_t ql_tts_demo_queue = NULL;
/*===========================================================================
 * Functions
 ===========================================================================*/
int userCallback(void *param, int param1, int param2, int param3, int data_len, const void *pcm_data)
{
	int err;

	err = ql_pcm_write(tts_decoder, (void *)pcm_data, data_len);
	if(err <= 0)
	{
		QL_TTS_LOG("write data to PCM player failed");
		return -1;
	}	
    return 0;
}

void ql_tts_thread_demo_2(void *param)
{
    int err = 0;
	tts_demo_play_info_t info = {0};
		
	ql_set_audio_path_earphone();
	ql_aud_set_volume(QL_AUDIO_PLAY_TYPE_LOCAL, AUDIOHAL_SPK_VOL_11);
		
	while(1)
	{	
		err = ql_rtos_queue_wait(ql_tts_demo_queue, (uint8 *)&info, sizeof(tts_demo_play_info_t), QL_WAIT_FOREVER);
		tts_demo_no_err(err, continue, "invalid queue");
		
		if(!info.str || !info.len){
			QL_TTS_LOG("invalid tts string");
			continue;
		}
	
		err = ql_tts_init(userCallback);
		tts_demo_no_err(err, goto exit, "tts init failed");

#if !QL_TTS_LANGUAGE_USE_ENGLISH //使用英文TTS库不需要设置编码
		err = ql_tts_set_config_param(QL_TTS_CONFIG_ENCODING, info.encode);
		tts_demo_no_err(err, goto exit, "config tts failed");
#endif

		err = ql_tts_start(info.str, info.len);
		tts_demo_no_err(err, goto exit, "tts start failed");
		
exit:
		ql_aud_data_done();
		ql_aud_wait_play_finish(QL_WAIT_FOREVER);
		ql_tts_deinit();
		if(info.str){
			free(info.str);
		}
	
		QL_TTS_LOG("tts done");
	}
}

int ql_tts_init(pUserCallback mCallback)
{
	if(!mCallback){
		return QL_TTS_INVALID_PARAM;
	}

	if(!ql_tts_is_running())
	{
		QL_PCM_CONFIG_T config = {1, 16000, 0};
		
		tts_decoder = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG);
		tts_demo_no_err(!tts_decoder, return QL_TTS_DEVICE_BUSY, "create pcm_player failed");

		int	err = ql_tts_engine_init(userCallback);
		tts_demo_no_err(err, return err, "tts session begain failed");
	}
	else
	{
		QL_TTS_LOG("tts is running");
		return QL_TTS_DEVICE_BUSY;
	}
	return QL_TTS_SUCCESS;
}

int ql_tts_deinit(void)
{
	int err = 0;

	err = ql_pcm_close(tts_decoder);
	tts_demo_no_err(err, return err, "close pcm failed");	
	tts_decoder = NULL;
	
	err = ql_tts_end();
	tts_demo_no_err(err, return err, "tts end failed");

	return err;
}

int ql_tts_play(ql_tts_encoding_e encoding, const char* string, uint len)
{
	tts_demo_play_info_t param = {0};
	int err = 0;

	if(encoding < QL_TTS_GBK || encoding > QL_TTS_UCS2 || !string || !len)
	{
		QL_TTS_LOG("invalid param");
		return QL_TTS_INVALID_PARAM;
	}

	param.encode = encoding;
	param.len = len;
	param.str = calloc(1, len);
	if(!param.str)
	{
		QL_TTS_LOG("tts no memory");
		return QL_TTS_NO_MEMORY;
	}
	
	memcpy(param.str, string, len);
	
	err = ql_rtos_queue_release(ql_tts_demo_queue, sizeof(tts_demo_play_info_t), (uint8 *)&param, 0);
	if(err)
	{
		free(param.str);
	}

	return err;
}

int ql_tts_play_english(const char* string, uint len)
{
	tts_demo_play_info_t param = {0};
	int err = 0;

	if(!string || !len)
	{
		QL_TTS_LOG("invalid param");
		return QL_TTS_INVALID_PARAM;
	}

	param.len = len;
	param.str = calloc(1, len);
	if(!param.str)
	{
		QL_TTS_LOG("tts no memory");
		return QL_TTS_NO_MEMORY;
	}

	memcpy(param.str, string, len);
	err = ql_rtos_queue_release(ql_tts_demo_queue, sizeof(tts_demo_play_info_t), (uint8 *)&param, 0);
	if(err)
	{
		free(param.str);
	}
	return err;
}

void ql_tts_demo2_init(void)
{
    uint8_t err = QL_OSI_SUCCESS;
	
	err = ql_rtos_queue_create(&ql_tts_demo_queue, sizeof(tts_demo_play_info_t), 10);
	if (err != QL_OSI_SUCCESS)
    {
        QL_TTS_LOG("TTS queue created failed");
		return;
    }
	
    err = ql_rtos_task_create(&ql_tts_demo_task2, QL_TTS_TASK_STACK, QL_TTS_TASK_PRIO-1, "ql_tts_task", ql_tts_thread_demo_2, NULL, 1);
    if (err != QL_OSI_SUCCESS)
    {
    	ql_rtos_queue_delete(ql_tts_demo_queue);
		ql_tts_demo_queue = NULL;
        QL_TTS_LOG("TTS demo task2 created failed");
		return;
    }

#if !QL_TTS_LANGUAGE_USE_ENGLISH	
	char *str1 = "支付宝收款 12345元";
	char *str2 = "您已超速, 请减速";
	char *str3 = "条形码为: 2 2 1 9 8 3 3 6 4 5 2 3 8 8"; //空格代表以号码的方式播报
	uint16 ucs_str[8] = {0x6B22, 0x8FCE, 0x4F7F, 0x7528, 0x79FB, 0x8FDC, 0x6A21, 0x5757}; //欢迎使用移远模块

	ql_tts_play(QL_TTS_UTF8, str1, strlen(str1));
	ql_tts_play(QL_TTS_UTF8, str2, strlen(str2));
	ql_tts_play(QL_TTS_UTF8, str3, strlen(str3));
	ql_tts_play(QL_TTS_UCS2, (const char *)ucs_str, sizeof(ucs_str));
#else
	char *str_eng = "The price of the shirt is $50, and the price of the computer is $1000";
	ql_tts_play_english(str_eng, strlen(str_eng));
#endif
	
}

