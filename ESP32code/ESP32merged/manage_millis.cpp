

/*

IDEA:
- create a single function that manage reading from serial, process the data and send via http
- create 2 separate function that manage reading form serial and process-sending phase
Put them in the //code section below.
This pice of code is needed if we want to garantee our program works for a long period of time.

*/

#define ULONG_MAX 18446744073709551615

using namespace std;

unsigned long timerDelay = 5000;
unsigned long lastTime = 0;

int main(){

    if(millis() > lastTime){
        if ((millis() - lastTime) > timerDelay){
            //codice
            lastTime = millis();
        }
    } else { // when millis() overflow
        if ((millis() + (ULONG_MAX - lastTime)) > timerDelay){
            //codice
            lastTime = millis();
        }
    }

    return 0;
}