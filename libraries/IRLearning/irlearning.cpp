/*
 * Copyright (c) 2016 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Jiankai Li 
 * Modified Time: Aug 2016
 * Description: IR Learning cpp file
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

#include "irlearning.h"


IRLearning::IRLearning() {
    
}

void IRLearning::init() {
    
    IR.Init(pinRecv);
    #if debug
    Serial.begin(9600);
    Serial.println("IR Learning remote controller");
    
    Serial.println(EEPROM.length());

    Serial.println("-------------------------");
    #endif
}

uint8_t IRLearning::IRCommandRec() {
    uint8_t dtaSend[20];
    uint16_t commandAddr;
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
        
        // if(commandIndex==0) {
            // CommandAddr[i] = 2*commandNum;
        // } 
        // if(i+1 < commandNum) {
        // CommandAddr[i+1]  = CommandAddr[i] + puIRData->IRdata.BIT_LEN_D + 1;
        // } 
        commandAddr = commandIndex*commandLen + commandNum*2;
        
        saveAddrs(2*commandIndex, commandAddr);
        saveCommand(commandAddr, puIRData);
        

        #if debug
        char buf[30];
        sprintf(buf,"IR Command %d Learning Done",commandIndex);
        Serial.println(buf);
        debugprint(dtaSend);   
        #endif
        if(++commandIndex == commandNum) {
            commandIndex == 0;
        }
        return 1;    
    } else {
        return 0;
    }
        
}

void IRLearning::IRCommandSend(uint8_t command) {
    uint16_t commandAddr = command*commandLen + commandNum*2;
    uint8_t dtaSend[20];
    
    readCommand(commandAddr,dtaSend);
    IR.Send(dtaSend, 38);
    #if debug
    debugprint(dtaSend);
    char buf[30];
    sprintf(buf,"IR Command %d Read Done",command);
    Serial.println(buf);
    #endif
    
}

void IRLearning::saveAddrs(uint16_t addr, uint16_t commandAddr) {
    uint8_t data_l, data_h;
    data_h = commandAddr / 255;
    data_l = commandAddr % 255;
    EEPROM.write(addr,data_h);
    EEPROM.write(addr+1,data_l);
}

void IRLearning::saveCommand(uint16_t addr, ptuIRData puIRData) {
    uint8_t datalen = puIRData->IRdata.BIT_LEN_D + 1;

    uint8_t data[20];
    memcpy(data,puIRData->ucdata,6);
    memcpy(data+6,puIRData->IRdata.pData,puIRData->IRdata.BIT_DATA_LEN_D);
    #if debug
    Serial.println("The data to save is:");
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

void IRLearning::readCommand(uint16_t addr, uint8_t *data) {
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
void IRLearning::debugprint(uint8_t *data) {
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
