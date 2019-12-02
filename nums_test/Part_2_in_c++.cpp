#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
using namespace std;

void setup() {
}

uint32_t generate_num(int n) {
	uint32_t answer=0;
	srand(time(0));
	for (int i=0; i<(n-1); i++) {
		//srand(time(0));
		int Val= rand();
		//cout<< Val<<endl;
		int partial= Val&1;
		answer= answer ^ partial<<i;
		
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
	long s[40];
	long t[40];
	unsigned long q;
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
		s[i-1] = s[i-1]%phi;
		cout<< s[i-1]<<endl;
		d = s[i-1]+phi;
		return d;
	}
	else if(phi < s[i-1]) {
		 d = s[i-1]%phi;
		return d;
	}
	else if ((s[i-1] > 0) && (s[i-1]<phi) ) {
		d = s[i-1];
		return d;
	}
}

int main() {
	setup();
	uint32_t one = 1;

	//P generation
	cout << "This is p: "<<endl;
	uint32_t p=0;
	p = generate_prime(15);
	cout << p <<endl;

	//Q generation
	cout << "This is q: "<<endl;
	uint32_t q=0;
	q = generate_prime(16);
	cout << q<<endl;

	//n generation
	cout << "This is n: "<<endl;
	uint32_t n=0;
	n = (p) * (q);
	cout << n <<endl;

	//Phi generation
	cout << "This is phi: "<<endl;
	uint32_t phi=0;
	phi= (p-one)*(q-one);
	cout << phi <<endl;

	cout << "This is e: "<<endl;
	uint32_t e=0;
	e = find_e(phi);
	cout << e <<endl;
	uint32_t d=0;
	d = find_d(e,phi);
	cout << "This is d: "<<endl;
	cout << d<<endl;
	return 0;
}