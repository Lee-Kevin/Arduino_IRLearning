/*
 * Copyright (c) 2015 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Jiankai Li 
 * Modified Time: Aug 2015
 * Description: Must connect the IR send pins to D13 for Leonardo,D3 for Arduino Uno. Connect IR Reciver to D5
 * 
 * This is a demo that can learn IR - Decode according to your remote controller and save the IR - Code to EEPROM
 * 
 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <IRSendRev.h>
#include <EEPROM.h>

#define BIT_LEN         0
#define BIT_START_H     1
#define BIT_START_L     2
#define BIT_DATA_H      3
#define BIT_DATA_L      4
#define BIT_DATA_LEN    5
#define BIT_DATA        6
/*
struct sCommandAddr {
    uint8_t Command1;
    uint8_t Command2;
    uint8_t Command3;
    uint8_t Command3;
};
*/

uint8_t CommandAddr[20];

struct sIRdata {
    uint8_t BIT_LEN_D;
    uint8_t BIT_START_H_D;
    uint8_t BIT_START_L_D;
    uint8_t BIT_DATA_H_D;
    uint8_t BIT_DATA_L_D;
    uint8_t BIT_DATA_LEN_D;
    uint8_t *pData;
};
typedef struct sIRdata tsIRdata;
typedef tsIRdata     *ptsIRData;

union uIRData {
    uint8_t ucdata[7];
    tsIRdata IRdata;
    
};
typedef union uIRData tuIRdata;
typedef tuIRdata    * ptuIRData;
tuIRdata   myuIRData;
ptuIRData  puIRData = &myuIRData;


const int ir_freq = 38;                 // 38k

unsigned char dtaSend[20];

#define debug 1

uint8_t Readdata[20];

void dtaInit()
{
/*
    dtaSend[BIT_LEN]        = 11;     // all data that needs to be sent
    dtaSend[BIT_START_H]    = 179;      // the logic high duration of "Start"
    dtaSend[BIT_START_L]    = 90;     // the logic low duration of "Start"
    dtaSend[BIT_DATA_H]     = 11;     // the logic "long" duration in the communication
    dtaSend[BIT_DATA_L]     = 33;     // the logic "short" duration in the communication
    
    dtaSend[BIT_DATA_LEN]   = 6;      // Number of data which will sent. If the number is other, you should increase or reduce dtaSend[BIT_DATA+x].
    
    dtaSend[BIT_DATA+0]     = 193;      // data that will sent
    dtaSend[BIT_DATA+1]     = 96;
    dtaSend[BIT_DATA+2]     = 224;
    dtaSend[BIT_DATA+3]     = 67;
    dtaSend[BIT_DATA+4]     = 0;
    dtaSend[BIT_DATA+5]     = 0;*/

    puIRData->IRdata.BIT_LEN_D      = 11;
    puIRData->IRdata.BIT_START_H_D  = 179;
    puIRData->IRdata.BIT_START_L_D  = 90;
    puIRData->IRdata.BIT_DATA_H_D   = 11;
    puIRData->IRdata.BIT_DATA_L_D   = 33;
    puIRData->IRdata.BIT_DATA_LEN_D = 6;
    
    puIRData->IRdata.pData[0]       = 193;
    puIRData->IRdata.pData[1]       = 96;
    puIRData->IRdata.pData[2]       = 224;
    puIRData->IRdata.pData[3]       = 67;
    puIRData->IRdata.pData[4]       = 0;
    puIRData->IRdata.pData[5]       = 0; 

}

void setup()
{
    dtaInit();

    #if debug
    Serial.begin(9600);
    Serial.println("IR Learning remote controller");
    
    Serial.println(EEPROM.length());
    
    for(int i=0; i<7; i++) {
        Serial.println(puIRData->ucdata[i],DEC);
    }
    Serial.println("-------------------------");
    #endif
    CommandAddr[0]  = 21;
    saveCommand(CommandAddr[0],puIRData);
    readCommand(CommandAddr[0],Readdata);
}

void loop()
{
    
//    IR.Send(dtaSend, 38);
    
    #if debug
    Serial.println("-------Group data----------");
    for(int i=0; i<20; i++) {
        Serial.print(Readdata[i],DEC);
    }
    #endif
    delay(2000);
}

void saveCommand(uint8_t addr, ptuIRData puIRData) {
    uint8_t datalen = puIRData->IRdata.BIT_LEN_D + 1;
    Serial.print("datalen:");
    Serial.print(datalen,DEC);
    Serial.println(" ");
    
    uint8_t data[20];
    memcpy(data,puIRData->ucdata,6);
    memcpy(data+6,puIRData->IRdata.pData,puIRData->IRdata.BIT_DATA_LEN_D);
    Serial.print("The data to save is:");
    for(int i=addr; i<datalen; i++) {
        EEPROM.write(i,data[i-addr]);
        #if debug
        Serial.print("Addr:");
        Serial.print(i,DEC);
        Serial.print(":");
        Serial.print(data[i-addr],DEC);
        Serial.println(" ");
        #endif
    }
}

void readCommand(uint8_t addr, uint8_t *data) {
    uint8_t i;
    uint8_t datalen = EEPROM.read(addr);
    #if debug
    Serial.println("The data is:");
    #endif
    for(i=addr; i<addr+datalen+1; i++) {
        data[i-addr] = EEPROM.read(i);
        #if debug
        Serial.print("Addr:");
        Serial.print(i,DEC);
        Serial.print(":");
        Serial.print(data[i-addr],DEC);
        Serial.println(" ");
        #endif
    }
}

