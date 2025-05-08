/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
ver sdk4 ver33 20/1/24   ERK            RPC data decode added in this version
=================================================================*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_nw.h"
#include "ql_log.h"
#include "ql_api_datacall.h"
#include "mqtt_thingsbrd.h"
#include "ql_mqttclient.h"
#include "uart_demo.h"
#include "gnss_cri.h"
#include "mqtt_packet_Encode_decode.c"
#include "fota_ftp.h"
#include "fota_ftp.c"
#include "ql_fs.h"
#define QL_MQTT_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_MQTT_LOG(msg, ...)			QL_LOG(QL_MQTT_LOG_LEVEL, "ql_MQTT", msg, ##__VA_ARGS__)
#define QL_MQTT_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_MQTT", msg, ##__VA_ARGS__)

#define MQTT_CLIENT_QUECTEL_URL                  "mqtt://cri.schnelliot.in:1883"
#define MQTT_CLIENT_QUECTEL_SSL_URL              "mqtts://pmkrms.hareda.gov.in:443"



#define topic_len 300


static ql_sem_t  mqtt_semp;
mqtt_client_t  mqtt_cli;
mqtt_client_t  mqtt_cli_sdm;
ql_task_t mqtt_task = NULL;
struct mqtt_connect_client_info_t  client_info = {0},client_info_sdm={0};
int  mqtt_connected = 0;
int  mqtt_connected_sdm = 0;
int mq_port=0;
uint8_t j_index=1,apn_set=0;
bool TB_send=0,panel_mode=0,disc_f=0;
char datapub[topic_len],heartbeatpub[topic_len],ondemandsub[topic_len],ondemandpub[topic_len],configpub[topic_len],configsub[topic_len],otpsub[topic_len],infosub[topic_len];
char apn_nam[30]={'\0'};
char rpc_buf[100];
char mqtt_ssl_url[100],mq_url[300],mq_operator[20];
char no_NwLog_data[2000]={'\0'};
char RPC_Log_data[2000]={'\0'};
unsigned char Periodic_data[]      = "$POPI#";
unsigned char Atribute_data[]      = "$FPDS#";	
unsigned char On_Demand_PumpOn[]   = "$$PON#";	
unsigned char On_Demand_PumpOff[]  = "$$POF#";
unsigned int DIndex = 0; 

// char mq_client_id[60]={"d:860057063288996$standalonesolarpump$28"};
// char mq_client_uname[60]={"860057063288996$standalonesolarpump$28"};
char mq_client_id[60]={'\0'};
char mq_client_uname[60]={'\0'};
char mq_client_pwd[20]={"c6c4dd14"};
char SedemPassword[20]={'\0'};
//char test_file[]="UFS:cred.txt";
int NOntwrk_data_flag =0;
int TBrd_missing_packet_flag =0;
int SDM_missing_packet_flag =0;
int NumberofPacketsInQueue = 0;


/*===========================================================================
 * USER FUNCTION STARTS FROM HERE*
 ===========================================================================*/
void airplane_mode() {
  int err;
  
  err=ql_dev_set_modem_fun(0, 0, 0);
   QL_MQTT_LOG("my_log_airplan mode before=%d",err);
  QL_MQTT_LOG("my_log_airplan mode");
   ql_rtos_task_sleep_s(2);
  err=ql_dev_set_modem_fun(1, 0, 0);
  QL_MQTT_LOG("my_log_airplan mode after=%d",err);

}

/*===========================================================================
 * Funcction Sets operatorName and APN*
 ===========================================================================*/
void check_operator()
{
	ql_nw_operator_info_s *oper_info = (ql_nw_operator_info_s *)calloc(1, sizeof(ql_nw_operator_info_s));
	ql_nw_get_operator_name(sim_id, oper_info);
	QL_MQTT_LOG("long_oper_name:%s, short_oper_name:%s\r\n",oper_info->long_oper_name, oper_info->short_oper_name);
	strcpy(operator_name,oper_info->long_oper_name);
	QL_MQTT_LOG("THE OPERATOR NAME IS=%s\r\n",operator_name);
	if((strcmp(operator_name,"Airtel")==0)||(strcmp(operator_name,"airtel")==0)||(strcmp(operator_name,"AIRTEL")==0)||(strcmp(operator_name,"AirTel")==0))apn_set=1;
	else if((strcmp(operator_name,"CellOne")==0)||(strcmp(operator_name,"cellone")==0)||(strcmp(operator_name,"CELLONE")==0)||(strcmp(operator_name,"Cellone")==0))apn_set=2;
	else if((strcmp(operator_name,"Vodafone")==0)||(strcmp(operator_name,"vodafone")==0)||(strcmp(operator_name,"VODAFONE")==0)||(strcmp(operator_name,"VI")==0)||(strcmp(operator_name,"Vi")==0)||(strcmp(operator_name,"vi")==0))apn_set=3;
	else if((strcmp(operator_name,"RJIO")==0)||(strcmp(operator_name,"Rjio")==0)||(strcmp(operator_name,"rjio")==0)||(strcmp(operator_name,"JIO")==0)||(strcmp(operator_name,"Jio")==0)||(strcmp(operator_name,"jio")==0))apn_set=4;
    else
    {
	 
	
		 apn_set=5;
		 QL_MQTT_LOG("inside apn_set 5\r\n");
	     QL_MQTT_LOG("apn undefined in check operator fn\r\n");	
	
	}
}

/*===========================================================================
 * Processes RPC and send the Data to Other threads*
 ===========================================================================*/
void send_rpc_to_criBrd(RPC_value_t *RPC)
{
   QL_MQTT_LOG("rpc value received RPC.PUMP_ON is:%d\r\n",RPC->PUMP_ON);
   if(RPC->PUMP_ON==1)
   {
	ql_rtos_task_sleep_s(5);
	 RPC->PUMP_ON=0;
	 uart_data_send(On_Demand_PumpOn);
	 QL_MQTT_LOG("Pump ON RPC response sent to CRI brd");
   }
   QL_MQTT_LOG("rpc value received RPC.PUMP_OFF is:%d\r\n",RPC->PUMP_OFF);
   if(RPC->PUMP_OFF==1)
   {
	ql_rtos_task_sleep_s(5);
	 RPC->PUMP_OFF= 0;
	 uart_data_send(On_Demand_PumpOff);
	 QL_MQTT_LOG("Pump OFF RPC response sent to CRI brd");
	}
   QL_MQTT_LOG("rpc value received RPC.ATRRIBUTES is:%d\r\n",RPC->GET_cfg);
   if( RPC->GET_cfg==1)
   {
	ql_rtos_task_sleep_s(5);
	 RPC->GET_cfg= 0;
	 uart_data_send(Atribute_data);
	 QL_MQTT_LOG("Pump config RPC response sent to CRI brd");
	}
   QL_MQTT_LOG("rpc value received RPC.GEL LIVE is:%d\r\n",RPC->GET_live);
    if(RPC->GET_live==1)
   {
	ql_rtos_task_sleep_s(5);
	 RPC->GET_live= 0;
	 uart_data_send(Periodic_data);
	 QL_MQTT_LOG("Pump periodic RPC response sent to CRI brd");
	}

	 if(RPC->OTA==1)
   {
	  QL_MQTT_LOG("rpc value received RPC.OTA is:%d\r\n",RPC->OTA);
	  RPC->OTA= 0;
	  QL_MQTT_LOG("OTA Process has begin when 0 RPC.OTA is:%d\r\n",RPC->OTA);
	  ql_rtos_task_suspend(gnss_task);
	  if(fota_ftp_app() ==1) 
	   {
		fota_ftp_app();
	   }
	}

	 if(RPC->Pint == 1)
   {
	 RPC->Pint= 0;
	 QL_MQTT_LOG("rpc value received RPC.pints \r\n");
	}

	if(RPC->MotorType_P1 == 1)
   {
	 RPC->MotorType_P1= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->MotorType_P1val);
	 QL_MQTT_LOG("rpc value received for Motor type \r\n");
	 RPC->GET_cfg = 1;
	}

	 if(RPC->RatedVol_P2 == 1)
   {
	 RPC->RatedVol_P2= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->RatedVol_P2val);
	 QL_MQTT_LOG("rpc value received for rated voltage \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->RatedFreq_P3 == 1)
   {
	 RPC->RatedFreq_P3= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->RatedFreq_P3val);
	 QL_MQTT_LOG("rpc value received Rated frequency \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->InstallHead_P4 == 1)
   {
	 RPC->InstallHead_P4= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->InstallHead_P4val);
	 QL_MQTT_LOG("rpc value received for Install head \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->MinFreq_P5 == 1)
   {
	 RPC->MinFreq_P5= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->MinFreq_P5val);
	 QL_MQTT_LOG("rpc value received for Min frequency\r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->MaxFreq_P6 == 1)
   {
	 RPC->MaxFreq_P6= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->MaxFreq_P6val);
	 QL_MQTT_LOG("rpc value received for Max frequncy \r\n");
	 RPC->GET_cfg = 1;
	 
	}

	if(RPC->LowVol_P7 == 1)
   {
	 RPC->LowVol_P7= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->LowVol_P7val);
	 QL_MQTT_LOG("rpc value received for low volt \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->RelOnVol_P8 == 1)
   {
	 RPC->RelOnVol_P8= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->RelOnVol_P8val);
	 QL_MQTT_LOG("rpc value received Relay on voltage \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->OverloadCur_P9 == 1)
   {
	 RPC->OverloadCur_P9= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->OverloadCur_P9val);
	 QL_MQTT_LOG("rpc value received overload current set \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->DryRun_PA == 1)
   {
	 RPC->DryRun_PA= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->DryRun_PAval);
	 QL_MQTT_LOG("rpc value received Dry run set \r\n");
	 RPC->GET_cfg = 1;
	}

	if(RPC->Temp_PB == 1)
   {
	 RPC->Temp_PB= 0;
	 ql_rtos_task_sleep_s(5);
	  uart_data_send(RPC->Temp_PBval);
	 QL_MQTT_LOG("rpc value received for temp \r\n");
	 RPC->GET_cfg = 1;
	}
  
}

/*===========================================================================
 * Sim network registration  Function*
 ===========================================================================*/

int sim_net_register(int nSim, int profile_idx)
{
	int ret = 0, i = 0;
	ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};

	while((ret = ql_network_register_wait(nSim, 20)) != 0 && i < 5){
		i++;
		// ql_rtos_task_sleep_ms(1000);
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0) {
		i = 0;
		QL_MQTT_LOG("====network registered!!!!====");
	} else {
		QL_MQTT_LOG("====network register failure!!!!!====");
		return ret;
	}
	check_operator();
	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);
	
	if(apn_set==1)
			{
				QL_MQTT_LOG("apn set as airtel\r\n");
				ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "airtelgprs.com", NULL, NULL, 0);
			}		
			else if(apn_set==2)
			{
				QL_MQTT_LOG("apn set as bsnl\r\n");
				ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "bsnlnet", NULL, NULL, 0); 
			}
			else if(apn_set==3)
			{
				QL_MQTT_LOG("apn set as vodafone\r\n");
				ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "www", NULL, NULL, 0); 
			}
			else if(apn_set==4)
			{
				QL_MQTT_LOG("apn set as jio\r\n");
				ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "jionet", NULL, NULL, 0); 
			}

			else if(apn_set==5)
			{
              	QL_MQTT_LOG("apn undefined\r\n");
				ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "www", NULL, NULL, 0); 
				
			}

			
	
	if(ret != 0) {
		QL_MQTT_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0) {
		QL_MQTT_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		return ret;
	}
		
	QL_MQTT_LOG("info->profile_idx: %d", info.profile_idx);
	QL_MQTT_LOG("info->ip_version: %d", info.ip_version);
					
	QL_MQTT_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);
		
	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);
		
	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_MQTT_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	return ret;
}

/*===========================================================================
 * Call backs for Thingsboard starts from here*
 ===========================================================================*/



static void mqtt_state_exception_cb(mqtt_client_t *client)
{
	QL_MQTT_LOG("mqtt session abnormal disconnect");
	mqtt_connected = 0;
}


static void mqtt_connect_result_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_e status)
{
	QL_MQTT_LOG("connect status for TB server is : %d", status);
	if(status == 0)
	{
		mqtt_connected = 1;
	}
	ql_rtos_semaphore_release(mqtt_semp);
}


static void mqtt_requst_result_cb(mqtt_client_t *client, void *arg,int err)
{
	QL_MQTT_LOG("mqtt TB err from request result callback occured: %d", err);
	
	ql_rtos_semaphore_release(mqtt_semp);
}


static void mqtt_inpub_data_cb(mqtt_client_t *client, void *arg, int pkt_id, const char *topic, const unsigned char *payload, unsigned short payload_len)
{
	QL_MQTT_LOG(" subscribe data received");
	QL_MQTT_LOG("topic: %s", topic);
	QL_MQTT_LOG("payload: %s", payload);
	memset(rpc_buf,0x00,100);
	int x=0;
	while(x<payload_len) {rpc_buf[x]=payload[x]; x++; }
	jsonextract(rpc_buf);
	QL_MQTT_LOG("rpc value received in struct TB_RPC.PUMP_ON is:%d\r\n",TB_RPC.PUMP_ON);
	QL_MQTT_LOG("rpc value received in struct TB_RPC.PUMP_OFF is:%d\r\n",TB_RPC.PUMP_OFF);
	QL_MQTT_LOG("rpc value received in struct TB_RPC.GET_cfg is:%d\r\n",TB_RPC.GET_cfg);
	QL_MQTT_LOG("rpc value received in struct TB_RPC.GET_live is:%d\r\n",TB_RPC.GET_live);
	QL_MQTT_LOG("rpc value received in struct TB_RPC.OTA is:%d\r\n",TB_RPC.OTA);
    QL_MQTT_LOG("rpc value received in struct TB_RPC.pint is:%d\r\n",TB_RPC.Pint);
    ql_rtos_queue_release(rpc_data,sizeof(RPC_value_t),(uint8 *)&TB_RPC,0);
	
}

/*===========================================================================
 * Call backs for THingsboard Ends here*
 ===========================================================================*/

 /*===========================================================================
 * Call backs for Sedem server Starts from here*
 ===========================================================================*/

static void mqtt_state_exception_cb_sdm(mqtt_client_t *client)
{
	QL_MQTT_LOG(" Sedem mqtt session abnormal disconnect");
	mqtt_connected_sdm = 0;
}


static void mqtt_connect_result_cb_sdm(mqtt_client_t *client, void *arg, mqtt_connection_status_e status)
{
	QL_MQTT_LOG("connect status for Sedem server is : %d", status);
	if(status == 0){
		mqtt_connected_sdm = 1;
	}
	ql_rtos_semaphore_release(mqtt_semp);
}


static void mqtt_requst_result_cb_sdm(mqtt_client_t *client, void *arg,int err)
{
	QL_MQTT_LOG("mqtt sedem err from request result callback occured: %d", err);
	
	ql_rtos_semaphore_release(mqtt_semp);
}


static void mqtt_inpub_data_cb_sdm(mqtt_client_t *client, void *arg, int pkt_id, const char *topic, const unsigned char *payload, unsigned short payload_len)
{
	QL_MQTT_LOG(" TB subscribe data received");
	QL_MQTT_LOG("TB topic: %s", topic);
	QL_MQTT_LOG("TB payload: %s", payload);
	if(strcmp(topic,otpsub)==0)
	{
		QL_MQTT_LOG("otp payload is received: %s", payload);
		// strcpy(otp_buf,payload);
		memset(otp_buf,0x00,100);
		int x=0;
		while(x<payload_len) {otp_buf[x]=payload[x]; x++; }
		jsonextract(otp_buf);             
	}

	else if(strcmp(topic, ondemandsub)==0)
	{
		QL_MQTT_LOG("ondemand payload is received: %s", payload);
		memset(ondem_buf,0x00,1500);
		int x=0;
		while(x<payload_len) {ondem_buf[x]=payload[x]; x++; }
		jsonextract(ondem_buf);
		QL_MQTT_LOG("Publish ondemand json data in sedem\r\n");
		ondemand_onceupdate = 1;
		/*		Respose for ondemand request       */

	}

		else if(strcmp(topic, configsub)==0)
	{
		QL_MQTT_LOG("CONFIG payload is received: %s", payload);
		memset(config_buf,0x00,1500);
		int x=0;
		while(x<payload_len) {config_buf[x]=payload[x]; x++; }
		//config_write_file(config_buf);
		jsonextract(config_buf);
		QL_MQTT_LOG("Publish config json data in sedem\r\n");
		configdata_onceupdate = 1;
		
		/*		Respose for ondemand request       */
														
	}
	ql_rtos_queue_release(rpc_data,sizeof(RPC_value_t),(uint8 *)&TB_RPC,0);
}


 /*===========================================================================
 * Call backs for Sedem server Ends here*
 ===========================================================================*/

 /*Publishes old log data by RPC ddmmyyyy.txt from sd card*/
void publish_lost_data(char* pubData)
{
	if ((mqtt_connected ==1) && (mqtt_connected_sdm ==1))
	{
		if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",pubData, strlen(pubData), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
	     {
    	   QL_MQTT_LOG("======wait for TB TELEMTERY publish result");
    	   ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
         }

	  if(ql_mqtt_publish(&mqtt_cli_sdm,datapub,pubData, strlen(pubData), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
       {
          QL_MQTT_LOG("======wait for sedem Datapub publish result");
          ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
       }
	}

	if ((mqtt_connected ==1) && (mqtt_connected_sdm ==0))
	{
		if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",pubData, strlen(pubData), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
	     {
    	   QL_MQTT_LOG("======wait for TB TELEMTERY publish result");
    	   ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
         }
	}

	if ((mqtt_connected ==0) && (mqtt_connected_sdm ==1))
	{
	  if(ql_mqtt_publish(&mqtt_cli_sdm,datapub,pubData, strlen(pubData), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
       {
          QL_MQTT_LOG("======wait for sedem Datapub publish result");
          ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
       }
	}
   			
}

void publish_missed_Packets(char* pubDatas,int flagr)
{
	if (flagr == 1)
	{
		if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",pubDatas, strlen(pubDatas), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
	     {
    	   QL_MQTT_LOG("======wait for TB TELEMTERY publish result");
    	   ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
         }

	  if(ql_mqtt_publish(&mqtt_cli_sdm,datapub,pubDatas, strlen(pubDatas), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
       {
          QL_MQTT_LOG("======wait for sedem Datapub publish result");
          ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
       }

	   flagr =0;
	}

	if (flagr == 2)
	{
		if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",pubDatas, strlen(pubDatas), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
	     {
    	   QL_MQTT_LOG("======wait for TB TELEMTERY publish result");
    	   ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
         }
		 flagr =0;
	}

	if (flagr == 3)
	{
	  if(ql_mqtt_publish(&mqtt_cli_sdm,datapub,pubDatas, strlen(pubDatas), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
       {
          QL_MQTT_LOG("======wait for sedem Datapub publish result");
          ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
       }
	    flagr =0;
	}
   			
}
 /*===========================================================================
 * Checks the Server health status and returns server status to*
 ===========================================================================*/
int Connected_server()
{
	int ret = 0;
	if ((mqtt_connected ==0) && (mqtt_connected_sdm ==0))
	{
		QL_MQTT_LOG("both servers down");
		ret = 3;
	}

	if ((mqtt_connected ==1) && (mqtt_connected_sdm ==0))
	{
		QL_MQTT_LOG("sedem server down");
		ret = 2;
	}

	if ((mqtt_connected ==0) && (mqtt_connected_sdm ==1))
	{
	  QL_MQTT_LOG("schnell iot server down");
	   ret = 1;
	}

	if ((mqtt_connected ==1) && (mqtt_connected_sdm ==1))
	{
	  QL_MQTT_LOG("schnell iot server down");
	   ret = 0;
	}
   	return ret;	
}
/**************Main MQTT thread starts here**********************/
static void mqtt_app_thread(void * arg)
{
	int ret = 0;
	int run_num = 1;
	int profile_idx = 1;
    //mqtt_client_t  mqtt_cli,mqtt_cli_sdm;
	uint8_t nSim = 0;
	uint16_t sim_cid;
	
	ql_rtos_queue_create(&sdData,sizeof(sd_buffer),650);//  sending queue
	ql_rtos_queue_create(&recvr_Sd_Data,sizeof(sd_buffer_rcvr),650);//  sending queue
 
   	sprintf(mq_client_id,"d:%s$standalonesolarpump$28",imei_no);
	QL_MQTT_LOG("client id for the Sedem server is:%s",mq_client_id);

    sprintf(mq_client_uname,"%s$standalonesolarpump$28",imei_no);
	QL_MQTT_LOG("client user name for the Sedem server is:%s",mq_client_uname);

	memset(datapub, 0, sizeof(datapub)); 
	sprintf(datapub,"iiot-1/standalonesolarpump/%s/data/pub",imei_no);

	memset(heartbeatpub, 0, sizeof(heartbeatpub)); 
	sprintf(heartbeatpub,"iiot-1/standalonesolarpump/%s/heartbeat/pub",imei_no);

	memset(ondemandsub, 0, sizeof(ondemandsub)); 
	sprintf(ondemandsub,"iiot-1/standalonesolarpump/%s/ondemand/sub",imei_no);

	memset(ondemandpub, 0, sizeof(ondemandpub)); 
	sprintf(ondemandpub,"iiot-1/standalonesolarpump/%s/ondemand/pub",imei_no);

	memset(configpub, 0, sizeof(configpub)); 
	sprintf(configpub,"iiot-1/standalonesolarpump/%s/config/pub",imei_no);

	memset(configsub, 0, sizeof(configsub)); 
	sprintf(configsub,"iiot-1/standalonesolarpump/%s/config/sub",imei_no);

	memset(otpsub, 0, sizeof(otpsub)); 
	sprintf(otpsub,"iiot-1/standalonesolarpump/%s/otp/sub",imei_no);

	memset(infosub, 0, sizeof(infosub)); 
	sprintf(infosub,"iiot-1/standalonesolarpump/%s/info/sub",imei_no);

   

	QL_MQTT_LOG("========== mqtt demo start ==========");
	QL_MQTT_LOG("wait for network register done");



	startAgain:
	ql_rtos_semaphore_create(&mqtt_semp, 0);
	ql_rtos_task_sleep_s(10);
	hdata_onceupdate =0;
 	configdata_onceupdate =0;
 	ondemand_onceupdate = 0;
 	Retry_server_flag = 0;


	ql_dev_get_imei(imei_no, sizeof(imei_no), 0);
	QL_MQTT_LOG("IMEI: %s\r\n", imei_no);

    
	QL_MQTT_LOG("========== mqtt demo start ==========");
	QL_MQTT_LOG("wait for network register done");


	ret = sim_net_register(nSim, profile_idx);  // checks network registration and returns the status
		
		if(ret == 0)
		{
		
		QL_MQTT_LOG("====network registered!!!!====");
	    }else
		{
		QL_MQTT_LOG("====network register failure!!!!!====");
		}
	 		
	while(run_num <= 50)
	{	
       
       
   
		int ret = MQTTCLIENT_SUCCESS;
		
		QL_MQTT_LOG("==============mqtt_client_test[%d]================\n",run_num++);

		if(QL_DATACALL_SUCCESS != ql_bind_sim_and_profile(nSim, profile_idx, &sim_cid))
		{
			QL_MQTT_LOG("nSim or profile_idx is invalid!!!!");
			break;
		}
		
		if(ql_mqtt_client_init(&mqtt_cli, sim_cid) != MQTTCLIENT_SUCCESS){
			QL_MQTT_LOG("mqtt client init failed!!!!");
			goto sedem_Publish_only;
			break;
		}

		QL_MQTT_LOG("mqtt_cli:%d", mqtt_cli);

        client_info.keep_alive = 60;
		client_info.clean_session = 1;
		client_info.will_qos = 0;
		client_info.will_retain = 0;
		client_info.will_topic = NULL;
		client_info.will_msg = NULL;
		client_info.client_id = imei_no;
		client_info.client_user = imei_no;
		client_info.client_pass = imei_no;
        client_info.ssl_cfg = NULL;

	    struct mqtt_ssl_config_t quectel_ssl_cfg = {
 				 .ssl_ctx_id =1,
 				 .verify_level = MQTT_SSL_VERIFY_NONE,
  				 .cacert_path = NULL,
  				 .client_cert_path = NULL,
				 .client_key_path = NULL,
 				 .client_key_pwd = NULL

 				};

		client_info.ssl_cfg = &quectel_ssl_cfg;

       ret = ql_mqtt_connect(&mqtt_cli, MQTT_CLIENT_QUECTEL_URL, mqtt_connect_result_cb, NULL, (const struct mqtt_connect_client_info_t *)&client_info, mqtt_state_exception_cb);
      
   
		if(ret  == MQTTCLIENT_WOUNDBLOCK)
		{
			QL_MQTT_LOG("====wait connect result");
			ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			if(mqtt_connected == 0)
			{
				ql_mqtt_client_deinit(&mqtt_cli);
				goto sedem_Publish_only;
				break;
			}
		}
		else
		{
			QL_MQTT_LOG("===mqtt connect failed ,ret = %d",ret);
			break;
		}

		ql_mqtt_set_inpub_callback(&mqtt_cli, mqtt_inpub_data_cb, NULL);
   
   	    if(ql_mqtt_sub_unsub(&mqtt_cli, "v1/devices/me/rpc/request/+", 1, mqtt_requst_result_cb,NULL, 1) == MQTTCLIENT_WOUNDBLOCK)
				{
    				QL_MQTT_LOG("======wait subscrible result");
    				ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
    			}
//publishes sedem server only bypassing init of Thingsboard client,necessary goto in dual server logic
// do not remove goto label without proper understaing of internal logic,it will make code not publish to both servers
sedem_Publish_only:
		if(ql_mqtt_client_init(&mqtt_cli_sdm, sim_cid) != MQTTCLIENT_SUCCESS)
		{
			QL_MQTT_LOG("mqtt client init failed!!!!");
			break;
		}
		DIndex =  SpiPageAdressLookup();
		QL_MQTT_LOG("DIndex is:%d",DIndex);
       // read_config_fileUFS();
		strcpy(mqtt_ssl_url, snl_conf_read.domain);
		strcpy(SedemPassword, snl_conf_read.password);
		QL_MQTT_LOG("sedem password is set as:%s", mq_client_pwd);
		QL_MQTT_LOG("mqtt_ssl_url is set as:%s", snl_conf_read.domain);
		QL_MQTT_LOG("sedem password is set as:%s", snl_conf_read.password);

		QL_MQTT_LOG("mqtt_cli:%d", mqtt_cli_sdm);		

		client_info_sdm.keep_alive = 60;
		client_info_sdm.clean_session = 1;
		client_info_sdm.will_qos = 0;
		client_info_sdm.will_retain = 0;
		client_info_sdm.will_topic = NULL;
		client_info_sdm.will_msg = NULL;
		client_info_sdm.client_id = mq_client_id;
		client_info_sdm.client_user = mq_client_uname;
		client_info_sdm.client_pass = mq_client_pwd;
		client_info_sdm.ssl_cfg = NULL;

       struct mqtt_ssl_config_t quectel_ssl_cfg_sdm = {
  		.ssl_ctx_id =2,
  		.verify_level = MQTT_SSL_VERIFY_NONE,
  		.cacert_path = NULL,
  		.client_cert_path = NULL,
  		.client_key_path = NULL,
  		.client_key_pwd = NULL
		 };
 
         client_info_sdm.ssl_cfg = &quectel_ssl_cfg_sdm;
         
          ret = ql_mqtt_connect(&mqtt_cli_sdm, mqtt_ssl_url , mqtt_connect_result_cb_sdm, NULL, (const struct mqtt_connect_client_info_t *)&client_info_sdm, mqtt_state_exception_cb_sdm);
   
		if(ret  == MQTTCLIENT_WOUNDBLOCK)
		{
			QL_MQTT_LOG("====wait connect result");
			ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			if(mqtt_connected_sdm == 0)
			{
				ql_mqtt_client_deinit(&mqtt_cli_sdm);
				break;
			}
		}
		else
		{
			QL_MQTT_LOG("===mqtt connect failed ,ret = %d",ret);
			break;
		}

		ql_mqtt_set_inpub_callback(&mqtt_cli_sdm, mqtt_inpub_data_cb_sdm, NULL);

           	if(ql_mqtt_sub_unsub(&mqtt_cli_sdm,infosub, 1, mqtt_requst_result_cb_sdm,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
				QL_MQTT_LOG("======wait info subscribe result");
				ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			}
			if(ql_mqtt_sub_unsub(&mqtt_cli_sdm, otpsub, 1, mqtt_requst_result_cb_sdm,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
				QL_MQTT_LOG("======wait otp subscribe result");
				ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			}
			if(ql_mqtt_sub_unsub(&mqtt_cli_sdm, configsub, 1, mqtt_requst_result_cb_sdm,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
				QL_MQTT_LOG("======wait config subscribe result");
				ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			}
			if(ql_mqtt_sub_unsub(&mqtt_cli_sdm,ondemandsub, 1, mqtt_requst_result_cb_sdm,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
				QL_MQTT_LOG("======wait ondemand subscribe result");
				ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
			}
Retry_publish_healthy_server:
    Connected_server();
/**************publishes in both server when both call back results are ok**********************/
	while((mqtt_connected == 1)&&(mqtt_connected_sdm == 1))
         {

           if(event_uart.param2 == 1)
            {
				QL_MQTT_LOG("=inside TB and sedem publish loop:%d\r\n",event_uart.param2);
				event_uart.param2 =0;
				if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
				{
    			  QL_MQTT_LOG("======wait for TB TELEMTERY publish result");
    			  ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
    			}

                if(ql_mqtt_publish(&mqtt_cli_sdm,datapub,json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
                {
                    QL_MQTT_LOG("======wait for sedem Datapub publish result");
                    ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                }

			}

			if(event_uart.param2 == 2)
                {
				  event_uart.param2 =0;
    			   if(ql_mqtt_publish(&mqtt_cli,"v1/devices/me/attributes",json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
					{
    				  QL_MQTT_LOG("======wait for TB attributes publish result");
    				  ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
    				}
    			}
			    	
			if(hdata_onceupdate ==1)  // publishing sedem heartbeat data
             { 
                hdata_onceupdate =0;
                signal_quality();
                //sprintf(heart_buf, "{\"VD\":\"0\",\"TIMESTAMP\":\"%s\",\"MAXINDEX\":\"96\",\"INDEX\":%d,\"LOAD\":\"0\",\"STINTERVAL\":%d,\"MSGID\":\"22\",\"DATE\":\"%s\",\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":\"1\",\"SIM\":\"1\",\"NET\":\"1\",\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":\"1\",\"GPS\":\"1\",\"GPSLOC\":%d,\"RF\":\"1\",\"RTCDATE\":\"%s\",\"RTCTIME\":\"%s\",\"TEMP\":\"%d\",\"LAT\":\"%s\",\"LONG\":\"%s\",\"SIMSLOT\":\"1\",\"SIMCHNGCNT\":\"10\",\"FLASH\":\"1\",\"BATTST\":\"0\",\"VBATT\":\"5.0\",\"PST\":\"1\"}",current_dt,d_index,pdata_int,only_dt,imei_no,potp_v,cotp_v,Rssi,sd_load_status,gpsloc,only_dt,only_tm,temperature_status,gnnsPosDat.lat,gnnsPosDat.lon);
                if(gpsloc == FALSE) 
			 	{
               		sprintf(heart_buf, "{\"VD\":0,\"TIMESTAMP\":\"%s\",\"MAXINDEX\":96,\"INDEX\":%d,\"LOAD\":0,\"STINTERVAL\":%d,\"MSGID\":\"%d\",\"DATE\":%s,\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":1,\"SIM\":1,\"NET\":1,\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":1,\"GPS\":1,\"GPSLOC\":%d,\"RF\":1,\"RTCDATE\":%s,\"RTCTIME\":%s,\"TEMP\":%d,\"SIMSLOT\":1,\"SIMCHNGCNT\":10,\"FLASH\":1,\"BATTST\":0,\"VBATT\":0,\"PST\":1}",current_dt,d_index,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v,csq,sd_load_status,gpsloc,only_dt,only_tm,temperature_status); 
			 	} 
			   else
			  	{
					sprintf(heart_buf, "{\"VD\":0,\"TIMESTAMP\":\"%s\",\"MAXINDEX\":96,\"INDEX\":%d,\"LOAD\":0,\"STINTERVAL\":%d,\"MSGID\":\"%d\",\"DATE\":%s,\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":1,\"SIM\":1,\"NET\":1,\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":1,\"GPS\":1,\"GPSLOC\":%d,\"RF\":1,\"RTCDATE\":%s,\"RTCTIME\":%s,\"TEMP\":%d,\"LAT\":%s,\"LONG\":%s,\"SIMSLOT\":1,\"SIMCHNGCNT\":10,\"FLASH\":1,\"BATTST\":0,\"VBATT\":0,\"PST\":1}",current_dt,d_index,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v,csq,sd_load_status,gpsloc,only_dt,only_tm,temperature_status,gnnsPosDat.lat,gnnsPosDat.lon); 
				}
			    QL_MQTT_LOG("\r\n heart_buf value is:%s\r\n",heart_buf);
				if(ql_mqtt_publish(&mqtt_cli_sdm,heartbeatpub,heart_buf, strlen(heart_buf), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
                {
                  QL_MQTT_LOG("======wait for Sedem Heartbeat publish result");
                  ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                }
            }

			if(configdata_onceupdate ==1)
             { 
				ql_rtos_task_sleep_s(5);
                configdata_onceupdate =0;
                //sprintf(config_buf, "{\"timestamp\":\"%s\",\"type\":\"config\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":\"0\",\"UPDATEINTERVAL\":\"%d\",\"HEARTINTERVAL\":\"%d\",\"URTCDATE\":\"%s\",\"URTCTIME\":\"%s\",\"UPDATERTC\":\"0\",\"GSMSYNC\":\"1\",\"DO1\":\"%d\",\"URL\":\"rms1.kusumiiot.co\",\"PORT\":\"8883\",\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":\"22\",\"FTPDOWN\":\"1\"}",current_dt,cfg_rw,msgid_ev,operator_name,pdata_int,hdata_int,only_dt,only_tm,Pump_Run_Status,mq_client_id,mq_client_uname,mq_client_pwd);
				sprintf(config_buf, "{\"timestamp\":\"%s\",\"type\":\"config\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":0,\"UPDATEINTERVAL\":%d,\"HEARTINTERVAL\":%d,\"URTCDATE\":%s,\"URTCTIME\":%s,\"UPDATERTC\":0,\"GSMSYNC\":1,\"DO1\":%d,\"URL\":\"rms1.kusumiiot.co\",\"PORT\":8883,\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":22,\"FTPDOWN\":0}", current_dt, cfg_rw, msgid_ev, operator_name, pdata_int, hdata_int, only_dt, only_tm, Pump_Run_Status, mq_client_id, mq_client_uname, mq_client_pwd);
				if(ql_mqtt_publish(&mqtt_cli_sdm,configpub,config_buf, strlen(config_buf), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
                  {
                    QL_MQTT_LOG("======wait for config sedem publish result");
                    ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                   }
             }

			 if(ondemand_onceupdate ==1)
             { 
				ql_rtos_task_sleep_s(5);
                ondemand_onceupdate =0;
                //sprintf(ondem_buf,"{\"timestamp\":\"%s\",\"type\":\"ondemand\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":\"0\",\"UPDATEINTERVAL\":\"%d\",\"HEARTINTERVAL\":\"%d\",\"URTCDATE\":\"%s\",\"URTCTIME\":\"%s\",\"UPDATERTC\":\"0\",\"GSMSYNC\":\"1\",\"DO1\":\"%d\",\"URL\":\"rms1.kusumiiot.co\",\"PORT\":\"8883\",\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":\"22\",\"FTPDOWN\":\"1\"}",current_dt,cfg_rw,msgid_ev,operator_name,pdata_int,hdata_int,only_dt,only_tm,Pump_Run_Status,mq_client_id,mq_client_uname,mq_client_pwd);
				sprintf(ondem_buf,"{\"timestamp\":\"%s\",\"type\":\"ondemand\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":0,\"UPDATEINTERVAL\":%d,\"HEARTINTERVAL\":%d,\"URTCDATE\":%s,\"URTCTIME\":%s,\"UPDATERTC\":0,\"GSMSYNC\":1,\"DO1\":%d,\"URL\":\"rms1.kusumiiot.co\",\"PORT\":8883,\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":22,\"FTPDOWN\":0}",current_dt,cfg_rw,msgid_ev,operator_name,pdata_int,hdata_int,only_dt,only_tm,Pump_Run_Status,mq_client_id,mq_client_uname,mq_client_pwd);
				if(ql_mqtt_publish(&mqtt_cli_sdm,ondemandpub,ondem_buf,strlen(ondem_buf), 1, 0, mqtt_requst_result_cb_sdm,NULL) == MQTTCLIENT_WOUNDBLOCK)
                {
                 QL_MQTT_LOG("======wait for ondemand sedem publish result");
                 ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                }
             }

            /************************Publishes Network down missing packets***************/

			 if(DataRepubFlag ==1)
			 { 
					QL_MQTT_LOG("retry connection to the down server inside mqtt thread");
					QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwPath_name_cmpr);
					
				if( (ql_file_exist(sd_NnwPath_name_cmpr)) == QL_FILE_OK)
				{
					 read_logged_files_toRetry(sd_NnwPath_name_cmpr);
					 QL_MQTT_LOG("Detected Data during no network Log");
				}

			  DataRepubFlag = 0;
			  NOntwrk_data_flag = 1; 
			 }


            /************Publishes Server down missing packets******************************/

			 if(DataRepubFlag_TB==1)
			 {
				QL_MQTT_LOG("retry connection to the down server schnell inside mqtt thread");
				QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwTBPath_name_cmpr);
			        
			 if( (ql_file_exist(sd_NnwTBPath_name_cmpr)) == QL_FILE_OK)
				{
					 read_logged_files_toRetry(sd_NnwTBPath_name_cmpr);
					 QL_MQTT_LOG("Detected Data during schnell iot down");
				}
              DataRepubFlag_TB = 0;
			  NOntwrk_data_flag = 2;
			
			 }

            /********************Publishes Sedem Server down missing packets**********************/
			if(DataRepubFlag_SDM==1)
			 {
				QL_MQTT_LOG("retry connection to the down server sedem inside mqtt thread");
				QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwSDMPath_name_cmpr);
                
				if( (ql_file_exist(sd_NnwSDMPath_name_cmpr)) == QL_FILE_OK)
				{
					read_logged_files_toRetry(sd_NnwSDMPath_name_cmpr);
					QL_MQTT_LOG("Detected Data during sedem server  down");
				}
              DataRepubFlag_SDM = 0;
			  NOntwrk_data_flag = 3;
			 }

			 
        /***********Publishes packets stored in SD card via RPC command DLOAD*****************/

        ql_rtos_queue_wait(sdData,(uint8 *)&no_NwLog_data,sizeof(no_NwLog_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",no_NwLog_data);
	    int strlenofBuff = strlen(no_NwLog_data);
	    QL_MQTT_LOG("=====string lenght of no network data is======:%d\r\n",strlenofBuff);
		if(strlenofBuff >= 500)
			 {
				QL_MQTT_LOG("publish packet  by RPC");
				publish_lost_data(no_NwLog_data);
			    memset(no_NwLog_data, 0,sizeof(no_NwLog_data));
			 }

        /***************Publishes Missing packets stored in SD card in timed interval *************/

		ql_rtos_queue_wait(recvr_Sd_Data,(uint8 *)&RPC_Log_data,sizeof(RPC_Log_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",RPC_Log_data);
		int strlenof_sdBuff = strlen(RPC_Log_data);
		if(strlenof_sdBuff >= 500)
		{
		  NumberofPacketsInQueue++;
          QL_MQTT_LOG("publish missing packet");
		  publish_missed_Packets(RPC_Log_data,NOntwrk_data_flag);
		  memset(RPC_Log_data, 0,sizeof(RPC_Log_data));
		  if(NumberOFLinesinFile == NumberofPacketsInQueue)
		 {
			QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);
            QL_MQTT_LOG("All datas from the Packets have been published ,delete the log file");
			if(NOntwrk_data_flag ==1)ql_remove(sd_NnwPath_name_cmpr);
			if(NOntwrk_data_flag ==2)ql_remove(sd_NnwTBPath_name_cmpr);
			if(NOntwrk_data_flag ==3)ql_remove(sd_NnwSDMPath_name_cmpr);
            NOntwrk_data_flag =0;
			NumberOFLinesinFile =0;
			NumberofPacketsInQueue =0;
		 }
		}
		 QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);
         
		
        QL_MQTT_LOG("=====in mqtt loop for dual publish======:\r\n");
        
		send_rpc_to_criBrd(&TB_RPC);
        ql_rtos_task_sleep_ms(250);

		 }

  /****************publishes in Thingboard server call back results are ok**********************/             
	while((mqtt_connected == 1) && (mqtt_connected_sdm == 0))
		{
    			
			if(event_uart.param2 == 1)
            	{
					event_uart.param2 =0;
					QL_MQTT_LOG("=inside Thingboard publish loop:%d\r\n",event_uart.param2);
    			    if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
				  	{
    					QL_MQTT_LOG("======wait publish result");
    					ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
    			  	}
                 }

			if(event_uart.param2 == 2)
                {
					event_uart.param2 =0;
    				if(ql_mqtt_publish(&mqtt_cli,"v1/devices/me/attributes",json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
						{
    						QL_MQTT_LOG("======wait publish result");
    						ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
    					}
    			}
			if(Retry_server_flag==1)
			   {
					Retry_server_flag =0;
					airplane_mode();
					goto startAgain;
					QL_MQTT_LOG("=====in mqtt loop for retry State Sedem server ======:\r\n");
			   }

			
            /************************Publishes Network down missing packets***************/

			 if(DataRepubFlag ==1)
			 { 
					QL_MQTT_LOG("retry connection to the down server inside mqtt thread");
					QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwPath_name_cmpr);
					
				if( (ql_file_exist(sd_NnwPath_name_cmpr)) == QL_FILE_OK)
				{
					 read_logged_files_toRetry(sd_NnwPath_name_cmpr);
					 QL_MQTT_LOG("Detected Data during no network Log");
				}

			  DataRepubFlag = 0;
			  NOntwrk_data_flag = 1; 
			 }

            /************Publishes Server down missing packets******************************/

			 if(DataRepubFlag_TB==1)
			 {
				QL_MQTT_LOG("retry connection to the down server schnell inside mqtt thread");
				QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwTBPath_name_cmpr);
			        
			 if( (ql_file_exist(sd_NnwTBPath_name_cmpr)) == QL_FILE_OK)
				{
					 read_logged_files_toRetry(sd_NnwTBPath_name_cmpr);
					 QL_MQTT_LOG("Detected Data during schnell iot down");
				}
              DataRepubFlag_TB = 0;
			  NOntwrk_data_flag = 2;
			
			 }
		 	 
        /***********Publishes packets stored in SD card via RPC command DLOAD*****************/

        ql_rtos_queue_wait(sdData,(uint8 *)&no_NwLog_data,sizeof(no_NwLog_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",no_NwLog_data);
	    int strlenofBuff = strlen(no_NwLog_data);
	    QL_MQTT_LOG("=====string lenght of no network data is======:%d\r\n",strlenofBuff);
		if(strlenofBuff >= 500)
			 {
				QL_MQTT_LOG("publish packet  by RPC");
				publish_lost_data(no_NwLog_data);
			    memset(no_NwLog_data, 0,sizeof(no_NwLog_data));
			 }

        /***************Publishes Missing packets stored in SD card in timed interval *************/

		ql_rtos_queue_wait(recvr_Sd_Data,(uint8 *)&RPC_Log_data,sizeof(RPC_Log_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",RPC_Log_data);
		int strlenof_sdBuff = strlen(RPC_Log_data);
		if(strlenof_sdBuff >= 500)
		{
		  NumberofPacketsInQueue++;
          QL_MQTT_LOG("publish missing packet");
		  publish_missed_Packets(RPC_Log_data,NOntwrk_data_flag);
		  memset(RPC_Log_data, 0,sizeof(RPC_Log_data));
		  if(NumberOFLinesinFile == NumberofPacketsInQueue)
		 {
			QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);
            QL_MQTT_LOG("All datas from the Packets have been published ,delete the log file");
			if(NOntwrk_data_flag ==2)ql_remove(sd_NnwTBPath_name_cmpr);
			if(NOntwrk_data_flag ==1)ql_remove(sd_NnwPath_name_cmpr);
			NOntwrk_data_flag =0;
			NumberOFLinesinFile =0;
			NumberofPacketsInQueue =0;
		 }
		}
		 QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);
   

			   

			QL_MQTT_LOG("=====in mqtt schnell iot loop ======:\r\n");
        	send_rpc_to_criBrd(&TB_RPC);
			ql_rtos_task_sleep_ms(250);
	  			
    	}
  /**************publishes in sedem server when call back results are ok*********************/  
	while((mqtt_connected == 0) && (mqtt_connected_sdm == 1))
	{
    	if (event_uart.param2 == 1) {
      	QL_MQTT_LOG("=inside sedem publish loop:%d\r\n", event_uart.param2);
      	event_uart.param2 = 0;
      	if (ql_mqtt_publish( & mqtt_cli_sdm, datapub, json_data_buf, strlen(json_data_buf), 1, 0, mqtt_requst_result_cb_sdm, NULL) == MQTTCLIENT_WOUNDBLOCK) {
        QL_MQTT_LOG("======wait for sedem Datapub publish result");
        ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
     	 }

    	}

    	if (hdata_onceupdate == 1) // publishing sedem heartbeat data
    	{
      		hdata_onceupdate = 0;
      		signal_quality();
      		//sprintf(heart_buf, "{\"VD\":\"0\",\"TIMESTAMP\":\"%s\",\"MAXINDEX\":\"96\",\"INDEX\":%d,\"LOAD\":\"0\",\"STINTERVAL\":%d,\"MSGID\":\"22\",\"DATE\":\"%s\",\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":\"1\",\"SIM\":\"1\",\"NET\":\"1\",\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":\"1\",\"GPS\":\"1\",\"GPSLOC\":%d,\"RF\":\"1\",\"RTCDATE\":\"%s\",\"RTCTIME\":\"%s\",\"TEMP\":\"45.5\",\"LAT\":\"%s\",\"LONG\":\"%s\",\"SIMSLOT\":\"1\",\"SIMCHNGCNT\":\"10\",\"FLASH\":\"1\",\"BATTST\":\"0\",\"VBATT\":\"5.0\",\"PST\":\"1\"}", current_dt, d_index, pdata_int, only_dt, imei_no, potp_v, cotp_v, Rssi,sd_load_status, gpsloc, only_dt, only_tm, gnnsPosDat.lat, gnnsPosDat.lon);
      		if(gpsloc == FALSE) 
			{
			  sprintf(heart_buf, "{\"VD\":0,\"TIMESTAMP\":\"%s\",\"MAXINDEX\":96,\"INDEX\":%d,\"LOAD\":0,\"STINTERVAL\":%d,\"MSGID\":\"%d\",\"DATE\":%s,\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":1,\"SIM\":1,\"NET\":1,\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":1,\"GPS\":1,\"GPSLOC\":%d,\"RF\":1,\"RTCDATE\":%s,\"RTCTIME\":%s,\"TEMP\":%d,\"SIMSLOT\":1,\"SIMCHNGCNT\":10,\"FLASH\":1,\"BATTST\":0,\"VBATT\":0,\"PST\":1}",current_dt,d_index,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v,csq,sd_load_status,gpsloc,only_dt,only_tm,temperature_status); 
			} 
			  else
			 {
			   sprintf(heart_buf, "{\"VD\":0,\"TIMESTAMP\":\"%s\",\"MAXINDEX\":96,\"INDEX\":%d,\"LOAD\":0,\"STINTERVAL\":%d,\"MSGID\":\"%d\",\"DATE\":%s,\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\",\"GSM\":1,\"SIM\":1,\"NET\":1,\"GPRS\":\"1\",\"RSSI\":%d,\"SD\":\"%d\",\"ONLINE\":1,\"GPS\":1,\"GPSLOC\":%d,\"RF\":1,\"RTCDATE\":%s,\"RTCTIME\":%s,\"TEMP\":%d,\"LAT\":%s,\"LONG\":%s,\"SIMSLOT\":1,\"SIMCHNGCNT\":10,\"FLASH\":1,\"BATTST\":0,\"VBATT\":0,\"PST\":1}",current_dt,d_index,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v,csq,sd_load_status,gpsloc,only_dt,only_tm,temperature_status,gnnsPosDat.lat,gnnsPosDat.lon); 
			}
			 
			if (ql_mqtt_publish( & mqtt_cli_sdm, heartbeatpub, heart_buf, strlen(heart_buf), 1, 0, mqtt_requst_result_cb_sdm, NULL) == MQTTCLIENT_WOUNDBLOCK) {
       		 QL_MQTT_LOG("======wait for Sedem Heartbeat publish result");
        	ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
      		}
    	}

    	if (configdata_onceupdate == 1) {
      		ql_rtos_task_sleep_s(5);
			configdata_onceupdate = 0;
      		//sprintf(config_buf, "{\"timestamp\":\"%s\",\"type\":\"config\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":\"0\",\"UPDATEINTERVAL\":\"%d\",\"HEARTINTERVAL\":\"%d\",\"URTCDATE\":\"%s\",\"URTCTIME\":\"%s\",\"UPDATERTC\":\"0\",\"GSMSYNC\":\"1\",\"DO1\":\"%d\",\"URL\":\"rms1.kusumiiot.co\",\"PORT\":\"8883\",\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":\"22\",\"FTPDOWN\":\"1\"}", current_dt, cfg_rw, msgid_ev, operator_name, pdata_int, hdata_int, only_dt, only_tm, Pump_Run_Status, mq_client_id, mq_client_uname, mq_client_pwd);
			sprintf(config_buf, "{\"timestamp\":\"%s\",\"type\":\"config\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":0,\"UPDATEINTERVAL\":%d,\"HEARTINTERVAL\":%d,\"URTCDATE\":%s,\"URTCTIME\":%s,\"UPDATERTC\":0,\"GSMSYNC\":1,\"DO1\":%d,\"URL\":\"rms1.kusumiiot.co\",\"PORT\":8883,\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":22,\"FTPDOWN\":0}", current_dt, cfg_rw, msgid_ev, operator_name, pdata_int, hdata_int, only_dt, only_tm, Pump_Run_Status, mq_client_id, mq_client_uname, mq_client_pwd);
			if (ql_mqtt_publish( & mqtt_cli_sdm, configpub, config_buf, strlen(config_buf), 1, 0, mqtt_requst_result_cb_sdm, NULL) == MQTTCLIENT_WOUNDBLOCK) {
       		 QL_MQTT_LOG("======wait for config sedem publish result");
        	ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
     		 }
    	}

    	if (ondemand_onceupdate == 1) {
    		  ql_rtos_task_sleep_s(5);
			  ondemand_onceupdate = 0;
      		//  sprintf(ondem_buf, "{\"timestamp\":\"%s\",\"type\":\"ondemand\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":\"0\",\"UPDATEINTERVAL\":\"%d\",\"HEARTINTERVAL\":\"%d\",\"URTCDATE\":\"%s\",\"URTCTIME\":\"%s\",\"UPDATERTC\":\"0\",\"GSMSYNC\":\"1\",\"DO1\":\"%d\",\"URL\":\"rms1.kusumiiot.co\",\"PORT\":\"8883\",\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":\"22\",\"FTPDOWN\":\"1\"}", current_dt, cfg_rw, msgid_ev, operator_name, pdata_int, hdata_int, only_dt, only_tm, Pump_Run_Status, mq_client_id, mq_client_uname, mq_client_pwd);
			  sprintf(ondem_buf,"{\"timestamp\":\"%s\",\"type\":\"ondemand\",\"cmd\":\"%s\",\"MSGID\":\"%d\",\"APN1\":\"%s\",\"RESTART\":0,\"UPDATEINTERVAL\":%d,\"HEARTINTERVAL\":%d,\"URTCDATE\":%s,\"URTCTIME\":%s,\"UPDATERTC\":0,\"GSMSYNC\":1,\"DO1\":%d,\"URL\":\"rms1.kusumiiot.co\",\"PORT\":8883,\"CID\":\"%s\",\"USERNAME\":\"%s\",\"PASSWORD\":\"%s\",\"FTPURL\":\"rms1.kusumiiot.co\",\"FTPUSER\":\"866191037709301\",\"FTPPASS\":\"908552f\",\"FTPPORT\":22,\"FTPDOWN\":0}",current_dt,cfg_rw,msgid_ev,operator_name,pdata_int,hdata_int,only_dt,only_tm,Pump_Run_Status,mq_client_id,mq_client_uname,mq_client_pwd);
			if (ql_mqtt_publish( & mqtt_cli_sdm, ondemandpub, ondem_buf, strlen(ondem_buf), 1, 0, mqtt_requst_result_cb_sdm, NULL) == MQTTCLIENT_WOUNDBLOCK) {
       		 QL_MQTT_LOG("======wait for ondemand sedem publish result");
       	     ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
      		}
    	}

		if(Retry_server_flag_Thingboard==1)
			   {
					Retry_server_flag_Thingboard =0;
					airplane_mode();
					goto startAgain;
					QL_MQTT_LOG("=====in mqtt loop for retry schnell iot CRI server ======:\r\n");
			   }


		
            /************************Publishes Network down missing packets***************/

			 if(DataRepubFlag ==1)
			 { 
					QL_MQTT_LOG("retry connection to the down server inside mqtt thread");
					QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwPath_name_cmpr);
					
				if( (ql_file_exist(sd_NnwPath_name_cmpr)) == QL_FILE_OK)
				{
					 read_logged_files_toRetry(sd_NnwPath_name_cmpr);
					 QL_MQTT_LOG("Detected Data during no network Log");
				}

			  DataRepubFlag = 0;
			  NOntwrk_data_flag = 1; 
			 }

            /********************Publishes Sedem Server down missing packets**********************/
			if(DataRepubFlag_SDM==1)
			 {
				QL_MQTT_LOG("retry connection to the down server sedem inside mqtt thread");
				QL_MQTT_LOG("Detected Data during no network Log :%s",sd_NnwSDMPath_name_cmpr);
                
				if( (ql_file_exist(sd_NnwSDMPath_name_cmpr)) == QL_FILE_OK)
				{
					read_logged_files_toRetry(sd_NnwSDMPath_name_cmpr);
					QL_MQTT_LOG("Detected Data during sedem server  down");
				}
              DataRepubFlag_SDM = 0;
			  NOntwrk_data_flag = 3;
			 }
			 
        /***********Publishes packets stored in SD card via RPC command DLOAD*****************/

        ql_rtos_queue_wait(sdData,(uint8 *)&no_NwLog_data,sizeof(no_NwLog_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",no_NwLog_data);
	    int strlenofBuff = strlen(no_NwLog_data);
	    QL_MQTT_LOG("=====string lenght of no network data is======:%d\r\n",strlenofBuff);
		if(strlenofBuff >= 500)
			 {
				QL_MQTT_LOG("publish packet  by RPC");
				publish_lost_data(no_NwLog_data);
			    memset(no_NwLog_data, 0,sizeof(no_NwLog_data));
			 }

        /***************Publishes Missing packets stored in SD card in timed interval *************/

		ql_rtos_queue_wait(recvr_Sd_Data,(uint8 *)&RPC_Log_data,sizeof(RPC_Log_data),1000);
		QL_MQTT_LOG("the value of sd buffer from queue wait is:%s",RPC_Log_data);
		int strlenof_sdBuff = strlen(RPC_Log_data);
		if(strlenof_sdBuff >= 500)
		{
		  NumberofPacketsInQueue++;
          QL_MQTT_LOG("publish missing packet");
		  publish_missed_Packets(RPC_Log_data,NOntwrk_data_flag);
		  memset(RPC_Log_data, 0,sizeof(RPC_Log_data));
		  if(NumberOFLinesinFile == NumberofPacketsInQueue)
		 {
			QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);
            QL_MQTT_LOG("All datas from the Packets have been published ,delete the log file");
			if(NOntwrk_data_flag ==3)ql_remove(sd_NnwSDMPath_name_cmpr);
			if(NOntwrk_data_flag ==1)ql_remove(sd_NnwPath_name_cmpr);
			NOntwrk_data_flag =0;
			NumberOFLinesinFile =0;
			NumberofPacketsInQueue =0;
		 }
		}
		 QL_MQTT_LOG("no of line is :%d,nof data in queue is :%d",NumberOFLinesinFile,NumberofPacketsInQueue);	   


     QL_MQTT_LOG("=====in mqtt loop for sedem publish only ======:\r\n");
     send_rpc_to_criBrd( & TB_RPC);
     ql_rtos_task_sleep_ms(250);
  }	   

  	while((mqtt_connected == 0) && (mqtt_connected_sdm == 0))
	{ 
		if(event_uart.param1 % 1 == 0)
			   {
					airplane_mode();
					goto startAgain;
					QL_MQTT_LOG("=====in mqtt loop for no network try refreshng the modem to reconnect ======:\r\n");
			   }
	}
//ql_rtos_task_sleep_s(1);	//if run num is not used remove this section
}
// retries publish to healthy  server when connection fails mid publish and exits loop due to call back errors
goto Retry_publish_healthy_server;	
return;		

}



