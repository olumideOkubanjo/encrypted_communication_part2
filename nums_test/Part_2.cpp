#include <Arduino.h>


void setup() {
	init();
	Serial.begin(9600);
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

uint32_t find_e(uint32_t phi){
	uint32_t tempNum = generate_num(15);
	uint32_t one = 1;
	while(gcd_euclid_fast(tempNum, phi)!=one){
		tempNum+=one;
	}
	return tempNum;
}

uint32_t find_d(uint32_t e, uint32_t phi) {
	unsigned long r[40];
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
	Serial.print("s[i-1] : ");
	Serial.println(s[i-1]);

	if(s[i-1] < 0) {
		while(s[i-1]<0){
			s[i-1]+=phi;
		}
		return d = s[i-1];
	}else if(phi < s[i-1]) {
		Serial.print("Second if :");
		 d = s[i-1]%phi;
		return d;
	}else if ((s[i-1] > 0) && (s[i-1]<phi) ) {
		Serial.print("Third OF if :");
		d = s[i-1];
		return d;
	}
}

int main() {
	setup();
	uint32_t one = 1;

	//P generation
	Serial.println("This is p: ");
	uint32_t p = generate_prime(15);
	Serial.println(p);

	//Q generation
	Serial.println("This is q: ");
	uint32_t q = generate_prime(16);
	Serial.println(q);

	//n generation
	Serial.println("This is n: ");
	uint32_t n = (p) * (q);
	Serial.println(n);

	//Phi generation
	Serial.println("This is phi: ");
	uint32_t phi= (p-one)*(q-one);
	Serial.println(phi);

	Serial.println("This is e: ");
	uint32_t e = find_e(phi);
	Serial.println(e);

	uint32_t d = find_d(e,phi);
	Serial.println("This is d: ");
	Serial.println(d);


	Serial.flush();

	return 0;
}
