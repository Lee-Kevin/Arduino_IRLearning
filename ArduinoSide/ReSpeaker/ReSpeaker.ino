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

#define pinRecv         5

#define commandMax      50
#define commandNum      20

#if commandNum > commandMax
#undef commandNum
#define commandNum commandMax
#endif 

enum status {
   IRLearning = 0,
   IRSend     = 1,
   UnKnow     = 2,
};
typedef enum status estatus_t;
estatus_t WorkingStatus;

enum command {
    SwitchAirCon  =  0,  // control the air conditioner
    Other         =  1,
};
typedef enum command ecommand_t;
ecommand_t IRCommand;

uint16_t CommandAddr[commandNum];

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

uint8_t irdataload[6];

#define debug 1

void dtaInit()
{
    puIRData->IRdata.BIT_LEN_D      = 11;
    puIRData->IRdata.BIT_START_H_D  = 179;
    puIRData->IRdata.BIT_START_L_D  = 90;
    puIRData->IRdata.BIT_DATA_H_D   = 11;
    puIRData->IRdata.BIT_DATA_L_D   = 33;
    puIRData->IRdata.BIT_DATA_LEN_D = 6;
    
    puIRData->IRdata.pData = irdataload;
    
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
    CommandAddr[0]  = 2*commandNum;
    saveCommand(CommandAddr[0],puIRData);
    readCommand(CommandAddr[0],dtaSend);
    IR.Init(pinRecv);
    #if debug
    Serial.println("init over");
    #endif
    WorkingStatus = IRLearning;
}

void loop()
{
    switch(WorkingStatus) {
    case IRLearning:
        #if debug
        Serial.println("This is IR Learning");
        #endif
        for(int i=0; i<commandNum; ) {
            if(IR.IsDta()) {
                IR.Recv(dtaSend);
                puIRData->IRdata.BIT_LEN_D      = dtaSend[BIT_LEN];
                puIRData->IRdata.BIT_START_H_D  = dtaSend[BIT_START_H];
                puIRData->IRdata.BIT_START_L_D  = dtaSend[BIT_START_L];
                puIRData->IRdata.BIT_DATA_H_D   = dtaSend[BIT_DATA_H];
                puIRData->IRdata.BIT_DATA_L_D   = dtaSend[BIT_DATA_L];
                puIRData->IRdata.BIT_DATA_LEN_D = dtaSend[BIT_DATA_LEN];
                puIRData->IRdata.pData          = irdataload;
                for(int i=0; i< dtaSend[BIT_DATA_LEN]; i++) {
                    puIRData->IRdata.pData[i]       = dtaSend[i+BIT_DATA];
                }
                
                #if debug
                for(int j=0; j<6; j++) {
                    Serial.println(puIRData->ucdata[j],DEC);
                }
                for(int j=0; j<dtaSend[BIT_DATA_LEN]; j++) {
                    Serial.println(puIRData->IRdata.pData[j],DEC);
                }
                #endif
                
                if(i==0) {
                    CommandAddr[i] = 2*commandNum;
                } 
                if(i+1 < commandNum) {
                CommandAddr[i+1]  = CommandAddr[i] + puIRData->IRdata.BIT_LEN_D + 1;
                saveCommand(CommandAddr[i],puIRData);
                } 
                saveAddrs(2*i,CommandAddr[i]);
                i++;
                
                #if debug
                char buf[30];
                sprintf(buf,"IR Command %d Learning Done",i);
                Serial.println(buf);
                debugprint(dtaSend);
                #endif
            }
        }
        WorkingStatus = IRSend;
        break;
    case IRSend:
        #if debug
        Serial.println("This is IR Send");
        #endif
        for(int i=0; i<commandNum; i++) {
            readCommand(CommandAddr[i],dtaSend);
            #if debug
            debugprint(dtaSend);
            char buf[30];
            sprintf(buf,"IR Command %d Read Done",i);
            Serial.println(buf);
            #endif
            IR.Send(dtaSend, 38);
            delay(1000);
        }
        WorkingStatus = UnKnow;
        break;
    default:
        break;
    }
    
   // 
}

void saveAddrs(uint16_t addr, uint16_t commandAddr) {
    uint8_t data_l, data_h;
    data_h = commandAddr / 255;
    data_l = commandAddr % 255;
    EEPROM.write(addr,data_h);
    EEPROM.write(addr+1,data_l);
}


void saveCommand(uint16_t addr, ptuIRData puIRData) {
    uint8_t datalen = puIRData->IRdata.BIT_LEN_D + 1;

    uint8_t data[20];
    memcpy(data,puIRData->ucdata,6);
    memcpy(data+6,puIRData->IRdata.pData,puIRData->IRdata.BIT_DATA_LEN_D);
    #if debug
    Serial.print("The data to save is:");
    #endif

    
    for(uint16_t i=addr; i<addr+datalen; i++) {
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

void readCommand(uint16_t addr, uint8_t *data) {
    uint16_t i;
    uint8_t datalen = EEPROM.read(addr)+1;
    #if debug
    Serial.println("The data is:");
    #endif
    for(i=addr; i<addr+datalen; i++) {
        data[i-addr] = EEPROM.read(i);
        #if debug
        Serial.print("ReadAddr:");
        Serial.print(i,DEC);
        Serial.print(":");
        Serial.print(data[i-addr],DEC);
        Serial.println(" ");
        #endif
    }
}

#if debug
void debugprint(uint8_t *data) {
    Serial.println("+------------------------------------------------------+");
    Serial.print("LEN = ");
    Serial.println(data[BIT_LEN]);
    Serial.print("START_H: ");
    Serial.print(data[BIT_START_H]);
    Serial.print("\tSTART_L: ");
    Serial.println(data[BIT_START_L]);
            
    Serial.print("DATA_H: ");
    Serial.print(data[BIT_DATA_H]);
    Serial.print("\tDATA_L: ");
    Serial.println(data[BIT_DATA_L]);

    Serial.print("\r\nDATA_LEN = ");
    Serial.println(data[BIT_DATA_LEN]);

    Serial.print("DATA: ");
    for(int i=0; i<data[BIT_DATA_LEN]; i++)
    {
        Serial.print("0x");
        Serial.print(data[i+BIT_DATA], HEX);
        Serial.print("\t");
    }
    Serial.println();

    Serial.print("DATA: ");
    for(int i=0; i<data[BIT_DATA_LEN]; i++)
    {
        Serial.print(data[i+BIT_DATA], DEC);
        Serial.print("\t");
    }               
    Serial.println();
    Serial.println("+------------------------------------------------------+\r\n\r\n");      
    }
#endif
