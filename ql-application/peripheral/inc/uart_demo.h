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
 * Macros
 ===========================================================================*/
 #define INVERTER_DATA_PARAMS 25
 #define DATALENGHT           25
 #define LOGS_PERDAY          100
 #define TOTAL_DATA_LOG_PARAM 30
 #define PAYLOAD_DATA_PARAMS  35
 #define MAX_STRING_LENGTH    185
 #define DELIMINATER_LEN      2
 #define TOTAL_STORED_PAYLOAD 33 // INDEXING STARTS AT 1

/*===========================================================================
 * Enumeration Definition
 ===========================================================================*/
 typedef enum
 {
	 E_PMAXFREQ1 = 0,
	 E_PFREQLSP1,
	 E_PFREQHSP1,
	 E_PCNTRMODE1,
	 E_PRUNST1,
	 E_PREFFREQ1,
	 E_POPFREQ1,
	 E_POPI1,
	 E_POPV1,
	 E_POPKW1,
	 E_PDC1V1,
	 E_PDC1I1,
	 E_PDCVOC1,
	 E_PDKWH1,
	 E_PTOTKWH1,
	 E_POPFLW1,
	 E_POPDWD1,
	 E_POPTOTWD1,
	 E_PMAXDCV1,
	 E_PMAXDCI1,
	 E_PMAXKW1,
	 E_PMAXFLW1,
	 E_PDHR1,
	 E_PTOTHR1,
	 E_ASN_11,
	 E_TIMESTAMP,
	 E_MSGID,
	 E_INDEX,
	 E_LOAD,
	 E_STINTERVAL,
	 E_DATE,
	 E_POTP,
	 E_COTP,
	 E_CRC8
 } E_Json_keyIndex;

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
	char Latitude[20];
	char Longitude[20];
	uint8_t LiveLocation;
	uint8_t LookupTimeSync;
	uint8_t DayIndex;
	uint8_t MonthIndex;
	uint16_t YearIndex;
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
extern char json_data_buf[1000];
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
extern char DataString[250];
/*========================================================================
		 *	function Prototypes*
*========================================================================*/
void uart_data_send(unsigned char* data);
int64 ql_sdmmc_fs_test(char* str);
int64 ql_sdmmc_fs_NetworkLog(char* str);
void SpiFlashDataUploader(char *data, int data_size,char *data2,int data_Stringsize);
void SpiFlashData_Jsonpacker(const char *data);
#ifdef __cplusplus
} /*"C" */
#endif

#endif /* UART_DEMO_H */


