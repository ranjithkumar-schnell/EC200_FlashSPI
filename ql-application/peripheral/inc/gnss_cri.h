/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _GNSSCRI_H
#define _GNSSCRI_H

#include "ql_gnss.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct gnss_date_lat_lon_sat 
{
	char *latlong_urt;
	char *lat;
	char *lon;
	char *satlites;
} lat_lon_sat_dttm_t;

 typedef struct gnss_time {
	int tm_sec;	 // seconds [0,59]
	int tm_min;	 // minutes [0,59]
	int tm_hour; // hour [0,23]
	int tm_mday; // day of month [1,31]
	int tm_mon;	 // month of year [1,12]
	int tm_year; // year [2000-2100] 
	
}gnss_timer_t;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
lat_lon_sat_dttm_t gps_strctdata;//gps lat, long ,sattelite struct used in json pack telemetery to thingsboard
gnss_timer_t g_dt_tm;
extern ql_event_t event;
void ql_gnss_app_init(void);
nmea_type nmea_get_type(const char *sentence);
struct nmea_s* nmea_parse(char *sentence, int length, int check_checksum);
int nmea_value_update(struct nmea_s *nmea, ql_gnss_data_t *gps_data);
char *strptime(const char *buf, const char *fmt, struct tm *tm);
int ql_gnss_device_info_request(void);
int  ql_gnss_assist_data_delete(unsigned int reset_type);
extern char O_lat[15];
extern char O_lon[15];
extern char O_sat[15];
extern ql_task_t gnss_task;
extern char valueString[25][15];

extern bool gpsloc;//gps lock flag


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _GNSSDEMO_H */


