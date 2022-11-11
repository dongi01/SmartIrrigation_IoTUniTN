#include <iostream>

using namespace std;

//function ready to use
uint8_t compressData(uint8_t input1, uint8_t input2);
void decompressedData(uint8_t recivedData, uint8_t* dataArray);

int main(){

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
    /*
    for (int i=0; i<2; i++) {
        cout << (int)*(array+i) << endl;
    }
    */

    cout << "Decompressing data: output1 = " << (int)output1 << ", output2 = " << (int)output2 << endl;

    return 0;
}

uint8_t compressData(uint8_t input1, uint8_t input2){

    uint8_t finalMsg;

    // shift the 4 bit input1 to the left half of the 8 bits
    finalMsg = (input1 << 4);
    // add the 4 bit input2 to the final variable
    finalMsg += input2;

    return finalMsg;
}

void decompressedData(uint8_t recivedData, uint8_t* dataArray){

    // saving the left part of the 8 bits
    dataArray[0] = recivedData >> 4;

    // saving the right part of the 8 bits using a 0000 1111 mask and a &
    uint8_t output2Mask = 15;
    dataArray[1] = recivedData & output2Mask;
}
