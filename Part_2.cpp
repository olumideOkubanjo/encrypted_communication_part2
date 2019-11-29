#include <Arduino.h>


void setup() {
	init();
	Serial.begin(9600);
}

uint16_t generate_num(int n) {
	uint16_t answer=0;

	for (int i=0; i<(n-1); i++) {
		int Val= analogRead(A1);
		int partial= Val&1;
		answer= answer ^ partial<<i;
		Serial.flush();
		delay(10);
	}
	answer= answer ^ 1<<(n-1);
	return answer;
}
bool prime_check(uint16_t num) {
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

uint16_t generate_prime(int n) {
	bool p_check= 0;
	uint16_t p=0;
	while (p_check==0) {
		p= generate_num(n);
		p_check= prime_check(p);
	}
	return p;

}
uint16_t find_d(uint16_t e, unsigned long phi) {
	unsigned long r[40];
	int s[40];
	int t[40];
	unsigned long q;
	s[0]=1;
	s[1]=0;
	t[0]=0;
	t[1]=1;
	r[0]= e;
	r[1]= phi;
	int  i=1;
	uint16_t d;

	while (r[i]>0) {
		q = r[i-1]/r[i];          // integer division
		r[i+1] = (r[i-1] - q*r[i]);  // same as r[i-1] mod r[i]
		s[i+1] = (s[i-1] - q*s[i]);
		t[i+1] = (t[i-1] - q*t[i]);
		i = (i+1);
	}
	if (s[i-1]!= phi ) {
		d= s[i-1];
	}

	if (s[i-1]>phi) {
		uint16_t d= s[i-1]%phi;
	}
	if (s[i-1]<phi) {
		s[i-1]= s[i-1]%phi;
		uint16_t d= s[i-1]+phi;
	}
	return d;
}

int main() {
	setup();
	uint16_t p= generate_prime(15);
	uint16_t q= generate_prime(16);
	uint16_t e= generate_prime(16);
	unsigned long n= long(p)*long (q);
	unsigned long phi= long(p-1)*long (q-1);
	Serial.println("This is p: ");
	Serial.println(p);
	Serial.println("This is q: ");
	Serial.println(q);
	Serial.println("This is n: ");
	Serial.println(n);
	Serial.println("This is phi: ");
	Serial.println(phi);
	Serial.println("This is e: ");
	Serial.println(e);
	uint16_t d= find_d(e,phi);
	Serial.println("This is d: ");
	Serial.println(d);
	Serial.flush();

	return 0;
}
