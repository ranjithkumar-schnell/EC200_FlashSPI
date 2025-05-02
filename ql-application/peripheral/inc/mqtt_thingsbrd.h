/**  
  @file
 mqtt_demo.h

  @brief
  This file provides the definitions for datacall demo, and declares the 
  API functions.

*/
/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/


#ifndef MQTT_THINGSBRD_H
#define MQTT_THINGSBRD_H



		
#ifdef __cplusplus
		extern "C" {
#endif
		
/*===========================================================================
 * Macro Definition
 ===========================================================================*/




/*===========================================================================
 * Struct
 ===========================================================================*/	
		
typedef struct
{
  unsigned char PUMP_ON;
  unsigned char PUMP_OFF;
  unsigned char GET_live;
  unsigned char GET_cfg;
  unsigned char OTA;
  unsigned char Pint;
  unsigned char MotorType_P1;
  unsigned char* MotorType_P1val;
  unsigned char RatedVol_P2;
  unsigned char* RatedVol_P2val;
  unsigned char RatedFreq_P3;
  unsigned char* RatedFreq_P3val;
  unsigned char InstallHead_P4;
  unsigned char* InstallHead_P4val;
  unsigned char MinFreq_P5;
  unsigned char* MinFreq_P5val;
  unsigned char MaxFreq_P6;
  unsigned char* MaxFreq_P6val;
  unsigned char LowVol_P7;
  unsigned char* LowVol_P7val;
  unsigned char RelOnVol_P8;
  unsigned char* RelOnVol_P8val;
  unsigned char OverloadCur_P9;
  unsigned char* OverloadCur_P9val;
  unsigned char DryRun_PA;
  unsigned char* DryRun_PAval;
  unsigned char Temp_PB;
  unsigned char* Temp_PBval;
}RPC_value_t;

typedef struct {
	char APN1[25];
	char USR1[20];
	char PASS1[20];
	char APN2[20];
	char USR2[20];
	char PASS2[20];
	char RESTART[20];
	char UPDATEINTERVAL[20];
	char HEARTINTERVAL[20];
	char URTCDATE[20];
	char URTCTIME[20];
	char UPDATERTC[20];
	char GSMSYNC[20];
	char DO1[20];
	char AI1ZERO[20];
	char AI1SPAN[20];
	char AI2ZERO[20];
	char AI2SPAN[20];
	char AI3ZERO[20];
	char AI3SPAN[20];
	char AI4ZERO[20];
	char AI4SPAN[20];
	char URL[40];
	char PORT[20];
	char CID[50];
	char USERNAME[50];
	char PASSWORD[20];
	char FTPURL[20];
	char FTPUSER[20];
	char FTPPASS[20];
	char FTPPORT[20];
	char FTPDOWN[20];

} rmu_config;

rmu_config r_conf;


/*========================================================================
		 *	function Definition
*========================================================================*/
ql_queue_t rpc_data;
RPC_value_t TB_RPC,SEDEM_RPC;
ql_queue_t sdData,recvr_Sd_Data;
extern ql_task_t mqtt_task;

extern char pdata_int;
extern char hdata_int;
extern int sd_load_status;
extern int msgid_ev;
extern char cotp_v[12],potp_v[12],otp_buf[100],ondem_buf[1500],config_buf[1500];
extern char cfg_rw[10]; 
extern char mq_client_id[60];
extern char mq_client_uname[60];
extern char mq_client_pwd[20];
char operator_name[15]={'\0'};
char apn_name[20]={'\0'};
extern uint8_t apn_set;
extern bool hdata_onceupdate;
extern bool Retry_server_flag;
extern bool Retry_server_flag_Thingboard;
extern bool DataRepubFlag ;	
extern bool DataRepubFlag_TB;	
extern bool DataRepubFlag_SDM;
extern bool configdata_onceupdate;
extern bool ondemand_onceupdate;
extern int Pump_Run_Status; 
extern int temperature_status;
extern char mq_url[300];
extern char mq_operator[20];
extern char sd_NnwPath_name_cmpr[25];
extern char sd_NnwTBPath_name_cmpr[25];
extern char sd_NnwSDMPath_name_cmpr[25];
extern int numberOFLInes;

/*========================================================================
		 *	function Prototypes*
*========================================================================*/
void write_config_fileUFS(void);
void read_config_fileUFS(void);
void read_logged_files_fromRPC(char* c);
void read_logged_files_toRetry(char* d);
void publish_lost_data(char* pubData);
void publish_missed_Packets(char* pubDatas,int f);
int Connected_server(void);	
void airplane_mode(void);

#ifdef __cplusplus
		}/*"C" */
#endif
		
#endif   /*DATACALL_DEMO_H*/

