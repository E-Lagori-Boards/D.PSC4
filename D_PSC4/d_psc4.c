/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


#include "d_psc4.h"

/// @brief Sets the RGB LED on the PSOC-4 module
/// @param status
/// 0 bit: RED LED
/// 1 bit: Green LED
/// 2 bit: Blue LED 
void dpsc4_Ledindicator(int8_t status){
    Pin_Red_Write((status & 0x1)?LED_ON:LED_OFF);
    Pin_Green_Write((status & 0x2)?LED_ON:LED_OFF);
    Pin_Blue_Write((status & 0x4)?LED_ON:LED_OFF);
}

/// @brief Set the channel of bidirectional Analog mux on the module.
/// @param p pointer to structure holding the data about the module
/// @param a channel number
void dpsc4_setmux(struct d_psc4 *p, int a){
    #ifdef CY_TCPWM_PWM5_muxseq_H
        PWM5_muxseq_Stop();
    #endif
    switch(a){
        case  I1:    S3_Write(0);   S2_Write(0);    S1_Write(0);    S0_Write(0);     break;
        case  I2:    S3_Write(0);   S2_Write(0);    S1_Write(0);    S0_Write(1);     break;
        case  I3:    S3_Write(0);   S2_Write(0);    S1_Write(1);    S0_Write(0);     break;
        case  I4:    S3_Write(0);   S2_Write(0);    S1_Write(1);    S0_Write(1);     break;
        case  I5:    S3_Write(0);   S2_Write(1);    S1_Write(0);    S0_Write(0);     break;
        case  I6:    S3_Write(0);   S2_Write(1);    S1_Write(0);    S0_Write(1);     break;
        case  I7:    S3_Write(0);   S2_Write(1);    S1_Write(1);    S0_Write(0);     break;
        case  I8:    S3_Write(0);   S2_Write(1);    S1_Write(1);    S0_Write(1);     break;
        case  I9:    S3_Write(1);   S2_Write(0);    S1_Write(0);    S0_Write(0);     break;
        case I10:    S3_Write(1);   S2_Write(0);    S1_Write(0);    S0_Write(1);     break;
        case I11:    S3_Write(1);   S2_Write(0);    S1_Write(1);    S0_Write(0);     break;
        case I12:    S3_Write(1);   S2_Write(0);    S1_Write(1);    S0_Write(1);     break;
        case I13:    S3_Write(1);   S2_Write(1);    S1_Write(0);    S0_Write(0);     break;
        case I14:    S3_Write(1);   S2_Write(1);    S1_Write(0);    S0_Write(1);     break;
        case I15:    S3_Write(1);   S2_Write(1);    S1_Write(1);    S0_Write(0);     break;
        case I16:    S3_Write(1);   S2_Write(1);    S1_Write(1);    S0_Write(1);     break;
    }
}

/// @brief Pause running the program for certain duration
/// @param val delay value in ms
void delay(int val){
    CyDelay(val);
}
/// @brief Switch between USB and BLE for serial port.
/// @param val USB - USB mode, BLE - BLE mode
void dpsc4_switchTo(uint8 val){
    uint16 timeout=10000;
    while(UART_SpiUartGetTxBufferSize() != 0 && timeout){timeout--;};
    UART_SpiUartClearTxBuffer();
    UART_SpiUartClearRxBuffer();
    CTRL_Write(val);
}

/// @brief Test serial port by loop back
void dpsc4_SerialPortTest(){
    dpsc4_switchTo(USB);
    char data;
    
    if(UART_SpiUartGetRxBufferSize()!=0ul){
            data = UART_SpiUartReadRxData();
            UART_SpiUartWriteTxData(data);
        }
}


/// @brief Initialise the module
/// @param p Pointer to structure holding the data about the module
/// @param bno board no (default 0x81)
void dpsc4_init(struct d_psc4* p,uint8_t bno){
    p->A_Dstate = ADC_MODE;
    p->b.reserved = 0B10000000;
    p->b.brdtype = 0B11100110;
    p->b.brdno = bno;
    p->con_ = 1;
    SPI_EXTMEM_Start();
    Serial_FLASH_Init();
    UART_Start();
    I2CM_Start();
    SPIM_Start();
    UART_AMOSI_Start();
    Counter_micros_Start();
    #ifdef CY_TCPWM_PWM7_Mux_H
        PWM7_Mux_Start();
        p->A_D_period = PWM7_Mux_ReadPeriod();
    #endif
    #ifdef CY_ADC_SAR_SEQ_ADC_H 
        ADC_Start();
        ADC_StartConvert();
    #endif 
//    dpsc4_analogWriteInit(p);
}

#ifdef CY_TCPWM_PWM7_Mux_H
/// @brief Generate clock on G port of the digital bus
/// @param p Pointer to structure holding the data about the module
/// @param f Frequency of the generated square wave
void dpsc4_setFreq(struct d_psc4 *p, uint32 f){
    PWM7_Mux_WritePeriod(48000000/f);
    p->A_D_period = PWM7_Mux_ReadPeriod();
    
}

/// @brief Intitalise all the channels to 0 
/// @param p Pointer to structure holding the data about the module
void dpsc4_analogWriteInit(struct d_psc4 *p){
    for(int i=0;i<16;i++){
        dpsc4_setmux(p, i);
        ADC_DAC_Write(0);
        delay(100);
    }
}

/// @brief Output voltage of the DAC
/// @param p Pointer to structure holding the data about the module
/// @param val Voltage to be generated (0 - 3.3v)
void dpsc4_analogWrite(struct d_psc4 *p, float val){
    if (p->A_Dstate == ADC_MODE){
        PWM7_Mux_Start();
        p->A_Dstate = DAC_MODE;
        ADC_DAC_SetDriveMode(ADC_DAC_DM_STRONG);
    }
    
    val = (val<0)?0.1:val;
    val = (val>3.3)?(3.3):val;
    PWM7_Mux_WriteCompare((int)(p->A_D_period*val/3.3));
}
#endif

#ifdef ADC
/// @brief Read ADC value
/// @param p Pointer to structure holding the data about the module
/// @return Voltage measured by the ADC
float dpsc4_analogRead(struct d_psc4 *p){
    if (p->A_Dstate == DAC_MODE){
        PWM7_Mux_Stop();
        p->A_Dstate = ADC_MODE;
        ADC_DAC_SetDriveMode(ADC_DAC_DM_DIG_HIZ);
        CyDelay(2);
    }
//    PWM7_Mux_Stop();
    ADC_DAC_SetDriveMode(ADC_DAC_DM_DIG_HIZ);
    float sample,volt,result=0; 
//    while(1){
//        if(0u != ADC_IsEndConversion(ADC_RETURN_STATUS)){
//                ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
                //result= (ADC_GetResult16(0)*5)/2048; 
                sample = ADC_GetResult16(0);
                volt = ADC_CountsTo_Volts(0, sample);
                volt = (volt<0)?0:volt;
                return volt;
//            }
//    }
}
#endif

#ifdef PWM5_muxseq
//muxseq starts
uint32 swtime_store;
uint8_t len_store;
uint8_t seq_store[16];
uint8_t muxstate;
uint8_t seq_cnt=0;

CY_ISR (PWM5_muxseq_int_Handler){
    
    S0_Write(seq_store[seq_cnt]>>0 & 0b00000001);
    S1_Write(seq_store[seq_cnt]>>1 & 0b00000001);
    S2_Write(seq_store[seq_cnt]>>2 & 0b00000001);
    S3_Write(seq_store[seq_cnt]>>3 & 0b00000001);
    muxstate = seq_store[seq_cnt];
    PWM5_muxseq_ClearInterrupt(PWM5_muxseq_INTR_MASK_TC);
    seq_cnt = (seq_cnt+1)%(len_store);
//    if (seq_cnt==len_int){Timer_Stop();seq_cnt=0;}
}

/// @brief Set automamatic Mux sequence
/// @param swtime time between switches
/// @param len Length of the sequence between 1 - 16 
/// @param seq Array of sequence
/// @param sync activate or de-activate sync pulse generation on channel H or C8 of the bus
void dpsc4_muxseq(float swtime, uint8_t len, uint8_t *seq,uint8_t sync){
    
    for(int i=0; i<16; i++){
        seq_store[i] = seq[i];
    }
    
    swtime_store = (uint32)(swtime);
    len_store = len;
    len_store = (len>15)?15:len;
    for (int i=0; i<len; i++)
        seq[i] = (seq[i]>15)?15:seq[i];
    PWM5_muxseq_WritePeriod(swtime_store);
    PWM5_muxseq_Start();
    PWM5_muxseq_int_StartEx(PWM5_muxseq_int_Handler);
}
#endif
//muxseq ends

//ble+wifi

/// @brief Sends UART command to BLE/WIFI module
/// @param p Pointer to structure holding the data about the module
/// @param cmd array of command to be sent
/// @param rec_data Received data array
/// @param len length of received data array
/// @param timeout timeout
/// @param Delay Activate optional delay typically 0
/// @return 
int dpsc4_sendCmd(struct d_psc4 *p, char *cmd, char *rec_data, int len, unsigned long long timeout, uint8_t Delay){

    //dpsc4_switchTo(BLE);
    
    int i=0, temp=0;
    char *ptr = rec_data;
    unsigned long long to = timeout;
    UART_UartPutString(cmd);
    while(UART_SpiUartGetTxBufferSize()){};
    
    if(Delay) delay(1000);
    
    for(;;)
    {
        to--;
        if(!to) {temp = 0; break;}
       
        if((UART_SpiUartGetRxBufferSize() != 0) && i<len)
        {
            rec_data[i] = UART_SpiUartReadRxData();

            if ((i>4) && (strncmp(ptr-5, "\r\nOK\r\n",6)==0))     {temp = i-6; break;}
            if ((i>7) && (strncmp(ptr-8, "\r\nERROR\r\n",9)==0))  {temp =  -1; break;}
          
            i++; ptr++;
        }
    }
    
    //dpsc4_switchTo(USB);

    return temp;
}

/// @brief Collect the mac address
/// @param p Pointer to structure holding the data about the module
/// @return Returns Mac address
int dpsc4_ble_mac(struct d_psc4 *p)
{
    int8_t result=0, j=0,i=0;
    char cmd[20] = "AT+BLEMAC?\r\n";
    char rec_data[65];
    //char *ptr = rec_data;
    char det = 0;

//    printf("Before sendCmd\n");
    result = dpsc4_sendCmd(p, cmd, rec_data, 65, 10000, 0);
//    printf("After sendCmd\n");
    j = 0;
    if(result>0)
    {
        for (i = 0; i<=result; i++){
            if (rec_data[i] == ':') {det = 1; continue;}
            if (det == 1) {p->bleWiFi.mac[j] = rec_data[i]; j++;} 
            if (rec_data[i] == 0) break;
        }
        rec_data[j] = 0;
    }
    return j;
}

/// @brief Returns the set RF power
/// @param p Pointer to structure holding the data about the module
/// @return RF power setting
int dpsc4_ble_checkRFpower(struct d_psc4 *p)
{
    int8_t result;
    char rec_data[60],*ptr;
    char cmd[18] = "AT+BLERFPWR?\r\n";

    delay(1000);
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 60, 100000, 0);
    ptr = &rec_data[result];
    if (result>=0)
    {
        while(*ptr != ':')  ptr--;
        ptr++;
        p->bleWiFi.rfpower = atof(ptr);
    }
    return result;
}

/// @brief Sets the RF power
/// @param p Pointer to structure holding the data about the module
/// @param val RF power setting
/// @return Set RF power
int dpsc4_ble_setRFpower(struct d_psc4 *p, int val)
{        
    char rec_data[60];
    int result;
    char buffer[20], cmd[15] = "AT+BLERFPWR=";

    if(val<0) return 0;

    sprintf(buffer, "%s%d\r\n", cmd, val);
    
    result = dpsc4_sendCmd(p, buffer, rec_data, 60, 1000000, 0);
    
    if(result >= 0) return 1;
    return 0;
}

/// @brief Returns the BLE mode
/// @param p Pointer to structure holding the data about the module
/// @return 9: BLE is OFF. 0: BLE is configured in Slave Mode. 1: BLE is configured in Host Mode
int dpsc4_ble_mode(struct d_psc4 *p)
{    
    uint8_t result;
    char rec_data[40];//,*ptr;
    char cmd[15] = "AT+BLEMODE?\r\n";

    //UART_UartPutString("9: BLE is OFF.\n0: BLE is configured in Slave Mode.\n1: BLE is configured in Host Mode.\n\n");
    result = dpsc4_sendCmd(p, cmd, rec_data, 40, 100000, 0);
    return(rec_data[result]-48);
}

/// @brief Returns BLE status
/// @param p Pointer to structure holding the data about the module
/// @return 0 - not connected, 1 connected
int dpsc4_ble_status(struct d_psc4 *p)
{    
    uint8_t result;//,det = 0,stat;
    char rec_data[40];//,*ptr;
    char cmd[15] = "AT+BLESTATE?\r\n";

    result = dpsc4_sendCmd(p, cmd, rec_data, 40, 100000, 0);
    return(rec_data[result]-48);
        
//    if (result>=0)
//    {
//        while(*ptr != ':')  ptr++;
//        ptr++;
//        return (atoi(ptr));
//    }
}

/// @brief Turns off BLE
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, 1 - Fail
int dpsc4_ble_OFF(struct d_psc4 *p)
{    
    int status;
    status = dpsc4_ble_mode(p);
    if(status == 9) return 9;
    
    char rec_data[25];//,*ptr;
    int result;
    char cmd[18] = "AT+BLEMODE=9\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 100000, 0);
    return(rec_data[result]-48);
}

/// @brief Turn echo on/off from BLE module
/// @param p Pointer to structure holding the data about the module
/// @param status 0 -  Echo off, 1 - Echo on
/// @return 0 - Success, 1 - Fail
int dpsc4_echo(struct d_psc4 *p, uint8 status)
{    
    uint8_t result;//,det = 0,stat;
    char rec_data[40];//,*ptr;

    if (status)
        result = dpsc4_sendCmd(p, "ATE1\r\n", rec_data, 40, 100000, 0);
    else
        result = dpsc4_sendCmd(p, "ATE0\r\n", rec_data, 40, 100000, 0);
    return(rec_data[result]-48);
}

/// @brief  Turn on BLE in host mode
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, 1 - Failure
int dpsc4_bleON_host(struct d_psc4 *p)
{
    int status;
    status = dpsc4_ble_mode(p);
    if (status == 1) return 1;
    
    char rec_data[25],*ptr;
    int result;
    char cmd[18] = "AT+BLEMODE=1\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 100, 1000000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != '=')  ptr++;
        ptr++;
        return (atoi(ptr));
    }
    return -1;
}

/// @brief Turn on in BLE in slave mode
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, 1 - Failure
int dpsc4_bleON_slave(struct d_psc4 *p)
{    
    int status;
    status = dpsc4_ble_mode(p);
    if (status == 0) return 0;
    
    char rec_data[25];//,*ptr;
    int result;
    char cmd[18] = "AT+BLEMODE=0\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 1000000, 0);
    return (result);
}

/// @brief Connects to BLE
/// @param p Pointer to structure holding the data about the module
/// @param mac MAC address of the device to connect 
/// @return 0 - Success, -1 - Failure
int dpsc4_ble_connect(struct d_psc4 *p, char *mac){
    int result;
    char rec_data[100];
    char buffer[40];
    
    sprintf(buffer, "AT+BLECONNECT=%s\r\n", mac);
    result = dpsc4_sendCmd(p, buffer, rec_data, 100, 1000000, 0);
    if(result >= 0) return 0;
    return -1;
}

/// @brief Dissconnects from BLE
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, -1 - Failure
int dpsc4_ble_disconnect(struct d_psc4 *p)
{
    int result;
    char rec_data[10];
    char cmd1[5] = "+++";
    char cmd2[18] = "AT+BLEDISCON\r\n";
    
    dpsc4_sendCmd(p, cmd1, rec_data, 10, 100000, 0);
    result = dpsc4_sendCmd(p, cmd2, rec_data, 10, 100000, 0);
    
    if(result) return 0;
    return -1;
}





/// @brief Gets the WIFI MAC address
/// @param p Pointer to structure holding the data about the module
/// @param mac pointer to MAC address
/// @return 0 - Success, 1 - Failure
int dpsc4_WiFi_mac(struct d_psc4 *p, char *mac)
{
    char rec_data[60],*ptr;
    int result, j=0;
    char cmd[25] = "AT+CIPSTAMAC_DEF?\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 60, 100000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != ':')  ptr++;
        ptr++;

        while(*ptr != '\r')
        {
          mac[j] = *ptr;
          j++; ptr++;
        }
    }
    if(result>=0)  return 0;
    return -1;
}

/// @brief Returns wifi status
/// @param p Pointer to structure holding the data about the module
/// @return 0 - SUccess, 1 - Failure
int dpsc4_WiFi_status(struct d_psc4 *p)
{    
    uint8_t result;
    char rec_data[25],*ptr;
    char cmd[15] = "AT+WMODE?\r\n";

    UART_UartPutString("0: WiFi is OFF.\n1: WiFi is configured as STA.\n2: WiFi is configured AP.\n3: WiFi is configured AP+STA.\n\n");
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 100000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != ':')  ptr++;
        ptr++;
        return (atoi(ptr));
    }
    return -1;
}

/// @brief Turn off Wifi
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, 1 - Failure
int dpsc4_WiFi_OFF(struct d_psc4 *p)
{    
    char rec_data[30];
    int result;
    char cmdRst[10] = "AT+RST\r\n";
    char cmdWoff[18] = "AT+WMODE=0,0\r\n";
    
    result = dpsc4_sendCmd(p, cmdRst, rec_data, 30, 10000, 0);
    delay(800);
    UART_SpiUartClearRxBuffer();
    result = dpsc4_sendCmd(p, cmdWoff, rec_data, 30, 100000, 0);
    
    if(result >= 0)  return 0;
    return -1;    
}

/// @brief Turn on Wifi
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFiON_STA(struct d_psc4 *p)
{    
    char rec_data[25],*ptr;
    int result;
    char cmd[18] = "AT+WMODE=1,0\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 100000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != ',')  ptr++;
        ptr--;
        return (atoi(ptr));
    }
    return -1;   
}

/// @brief Turns on Wifi Access point
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFiON_AP(struct d_psc4 *p)
{    
    char rec_data[25],*ptr;
    int result;
    char cmd[18] = "AT+WMODE=2,0\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 100000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != ',')  ptr++;
        ptr--;
        return (atoi(ptr));
    }
    return -1;   
}

/// @brief Wifi on Status
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFiON_AP_STA(struct d_psc4 *p)
{    
    char rec_data[25],*ptr;
    int result;
    char cmd[18] = "AT+WMODE=3,0\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 25, 100000, 0);
    ptr = rec_data;
    if (result>=0)
    {
        while(*ptr != ',')  ptr++;
        ptr--;
        return (atoi(ptr));
    }
    return -1;   
}

/// @brief Scan for wifi connections
/// @param p Pointer to structure holding the data about the module
/// @param rec_data pointer to array of available wifi connections
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_scan(struct d_psc4 *p, char *rec_data)
{
    int result, i=0;
    char *ptr;
    char cmd[12] = "AT+WSCAN\r\n";

    UART_UartPutString("Scanning...");
    result = dpsc4_sendCmd(p, cmd, rec_data, 1000, 80000000, 0);
    ptr = rec_data;

    while(*ptr != '+')  ptr++;
    while(strncmp(ptr, "\r\nOK\r\n", 6) != 0)
    {
      rec_data[i] = *ptr;
      i++; ptr++;
    }
    rec_data[i] = 0;
    UART_UartPutString("Scanning Complete.");
    
    if(result >= 0)  return 0;
    return -1;
}

/// @brief Connect to wifi
/// @param p Pointer to structure holding the data about the module
/// @param ssid Wifi user name
/// @param password Wifi password
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_connect(struct d_psc4 *p, char* ssid, char* password)
{
    char buffer[100],rec_data[200];
    int result;

    UART_UartPutString("Trying to connect with ");
    UART_UartPutString(ssid);UART_UartPutString("...\n");
    sprintf(buffer, "AT+WJAP=%s,%s\r\n", ssid,  password);
    result = dpsc4_sendCmd(p, buffer, rec_data, 200, 100000000, 0);
    if(result >= 0)  {UART_UartPutString("Connected\n"); return 0;}
    UART_UartPutString("Connection failed\n"); return -1;
}

/// @brief Disconnect wifi connection
/// @param p Pointer to structure holding the data about the module
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_disconnect(struct d_psc4 *p)
{
    int result;
    char rec_data[200];
    char cmd[10] = "AT+RST\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 200, 1000000, 0);
    if(result >= 0)  return 0;
    return -1;
}

/// @brief SSID of the connected AP
/// @param p Pointer to structure holding the data about the module
/// @param ssid Pointer to the string holding the SSID
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_whoRu(struct d_psc4 *p, char *ssid)
{
    int result, i=0;
    char rec_data[200], *ptr;
    char cmd[13] = "AT+WJAP?\r\n";
    
    result = dpsc4_sendCmd(p, cmd, rec_data, 200, 1000000, 0);
    ptr = rec_data;

    if(result >= 0)
    {
      while(*ptr != ',')  ptr++;
      ptr++;
      
      while(*ptr != ',')
      {
        ssid[i] = *ptr;
        i++; ptr++;
      }
      ssid[i] = 0;
    }
    
    if(result >= 0)  return 0;
    return -1;
}

/// @brief Send data via Wifi connection
/// @param p Pointer to structure holding the data about the module
/// @param trans_type Pointer to Transaction type HTTP or HTTPS
/// @param opt Pointer to type of operation - GET ot PUT
/// @param host Pointer to host name
/// @param path Pointer ot path
/// @param data data value
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_sendData(struct d_psc4 *p, char *trans_type, char *opt, char *host, char *path, int data)
{    
    int type, optype, port, result;
    char buffer[200], rec_data[200];
    
    if((strcmp(trans_type,"HTTP")==0)  ||  (strcmp(trans_type,"http")==0)) {type = 1; port = 80 ;}
    if((strcmp(trans_type,"HTTPS")==0) || (strcmp(trans_type,"https")==0)) {type = 2; port = 443;}
    if((strcmp(opt,"GET")==0)  ||  (strcmp(opt,"get")==0)) optype = 2;
    if((strcmp(opt,"POST")==0) || (strcmp(opt,"post")==0)) optype = 3;
    
    sprintf(buffer, "AT+HTTTPCLIENTLINE=%d,%d,application/json,%s,%d,%s,%d\r\n", type, optype, host, port, path, data);
    result = dpsc4_sendCmd(p, buffer, rec_data, 200, 100000000, 0);
    
    if(result>=0)  return 0;
    return -1;
}

/// @brief Receive data via Wifi connection
/// @param p Pointer to structure holding the data about the module
/// @param opt Pointer to type of operation - GET ot PUT
/// @param host Pointer to host name
/// @param path Pointer ot path
/// @param readdata data value
/// @return 0 - Success, -1 - Failure
int dpsc4_WiFi_receiveData(struct d_psc4 *p, char *trans_type, char *opt, char *host, char *path, char *readData)
{    
    int type, optype, port, result, i=0, j=0;
    char buffer[200], rec_data[500];
    
    if((strcmp(trans_type,"HTTP")==0)  ||  (strcmp(trans_type,"http")==0)) {type = 1; port = 80 ;}
    if((strcmp(trans_type,"HTTPS")==0) || (strcmp(trans_type,"https")==0)) {type = 2; port = 443;}
    if((strcmp(opt,"GET")==0)  ||  (strcmp(opt,"get")==0)) optype = 2;
    if((strcmp(opt,"POST")==0) || (strcmp(opt,"post")==0)) optype = 3;
    
    sprintf(buffer, "AT+HTTTPCLIENTLINE=%d,%d,application/json,%s,%d,%s\r\n", type, optype, host, port, path);
    result = dpsc4_sendCmd(p, buffer, rec_data, 500, 10000000, 0);

    while(rec_data[i] != '{')  i++;
    while(rec_data[i] != '\n')
    {
      readData[j] = rec_data[i];
      if(rec_data[i] == ',')
      {
        j++;
        readData[j] = '\n';
      }
      i++; j++;
    }
    while(readData[j] != '}')  j--;
    readData[j+1] = 0;

    UART_UartPutString("Getting data. Please wait...");
    delay(40000);
    UART_UartPutString("Just a bit longer...\n");
    delay(10000);
    
    if(readData[j] == '}')  return 0;
    return -1;
}
/* [] END OF FILE */
