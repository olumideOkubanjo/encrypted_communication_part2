#include <Arduino.h>

//Forward Declaration of Functions in order
void setup();
uint32_t uint32_from_serial3();
bool wait_on_serial3( uint8_t nbytes , long timeout );
void uint32_to_serial3(uint32_t num);

//Enum States
enum clientState {
  START, WAITINGFORACK, DATAEXCHANGECLIENT
};

enum serverState {
  LISTEN, WAITINGFORKEY1,WAITINGFORKEY2, WAITFORACK, DATAEXCHANGESERVER
};

//Main Program
int main(){
    setup();

    //Global Constants
    uint32_t serverPublicKey = 7;
    uint32_t serverPrivateKey = 27103;
    uint32_t serverModulus = 95477;
    uint32_t clientPublicKey = 11;
    uint32_t clientPrivateKey = 38291;
    uint32_t clientModulus = 84823;
    
    bool serverStatebool = false;
    char C = 'C';
    char A = 'A';

    //Determining the correct variables based on client or server status
    if( digitalRead(13) == HIGH){
        serverStatebool = true;
        Serial.println("Server");
    }else{
        Serial.println("Client");
    }

    //Handshaking Code
    if(serverStatebool){
        serverState serverState = LISTEN;
        while(serverState!=DATAEXCHANGESERVER){
            if(serverState==LISTEN){
                Serial.println("Listening");
                if(wait_on_serial3(1,1000)){
                    if(Serial3.read()==C){
                        serverState = WAITINGFORKEY1;
                    }
                }
            }
            if(serverState==WAITINGFORKEY1){
                Serial.println("Waiting for key");
                if(wait_on_serial3(8,1000)){
                    clientPublicKey = uint32_from_serial3();
                    clientModulus = uint32_from_serial3();

                    Serial3.write(A);
                    uint32_to_serial3(serverPublicKey);
                    uint32_to_serial3(serverModulus);
                    serverState=WAITFORACK;
                }else{
                    serverState=LISTEN;
                }
            }else if(serverState==WAITFORACK){
                Serial.println("Waiting for ACK");
                if(wait_on_serial3(1,1000)){
                    Serial.println("Char Received");
                    char tempRead = Serial3.read();
                    if(tempRead==C){
                        Serial.println("Reading the C");
                        serverState=WAITINGFORKEY2;
                    }
                    if(tempRead==A){
                        Serial.println("Reading the A");
                        serverState=DATAEXCHANGESERVER;
                    }
                }else{
                    serverState=LISTEN;
                }
            }else if(serverState==WAITINGFORKEY2){
                Serial.println("Waiting for key 2");
                if(wait_on_serial3(8,1000)){
                    clientPublicKey = uint32_from_serial3();
                    clientModulus = uint32_from_serial3();
                    serverState=WAITFORACK;
                }else{
                    serverState=LISTEN;
                }
            }
        }
    }else{
        clientState clientState = START;
        while(clientState!=DATAEXCHANGECLIENT){
            if(clientState==START){
                Serial.println("Starting");
                Serial3.write(C);
                uint32_to_serial3(clientPublicKey);
                uint32_to_serial3(clientModulus);
                clientState = WAITINGFORACK;

            }
            if(clientState==WAITINGFORACK){
                Serial.println("Waiting for ack");
                if(wait_on_serial3(1,1000)){
                    if(Serial3.read()==A){
                        if(wait_on_serial3(8,1000)){
                            serverPublicKey = uint32_from_serial3();
                            serverModulus = uint32_from_serial3();
                            // for(int i =0; i < 50; i++){
                            //     Serial3.write(A);
                            // }
                            Serial3.write(A);
                            clientState = DATAEXCHANGECLIENT;
                            Serial.println("DATA EXCHANGE");
                        }else{
                            clientState=START;
                        }
                    }
                }else{
                    clientState=START;
                }
            }
        }

    }

    Serial.println("CONNECTED");

    Serial.flush();
    return 0;
}

void setup(){

    //Initalizing Arduino
    init();

    //Opening Serial connection
    Serial.begin(9600);
    Serial3.begin(9600);

    //Setting the pin mode for pin 13
    pinMode(13, INPUT);    
}

/* * Waits for a certain number of bytes on Serial3 or timeout
* @param nbytes : the number of bytes we want
* @param timeout : timeout period ( ms ) ; specifying a negative number
*
turns off timeouts ( the function waits indefinitely
*
if timeouts are turned off ) .
* @return True if the required number of bytes have arrived .
*/
bool wait_on_serial3( uint8_t nbytes , long timeout ) {
    unsigned long deadline = millis() + timeout; // wraparound not a problem
    while( Serial3.available() < nbytes && (timeout<0 || millis()< deadline) ){
        delay (1); // be nice , no busy loop
    }
    return Serial3.available()>=nbytes ;
}


/* * Writes an uint32_t to Serial3 , starting from the least - significant
* and finishing with the most significant byte .
*/
void uint32_to_serial3 ( uint32_t num ) {
    Serial3.write(( char ) ( num >> 0) ) ;
    Serial3.write(( char ) ( num >> 8) ) ;
    Serial3.write(( char ) ( num >> 16) ) ;
    Serial3.write(( char ) ( num >> 24) ) ;
}
/* * Reads an uint32_t from Serial3 , starting from the least - significant
* and finishing with the most significant byte .
*/
uint32_t uint32_from_serial3() {
    uint32_t num = 0;
    num = num | (( uint32_t ) Serial3.read() ) << 0 ;
    num = num | (( uint32_t ) Serial3.read() ) << 8 ;
    num = num | (( uint32_t ) Serial3.read() ) << 16 ;
    num = num | (( uint32_t ) Serial3.read() ) << 24 ;
    return num ;
}

