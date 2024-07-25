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
#include "project.h"
#include "stdlib.h"
#include "stdio.h"
#include "Boardconf.h"

#ifndef DPSC4_H
#define DPSC4_H

#define TIMEOUT 1000
#define ADC_MODE 0
#define DAC_MODE 1

#define BLE 1
#define USB 0
    
#define LOW   0
#define HIGH  1

#define LED_ON  0
#define LED_OFF 1

#define C1   11
#define C2   12
#define C3   13
#define C4   14
#define C5   15
#define C6   16
#define C7   17
#define C8   18
#define C9   19
#define C10 110
#define C11 111
#define C12 112
#define C13 113
#define C14 114
#define C15 115
#define C16 116

#define C_A  11
#define C_B  12
#define C_C  13
#define C_D  14
#define C_E  15
#define C_F  16
#define C_G  17
#define C_H  18
#define C_I  19
#define C_J 110
#define C_K 111
#define C_L 112
#define C_M 113
#define C_N 114
#define C_O 115
#define C_P 116

# define O1  0
# define O2  1
# define O3  2
# define O4  3
# define O5  4
# define O6  5
# define O7  6
# define O8  7
# define O9  8
# define O10 9
# define O11 10
# define O12 11
# define O13 12
# define O14 13
# define O15 14
# define O16 15

# define I1  0
# define I2  1
# define I3  2
# define I4  3
# define I5  4
# define I6  5
# define I7  6
# define I8  7
# define I9  8
# define I10 9
# define I11 10
# define I12 11
# define I13 12
# define I14 13
# define I15 14
# define I16 15

#define MISO 59
#define MOSI 58
#define SCLK 60

#define SCL 12
#define SDA 13

#define AMOSI 38
#define AMISO 37

#define ANALOG_IO 8
#define R_LED 64
#define G_LED 15
#define B_LED 6
    
uint8_t LOGON;


struct d_psc4{
    uint8 A_Dstate;
    uint32 A_D_period;
    struct boardconf b;
    uint8_t con_, sync_;
    struct {
        uint8 timer;// = 6;
        uint freq;// = 5000;
        uint res;//  = 12;
    }dac;
    struct {
        uint32_t device_id;
    }flash;
    struct{
        char ssid[100];
        char password[100];
        char mac[20];
        uint8 rfpower;
    }bleWiFi;
};

void dpsc4_Ledindicator(int8_t status);


void dpsc4_setmux(struct d_psc4 *, int );

void delay(int val);

void dpsc4_switchTo(uint8 val);

void dpsc4_SerialPortTest();

void dpsc4_analogWriteInit(struct d_psc4 *p);

void dpsc4_init(struct d_psc4 *p,uint8_t bno);

void dpsc4_digitalWrite(struct d_psc4 *p,uint8 bus_name, uint8 val);

void dpsc4_setFreq(struct d_psc4 *p, uint32 f);

void dpsc4_analogWrite(struct d_psc4 *p, float val);

float dpsc4_analogRead(struct d_psc4 *p);

//muxseq starts

CY_ISR (PWM5_muxseq_int_Handler);
        
void dpsc4_muxseq(float swtime, uint8_t len, uint8_t *seq,uint8_t sync);
//muxseq ends

//ble+wifi
int dpsc4_sendCmd(struct d_psc4 *p, char *cmd, char *rec_data, int len, unsigned long long timeout, uint8_t Delay);

int dpsc4_ble_mac(struct d_psc4 *p);

int dpsc4_ble_checkRFpower(struct d_psc4 *p);

int dpsc4_ble_setRFpower(struct d_psc4 *p, int val);

int dpsc4_ble_mode(struct d_psc4 *p);

int dpsc4_ble_status(struct d_psc4 *p);

int dpsc4_ble_OFF(struct d_psc4 *p);

int dpsc4_bleON_host(struct d_psc4 *p);

int dpsc4_bleON_slave(struct d_psc4 *p);

int dpsc4_ble_connect(struct d_psc4 *p, char *mac);

int dpsc4_ble_disconnect(struct d_psc4 *p);



int dpsc4_WiFi_mac(struct d_psc4 *p, char *mac);

int dpsc4_WiFi_status(struct d_psc4 *p);

int dpsc4_WiFi_OFF(struct d_psc4 *p);

int dpsc4_WiFiON_STA(struct d_psc4 *p);

int dpsc4_WiFiON_AP(struct d_psc4 *p);

int dpsc4_WiFiON_AP_STA(struct d_psc4 *p);

int dpsc4_WiFi_scan(struct d_psc4 *p, char *rec_data);

int dpsc4_WiFi_connect(struct d_psc4 *p, char* ssid, char* password);

int dpsc4_WiFi_disconnect(struct d_psc4 *p);

int dpsc4_WiFi_whoRu(struct d_psc4 *p, char *ssid);

int dpsc4_WiFi_sendData(struct d_psc4 *p, char *trans_type, char *opt, char *host, char *path, int data);

int dpsc4_WiFi_receiveData(struct d_psc4 *p, char *trans_type, char *opt, char *host, char *path, char *readData);

    
#endif
/* [] END OF FILE */
