/*
 * Copyright (c) 2016 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Jiankai Li 
 * Modified Time: Aug 2016
 * Description: IR Learning head file
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

#ifndef __IRLEARNING_H__
#define __IRLEARNING_H__

#include "Arduino.h"
#include "IRSendRev.h"
#include <EEPROM.h>
// For Arduino Leonardo the IR default send pin D13
// For Arduino Uno      the IR default send pin D3
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
#define commandLen      20   

#define debug  1

#if commandNum > commandMax
#undef commandNum
#define commandNum commandMax
#endif 

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




class IRLearning {   
private:

    tuIRdata   myuIRData;
    
    ptuIRData  puIRData = &myuIRData;
    
    uint8_t    irdataload[10];
    
    uint8_t    commandIndex = 0;
    
    #if debug
    void debugprint(uint8_t *data);
    #endif

    
    void saveAddrs(uint16_t addr, uint16_t commandAddr);
    
    void saveCommand(uint16_t addr, ptuIRData puIRData);
    
    void readCommand(uint16_t addr, uint8_t *data);
    
public:
    IRLearning();
    
    void init();
    
    uint8_t IRCommandRec();
    
    void IRCommandSend(uint8_t command);

};

extern IRLearning myir;


#endif // __IRLEARNING_H__