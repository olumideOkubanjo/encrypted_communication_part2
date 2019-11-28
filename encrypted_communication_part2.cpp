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
void uint32_to_serial3(uint32_t num);

//Global Constants
uint32_t serverPublicKey = 7;
uint32_t serverPrivateKey = 27103;
uint32_t serverModulus = 95477;
uint32_t clientPublicKey = 11;
uint32_t clientPrivateKey = 38291;
uint32_t clientModulus = 84823;

//Main Program
int main(){
    setup();

    //Determining the correct variables based on client or server status
    uint32_t d, m ,n ,e;
    if ( digitalRead(13) == HIGH){
        //Serial.println("Server");
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

    //Main Loop
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

