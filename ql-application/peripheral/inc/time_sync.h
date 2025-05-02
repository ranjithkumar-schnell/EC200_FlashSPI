/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _TIMESYNC_H
#define _TIMESYNC_H

#include "time_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/


/*===========================================================================
 * Functions declaration
 ===========================================================================*/

extern char current_dt[25];
extern char only_dt[10];
extern char only_tm[10];
extern char dt_time_CRI[25];
extern int sim_flag;
extern unsigned char csq;//
extern uint8_t sim_id;//
extern int Rssi;

typedef struct
{
	int hour;
	int min;
	int sec;
	int date;
	int month;
	int year;
	int time_zone;
}nitz_time_v;

nitz_time_v time_z;


/*========================================================================
		 *	function Prototypes*
*========================================================================*/


void set_network_time(void);
void get_time(void);
void get_network_time(void);
int sim_status(void);
unsigned char signal_quality(void);
void set_gps_time(void);
int Timesync(void);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _TIMESYNC_H */


