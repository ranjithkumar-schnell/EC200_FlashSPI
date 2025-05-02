
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#include "ql_api_osi.h"
#include "ql_gpio.h"
#include "osi_api.h"
#include "ql_api_nw.h"
#include "ql_api_dev.h"
#include "ql_power.h"
#include "ql_log.h"
#include "ql_api_sim.h"
#include "ql_api_rtc.h"
#include "ql_uart.h"
#include "time_sync.h"
#include "gnss_cri.h"
#include "ql_api_datacall.h"


#define QL_UART_DEMO_LOG_LEVEL			QL_LOG_LEVEL_INFO
#define QL_UART_DEMO_LOG(msg, ...)		QL_LOG(QL_UART_DEMO_LOG_LEVEL, "ql_uart_demo", msg, ##__VA_ARGS__)




/*===========================================================================
 *Definition
 ===========================================================================*/

	

ql_rtc_time_t tm;
unsigned char csq=0;
int Rssi = 0;
uint8_t sim_id = 0;
ql_sim_status_e card_status = 0;
int sim_flag =0;
int ret = 0;
int networkStatus =1;
int Ist_Hour,Ist_Min,Ist_Sec,Ist_test,Gps_Year,Gps_Month,Gps_Day;


/*===========================================================================
 * Loads time value  to RTC berfore validation *
 ===========================================================================*/
void set_network_time()
{

	tm.tm_year = 2011;  //setting dummy time before getting time from the network 
   	tm.tm_mon  = 1;
    tm.tm_mday = 1;
    tm.tm_hour = 1;
    tm.tm_min  = 1;
    tm.tm_sec  = 1;
	networkStatus = Timesync(); // to set network or gps time manipulae this function
	QL_UART_DEMO_LOG("networkStatus flag is [0] in newtwork,[1] insige gps loop:%d",networkStatus);
	
}
/*===========================================================================
 * Function which controls change over between network and GNSS time  *
 ===========================================================================*/
void get_time()
{

	if(networkStatus==0)
	{
		QL_UART_DEMO_LOG("inside network set  loop");
		get_network_time();
	} 
	else 
	{
	   QL_UART_DEMO_LOG("gps location locked:%d \r\n",event.param3);
       if(event.param3==1)  // only sets when gps is locked ,event sent by gnss task
	   {
		 QL_UART_DEMO_LOG("inside gps time set loop");
		 set_gps_time();
	   }
	}
}
/*===========================================================================
 * Function returns network  set time  *
 ===========================================================================*/
void get_network_time()
{
	tm.tm_year = time_z.year;  //get network time from nitz stuct from network or network time
   	tm.tm_mon  = time_z.month;
    tm.tm_mday = time_z.date;
    tm.tm_hour = time_z.hour;
    tm.tm_min  = time_z.min;
    tm.tm_sec  = time_z.sec;
    ql_rtc_set_time(&tm);
	ql_rtc_get_time(&tm);
	ql_rtc_print_time(tm);
    ql_rtc_set_timezone(22);    //time zone calculation in 15min increments +5:30 for 1 hour 4--> 15 mins so for 5:30 hrs 22
    ql_rtc_get_localtime(&tm);
	ql_rtc_print_time(tm);
	
	QL_UART_DEMO_LOG("Network year is :%d\r\n",(tm.tm_year-2000));
	if(tm.tm_year>= 2023)
	{
    	sprintf(only_tm,"%02d%02d%02d",tm.tm_hour,tm.tm_min,tm.tm_sec);
		QL_UART_DEMO_LOG("time net value is :%s ",only_tm);
		sprintf(only_dt,"%02d%02d%02d",(tm.tm_year-2000),tm.tm_mon,tm.tm_mday);
		QL_UART_DEMO_LOG("date time sent to cri frm net is :%s ",only_dt);
		sprintf(dt_time_CRI,"$D%02d/%02d/%02d%02d.%02d.%02d#",tm.tm_mday,tm.tm_mon,(tm.tm_year-2000),tm.tm_hour,tm.tm_min,tm.tm_sec);// D30/10/2311.30.50
		QL_UART_DEMO_LOG("curent date value frm net is :%s ",dt_time_CRI);
    	sprintf(current_dt,"%04d-%02d-%02d %02d:%02d:%02d",(tm.tm_year-0000),tm.tm_mon,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        QL_UART_DEMO_LOG("inside network time sync loop\r\n");
	}

	else set_gps_time();  //switch over to GPS network

}
/*===========================================================================
 * Function returns GNSS set time when network time fails *
 ===========================================================================*/
void set_gps_time()
{      
	   	tm.tm_year =g_dt_tm.tm_year;
        tm.tm_mon  = g_dt_tm.tm_mon; // get network time from gnss struct from gga
        tm.tm_mday = g_dt_tm.tm_mday;
        tm.tm_hour = g_dt_tm.tm_hour;
        tm.tm_min  = g_dt_tm.tm_min;
        tm.tm_sec  = g_dt_tm.tm_sec;
	    ql_rtc_set_time(&tm);
	    ql_rtc_get_time(&tm);
	    ql_rtc_print_time(tm);
        ql_rtc_set_timezone(22);   //time zone calculation in 15min increments +5:30 for 1 hour 4--> 15 mins so for 5:30 hrs 22
        ql_rtc_get_localtime(&tm);
	    ql_rtc_print_time(tm);
		Ist_Hour = g_dt_tm.tm_hour;
        Ist_Min = g_dt_tm.tm_min;
        Ist_Sec  = g_dt_tm.tm_sec;
	if(Ist_Min > 30)
	{
		Ist_Hour = Ist_Hour + 6;
		Ist_Min = Ist_Min- 30;
					
	}
	else
	{ 
		Ist_Hour = Ist_Hour + 5;
		Ist_Min = Ist_Min + 30;
    }
	if(tm.tm_year>= 2023)
	{
       	QL_UART_DEMO_LOG("gps location locked:%d \r\n",event.param3);
	    sprintf(only_tm,"%02d%02d%02d",Ist_Hour,Ist_Min,Ist_Sec);
		QL_UART_DEMO_LOG("time value is :%s ",only_tm);
		sprintf(only_dt,"%02d%02d%02d",(tm.tm_year-2000),tm.tm_mon,tm.tm_mday);
		QL_UART_DEMO_LOG("date time sent to cri is is :%s ",only_dt);
		sprintf(dt_time_CRI,"$D%02d/%02d/%02d%02d.%02d.%02d#",tm.tm_mday,tm.tm_mon,(tm.tm_year-2000),Ist_Hour,Ist_Min,Ist_Sec);// D30/10/2311.30.50
		QL_UART_DEMO_LOG("curent date value is :%s ",dt_time_CRI);
    	sprintf(current_dt,"%04d-%02d-%02d %02d:%02d:%02d",(tm.tm_year-0000),tm.tm_mon,tm.tm_mday,Ist_Hour,Ist_Min,Ist_Sec);
       	 QL_UART_DEMO_LOG("inside gps time sync loop\r\n");
	}
	else
	{
		sprintf(only_tm,"%02d%02d%02d",0,0,0);
		QL_UART_DEMO_LOG("time value is :%s ",only_tm);
		sprintf(only_dt,"%02d%02d%02d",0,0,0);
		QL_UART_DEMO_LOG("date time sent to cri is is :%s ",only_dt);
		sprintf(dt_time_CRI,"$D%02d/%02d/%02d%02d.%02d.%02d#",0,0,0,0,0,0);// D30/10/2311.30.50
		QL_UART_DEMO_LOG("curent date value is :%s ",dt_time_CRI);
    	sprintf(current_dt,"%04d-%02d-%02d %02d:%02d:%02d",0,0,0,0,0,0);
       	QL_UART_DEMO_LOG("inside gps time sync loop\r\n");

	}
}
/*===========================================================================
 * returns Sim in slot status *
 ===========================================================================*/

int sim_status()
{
	
	ret = ql_sim_get_card_status(sim_id, &card_status);
	QL_UART_DEMO_LOG("SIM CARD STATUS: %d,ret=%d\r\n", card_status,ret);
	
	if(QL_SIM_STATUS_NOSIM==card_status)
	{
		QL_UART_DEMO_LOG("SIM CARD NOT INSERTED\r\n");
		sim_flag = 0;
	}
	else if(QL_SIM_STATUS_READY==card_status)
	{
		QL_UART_DEMO_LOG("SIM CARD INSERTED\r\n");
		sim_flag = 1;
	}
	return sim_flag;
}
/*===========================================================================
 * Api which returns signal strenght in Db*
 ===========================================================================*/
unsigned char signal_quality()
{       
	ql_nw_signal_strength_info_s *signal_info = (ql_nw_signal_strength_info_s *)calloc(1, sizeof(ql_nw_signal_strength_info_s));
	ql_nw_get_csq(sim_id, &csq);
    QL_UART_DEMO_LOG("SIGNAL STRENGTH IS:%d\r\n", csq);
	ql_nw_get_signal_strength(sim_id, signal_info);
    QL_UART_DEMO_LOG(" rssi:%d, bitErrorRate:%d, rsrp:%d, rsrq:%d",signal_info->rssi, signal_info->bitErrorRate,signal_info->rsrp, signal_info->rsrq);
	Rssi = signal_info->rssi;
	free(signal_info);
    return csq;	
}
/*===========================================================================
 * Main entry function for Syncing time from nwtrk or Gnss*
 ===========================================================================*/
int Timesync()
{
	
	int ret = 0;
	
  if(sim_status()==1)
  {
    QL_UART_DEMO_LOG("try getting network time\r\n");
	 ret =0;
	
	}
  else if(sim_status()==0)
  {
     QL_UART_DEMO_LOG("try getting gps time time\r\n");
	  ret=1;
  }
 QL_UART_DEMO_LOG("network[0] or gps flag[1]is set as:%d\r\n",ret);
 return ret;
}

/*===========================================================================
 * Time sync Module Ends here*
 ===========================================================================*/