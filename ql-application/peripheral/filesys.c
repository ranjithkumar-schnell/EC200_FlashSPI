
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_gpio.h"
#include "ql_pin_cfg.h"
#include "osi_api.h"
#include "ql_api_nw.h"
#include "ql_api_dev.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_api_rtc.h"
#include "quec_pin_index.h"
#include "uart_demo.h"
#include "ql_sdmmc.h"
#include "mqtt_thingsbrd.h"
#include "time_sync.h"


#define QL_SDMMC_PIN_DET            QUEC_PIN_DNAME_GPIO_15
#define QL_PIN_SDMMC_CMD            QUEC_PIN_DNAME_SDMMC1_CMD
#define QL_PIN_SDMMC_DATA_0         QUEC_PIN_DNAME_SDMMC1_DATA_0
#define QL_PIN_SDMMC_DATA_1         QUEC_PIN_DNAME_SDMMC1_DATA_1
#define QL_PIN_SDMMC_DATA_2         QUEC_PIN_DNAME_SDMMC1_DATA_2
#define QL_PIN_SDMMC_DATA_3         QUEC_PIN_DNAME_SDMMC1_DATA_3
#define QL_PIN_SDMMC_CLK            QUEC_PIN_DNAME_SDMMC1_CLK

#define QL_PIN_SDMMC_MODE_FUNC      0
#define QL_PIN_SDMMC_MODE_FUNC_GPIO 0
#define QL_FM_FAT32                     0x02
#define QL_SDMMC_CLK_FREQ               25000000
#define QL_SDMMC_BLOCK_NUM              10



char log_path_name[25]={'\0'};
char test_file[]="UFS:cred.txt";
int sd_load_status = 0;// use data from ql_sdmmc_mount_demo

char sd_buffer[2000]={'\0'};
char sd_buffer_rcvr[2000]={'\0'};
char sd_test_path_name[25]={'\0'};
char sd_path_name_RPC[25]={'\0'};
char sd_NnwPath_name[25]={'\0'};
char sd_NnwTBPath_name[25]={'\0'};
char sd_NnwSDMPath_name[25]={'\0'};
int NumberOFLinesinFile = 0;
char DataString[250] = {0}; 





uint8_t calculate_crc8(const char *data, size_t length) 
{
    uint8_t crc = 0x00;
    const uint8_t polynomial = 0x07;
    for (size_t i = 0; i < length; i++) 
    {
        crc ^= (uint8_t)data[i];
        for (int j = 0; j < 8; j++) 
        {
            crc = (uint8_t)((crc & 0x80) ? ((crc << 1) ^ polynomial) : (crc << 1));
        }
    }
    QL_MQTT_LOG("CRC-8: 0x%02X\n", crc);
    return crc;
}


void ql_sdmmc_pin_init(void)
{
	ql_pin_set_func(QL_PIN_SDMMC_CMD    , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
	ql_pin_set_func(QL_PIN_SDMMC_DATA_0 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
	ql_pin_set_func(QL_PIN_SDMMC_DATA_1 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
	ql_pin_set_func(QL_PIN_SDMMC_DATA_2 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
	ql_pin_set_func(QL_PIN_SDMMC_DATA_3 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
	ql_pin_set_func(QL_PIN_SDMMC_CLK    , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
}

ql_errcode_sdmmc_e ql_sdmmc_mount_demo(void)
{
	if(QL_SDMMC_SUCCESS != ql_sdmmc_mount())
	{
		QL_MQTT_LOG("SD CARD Mount failed");
        sd_load_status = 0;
        QL_MQTT_LOG("sd load status is:%d",sd_load_status);
		return QL_SDMMC_MOUNT_ERR;
	}
	else 
	{
		QL_MQTT_LOG("SD CARD Mount succeed");
        sd_load_status =1;
        QL_MQTT_LOG("sd load status is:%d",sd_load_status);
	}
	//ql_rtos_task_sleep_s(3);
	return QL_SDMMC_SUCCESS;
}


int64 ql_sdmmc_fs_test(char* str)
{
    int fd,err = 0 ;
    strcat (str,"\n");
    ql_sdmmc_mount_demo();
    QL_MQTT_LOG("sd load status is:%d",sd_load_status);
    sprintf(log_path_name, "SD:%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
    QL_MQTT_LOG("log_path_name is created for data logging as:%s",log_path_name);
    fd = ql_fopen(log_path_name,"ab+");
    err = ql_fwrite(str, strlen(str), 1, fd);  
	ql_fclose(fd);
     if(err < 0)
        {
          QL_MQTT_LOG("read file failed");
		  QL_MQTT_LOG("write file error :%d",err);
          ql_fclose(fd);
          return err;      
        }
     return err;           
}

int64 ql_sdmmc_fs_NetworkLog(char* str)
{   int fd, err = 0;
    ql_sdmmc_mount_demo();
    QL_MQTT_LOG("sd load status is:%d",sd_load_status);
    if(Connected_server() == 1)
    {
     sprintf(sd_test_path_name, "SD:TBNNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
     QL_MQTT_LOG("log_path_name is created for no network data logging as:%s",sd_test_path_name);
     fd = ql_fopen(sd_test_path_name,"ab+");
     err = ql_fwrite(str, strlen(str), 1, fd);  
	 ql_fclose(fd);
     if(err == 0)
     {
        strcpy(sd_NnwTBPath_name,sd_test_path_name);
        QL_MQTT_LOG("cpoied string for comparison is:%s",sd_NnwTBPath_name);
     }
    if(err < 0)
     {
       QL_MQTT_LOG("read file failed");
	   QL_MQTT_LOG("write file error :%d",err);
       ql_fclose(fd);
       return err;      
     }
    }
    if(Connected_server() == 2)
    {
     sprintf(sd_test_path_name, "SD:SDNNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
     QL_MQTT_LOG("log_path_name is created for no network data logging as:%s",sd_test_path_name);
     fd = ql_fopen(sd_test_path_name,"ab+");
     err = ql_fwrite(str, strlen(str), 1, fd);  
	 ql_fclose(fd);
      if(err == 0)
     {
        strcpy(sd_NnwSDMPath_name,sd_test_path_name);
        QL_MQTT_LOG("cpoied string for comparison is:%s",sd_NnwSDMPath_name);
     }
    if(err < 0)
     {
       QL_MQTT_LOG("read file failed");
	   QL_MQTT_LOG("write file error :%d",err);
       ql_fclose(fd);
       return err;      
     }
    }
    if(Connected_server() == 3)
    {
     sprintf(sd_test_path_name, "SD:NNW%d%d%d.txt",tm.tm_mday,tm.tm_mon,tm.tm_year);
     QL_MQTT_LOG("log_path_name is created for no network data logging as:%s",sd_test_path_name);
     fd = ql_fopen(sd_test_path_name,"ab+");
     err = ql_fwrite(str, strlen(str), 1, fd);  
	 ql_fclose(fd);
    if(err == 0)
     {
        strcpy(sd_NnwPath_name,sd_test_path_name);
        QL_MQTT_LOG("cpoied string for comparison is:%s",sd_NnwPath_name);
     }
    if(err < 0)
     {
       QL_MQTT_LOG("read file failed");
	   QL_MQTT_LOG("write file error :%d",err);
       ql_fclose(fd);
       return err;      
     }
    
    }
    if(Connected_server() == 0)
    {
     QL_MQTT_LOG("both servers up");
   }

  return err;
 }

void read_logged_files_fromRPC(char* sd_path_name_From_RPC)
{  
   //int Numval = 98;  // value received from RPC to print number of index
   int fd,err = 0 ;
   int lenghtofBuf;
   char sd_fullpath_name[25]={'\0'};
   sprintf(sd_fullpath_name,"SD:%s.txt",sd_path_name_From_RPC);
   QL_MQTT_LOG("log_path_name acessing date from RPc is:%s",sd_fullpath_name);
   fd = ql_fopen(sd_fullpath_name, "r");
   err = ql_fread(sd_buffer,2000,1, fd);
   QL_MQTT_LOG("The value of A from rpc is %d",numberOFLInes);
   if(err < 0 )
   {
    QL_MQTT_LOG("The SD card has no data to read");
   }
   if(err > 0 )
   {
     ql_fseek(fd,0,SEEK_SET);
     ql_fgets(sd_buffer,sizeof(sd_buffer),fd);
     lenghtofBuf = strlen(sd_buffer);
     QL_MQTT_LOG("The read content is :%s",sd_buffer);
     ql_rtos_queue_release(sdData,sizeof(sd_buffer),(uint8 *)&sd_buffer,0);
     QL_MQTT_LOG("string lengh of json data buffer befor sending is :%d",lenghtofBuf);
     ql_rtos_task_sleep_ms(5);
     if (numberOFLInes <= 5)
     {
       numberOFLInes = 450;
       QL_MQTT_LOG("Number of lines is default set value :%d", numberOFLInes);
     }
     QL_MQTT_LOG("Number of lines is : %d",numberOFLInes);
     for(int i=0;i<=(numberOFLInes-2);i++)  // val decides how many data can be published
        {
            ql_fseek(fd,0,SEEK_CUR);  // in place of 0 old variable was int Numval
            ql_fgets(sd_buffer,sizeof(sd_buffer),fd);
            lenghtofBuf = strlen(sd_buffer);
            ql_rtos_queue_release(sdData,sizeof(sd_buffer),(uint8 *)&sd_buffer,0);
            QL_MQTT_LOG("The read content is :%s",sd_buffer);
            QL_MQTT_LOG("string lengh of json data buffer befor sending is :%d",lenghtofBuf);
            ql_rtos_task_sleep_ms(5);
        }
    memset(sd_buffer, 0,sizeof(sd_buffer));
   }
    ql_fclose(fd);
}

void read_logged_files_toRetry(char* sd_path_name_retry)
{  
   int Numval = 650;  // value received from RPC to print number of index
   int fd,err = 0 ;
   int lenghtofBuf;
    QL_MQTT_LOG("log_path_name for  acessing data during recovery is:%s",sd_path_name_retry);
   fd = ql_fopen(sd_path_name_retry, "r");
   err = ql_fread(sd_buffer_rcvr,2000,1, fd);
   QL_MQTT_LOG("The value of A from rpc is %d",Numval);
   if(err < 0 )
   {
    QL_MQTT_LOG("The SD card has no data to read");
   }
   if(err > 0 )
   {
     ql_fseek(fd,0,SEEK_SET);
     ql_fgets(sd_buffer_rcvr,sizeof(sd_buffer_rcvr),fd);
     lenghtofBuf = strlen(sd_buffer_rcvr);
     QL_MQTT_LOG("The read content is :%s",sd_buffer_rcvr);
     ql_rtos_queue_release(recvr_Sd_Data,sizeof(sd_buffer_rcvr),(uint8 *)&sd_buffer_rcvr,0);
     QL_MQTT_LOG("string lengh of json data buffer befor sending is :%d",lenghtofBuf);
     ql_rtos_task_sleep_ms(5);
      if(lenghtofBuf >= 500)
      {
         NumberOFLinesinFile ++;
      }
     for(int i=1;i<=Numval;i++)  // val decides how many data can be published
        {
            ql_fseek(fd,0,SEEK_CUR);  // in place of 0 old variable was int Numval
            ql_fgets(sd_buffer_rcvr,sizeof(sd_buffer_rcvr),fd);
            lenghtofBuf = strlen(sd_buffer_rcvr);
            ql_rtos_queue_release(recvr_Sd_Data,sizeof(sd_buffer_rcvr),(uint8 *)&sd_buffer_rcvr,0);
            QL_MQTT_LOG("The read content is :%s",sd_buffer_rcvr);
            QL_MQTT_LOG("string lengh of json data buffer befor sending is :%d",lenghtofBuf);
            ql_rtos_task_sleep_ms(5);
            if(lenghtofBuf >= 500)
           {
             NumberOFLinesinFile ++;
            }
        }
    memset(sd_buffer_rcvr, 0,sizeof(sd_buffer_rcvr));
   }
     
     QL_MQTT_LOG("total line in the read file is :%d",NumberOFLinesinFile);
    ql_fclose(fd);
}
/*===========================================================================
 * Main entry function for setting Config values*
 ===========================================================================*/

void read_config_fileUFS()
{
      int fd,size_f,err = 0 ;
    fd = ql_fopen(test_file, "rb");
    size_f=ql_fsize(fd);
    QL_MQTT_LOG("cred.txt size is =%d",size_f);
    err = ql_fread(&snl_conf_read, sizeof(struct Schnell) + 1, 1, fd);
    ql_fclose(fd);
    if(err < 0)
    {
        QL_MQTT_LOG("read file failed :%d",err);
		QL_MQTT_LOG("read file error :%d",err);
        ql_fclose(fd);
                // return fd;           
    }

    QL_MQTT_LOG("read pub snl_conf_read.domain is %s", snl_conf_read.domain);
    QL_MQTT_LOG("read pub snl_conf_read.interval is %d", snl_conf_read.interval);
	QL_MQTT_LOG("read pub snl_conf_read.password is %s", snl_conf_read.password);
    QL_MQTT_LOG("read pub snl_conf_read.operator is %s", snl_conf_read.operator);
    QL_MQTT_LOG("read pub snl_conf_read.LiveLocation is %d", snl_conf_read.LiveLocation);
    QL_MQTT_LOG("read pub snl_conf_read.Latitude is %s", snl_conf_read.Latitude);
    QL_MQTT_LOG("read pub snl_conf_read.Longitude is %s", snl_conf_read.Longitude);
    QL_MQTT_LOG("read pub snl_conf_read.DayIndex is %d", snl_conf_read.DayIndex);
    QL_MQTT_LOG("read pub snl_conf_read.MonthIndex is %d", snl_conf_read.MonthIndex);
    QL_MQTT_LOG("read pub snl_conf_read.YearIndex is %d", snl_conf_read.YearIndex);
    QL_MQTT_LOG("read pub snl_conf_read.LookupTimeSync is %d", snl_conf_read.LookupTimeSync);
	strcpy(mq_client_pwd,snl_conf_read.password);
    strcpy(mq_url,snl_conf_read.domain);
    strcpy(mq_operator,snl_conf_read.operator);
    QL_MQTT_LOG("read pub snl_conf_read.operator is %s",mq_operator);
	pdata_int=snl_conf_read.interval;
	if(pdata_int==0) pdata_int =10;
 
 }

void write_config_fileUFS()
{
   
    int fd,err = 0 ;
    snl_conf_read.interval=pdata_int;   //all other values are set from RPC go to the section for more info
    // strcpy(snl_conf.domain,"peda1.schnelliot.in:1883");
            fd = ql_fopen(test_file, "wb");
             err = ql_fwrite(&snl_conf_read, sizeof(struct Schnell) + 1, 1, fd);
			ql_fclose(fd);
            if(err < 0)
            {
                QL_MQTT_LOG("read file failed");
				QL_MQTT_LOG("write file error :%d",err);
                ql_fclose(fd);
                        
            }
            
            QL_MQTT_LOG("write pub snl_conf_write.domain is %s", snl_conf_read.domain);
            QL_MQTT_LOG("write pub snl_conf_write.interval is %d", snl_conf_read.interval);
		    QL_MQTT_LOG("write pub snl_conf_write.interval is %s", snl_conf_read.password);
            QL_MQTT_LOG("write pub snl_conf_write.operator is %s", snl_conf_read.operator);
            strcpy(mq_url,snl_conf_read.domain);
            pdata_int=snl_conf_read.interval;
            strcpy(mq_operator,snl_conf_read.operator);
             QL_MQTT_LOG("write pub snl_conf_write.operator is %s",mq_operator);
        
}



void SpiFlashDataUploader(char *data, int data_size,char *DataString,int data_Stringsize) 
{
    char modemMetadata[150] = {0};
    uint8_t crc8 = 0;
    char crc_str[6] = {0};
    memset(DataString, 0, data_Stringsize); // Clear DataString buffer

    int ret = snprintf(modemMetadata, sizeof(modemMetadata),"%s,%d,%d,%d,%d,%s,%s,%s",current_dt,msgid_ev,d_index,sd_load_status,pdata_int,only_dt,cotp_v,potp_v);
       
    if (ret < 0 || ret >= (int)sizeof(modemMetadata)) 
        {
          QL_MQTT_LOG("Error: snprintf failed or output was truncated.\n");
            
        }
 
    size_t modemData_size = strlen(modemMetadata);
    size_t stripped_len = (data_size <= MAX_STRING_LENGTH) ? data_size - DELIMINATER_LEN : 0; //FRAME STRIPPED LENGHT IS 179 BYTES with zero indexing
    if ((stripped_len + modemData_size) < data_Stringsize) 
    {
        strncpy(DataString, &data[1], stripped_len);    //STRIPS THE FIRST AND LAST CHARACTERS $ AND #
        strncat(DataString, modemMetadata, modemData_size);
        QL_MQTT_LOG("DataString without Crc attached: %s\n", DataString);        // CONTAINS PACKED DATA STRING WITHOUT CRC

        crc8 = calculate_crc8(DataString, strlen(DataString));
        snprintf(crc_str, sizeof(crc_str), ",#%d", crc8);
        strncat(DataString, crc_str, sizeof(crc_str) - 1); // appending crc string to the main  packed data frame
        DataString[strlen(DataString) + 1] = '\0';   // Ensure null termination
        QL_MQTT_LOG("DataStringstrlen: %d\n", strlen(DataString));
        QL_MQTT_LOG("DataString with CRC added for validation: %s\n", DataString);   // CONTAINS tOTAL PACKED DATA STRING WITH CRC
        
        
    }
    else 
    {
      QL_MQTT_LOG("Error: Combined data exceeds buffer limit.\n");
        
    }
 
}

// caution segmentation fualt occured after addition of crc section which has been cleared after basic logics recheck this function
// this function is used to unpack the data from the packed data string and convert it to json format for sending to thingsboard/Sedem server
void SpiFlashData_Jsonpacker(const char *data)
 {
    const char Spi_Json_key[PAYLOAD_DATA_PARAMS][DATALENGHT]= {"PMAXFREQ1","PFREQLSP1","PFREQHSP1","PCNTRMODE1","PRUNST1","PREFFREQ1","POPFREQ1","POPI1","POPV1","POPKW1","PDC1V1","PDC1I1","PDCVOC1","PDKWH1","PTOTKWH1","POPFLW1","POPDWD1","POPTOTWD1","PMAXDCV1","PMAXDCI1","PMAXKW1","PMAXFLW1","PDHR1","PTOTHR1","ASN_11","TIMESTAMP","MSGID","INDEX","LOAD","STINTERVAL","DATE","POTP","COTP"};
    char  Spi_Json_Value[PAYLOAD_DATA_PARAMS][DATALENGHT] ;
    char PackedJsonData[1000]= {0};
    char TempJsonBuff[200]= {0};
    char *token;
    char *CrcToken;
    char *rest = (char *)data;
    int i =0;
    uint8_t crc8 = 0;
    char SPIString[250] = {0}; 
    uint8_t CrcStringLen = 0;

   /*validate the tokenised Raw data by crc calculation */
    strcpy(SPIString, data);
    QL_MQTT_LOG("SPIString: %s\n", SPIString); // CONTAINS PACKED Raw SPI DATA
    
    char *CrcRest = (char *)SPIString;
    char *saveptr; // Save pointer for strtok_r
    char *saveptrCRC;
    CrcToken = strtok_r(CrcRest, "#", &saveptrCRC); // get the crc8 value from the string
    CrcStringLen = strlen(CrcToken); // get the length of the string
    QL_MQTT_LOG(" Crc token string length is :%d", strlen(CrcToken));
    QL_MQTT_LOG("crc tokenized : %s\n", CrcToken); // CONTAINS PACKED Raw SPI DATA
    crc8 = calculate_crc8(SPIString, CrcStringLen - 1); // remove null, comma, and crc8 value from the string to calculate crc8 value
  
  /* Tokenise raw dataa to value pair for json packing*/

    memset(Spi_Json_Value, 0, sizeof(Spi_Json_Value)); // Clear valueString buffer
    token = strtok_r(rest, ",#", &saveptr);
        while (token != NULL) 
        {
            strcpy(Spi_Json_Value[i], token);
            QL_MQTT_LOG("valueString[%d]: %s\n", i, Spi_Json_Value[i]);
            i++;
            token = strtok_r(NULL, ",#", &saveptr);
        }

    QL_MQTT_LOG("Calculated crc8 value is :%d\n", crc8); // CONTAINS PACKED Raw SPI DATA
    QL_MQTT_LOG("crc8 value is :%s",Spi_Json_Value[E_CRC8]);
    if(atoi(Spi_Json_Value[E_CRC8]) == crc8) // check if the crc8 value is same as the calculated value
    {
        QL_MQTT_LOG("crc8 value is same as calculated value\n");
        //snprintf(PackedJsonData, sizeof(PackedJsonData), "{\"VD\":1,\"MAXINDEX\":200,\"IMEI\":\"%s\",\"INDEX\":%s,\"LOAD\":%s,\"STINTERVAL\":%s,\"DATE\":%s,\"POTP\":\"%s\",\"COTP\":\"%s\"", imei_no, Spi_Json_Value[E_INDEX], Spi_Json_Value[E_LOAD], Spi_Json_Value[E_STINTERVAL], Spi_Json_Value[E_DATE], Spi_Json_Value[E_POTP], Spi_Json_Value[E_COTP]); // used separately to format the data to int type as per sedem document payload list
        const char *json_format = "{\"VD\":1,\"MAXINDEX\":200,\"IMEI\":\"%s\",\"INDEX\":%s,\"LOAD\":%s,\"STINTERVAL\":%s,\"DATE\":%s,\"POTP\":\"%s\",\"COTP\":\"%s\"";
        const char *imei_s = imei_no;
        const char *index_s = Spi_Json_Value[E_INDEX];
        const char *load_s = Spi_Json_Value[E_LOAD];
        const char *stinterval_s = Spi_Json_Value[E_STINTERVAL];
        const char *date_s = Spi_Json_Value[E_DATE];
        const char *potp_s = Spi_Json_Value[E_POTP];
        const char *cotp_s = Spi_Json_Value[E_COTP];

    snprintf(PackedJsonData, sizeof(PackedJsonData), json_format, imei_s, index_s, load_s, stinterval_s, date_s, potp_s, cotp_s); // used separately to format the data to int type as per sedem document payload list
    for(int j = E_PMAXFREQ1; j <= E_INDEX ;j++)// till E_INDEX already packed so before e index Pack values to json format from the Received spi data
    {
        if(j== E_INDEX ) snprintf(TempJsonBuff, sizeof(TempJsonBuff), "}");
        else snprintf(TempJsonBuff, sizeof(TempJsonBuff), ",\"%s\":\"%s\"", Spi_Json_key[j], Spi_Json_Value[j]);
        strcat(PackedJsonData,TempJsonBuff);
        memset(TempJsonBuff, 0, sizeof(TempJsonBuff));
    }    
    QL_MQTT_LOG("PackedJsonData: %s\n", PackedJsonData);
    QL_MQTT_LOG(" lenght of PackedJsonData: %d\n", strlen(PackedJsonData));
    }
    else
    {
        QL_MQTT_LOG("crc8 value is not same as calculated value\n");
    }



}

/*===========================================================================
 * Filesys module ends here*
 ===========================================================================*/