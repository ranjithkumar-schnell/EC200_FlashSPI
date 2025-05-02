/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef UART_DEMO_H
#define UART_DEMO_H


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Enumeration Definition
 ===========================================================================*/


/*===========================================================================
 * Struct
 ===========================================================================*/

typedef struct gnss_thrd_data 
{
	char *latlong_urt;
	char *lat;
	char *lon;
	char *satlites;
} gnssPositionData_t;

struct Schnell {
	int interval;
    char domain[100];
	char password[20];
	char operator[20];
};
struct Schnell snl_conf,snl_conf_read;
/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_uart_app_init(void);


gnssPositionData_t gnnsPosDat;
extern ql_event_t event_uart;
extern ql_task_t uart_task;
extern char imei_no[20];
extern char json_data_buf[2000];
extern uint8_t sim_id;
extern char version[15];
extern unsigned int d_index;
extern char imei_no1[20];
extern char sd_buffer[2000];
extern char sd_buffer_rcvr[2000];
extern char sd_test_path_name[25];
extern char sd_path_name_RPC[25];
extern char sd_NnwPath_name[25];
extern char sd_NnwTBPath_name[25];
extern char sd_NnwSDMPath_name[25];
extern int NumberOFLinesinFile;
/*========================================================================
		 *	function Prototypes*
*========================================================================*/
void uart_data_send(unsigned char* data);
int64 ql_sdmmc_fs_test(char* str);
int64 ql_sdmmc_fs_NetworkLog(char* str);
#ifdef __cplusplus
} /*"C" */
#endif

#endif /* UART_DEMO_H */


