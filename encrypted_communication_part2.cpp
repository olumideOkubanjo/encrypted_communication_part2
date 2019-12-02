/*
Name1: Olumide Okubanjo (1573500)
Name2: Mohamed Ali (1573724) 
CMPUT 274 Fa19
Major Assignment 2: Part 2
*/

//Arduino library
#include <Arduino.h>

//Forward Declaration of Functions in order
void setup();
uint32_t generate_num(int n);
bool prime_check(uint32_t num);
uint32_t generate_prime(int n);
uint32_t find_e(uint32_t phi);
uint32_t find_d(uint32_t e, uint32_t phi);
uint32_t encrypt(uint32_t x, uint32_t e, uint32_t m);
uint32_t decrypt(uint32_t x, uint32_t d, uint32_t n);
uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m);
uint32_t gcd_euclid_fast(uint32_t a, uint32_t b);
uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m);
uint32_t uint32_from_serial3();
void uint32_to_serial3(uint32_t num);
bool wait_on_serial3( uint8_t nbytes , long timeout );

//Enum States for state3 machine implementation
enum clientState {
  START, WAITINGFORACK, DATAEXCHANGECLIENT
};

enum serverState {
  LISTEN, WAITINGFORKEY1,WAITINGFORKEY2, WAITFORACK, DATAEXCHANGESERVER
};

//Main Program
int main(){
    setup();

    //Nessesary Variables
    bool serverStatebool = false;
    char C = 'C';
    char A = 'A';

    Serial.println("CONNECTING to arduino...");

    //Determining the correct variables based on client or server status
    /*
    e - arduino's public key
    d - arduino's private key
    n - arduino's modulus
    */
    uint32_t d, m ,n ,e;
    if ( digitalRead(13) == HIGH){
        Serial.println("Server");
        serverStatebool = true;
        uint32_t one = 1;
        uint32_t p = generate_prime(15);
        uint32_t q = generate_prime(16);
        n = (p) * (q);
        uint32_t phi= (p-one)*(q-one);
        e = find_e(phi);
        d = find_d(e,phi);
    }else{
        Serial.println("Client");
        uint32_t one = 1;
        uint32_t p = generate_prime(15);
        uint32_t q = generate_prime(16);
        n = (p) * (q);
        uint32_t phi= (p-one)*(q-one);
        e = find_e(phi);
        d = find_d(e,phi);
    }

    //Dataexchange loop
    if(serverStatebool){
        serverState serverState = LISTEN;
        while(serverState!=DATAEXCHANGESERVER){
            // Serial.println("Listening...");
            if(serverState==LISTEN){
                if(wait_on_serial3(1,1000)){
                    if(Serial3.read()==C){
                        serverState = WAITINGFORKEY1;
                    }
                }
            }
            if(serverState==WAITINGFORKEY1){
                if(wait_on_serial3(8,1000)){
                    uint32_t ecl = uint32_from_serial3();
                    m = uint32_from_serial3();
                    Serial3.write(A);
                    uint32_to_serial3(e);
                    uint32_to_serial3(n);
                    serverState=WAITFORACK;
                    e = ecl;
                }else{
                    serverState=LISTEN;
                }
            }else if(serverState==WAITFORACK){
                if(wait_on_serial3(1,1000)){
                    char tempRead = Serial3.read();
                    if(tempRead==C){
                        serverState=WAITINGFORKEY2;
                    }
                    if(tempRead==A){
                        serverState=DATAEXCHANGESERVER;
                    }
                }else{
                    serverState=LISTEN;
                }
            }else if(serverState==WAITINGFORKEY2){
                if(wait_on_serial3(8,1000)){
                    e = uint32_from_serial3();
                    m = uint32_from_serial3();
                    serverState=WAITFORACK;
                }else{
                    serverState=LISTEN;
                }
            }
        }
    }else{
        clientState clientState = START;
        while(clientState!=DATAEXCHANGECLIENT){
            // Serial.println("Starting...");
            if(clientState==START){
                Serial3.write(C);
                uint32_to_serial3(e);
                uint32_to_serial3(n);
                clientState = WAITINGFORACK;

            }
            if(clientState==WAITINGFORACK){
                if(wait_on_serial3(1,1000)){
                    if(Serial3.read()==A){
                        if(wait_on_serial3(8,1000)){
                            e = uint32_from_serial3();
                            m = uint32_from_serial3();
                            Serial3.write(A);
                            clientState = DATAEXCHANGECLIENT;
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
                Serial.write(decrypt( newlinechar, d , n ));
                
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

uint32_t generate_num(int n) {
	uint32_t answer=0;

	for (int i=0; i<(n-1); i++) {
		int Val= analogRead(A1);
		int partial= Val&1;
		answer= answer ^ partial<<i;
		Serial.flush();
		delay(10);
	}
	answer = answer ^ (uint32_t)1<<(n-1);
	return answer;
}

bool prime_check(uint32_t num) {
	int z= floor(sqrt(num));
	int i= 2;
	int not_p=0;
	while(i<=z) {
		if (num%i==0) {
			not_p++;
			i++;
			
		}
		if (num%i!=0) {
			i++;
		}
	}

	if (not_p==0) {
		return true;
	}
	if (not_p!=0) {
		return false;
	}
}

uint32_t generate_prime(int n) {

	bool p_check= 0;
	uint32_t p=0;
	while (p_check==0) {
		p= generate_num(n);
		p_check= prime_check(p);
	}
	return p;

}

uint32_t find_e(uint32_t phi){
	uint32_t tempNum = generate_num(15);
	uint32_t one = 1;
	while(gcd_euclid_fast(tempNum, phi)!=one){
		tempNum+=one;
	}
	return tempNum;
}

uint32_t find_d(uint32_t e, uint32_t phi) {
	long r[40];
	int32_t s[40];
	int32_t t[40];
	long q;
	s[0]=1;
	s[1]=0;
	t[0]=0;
	t[1]=1;
	r[0]= e;
	r[1]= phi;
	int i=1;
	uint32_t d;

	while (r[i]>0) {
		q = r[i-1]/r[i];          // integer division
		r[i+1] = (r[i-1] - q*r[i]);  // same as r[i-1] mod r[i]
		s[i+1] = (s[i-1] - q*s[i]);
		t[i+1] = (t[i-1] - q*t[i]);
		i = (i+1);
	}

	if(s[i-1] < 0) {
		while(s[i-1]<0){
			s[i-1]+=phi;
		}
		return d = s[i-1];
	}else if(phi < s[i-1]) {
		d = s[i-1]%phi;
		return d;
	}else if ((s[i-1] > 0) && (s[i-1]<phi) ) {
		d = s[i-1];
		return d;
	}
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

//ALL BELOW CODE OF PROVIDED BY CLASS
//computes the value x^pow mod m ("x to the power of pow" mod m)
uint32_t gcd_euclid_fast(uint32_t a, uint32_t b) {
  while (b > 0) {
    a %= b;

    // now swap them
    uint32_t tmp = a;
    a = b;
    b = tmp;
  }
  return a; // b is 0
}

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

