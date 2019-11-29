/*
Name1: Olumide Okubanjo (1573500)
Name2: Mohamed Ali (1573724) 
CMPUT 274 Fa17
Major Assignment 2: Part 1
*/

#include <Arduino.h>

//Forward Declaration of Functions in order
void setup();
uint32_t encrypt(uint32_t x, uint32_t e, uint32_t m);
uint32_t decrypt(uint32_t x, uint32_t d, uint32_t n);
uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m);
uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m);
uint32_t uint32_from_serial3();
bool wait_on_serial3( uint8_t nbytes , long timeout );
void uint32_to_serial3(uint32_t num);

//Enum States
enum clientState {
  // **** you will have add more state names here!
  START, WAITINGFORACK, DATAEXCHANGECLIENT
};
enum serverState {
  // **** you will have add more state names here!
  LISTEN, WAITINGFORKEY, WAITRFORACK, DATAEXCHANGESERVER
};

//Main Program
int main(){
    setup();

    clientState clientState = START;
    serverState serverState = LISTEN;

    //Global Constants
    uint32_t serverPublicKey = 7;
    uint32_t serverPrivateKey = 27103;
    uint32_t serverModulus = 95477;
    uint32_t clientPublicKey = 11;
    uint32_t clientPrivateKey = 38291;
    uint32_t clientModulus = 84823;
    
    bool serverStatebool = false;
    uint32_t C = 67;
    uint32_t A = 35;

    //Determining the correct variables based on client or server status
    if ( digitalRead(13) == HIGH){
        //Serial.println("Server");
        serverStatebool = true;
    }else{
        // Serial.println("Client");
    }

    //Handshaking Code
    while((clientState!=DATAEXCHANGECLIENT) && (serverState!=DATAEXCHANGESERVER)){
        if(serverStatebool){
            //Server Finite state machine
            serverState = LISTEN;
            if(uint32_from_serial3()==C){
                serverState = WAITINGFORKEY;
                if(wait_on_serial3(8,1000)){
                    clientPublicKey = uint32_from_serial3();
                    if(clientPublicKey!=C){
                        clientModulus = uint32_from_serial3();
                        uint32_to_serial3(A);
                        uint32_to_serial3(serverPublicKey);
                        uint32_to_serial3(serverModulus);
                        if(wait_on_serial3(4,1000)){
                            serverState = WAITRFORACK;
                            if(uint32_from_serial3==A){
                                serverState=DATAEXCHANGESERVER;
                            }
                        }
                    }
                }
            }

        }else{
            //Client Finite State machine
            clientState = START;
            uint32_to_serial3(C);
            uint32_to_serial3(clientPublicKey);
            uint32_to_serial3(clientModulus);

            clientState = WAITINGFORACK;
            if(wait_on_serial3(9, 1000)){
                if(uint32_from_serial3() == A){
                    serverPublicKey = uint32_from_serial3();
                    serverModulus = uint32_from_serial3();
                    uint32_to_serial3(A);
                    clientState = DATAEXCHANGECLIENT;
                }
            }
        }
    }
    Serial.print("CONNECTED");

    // //Main Loop
    // while(true){

    //     //Typing on own serial monitor
    //     if (Serial.available() > 0) {
    //         // Read and display the entered char
    //         uint32_t byte_read = Serial.read();
    //         Serial.write(byte_read);

    //         //Check if char is the /r
    //         if (byte_read == 13){
    //             //Encrypt and send both the /r and /n 
    //             Serial.write("\n");

    //             uint32_t returnchar = encrypt(byte_read, e , m);
    //             uint32_to_serial3(returnchar);

    //             uint32_t newlinechar32 = 10;
    //             uint32_t newlinechar = encrypt(newlinechar32, e , m);
    //             uint32_to_serial3(newlinechar);
    //         }else{
    //             //Encrypt and send the char
    //             uint32_t regularChar = encrypt(byte_read, e , m );
    //             uint32_to_serial3(regularChar);
    //         }
    //     }
    //     //Reading from the serial3 "Stream"
    //     if (Serial3.available() > 3 ) {
    //         //Read the char from stream 
    //         uint32_t byte_read = uint32_from_serial3();

    //         //Decrypt it
    //         uint32_t charRead = decrypt(byte_read, d , n );

    //         //Checks if the return character was received or not
    //         if (charRead == 13){
    //             //Prints the return char to your own serial monitor
    //             Serial.write((char) charRead);

    //             //Reads the new line char , decrypt and displays it
    //             uint32_t newlinechar = uint32_from_serial3();
    //             Serial.write(decrypt(newlinechar, d , n ));
                
    //         }else{
    //             //Display it
    //             Serial.write((char) charRead);
    //         }
            
    //     }  
    
    // }

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

