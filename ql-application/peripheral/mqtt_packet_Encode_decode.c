#include "stdbool.h"
#include "mqtt_thingsbrd.h"
#include "jsmn.h"
#include "uart_demo.h"
#include "time_sync.h"
#include "ql_power.h"
#include "gnss_cri.h"

#define QL_MQTT_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_MQTT_LOG(msg, ...)			QL_LOG(QL_MQTT_LOG_LEVEL, "ql_MQTT", msg, ##__VA_ARGS__)
#define QL_MQTT_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_MQTT", msg, ##__VA_ARGS__)

char valueString[25][15]={'\0'};    // values for the periodic key values of json data cri
char valConfString[15][15]={'\0'}; //values for the config  key values of json data cri
char keyString[38][25]= {"PMAXFREQ1","PFREQLSP1","PFREQHSP1","PCNTRMODE1","PRUNST1","PREFFREQ1","POPFREQ1","POPI1","POPV1","POPKW1","PDC1V1","PDC1I1","PDCVOC1","PDKWH1","PTOTKWH1","POPFLW1","POPDWD1","POPTOTWD1","PMAXDCV1","PMAXDCI1","PMAXKW1","PMAXFLW1","PDHR1","PTOTHR1","ASN_11"};
char akeyString[17][20]= {"mt","rv","rf","rp","rms","ih","ps","mif","maf","lv","rov","olc","drc","tmp"};
char json_data_buf[1000]={'\0'},temp_buf[200];

bool hdata_onceupdate =0;
bool configdata_onceupdate =0;
bool ondemand_onceupdate = 0;
bool Retry_server_flag = 0;
bool Retry_server_flag_Thingboard =0;
bool DataRepubFlag = 0;	
bool DataRepubFlag_TB = 0;	
bool DataRepubFlag_SDM = 0;	
int Pump_Run_Status; 
int temperature_status = 0;
char heart_buf[600]={'\0'};
int msgid_ev = 0;
char cfg_rw[10]={'\0'};
bool fun_flag=0,write_cmd=0,nw_reg_flag=0,read_cmd=0,conf_f=0,init_f=0,conf_wr_f=0,cred_f;
char cotp_v[12]={"0"},potp_v[12]={"0"},otp_buf[100]={'\0'},ondem_buf[1500]={'\0'},config_buf[1500]={'\0'};
int numberOFLInes =0;

/* TELEMETRY OUTGOING DATA PACKING AND ENCODE SECTION STARTS HERE*/

 /*===========================================================================
 *Deserialisation of Uart received data Happens here*
 ===========================================================================*/

void tokenize_UartData(char* string,int data_len) // "string pointer" argument is from "uart thread" good candidate for queue variable
{
	 char delims[] = "$/:,{}\"+";  // deliminator for data our's is ",", and "$"
    char *p;
	char Token[250]={0};

	memcpy(Token, string, data_len); // copy imei no from the received data to the global variable
	if(data_len > 70)            // recived event of periodic data 180 bytes
	{
	p = strtok (Token,delims);
  	memset(valueString, 0, sizeof(valueString)); 
  	int i = 0;
  	while (p!= NULL)
	{
    	strcpy(valueString[i], p);
		QL_MQTT_LOG("\r\n value [%d] :%s\r\n",i,valueString[i]);
		Pump_Run_Status =atoi(valueString[4]);
		QL_MQTT_LOG("Pump_Run_Status :%d\n",Pump_Run_Status);
    	p = strtok (NULL, delims);
		i++;
  	}
	}
	else         // recived config values(<70 bytes) to be updated in attributes
	{
      p = strtok (Token,delims);
  	 memset(valConfString, 0, sizeof(valConfString)); 
  	 int i = 0;
  	while (p!= NULL)
	{
    	strcpy(valConfString[i], p);
		QL_MQTT_LOG("\r\n value [%d] :%s\r\n",i,valConfString[i]);
		temperature_status =atoi(valConfString[13]);
		QL_MQTT_LOG("temperature_status :%d\n",temperature_status);
		p = strtok (NULL, delims);
		i++;
  	}
	
	}
}


/*===========================================================================
 * Data packing of Uart to JSon Payload Happens here*
 ===========================================================================*/
void ConvertJsonData()
{
	int write_leng;
	//d_index++;
	memset(json_data_buf, 0, sizeof(json_data_buf));
	if(event_uart.param2 == 1)
	{
		d_index++;
	  //sprintf(json_data_buf, "{\"TIMESTAMP\":\"%s\",\"INDEX\":%d,\"IMEI\":\"%s\",\"VER\":%s,\"LAT\":\"%s\",\"LONG\":\"%s\",\"SAT\":\"%s\"",current_dt,d_index,imei_no,version,gnnsPosDat.lat,gnnsPosDat.lon,gnnsPosDat.satlites);
      //sprintf(json_data_buf, "{\"VD\":\"1\",\"TIMESTAMP\":\"%s\",\"INDEX\":\"%03d\",\"VER\":%s,\"LOAD\":\"%d\",\"STINTERVAL\":\"%02d\",\"MSGID\":\"%03d\",\"DATE\":\"%s\",\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\"",current_dt,d_index,version,sd_load_status,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v);
	  sprintf(json_data_buf, "{\"VD\":1,\"TIMESTAMP\":\"%s\",\"MAXINDEX\":96,\"INDEX\":%d,\"LOAD\":%d,\"STINTERVAL\":%d,\"MSGID\":\"%d\",\"DATE\":%s,\"IMEI\":\"%s\",\"POTP\":\"%s\",\"COTP\":\"%s\"",current_dt,d_index,sd_load_status,pdata_int,msgid_ev,only_dt,imei_no,potp_v,cotp_v);	
	  for(int i=0,j=0;i<26;i++,j++)
		{
			if(i==25) sprintf(temp_buf,"}");
			else sprintf(temp_buf,",\"%s\":\"%s\"",keyString[i],valueString[j]);
			strcat(json_data_buf,temp_buf);
			memset(temp_buf, 0, sizeof(temp_buf));
			QL_MQTT_LOG(" TB sending periodic  pub:%s",json_data_buf);
		}
	write_leng = strlen(json_data_buf);
	QL_MQTT_LOG("\r\n string lengh of json data buffer is :%d\r\n",write_leng);
	QL_MQTT_LOG("\r\n json packed value is:%s\r\n",json_data_buf);
	}

	if(event_uart.param2 == 2)
		{
			signal_quality();
			sprintf(temp_buf,"{\"imei\":\"%s\",\"VER\":%s,\"rssi\":%d,\"url\":\"%s\",\"pint\":%d,\"LAT\":\"%s\",\"LONG\":\"%s\",\"SAT\":\"%s\",\"APN\":\"%s\"",imei_no,version,csq,snl_conf_read.domain,pdata_int,gnnsPosDat.lat,gnnsPosDat.lon,gnnsPosDat.satlites,operator_name);
    		strcat(json_data_buf,temp_buf);
	   		 memset(temp_buf, 0, sizeof(temp_buf));
		for(int i=0,j=0;j<=14;i++,j++)
	  	{
			if(j==14) sprintf(temp_buf,"}");
			else sprintf(temp_buf,",\"%s\":\"%s\"",akeyString[i],valConfString[j]);
			strcat(json_data_buf,temp_buf);
			memset(temp_buf, 0, sizeof(temp_buf));
			QL_MQTT_LOG(" TB sending attributes pub:%s",json_data_buf);
	        
		}
	write_leng = strlen(json_data_buf);
	QL_MQTT_LOG("\r\n string lengh of attributes data buffer is :%d\r\n",write_leng);

	
	}
	 
	 //uart_data_send((unsigned char*)json_data_buf);
	
}


 /* TELEMETRY OUTGOING DATA PACKING AND ENCODE  SECTION ENDS HERE*/



/*===========================================================================
 * RPC INCOMING DATA DECODE SECTION STARTS HERE*
 ===========================================================================*/


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
    	strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}


int jsonextract(char *data)
{
	int i;
    int r;	
	jsmn_parser p;
    jsmntok_t t[200]; /* We expect no more than 128 tokens *///value changed to 200 to accept atleast 17 key values
	char vv[20]={"\0"};
    jsmn_init(&p);
	r = jsmn_parse(&p, data, strlen(data), t,sizeof(t) / sizeof(t[0]));
	if (r < 0) 
	{
		QL_MQTT_LOG("Failed to parse JSON: %d\n", r);
	}
	if (r < 1 || t[0].type != JSMN_OBJECT) 
	{
		QL_MQTT_LOG("Object expected\n");
	}
	QL_MQTT_LOG("total key value pair in payload is =%d\n",r);
	for (i = 1; i < r; i++) 
	{
		if (jsoneq(data, &t[i], "params") == 0) 
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			if(atoi(vv)==0)
			{
				ql_rtos_task_sleep_s(1);
				QL_MQTT_LOG("Getlive rpc published =%d\n",atoi(vv)); 
				TB_RPC.PUMP_OFF = 1;

			}

			else if(atoi(vv)==1)
			{
				ql_rtos_task_sleep_s(1);              // 1  changed as 5 to get increased freq value in telemetry
				QL_MQTT_LOG("Getlive rpc published =%d\n",atoi(vv)); 
				TB_RPC.PUMP_ON = 1;
				
			}

			else if(atoi(vv)==2)
			{
				ql_rtos_task_sleep_s(1);
				QL_MQTT_LOG("Getlive rpc published =%d\n",atoi(vv));
				TB_RPC.GET_live = 1;
				
			}

			else if(atoi(vv)==3)
			{
				ql_rtos_task_sleep_s(1);
				QL_MQTT_LOG("Getlive rpc published =%d\n",atoi(vv));
				TB_RPC.GET_cfg =1;
				
			}

			else if(atoi(vv)==4)
			{
				ql_rtos_task_sleep_s(1);
				QL_MQTT_LOG("Getlive rpc published =%d\n",atoi(vv));
			
				
			}

			else if(atoi(vv)==5)
			{
				
				QL_MQTT_LOG(" OTA rpc received ",atoi(vv)); 
				TB_RPC.OTA = 1;
				
			}

			else if(atoi(vv)==6)
			{
				ql_rtos_task_sleep_s(3);
				QL_MQTT_LOG("reset command received =%d\n",atoi(vv));
				ql_power_reset(RESET_NORMAL);
				
			}

			else
			{
			  
			  QL_MQTT_LOG(" set rpc received pub =%s\n ",vv);
			}
			i++;
		} 
        
		if (jsoneq(data, &t[i], "pints") == 0)        //this value to be loaded in ufs along with password  and urls            
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			pdata_int=atoi(vv);
			QL_MQTT_LOG("push data interval=%d configured \n",pdata_int); 
			write_config_fileUFS();
			TB_RPC.Pint =1;
			i++;
		}

		if (jsoneq(data, &t[i], "urls") == 0)        //this value to be loaded in ufs along with password and periodic interval           
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			
			QL_MQTT_LOG("url set from rpc is:%s \n",vv); 
			strcpy(snl_conf_read.domain, vv);
			write_config_fileUFS();
			i++;
		}
		if (jsoneq(data, &t[i], "loc") == 0)        //this value to be loaded in ufs along with password and periodic interval           
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			
			QL_MQTT_LOG("Gps Locaion set from rpc is:%s \n",vv); 
			snl_conf_read.LiveLocation = atoi(vv);
			if(snl_conf_read.LiveLocation == 1)
			{
				strcpy(snl_conf_read.Latitude,gnnsPosDat.lat);
				strcpy(snl_conf_read.Longitude,gnnsPosDat.lon);
				QL_MQTT_LOG("lat  set from rpc is:%s\n",snl_conf_read.Latitude);
				QL_MQTT_LOG("long set from rpc is:%s \n",snl_conf_read.Longitude);
			}

			write_config_fileUFS();
			i++;
		}

		if (jsoneq(data, &t[i], "tsync") == 0)        //this value to be loaded in ufs along with password and periodic interval           
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			
			QL_MQTT_LOG("Time sync set from rpc is:%s \n",vv); 
			snl_conf_read.LookupTimeSync = atoi(vv);
			if(snl_conf_read.LiveLocation == 1)
			{
				snl_conf_read.DayIndex = tm.tm_mday;
				snl_conf_read.MonthIndex = tm.tm_mon;
				snl_conf_read.YearIndex = tm.tm_year;	
				
	
				QL_MQTT_LOG("Day is set as:%d\n",snl_conf_read.DayIndex);
				QL_MQTT_LOG("Month is set as:%d\n",snl_conf_read.MonthIndex);
				QL_MQTT_LOG("Year is set as:%d\n",snl_conf_read.YearIndex);
			}

			write_config_fileUFS();
			i++;
		}
		if (jsoneq(data, &t[i], "password") == 0)        //this value to be loaded in ufs along with password and periodic interval           
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			
			QL_MQTT_LOG("password set from rpc is:%s \n",vv); 
			strcpy(snl_conf_read.password, vv);
			write_config_fileUFS();
			ql_power_reset(RESET_NORMAL);
			i++;
		}

		if (jsoneq(data, &t[i], "operator") == 0)        
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			
			QL_MQTT_LOG("operator set from rpc is:%s \n",vv); 
			strcpy(snl_conf_read.operator, vv);
			write_config_fileUFS();
			ql_power_reset(RESET_NORMAL);
			i++;
		}

		if (jsoneq(data, &t[i], "P1") == 0)  
		{
			TB_RPC.MotorType_P1 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start); 
			ql_rtos_task_sleep_s(1);
			TB_RPC.MotorType_P1val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P2") == 0)
		{
			TB_RPC.RatedVol_P2 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.RatedVol_P2val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P3") == 0)
		{
			TB_RPC.RatedFreq_P3 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.RatedFreq_P3val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P4") == 0)
		{
			TB_RPC.InstallHead_P4 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.InstallHead_P4val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P5") == 0)
		{
			TB_RPC.MinFreq_P5 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.MinFreq_P5val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P6") == 0)
		{
			TB_RPC.MaxFreq_P6 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.MaxFreq_P6val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P7") == 0)
		{
			TB_RPC.LowVol_P7 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.LowVol_P7val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P8") == 0)
		{
			TB_RPC.RelOnVol_P8 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.RelOnVol_P8val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "P9") == 0)
		{
			TB_RPC.OverloadCur_P9 =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			// mq_port=atoi(vv);
			ql_rtos_task_sleep_s(1);
			TB_RPC.OverloadCur_P9val = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}
		if (jsoneq(data, &t[i], "PA") == 0)
		{
			TB_RPC.DryRun_PA =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.DryRun_PAval = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv);
			i++;
		}
		if (jsoneq(data, &t[i], "PB") == 0)
		{
			TB_RPC.Temp_PB =1;
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			ql_rtos_task_sleep_s(1);
			TB_RPC.Temp_PBval = (unsigned char*)vv;
			QL_MQTT_LOG("set rpc rcvd for pub =%s \n",vv); 
			i++;
		}

		if (jsoneq(data, &t[i], "msgid") == 0) 
		  {
				sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
				msgid_ev=atoi(vv);
				QL_MQTT_LOG("msgid_ev=%d\n",msgid_ev); //myFSAtoi
				i++;
		   }
	
		if (jsoneq(data, &t[i], "COTP") == 0) 
			{
				sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
				strcpy(cotp_v, vv);
				QL_MQTT_LOG("cotp_v=%s\n",cotp_v); //myFSAtoi
				i++;
			} 
		 if (jsoneq(data, &t[i], "POTP") == 0)
			{
				sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
				strcpy(potp_v, vv);
				QL_MQTT_LOG("potp_v=%s\n",potp_v);
				i++;
			} 

		if (jsoneq(data, &t[i], "cmd") == 0) 
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			if(strcmp(vv,"write")==0)
			{
				QL_MQTT_LOG("write cmd received =%s\n",vv); 
				write_cmd = 1;
				read_cmd = 0;
				strcpy(cfg_rw, vv);
			}
			if(strcmp(vv,"read")==0)
			{
				QL_MQTT_LOG("read cmd received =%s\n",vv); 
				read_cmd = 1;
				write_cmd = 0;
				strcpy(cfg_rw, vv);
			}
			QL_MQTT_LOG("cmd=%s\n",vv); //myFSAtoi
			i++;
		}	

		if (jsoneq(data, &t[i], "type") == 0) 
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			if(strcmp(vv,"config")==0)
			{
			QL_MQTT_LOG("request type=%s\n",vv); //myFSAtoi
			
			}
				i++;
		}

			if (jsoneq(data, &t[i], "DO1") == 0) 
		{
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
			QL_MQTT_LOG("DO1=%d\n",atoi(vv));

			if(atoi(vv)==1 && (write_cmd ==1)) TB_RPC.PUMP_ON = 1;		//ond_wri = ondemand write, to pump on					
									
		     else if(atoi(vv)==0 && (write_cmd ==1)) TB_RPC.PUMP_OFF = 1; //ond_wri = ondemand write, to pump off
			i++;
		}
	
		if (jsoneq(data, &t[i], "HEARTINTERVAL") == 0) 
        {
            sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            QL_MQTT_LOG("HEARTINTERVAL=%d\n",atoi(vv));
            hdata_int  = atoi(vv);
            QL_MQTT_LOG("hdata_int value is chnaged from rpc =%d\n",atoi(vv));  
            i++;
        }


            if (jsoneq(data, &t[i], "UPDATEINTERVAL") == 0) 
        {
            sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            QL_MQTT_LOG("UPDATEINTERVAL=%d\n",atoi(vv));
            pdata_int  = atoi(vv);
            QL_MQTT_LOG("update data_int value is chnaged from rpc =%d\n",atoi(vv)); 
			write_config_fileUFS();
			TB_RPC.Pint =1;   
            i++;
        }

		if (jsoneq(data, &t[i], "RESTART") == 0) 
        {
            sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            QL_MQTT_LOG("RESTART=%d\n",atoi(vv));
			if(atoi(vv)==1 && (write_cmd ==1)) ql_power_reset(RESET_NORMAL);
            i++;
        }
		if (jsoneq(data, &t[i], "DLOAD") == 0) 
        {
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            strcpy(sd_path_name_RPC, vv);
			QL_MQTT_LOG("DLOAD=%s\n",sd_path_name_RPC);
			read_logged_files_fromRPC(sd_path_name_RPC);
			i++;
		}

		if (jsoneq(data, &t[i], "LOAD") == 0) 
        {
			char RPC_filename[25]={'\0'};
            sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            sprintf(RPC_filename,"NNW%d%d%d",tm.tm_mday,tm.tm_mon,tm.tm_year);
            QL_MQTT_LOG("LOAD RPC received=%s\n",RPC_filename);
            read_logged_files_fromRPC(RPC_filename);
            i++;
		}

		if (jsoneq(data, &t[i], "REFRESH") == 0) 
        {
			sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            QL_MQTT_LOG("REFRESH RPC received=%d\n",atoi(vv));
			airplane_mode();
			i++;
        }

		if (jsoneq(data, &t[i], "LogSize") == 0)        //this value to be loaded in ufs along with password  and urls            
        {
            sprintf(vv,"%.*s", t[i + 1].end - t[i + 1].start,data + t[i + 1].start);
            numberOFLInes=atoi(vv);
			QL_MQTT_LOG("Number of lines set by RPC is : %d",numberOFLInes);
            i++;
        }

	} 
    return 1; 
}





/*===========================================================================
 * RPC INCOMING DATA DECODE SECTION ENDS HERE*
 ===========================================================================*/