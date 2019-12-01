/*
Name1: Olumide Okubanjo (1573500)
Name2: Mohamed Ali (1573724) 
CMPUT 274 Fa17
Major Assignment 2: Part 2
*/

#include <Arduino.h>

//Forward Declaration of Functions in order
void setup();
uint32_t encrypt(uint32_t x, uint32_t e, uint32_t m);
uint32_t decrypt(uint32_t x, uint32_t d, uint32_t n);
uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m);
uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m);
uint32_t uint32_from_serial3();
void uint32_to_serial3(uint32_t num);
bool wait_on_serial3( uint8_t nbytes , long timeout );

//Global Constants
uint32_t serverPublicKey = 7;
uint32_t serverPrivateKey = 27103;
uint32_t serverModulus = 95477;
uint32_t clientPublicKey = 11;
uint32_t clientPrivateKey = 38291;
uint32_t clientModulus = 84823;

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
    bool serverStatebool = false;
    char C = 'C';
    char A = 'A';

    Serial.println("Connecting...");
    //Determining the correct variables based on client or server status
    /*
    e - arduino's public key
    d - arduino's private key
    n - arduino's modulus
    */
    uint32_t d, m ,n ,e;
    if ( digitalRead(13) == HIGH){
        //Serial.println("Server");
        serverStatebool = true;
        d = serverPrivateKey;
        n = serverModulus;
        e = clientPublicKey; 
        m = clientModulus;
    }else{
        // Serial.println("Client");
        d = clientPrivateKey;
        n = clientModulus;
        e = serverPublicKey; 
        m = serverModulus;
    }

    //HandShake Loop
    //Handshaking Code
    if(serverStatebool){
        serverState serverState = LISTEN;
        while(serverState!=DATAEXCHANGESERVER){
            if(serverState==LISTEN){
                // Serial.println("Listening");
                if(wait_on_serial3(1,1000)){
                    if(Serial3.read()==C){
                        serverState = WAITINGFORKEY1;
                    }
                }
            }
            if(serverState==WAITINGFORKEY1){
                // Serial.println("Waiting for key");
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
                // Serial.println("Waiting for ACK");
                if(wait_on_serial3(1,1000)){
                    // Serial.println("Char Received");
                    char tempRead = Serial3.read();
                    if(tempRead==C){
                        // Serial.println("Reading the C");
                        serverState=WAITINGFORKEY2;
                    }
                    if(tempRead==A){
                        // Serial.println("Reading the A");
                        serverState=DATAEXCHANGESERVER;
                    }
                }else{
                    serverState=LISTEN;
                }
            }else if(serverState==WAITINGFORKEY2){
                // Serial.println("Waiting for key 2");
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
                // Serial.println("Starting");
                Serial3.write(C);
                uint32_to_serial3(clientPublicKey);
                uint32_to_serial3(clientModulus);
                clientState = WAITINGFORACK;

            }
            if(clientState==WAITINGFORACK){
                // Serial.println("Waiting for ack");
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
                            // Serial.println("DATA EXCHANGE");
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

    Serial.println("CONNECTED Enjoy Chatting: ");

    //Chat  Loop
    while(true){

        //Typing on own serial monitor
        if (Serial.available() > 0) {
            // Read and display the entered char
            uint32_t byte_read = Serial.read();
            Serial.write(byte_read);

            //Check if char is the /r
            if (byte_read == 13){
                //Encrypt and send both the /r and /n 
                Serial.write("\n");

                uint32_t returnchar = encrypt(byte_read, e , m);
                uint32_to_serial3(returnchar);

                uint32_t newlinechar32 = 10;
                uint32_t newlinechar = encrypt(newlinechar32, e , m);
                uint32_to_serial3(newlinechar);
            }else{
                //Encrypt and send the char
                uint32_t regularChar = encrypt(byte_read, e , m );
                uint32_to_serial3(regularChar);
            }
        }
        //Reading from the serial3 "Stream"
        if (Serial3.available() > 3 ) {
            //Read the char from stream 
            uint32_t byte_read = uint32_from_serial3();

            //Decrypt it
            uint32_t charRead = decrypt(byte_read, d , n );

            //Checks if the return character was received or not
            if (charRead == 13){
                //Prints the return char to your own serial monitor
                Serial.write((char) charRead);

                //Reads the new line char , decrypt and displays it
                uint32_t newlinechar = uint32_from_serial3();
                Serial.write(decrypt(newlinechar, d , n ));
                
            }else{
                //Display it
                Serial.write((char) charRead);
            }
            
        }  
    
    }
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

uint32_t encrypt(uint32_t c, uint32_t e, uint32_t m){
    //Using powmod and mulmod to encrpyt the character
    return powmod(c, e, m);
}

uint32_t decrypt(uint32_t x, uint32_t d, uint32_t n){
    //Using powmod and mulmod to decrypt the character
    return powmod(x, d, n);
}

uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m){
    //Taking the modulus of a and b for good measure
    a = a % m;
    b = b % m;

    //Variables needed
    uint32_t ans = 0;
    uint32_t one_32bit = 1;

    //Looping through all 31 bits of a
    for(int i = 0; i < 31; i++){
        if ((a & one_32bit<<i) > 0 ){
            //Calculating (2 ^ i) * b for every i = 1 in a 
            uint32_t twoPowI = b;
            for(int j = 0; j<i; j++){
                twoPowI = (twoPowI * 2 ) % m ;
            }
            //Adding twoPowI to the answer
            ans = (twoPowI + ans) % m ; 
        }
    }
    return ans ;
}

// computes the value x^pow mod m ("x to the power of pow" mod m)
//(CODE PROVIDED BY CLASS)
uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m) {
  // uint32 Valribales
  uint32_t ans = 1;
  uint32_t pow_x = x;

  //Power modulus calculation
  while (pow > 0) {
    if ((pow&1) == 1) {
      ans = mulmod(ans , pow_x, m);
    }
    pow_x = mulmod(pow_x , pow_x, m);
    pow >>= 1; // divides by 2
  }

  return ans;
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
