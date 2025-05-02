/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			                        WHO		    WHAT, WHERE, WHY
------------	                      -------	    -------------------------------------------------------------------------------
ver sdk4           ver32 20/1/24 	     ERK		Tested code which donot reset,ufs file removed,pub interval hardcoded value pdata int
ver sdk9           ver33 20/1/24         ERK        RPC data decode added in this version
ver sdk9_ota       ver34 23/1/24         ERK        tested and added ota codes and found "stable" ,atrributes pdata and url missing rpc for pdata not added
ver sdk10/_otA     ver35  21/1/24        ERK        *ufs file section added in this version ,added cred.txt,response.txt,config.txt in prepack also prepack tts gps.json
                                                    *Added rpc pdata set in this version(unstable version donot use)
VER SDK11_OTA       VER36 23/1/232		 ERK         RPC pdat url added as hardcoded value ufs not integrated baase version from sdk9_ota  
										             Power reset normal rpc is also added
ver sdk37_OTA		ver37 24/1/23		ERK			 Made apn set as global extern variable and chnages in fota to accept any sim card

ver sdk39_OTA		ver39 24/1/23		ERK	         Added RPC commands for parameters/config set ,
                                                     changes made in mqtt thingboard .h struct for rpc
													 changes made in mqtt tingboard.c file in send rpc to cri function
													 chnages made in packet encode decode section of json extract function.
													 ota checked ok,also fpds for every config info also update for cri app in "send rpc to cri function".
ver sdk40		    ver40 2/2/24		ERK			 *sim net register function checking,ota aslo to be chcked,this version sent as  ota test to cri 	
ver sdk40_ota		ver40 2/2/24		ERK			 sim net register function checked ok ,ota aslo  chcked, dual is going to be present from above this version 										 
ver sdk41_ota		ver41 2/2/24		ERK	         This is Dual Mqtt sedem server connect only and TB all function code(ota not ready)
ver sdk42_ota		ver42 5/2/24		ERK	         rpc functioality and all variables for sedem server added and checked ok,no restart,json extratct token buffer size increased to 200
ver sdk43_ota		ver43 5/2/24		ERK	         Dual functionality publish implemented and tested here,config publish not yet done to be added in future version,ota rpc dual to  be tested and validated
ver sdk44_ota		ver44 6/2/24		ERK	         Dual functionality with failure protection when one server down switch over coccurs implemented,config publish not yet done to be added in future version,ota rpc dual tested and validated
ver sdk45_ota		ver45 7/2/24		ERK	         configand Ondemand publish/sunscribe added ,ota rpc dual to  be tested and validated
ver sdk45_otaufs    ver45 11/2/24		ERK			 Sedem url set from RPC and tested ok,rpc urls,changeover to dual server in next retsart
ver sdk46_ota       ver46 11/2/24		ERK			 derived from "ver sdk45_otaufspwd" ,reset when new config for server connect URL and Password for sedem servers implemented,
                                                     Attributes values changed to reflect sedem connected status.in writeufs function struct fro snl_conf removed and changed to snl_conf_read.,
													 restart section added,pdata int updated in UPDATE interval RPC
ver sdk47_ota       ver47 13/2/24		ERK			 Changed code in Timesync.c section signal quality function to get rssi value from net signal API,changes made to display signal strenght in atrributes and Sedem server values									 
ver sdk48_ota       ver48 13/2/24		ERK		     SD card setion and internal File system UFS has been added in a single .c file called filesys.c,
													 ota and other functionalities yest to betested 
ver sdk49_ota       ver49 13/2/24		ERK			 Time sync file RTC validation and retry mechanism updated			
ver sdk50_ota       ver50 22/2/24		ERK			 in filsesys.c section-->SD card new section added,evry day log created during runtime in ddmmyyy.txt format,
													 in Mqtt_thingsboard.c -->publish section new goto labels created for publish in seprate server	
													 SDload status and temp added in heartbeat pub,restart from sedem added ,update rtc,load key values chnaged to default values in pub pack section
													 Config_write_file removed in rpc call back for sedem,config related restart solved.	
ver sdk51_ota       ver51 28/2/24		ERK			 removed event_uart.param3 event for checking TX buffer full in calling send_rpc_to_criBrd function located in mqtt.c file,
                                                     this makes All rpc response and also for pump on\off faster(7 sec response)	
ver sdk52_ota       ver52 4/3/24		ERK			 Reconnect issue when sim removed and reinserted is solved in this version earlier took 1 hour to reconnect 
                                                     now it takes 2 mins to reconnect when sim is reinserted
ver sdk53_ota       ver53 8/3/24		ERK			 GPS time sync RTC section removed and GPS network time changeover modification done 
ver sdk54_ota       ver54 14/3/24		ERK          SD card load from RPC ,teletrty lat long moved to attributes card.
                                                     sd card load function read_logged_files() can be used on 53ver code without any issues.	
ver sdk55_ota       ver55 15/3/24		ERK 		 ongoing* config_write_file(char *con) function deleted	
ver sdk56_ota       ver56 19/3/24		ERK 		 cointains from rpc publish missed packets with arguments in rpc load giving nof datas to be pushed.	
ver sdk57_ota       ver57 22/3/24		ERK			 contains DLOAD rpc to retrived old logged data from SD card,
                                                     Also stored 3 missed packet files for network down,server1 missed packet and server2 missed packet		
													 After sucesful publish of those file,it will be deleted
													 Code clean up done,missed files retry for 3 data packets if present is now given as 15/30/45 mins interval	 
ver sdk57_ota       ver57 22/3/24		ERK	         Config pub ondemand pub in publish section 5 sec delay added updation of set values,"load" RPC added,pdata hdata set value incresed to 5mins and 3 mins.	
ver sdk58_ota       ver58 29/3/24		ERK			 Refresh RPC added,nonetwork load,ondemand config pub delay for updated value
ver sdk59_ota       ver59 5/4/24		ERK			 Filesys ReadLoggedFilesFrom RPC function ,retry function index of log read lines increased,RPC created for numer of lines to read apart tfrom default	
ver sdk59.1_ota     ver59 5/4/24		ERK	 		 Apn issue to Maharastra sedem server resolving...		
ver sdk59.2_ota     ver59.2       		ERK	 		 Last stable versiong...	
ver sdk59.4_ota     ver59.4             ERK	 		 Intermediate versionf for RMS integration gps format changed..	
ver sdk59.5_ota     ver59.4.1           ERK	 	     Format all changed to match RMS Documents...	
ver SPI_FLASH_OTA                       ERK	 	     WINBOND SPI FLASH INTEGRATION 								 											  										 					 
=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_uart.h"
#include "ql_gpio.h"
#include "ql_pin_cfg.h"

#include "osi_api.h"
#include "ql_api_nw.h"
#include "ql_api_dev.h"
#include "ql_power.h"
#include "ql_mqttclient.h"
#include "ql_fs.h"
#include "ql_api_sim.h"
#include "ql_api_rtc.h"
#include "quec_pin_index.h"
#include "led_cfg_demo.h"
#include "gnss_cri.h"
#include "gnss_cri.c"
#include "time_sync.h"
#include "time_sync.c"
#include "Mqtt_thingsbrd.c"
#include "Mqtt_thingsbrd.h"
#include "uart_demo.h"
#include "fota_ftp.h"
#include "ql_sdmmc.h"
#include "filesys.c"



/*===========================================================================
 *Definition
 ===========================================================================*/
#define QL_UART_DEMO_LOG_LEVEL			QL_LOG_LEVEL_INFO
#define QL_UART_DEMO_LOG(msg, ...)		QL_LOG(QL_UART_DEMO_LOG_LEVEL, "ql_uart_demo", msg, ##__VA_ARGS__)


#define QL_UART_TASK_STACK_SIZE     		4096
#define QL_UART_TASK_PRIO          	 	    APP_PRIORITY_NORMAL
#define QL_UART_TASK_EVENT_CNT      		5


#define QL_UART_RX_BUFF_SIZE                500
#define QL_UART_TX_BUFF_SIZE                500



#define QUEC_TIMER_EVENT_IND 1001


#define MIN(a,b) ((a) < (b) ? (a) : (b))


/*===========================================================================
 * Variate
 ===========================================================================*/
ql_task_t uart_task = NULL;
ql_timer_t cnt_timer = NULL;
ql_event_t event_uart;

volatile int count = 0;
char rcv_data_buf[500]={'\0'};
unsigned char tot_rcvd=0;
char Usart_rcvbf_collect_flag=0;                 // 0 indicates collation not occured
char imei_no[20]={'\0'};
unsigned char Periodic_data_req[]   = "$POPI#";
unsigned char Atribute_data_req[]  = "$FPDS#";	
unsigned char On_Demand_Pump_On[]   = "$$PON#";	
unsigned char On_Demand_Pump_Off[]  = "$$POF#";	
unsigned int d_index=0;
char current_dt[25]={'\0'};
char only_dt[10]={'\0'};
char only_tm[10]={'\0'};
char dt_time_CRI[25]={'\0'};
char version[15]={"59.5"};

char pdata_int = 10;
char hdata_int=3;
char Retry_server =11;  // Time in mins Retry to Sedem.
char Retry_server_thingsboard =57;// Time in mins Retry to Thingsboard.
char sd_NnwPath_name_cmpr[25]={'\0'};
char sd_NnwTBPath_name_cmpr[25]={'\0'};
char sd_NnwSDMPath_name_cmpr[25]={'\0'};			
char imei_no1[20]={"123"};
/*===========================================================================
 * Functions
 ===========================================================================*/




void timer_callback(void *ctx)
{
	int ret;
	event_uart.id = QUEC_TIMER_EVENT_IND;
	ql_rtos_timer_stop(cnt_timer);
	if (count >= 60)                        // 60 = 1 hour 
	count = 0;
	QL_UART_DEMO_LOG("timer run %d values ", count);
	event_uart.param1 = count;
	count++;
	ret = ql_rtos_timer_start(cnt_timer, 60000, 0);   // each 1 min
	if(ret != 0)
	{
		QL_UART_DEMO_LOG("demo_timer start failed");
		// goto exit;
	}
	ql_rtos_event_send(uart_task, &event_uart);
	ql_rtos_event_send(gnss_task, &event_uart);

}


/*===========================================================================
 * Function which Transmits UART data *
 ===========================================================================*/

void uart_data_send(unsigned char* data)
{
	int write_len = 0;
	ql_uart_tx_status_e tx_status;
	write_len = ql_uart_write(QL_UART_PORT_1, data, strlen((char *)data));
	QL_UART_DEMO_LOG(" write_len :%d", write_len);
	ql_uart_get_tx_fifo_status(QL_UART_PORT_1, &tx_status);
	// QL_UART_DEMO_LOG("tx_status:%d", tx_status);
	//ql_rtos_task_sleep_ms(1000);
}

/*===========================================================================
 * Call back Function which Recives UART data *
 ===========================================================================*/

void ql_uart_notify_cb(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_UART_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    int read_len = 0;
	int lenght_of_rcvBUffer;
     QL_UART_DEMO_LOG("UART port %d receive ind type:0x%x, receive data size:%d", port, ind_type, size);
    switch(ind_type)
    {
        case QUEC_UART_RX_OVERFLOW_IND:  //rx buffer overflow
        case QUEC_UART_RX_RECV_DATA_IND:
        {
			memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE+1);
			real_size= MIN(size, QL_UART_RX_BUFF_SIZE);
			read_len = ql_uart_read(port, recv_buff, real_size);                   // reads only 64 bytes of data at a time
			 QL_UART_DEMO_LOG("read_len=%d, recv_data=%s", read_len, recv_buff);
			if((recv_buff[0]=='$')||(Usart_rcvbf_collect_flag==1))                 // "$"" is entry point for data head deliminator
			{			
					
				if(Usart_rcvbf_collect_flag==0)	                                // clears buffer for next new cycle	of data	
				{	
					memset(rcv_data_buf, 0, sizeof(rcv_data_buf));
					
                }
				strncat((char *)rcv_data_buf,(const char *)recv_buff,(read_len)); // 64 bytes of data added to collect total 180 bytes
				Usart_rcvbf_collect_flag=1;
				tot_rcvd=tot_rcvd+read_len;
				QL_UART_DEMO_LOG("rcv_data_buf data=%s and tot_rcvd:%d\n",rcv_data_buf,tot_rcvd);
				if(recv_buff[0]=='#')                                             // indicates data end deliminator
				{
					Usart_rcvbf_collect_flag=0;
					tot_rcvd=0;
					QL_UART_DEMO_LOG("rcv_data_buf inside data=%s\n",rcv_data_buf);
					QL_UART_DEMO_LOG(" strlen of(rcv_data_buf) is %d\n",strlen(rcv_data_buf));// gives total data size
					QL_UART_DEMO_LOG(" strlen of(rcv_data_buf) is %d\n",sizeof(rcv_data_buf));// gives total buffer size
                     lenght_of_rcvBUffer = strlen(rcv_data_buf);
					if(strlen(rcv_data_buf)>70)        // received event data and periodid data response
					{
					    QL_UART_DEMO_LOG("Received periodic push on interval data\n");
						event_uart.param2 = 1;
						event_uart.param3 =0;
						ql_rtos_event_send(mqtt_task, &event_uart);
						tokenize_UartData(rcv_data_buf,lenght_of_rcvBUffer);
						networkStatus =Timesync();
						get_time();
						ConvertJsonData();
						ql_sdmmc_fs_test(json_data_buf);
					    ql_sdmmc_fs_NetworkLog(json_data_buf);// create log and append no network/server down log data
					}
					else                   // attributes dat was received 60 bytes of config data
                     {
                        QL_UART_DEMO_LOG("Received Attributes data\n");  
						event_uart.param2 = 2;
						event_uart.param3 =0;
						ql_rtos_event_send(mqtt_task, &event_uart);
						tokenize_UartData(rcv_data_buf,lenght_of_rcvBUffer);
						networkStatus = Timesync();
						get_time();
                        ConvertJsonData();
					} 
				}
				
				memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE+1);
			}
		}
		break;
      
        case QUEC_UART_TX_FIFO_COMPLETE_IND: 
        {
            QL_UART_DEMO_LOG("tx fifo complete");
			event_uart.param3 =1;
			ql_rtos_event_send(mqtt_task, &event_uart);
            break;
        }
    }
    free(recv_buff);
    recv_buff = NULL;
}

/*===========================================================================
 *Uart Pin initialisation happens here *
 ===========================================================================*/

int uart_init()
{
	int ret = 0;
	ql_uart_config_s uart_cfg = {0};
    uart_cfg.baudrate = QL_UART_BAUD_9600;
    uart_cfg.flow_ctrl = QL_FC_NONE;
    uart_cfg.data_bit = QL_UART_DATABIT_8;
    uart_cfg.stop_bit = QL_UART_STOP_1;
    uart_cfg.parity_bit = QL_UART_PARITY_NONE;

    ret = ql_uart_set_dcbconfig(QL_UART_PORT_1, &uart_cfg);
    QL_UART_DEMO_LOG("uart init return is: 0x%x", ret);
	
    ret = ql_uart_open(QL_UART_PORT_1);
    QL_UART_DEMO_LOG("uart port open return is : 0x%x", ret);

	 
	memset(&uart_cfg, 0, sizeof(ql_uart_config_s));
	ret = ql_uart_get_dcbconfig(QL_UART_PORT_1, &uart_cfg);
	QL_UART_DEMO_LOG("ret: 0x%x, baudrate=%d, flow_ctrl=%d, data_bit=%d, stop_bit=%d, parity_bit=%d",ret, uart_cfg.baudrate, uart_cfg.flow_ctrl, uart_cfg.data_bit, uart_cfg.stop_bit, uart_cfg.parity_bit);
    
	 ret = ql_uart_register_cb(QL_UART_PORT_1, ql_uart_notify_cb);
	 QL_UART_DEMO_LOG("return from uart register callback is : 0x%x", ret);
	
	return ret;

}

/*===========================================================================
 ***************************Main thread *********************************
 ===========================================================================*/

static void ql_uart_demo_thread(void *param)
{
	
	snl_conf.interval=pdata_int;
	strcpy(snl_conf.domain,"mqtts://pmkrms.hareda.gov.in:443");
	strcpy(snl_conf.password,"c6c4dd14");
	int ret = 0;
	ret = uart_init();
	//ql_sdmmc_pin_init();
	if(QL_UART_SUCCESS == ret)
	{
	 	set_network_time();
	    while(1)
	     {
			get_time();
			
			// ql_rtos_queue_wait(gnss_location_time_Data,(uint8 *)&gps_strctdata,sizeof(lat_lon_sat_dttm_t),500);// donot delete this is queue implementation example for ref
			// QL_UART_DEMO_LOG("quue from gnss to uart new struct data gps_strctdata.lat :%s",gps_strctdata.lat);
		    // QL_UART_DEMO_LOG("quue from gnss to uart new struct data gps_strctdata.lon :%s",gps_strctdata.lon);
		    // QL_UART_DEMO_LOG("quue from gnss to uart new struct data gps_strctdata.satlites :%s",gps_strctdata.satlites);
		    // QL_UART_DEMO_LOG("quue from gnss to uartnew struct data gps_strctdata.date_time_urt :%s",gps_strctdata.latlong_urt);
			ql_rtos_queue_wait(gnss_location_time_Data,(uint8 *)&gnnsPosDat,sizeof(gnssPositionData_t),500);
			QL_UART_DEMO_LOG("quue from gnss to uart new struct data gnnsPosDat.lat :%s",gnnsPosDat.lat);
		    QL_UART_DEMO_LOG("quue from gnss to uart new struct data gnnsPosDat.lon :%s",gnnsPosDat.lon);
		    QL_UART_DEMO_LOG("quue from gnss to uart new struct data gnnsPosDat.satlites :%s",gnnsPosDat.satlites);
		    QL_UART_DEMO_LOG("quue from gnss to uartnew struct data gnnsPosDat.date_time_urt :%s",gnnsPosDat.latlong_urt);
	        

			ql_rtos_queue_wait(rpc_data,(uint8 *)&TB_RPC,sizeof(RPC_value_t),500);
		    QL_UART_DEMO_LOG("rpc value received in struct TB_RPC.PUMP_ON is:%d\r\n",TB_RPC.PUMP_ON);
			QL_UART_DEMO_LOG("rpc value received in struct TB_RPC.PUMP_OFF is:%d\r\n",TB_RPC.PUMP_OFF);
			QL_UART_DEMO_LOG("rpc value received in struct TB_RPC.GET_cfg is:%d\r\n",TB_RPC.GET_cfg);
			QL_UART_DEMO_LOG("rpc value received in struct TB_RPC.GET_live is:%d\r\n",TB_RPC.GET_live);
			QL_UART_DEMO_LOG("rpc value received in struct TB_RPC.OTA is:%d\r\n",TB_RPC.OTA);

            ql_rtos_task_sleep_ms(50);
			QL_UART_DEMO_LOG("IMEI: %s\r\n", imei_no);
			QL_UART_DEMO_LOG("A MINUTE HAS ELAPSED\r\n");
			QL_UART_DEMO_LOG("event gnss event param3_sets when gps locked:%d\r\n",event.param3);
			QL_UART_DEMO_LOG("periodic data interval is :%d\r\n",pdata_int);
			QL_UART_DEMO_LOG("event uart param1:%d\r\n",event_uart.param1);
			QL_UART_DEMO_LOG("event uart param1:%d\r\n",event_uart.param1);
			QL_UART_DEMO_LOG("event uart param2:%d\r\n",event_uart.param2);
			QL_UART_DEMO_LOG("event uart param3:%d\r\n",event_uart.param3);
			QL_UART_DEMO_LOG("date from time sync:%s\r\n",dt_time_CRI);

if(ql_event_try_wait(&event_uart) !=0)
{
	continue;
}

		if(event_uart.id ==QUEC_TIMER_EVENT_IND)	
		{
			if(event_uart.param1 == 1)
			   {
				  ql_rtos_task_sleep_s(5);
				  uart_data_send(Atribute_data_req);
				  QL_UART_DEMO_LOG("Printing attributes values\r\n");
				  sprintf(sd_NnwPath_name_cmpr,"SD:NNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
	              sprintf(sd_NnwTBPath_name_cmpr,"SD:TBNNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
	              sprintf(sd_NnwSDMPath_name_cmpr,"SD:SDNNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
			 		
		        
				} 
			if((event_uart.param1 % pdata_int)== 0)
			 {
				ql_rtos_task_sleep_s(5);
				uart_data_send(Periodic_data_req);
				QL_UART_DEMO_LOG("Printing periodic values\r\n");
				
			}

			//if((event_uart.param1 % hdata_int)== 0)
			if((event_uart.param1 % hdata_int == 0)&& (event_uart.param1 != 0))
			 {
				ql_rtos_task_sleep_s(5);
				hdata_onceupdate = 1;
				QL_UART_DEMO_LOG("Publish heartbeat data in sedem\r\n");
			}
				if((event_uart.param1 % 1  == 0) && (event_uart.param1 !=5)&& (event_uart.param1!=7))// &&(event_uart.param3==1))
			 {
				 networkStatus = Timesync();		
				 get_time();		      
				 ql_rtos_task_sleep_s(5);
				 uart_data_send((unsigned char*)dt_time_CRI);
				 QL_UART_DEMO_LOG("Printing time values\r\n");

			}

			if((event_uart.param1 ==5)||(event_uart.param1==7))//&&(event_uart.param3==1))
			{
                ql_rtos_task_sleep_s(5);
				uart_data_send((unsigned char*)gnnsPosDat.latlong_urt);
				QL_UART_DEMO_LOG("printing gps values\r\n");

			}
			if((event_uart.param1 % Retry_server)== 0)
			 {
				
				Retry_server_flag = 1;
				QL_UART_DEMO_LOG("retry connection to the down server\r\n");
			
			}

			if((event_uart.param1 % Retry_server_thingsboard)== 0)
			 {
				
				Retry_server_flag_Thingboard = 1;
				QL_UART_DEMO_LOG("retry connection to the down server\r\n");
			    
			}
			if(event_uart.param1 == 31)
			{
				DataRepubFlag =1;
				QL_UART_DEMO_LOG("search for missing packet\r\n");
			}

			if(event_uart.param1 == 43)
			{
				DataRepubFlag_TB =1;
				QL_UART_DEMO_LOG("search for missing  packet for Schnell iot\r\n");
			}

			if(event_uart.param1 == 53)
			{
				DataRepubFlag_SDM =1;
				QL_UART_DEMO_LOG("search for missing packet for sedem server\r\n");
			}
			
			}	
		}	
	}	
	
}

/*===========================================================================
 *Main thread which initialises all Thread*
 ===========================================================================*/

void ql_uart_app_init(void)
{
	QlOSStatus err = 0;
	int ret = 0;
    ql_sdmmc_pin_init();
	err = ql_sdmmc_mount_demo();
    if(err!= QL_SDMMC_SUCCESS)
    {
        QL_MQTT_LOG("sdmmc mount demo fail err = %d", err);
    }
	ql_dev_get_imei(imei_no, sizeof(imei_no), 0);
	QL_UART_DEMO_LOG("IMEI: %s\r\n", imei_no);
	
	ret = ql_rtos_timer_create(&cnt_timer, uart_task, timer_callback, NULL);
	
	QL_UART_DEMO_LOG("create timer ret: 0x%x", ret);
	if(ret != 0)
	{
		QL_UART_DEMO_LOG("demo_timer created failed");
	
	}
	
	
	ret = ql_rtos_timer_start(cnt_timer, 60000, 0);   
	if(ret != 0)
	{
		QL_UART_DEMO_LOG("demo_timer start failed");
				
		}

	

	err = ql_rtos_task_create(&uart_task, 4*QL_UART_TASK_STACK_SIZE, QL_UART_TASK_PRIO, "QUARTDEMO", ql_uart_demo_thread, NULL, QL_UART_TASK_EVENT_CNT);
	if (err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("demo task created failed");
        return;
	}
   

	err = ql_rtos_task_create(&gnss_task, 4096, APP_PRIORITY_NORMAL, "ql_gnssdemo", ql_gnss_demo_thread, NULL, 5);
    if( err != QL_OSI_SUCCESS )
    {
       QL_UART_DEMO_LOG("gnss demo task created failed");
    }

	err = ql_rtos_task_create(&mqtt_task, 16*1024, APP_PRIORITY_NORMAL, "mqtt_app", mqtt_app_thread, NULL, 5); 
	if(err != QL_OSI_SUCCESS)
    {
		QL_MQTT_LOG("mqtt_app init failed");
	}
		
	
}


