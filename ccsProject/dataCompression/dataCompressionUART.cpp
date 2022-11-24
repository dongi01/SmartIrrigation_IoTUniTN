#include <iostream>

using namespace std;

//function ready to use
uint8_t compressData(uint8_t input1, uint8_t input2);
void decompressedData(uint8_t recivedData, uint8_t* dataArray);
bool compressDataAdvanced(uint8_t input1, uint8_t input2, uint8_t input3, uint8_t* arrayResult);
void decompressDataAdvanced(uint8_t* inputArray, uint8_t* outputArray);
bool checkDimension(uint8_t bitDimension, uint8_t variable);
int mapValue4or6bits(uint8_t value, uint8_t valueDim, int minF, int maxF);
int SevenBitsToRange(uint8_t value, int minF, int maxF);
uint8_t RangeTo7bits(int value, int minI, int maxI);
uint8_t setControlBit(uint8_t firstData);
uint8_t mapToPercentage(int value, int minI, int maxI);

int main(){

/* 
    // ------------ sending part ------------- 
    //max data size: 15 without mapping
    uint8_t input1 = 3; 
    uint8_t input2 = 12; 

    cout << "input1 = " << (int)input1 << ", input2 = " << (int)input2 << endl;

    uint8_t finalMsg;

    finalMsg = (int)compressData(input1, input2);

    cout << "Sending single message as uint8_t: " << (int)finalMsg << endl;

// ------------ reading part ------------- 
    uint8_t output1;
    uint8_t output2;
    uint8_t recivedMsg = finalMsg;

    cout << "Reciving uint8_t compressed: " << (int)recivedMsg << endl;

    uint8_t dataRX[2];

    decompressedData(recivedMsg, dataRX);

    output1 = dataRX[0];
    output2 = dataRX[1];


    cout << "Decompressing data: output1 = " << (int)output1 << ", output2 = " << (int)output2 << endl;
*/

/*     uint8_t primo = 15;
    uint8_t secondo = 38;
    uint8_t terzo = 1;
    uint8_t msgs[2];

    cout << "input1 = " << (int)primo << ", input2 = " << (int)secondo << ", input3 = " << (int)terzo << endl;

    if (compressDataAdvanced(primo, secondo, terzo, msgs)) {
        cout << "first msg = " << (int)msgs[0] << ", second msg = " << (int)msgs[1] << endl;

        uint8_t* inputMsgs = msgs;
        uint8_t numbersArray[3];

        decompressDataAdvanced(inputMsgs, numbersArray);

        cout << "Decompressing data: output1 = " << (int)numbersArray[0] << ", output2 = " << (int)numbersArray[1] << ", output3 = " << (int)numbersArray[2] << endl;

    } else {
        cout << "error: use 4 bit input 1 and 3 and 6 bit input 2" << endl;
    }

    cout << mapValue(4, 6, 20, 200) << endl;
*/

    int grande = 16000;
    int piccolo = 14000;

    cout << "grande " << grande << " convertito diventa " << RangeTo7bits(grande, 0, 10000) << endl;
    cout << "piccolo " << piccolo << " convertito diventa " << SevenBitsToRange(piccolo, 0, 10000) << endl;


    cout << "to percetuale " << (int)mapToPercentage(15346, piccolo, grande) << endl;

    
    return 0;
}

// sends 2 integer data in 8 bit
uint8_t compressData(uint8_t input1, uint8_t input2){

    uint8_t finalMsg;

    // shift the 4 bit input1 to the left half of the 8 bits
    finalMsg = (input1 << 4);
    // add the 4 bit input2 to the final variable
    finalMsg += input2;

    return finalMsg;
}

// obtains 2 integer from 8 bit
void decompressedData(uint8_t recivedData, uint8_t* dataArray){

    // saving the left part of the 8 bits
    dataArray[0] = recivedData >> 4;

    // saving the right part of the 8 bits using a 0000 1111 mask and a &
    uint8_t output2Mask = 15;
    dataArray[1] = recivedData & output2Mask;
}

// *** comunication with 2 byte and 3 integer to send ***

// creates 2 byte with 3 integer and one control bit for each one
// input1 and input3 must be at most 4 bit long and input 2 must be at most 6 bit long
bool compressDataAdvanced(uint8_t input1, uint8_t input2, uint8_t input3, uint8_t* outputArray) {

    bool returnValue = true;

    // outputArray[0] -> first byte
    // outputArray[1] -> second byte

    if (checkDimension(4, input1) && checkDimension(6, input2) && checkDimension(4, input3)) {
        uint8_t mask;

        // 3 bit for the first part (input2 is 6 bit in total) to send in firstByte
        uint8_t FIRSTinput2 = input2 >> 3;
        // 3 bit for the second part to send in secondByte
        mask = 7; // 0000 0111
        uint8_t SECONDinput2 = input2 & mask;


        // firt byte has 1 control bit(1), 4 bit for the first data, 3 bit for second data(half data)
        // second byte has 1 control bit(0), 4 bit for the third data, 3 bit for second data(half data)

        // to set MSB to 1; for the second byte the MSB bit is already 0
        outputArray[0] = (1<<7); // or firstByte = 128

        // to insert input1, input 3 and the 2 part of input 2
        outputArray[0] += ((input1 << 3) + FIRSTinput2);
        outputArray[1] = ((input3 << 3) + SECONDinput2);

    } else {
        returnValue = false;
    }

    return returnValue;

}

// creates 3 integer from 2 byte as defined by our standard
void decompressDataAdvanced(uint8_t* inputArray, uint8_t* outputArray) {

    // outputArray[0] -> first data
    // outputArray[1] -> second data
    // outputArray[2] -> third data

    uint8_t mask;
    uint8_t firstByteCopy = inputArray[0];
    uint8_t secondByteCopy = inputArray[1];

    mask = 120; // 0111 1000
    outputArray[0] = firstByteCopy & mask;
    outputArray[0] = (outputArray[0] >> 3);

    outputArray[2] = secondByteCopy & mask;
    outputArray[2] = outputArray[2] >> 3;

    mask = 7; // 0000 0111
    outputArray[1] = ((firstByteCopy & mask) << 3) + (secondByteCopy & mask);
}

// checks if the input number has the right dimention
bool checkDimension(uint8_t bitDimension, uint8_t variable) {
    variable = variable >> bitDimension;
    return (variable == 0) ? true : false;
}

// maps the input value in a larger range
int mapValue2byteData(uint8_t value, uint8_t valueDim, int minF, int maxF) {
    int returnValue = -1;
    if (checkDimension(valueDim, value)) {
        if (valueDim == 4) {
            returnValue = minF + value * (maxF - minF) / 15; // 15 max value for 4 bits
        } else if (valueDim == 6) {
            returnValue = minF + value * (maxF - minF) / 63; // 63 max value for 6 bits
        }
    }
    return returnValue;
}

// maps the input value in a larger range (reciver side)
int SevenBitsToRange(uint8_t value, int minF, int maxF) {
    int returnValue = -1;
    if (checkDimension(7, value)) {
        returnValue = minF + value * (maxF - minF) / 127; // 127 max value for 7 bits
    }
    return returnValue;
}

// maps the input value in a smaller range (sender side)
uint8_t RangeTo7bits(int value, int minI, int maxI) {  // I for initial, F for final
    uint8_t returnValue = 0.0 + (127.0 / (maxI - minI)) * (value - minI);
    return returnValue;
}

uint8_t setControlBit(uint8_t firstData){
    return 128 + firstData;
}

uint8_t mapToPercentage(int value, int minI, int maxI){
    uint8_t returnValue = 0.0 + (100.0 / (maxI - minI)) * (value - minI);;
    return returnValue;
}
// general formula for mapping value in different ranges
// output = output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);