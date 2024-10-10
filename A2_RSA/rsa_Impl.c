#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>

#define	MAXDIGITS	10000		/* maximum length bignum */

#define PLUS		1		/* positive sign bit */
#define MINUS		-1		/* negative sign bit */

clock_t start,end;
double cpu_time_used;
void encrypt();
void decrypt1();
void decrypt2();

typedef struct {
        char digits[MAXDIGITS];         /* represent the number */
	int signbit;			/* 1 if positive, -1 if negative */
        int lastdigit;			/* index of high-order digit */
} bignum;
bignum p,q,n,d,j,m,i,c,e,output,d1,dx,one,zero;


void print_bignum(bignum *n)
{
	int i;

	if (n->signbit == MINUS) printf("- ");
	for (i=n->lastdigit; i>=0; i--)
		printf("%c",'0'+ n->digits[i]);

	printf("\n");
}

void int_to_bignum(int s, bignum *n)
{
	int i;				/* counter */
	int t;				/* int to work with */

	if (s >= 0) n->signbit = PLUS;
	else n->signbit = MINUS;

	for (i=0; i<MAXDIGITS; i++) n->digits[i] = (char) 0;

	n->lastdigit = -1;

	t = abs(s);

	while (t > 0) {
		n->lastdigit ++;
		n->digits[ n->lastdigit ] = (t % 10);
		t = t / 10;
	}

	if (s == 0) n->lastdigit = 0;
}

void initialize_bignum(bignum *n)
{
	int_to_bignum(0,n);
}


int max(int a, int b)
{
	if (a > b) return(a); else return(b);
}


void add_bignum(bignum *a, bignum *b, bignum *c)
{
	int carry;			/* carry digit */
	int i;				/* counter */

	initialize_bignum(c);

	if (a->signbit == b->signbit) c->signbit = a->signbit;
	else {
		if (a->signbit == MINUS) {
			a->signbit = PLUS;
			subtract_bignum(b,a,c);
			a->signbit = MINUS;
		} else {
                        b->signbit = PLUS;
                        subtract_bignum(a,b,c);
                        b->signbit = MINUS;
		}
		return;
	}

	c->lastdigit = max(a->lastdigit,b->lastdigit)+1;
	carry = 0;

	for (i=0; i<=(c->lastdigit); i++) {
		c->digits[i] = (char) (carry+a->digits[i]+b->digits[i]) % 10;
		carry = (carry + a->digits[i] + b->digits[i]) / 10;
	}

	zero_justify(c);
}


void subtract_bignum(bignum *a, bignum *b, bignum *c)
{
	int borrow;
	int v;
	int i;

	initialize_bignum(c);

	if ((a->signbit == MINUS) || (b->signbit == MINUS)) {
                b->signbit = -1 * b->signbit;
                add_bignum(a,b,c);
                b->signbit = -1 * b->signbit;
		return;
        }

	if (compare_bignum(a,b) == PLUS) {
		subtract_bignum(b,a,c);
		c->signbit = MINUS;
		return;
	}

        c->lastdigit = max(a->lastdigit,b->lastdigit);
        borrow = 0;

        for (i=0; i<=(c->lastdigit); i++) {
		v = (a->digits[i] - borrow - b->digits[i]);
		if (a->digits[i] > 0)
			borrow = 0;
		if (v < 0) {
			v = v + 10;
			borrow = 1;
		}

                c->digits[i] = (char) v % 10;
        }

	zero_justify(c);
}

int compare_bignum(bignum *a, bignum *b)
{
	int i;				/* counter */

	if ((a->signbit == MINUS) && (b->signbit == PLUS)) return(PLUS);
	if ((a->signbit == PLUS) && (b->signbit == MINUS)) return(MINUS);

	if (b->lastdigit > a->lastdigit) return (PLUS * a->signbit);
	if (a->lastdigit > b->lastdigit) return (MINUS * a->signbit);

	for (i = a->lastdigit; i>=0; i--) {
		if (a->digits[i] > b->digits[i]) return(MINUS * a->signbit);
		if (b->digits[i] > a->digits[i]) return(PLUS * a->signbit);
	}

	return(0);
}

void zero_justify(bignum *n)
{
	while ((n->lastdigit > 0) && (n->digits[ n->lastdigit ] == 0))
		n->lastdigit --;

        if ((n->lastdigit == 0) && (n->digits[0] == 0))
		n->signbit = PLUS;
}


void digit_shift(bignum *n, int d)
{
	int i;

	if ((n->lastdigit == 0) && (n->digits[0] == 0)) return;

	for (i=n->lastdigit; i>=0; i--)
		n->digits[i+d] = n->digits[i];

	for (i=0; i<d; i++) n->digits[i] = 0;

	n->lastdigit = n->lastdigit + d;
}



void multiply_bignum(bignum *a, bignum *b, bignum *c)
{
	bignum row;			/* represent shifted row */
	bignum tmp;			/* placeholder bignum */
	int i,j;			/* counters */

	initialize_bignum(c);

	row = *a;

	for (i=0; i<=b->lastdigit; i++) {
		for (j=1; j<=b->digits[i]; j++) {
			add_bignum(c,&row,&tmp);
			*c = tmp;
		}
		digit_shift(&row,1);
	}

	c->signbit = a->signbit * b->signbit;

	zero_justify(c);
}


void divide_bignum(bignum *a, bignum *b, bignum *c)
{
        bignum row;                     /* represent shifted row */
        bignum tmp;                     /* placeholder bignum */
	int asign, bsign;		/* temporary signs */
        int i,j;                        /* counters */

	initialize_bignum(c);

	c->signbit = a->signbit * b->signbit;

	asign = a->signbit;
	bsign = b->signbit;

	a->signbit = PLUS;
        b->signbit = PLUS;

	initialize_bignum(&row);
	initialize_bignum(&tmp);

	c->lastdigit = a->lastdigit;

	for (i=a->lastdigit; i>=0; i--) {
		digit_shift(&row,1);
		row.digits[0] = a->digits[i];
		c->digits[i] = 0;
		while (compare_bignum(&row,b) != PLUS) {
			c->digits[i] ++;
			subtract_bignum(&row,b,&tmp);
			row = tmp;
		}
	}

	zero_justify(c);

	a->signbit = asign;
	b->signbit = bsign;
}
void modulo(bignum *num, bignum *divisor, bignum *remainder)
{   bignum nx1,nx2;
    initialize_bignum(&nx1);
    initialize_bignum(&nx2);
    divide_bignum(num,divisor,&nx1);
    multiply_bignum(divisor,&nx1,&nx2);
    subtract_bignum(num,&nx2,remainder);

}
void power(bignum *base, bignum *exponent, bignum *result){
    bignum zero,one,nx1,nx2;
    int_to_bignum(0,&zero);
    int_to_bignum(1,&one);
    int_to_bignum(1,&nx2);
    nx1=*exponent;
while (compare_bignum(&zero, exponent) != 0) {
        multiply_bignum(&nx2,base,result);
        subtract_bignum(&nx1,&one,exponent);
        nx1=*exponent;
        nx2=*result;
    }
}

void gcdExtended(bignum a,bignum b,bignum *s,bignum *t)
{
     bignum zero,one;
    int_to_bignum(0,&zero);
    if(compare_bignum(&zero,&a) ==0){

    	int_to_bignum(1,&one);
    	*s = zero;
    	*t = one;
    	return;
    }
    bignum x1,y1;

    bignum b1 = b;
    bignum a1 = a;
    bignum c;
    modulo(&b1,&a1,&c);
    gcdExtended(c,a,&x1,&y1);
    bignum b2 = b;
    bignum a2 = a;
    bignum temp1,temp2;
    divide_bignum(&b2,&a2,&temp2);
    multiply_bignum(&temp2,&x1,&temp1);
    subtract_bignum(&y1,&temp1,s);
    *t= x1;


}

bignum modInverse(bignum a,bignum m)
{
    bignum s,t,res;
    gcdExtended(a,m,&s,&t);
    bignum temp1,temp2;
    bignum m2 = m;
    modulo(&s,&m2,&temp2);
    add_bignum(&temp2,&m,&temp1);
    bignum m1 = m;
    modulo(&temp1,&m1,&res);
    return res;
}

void encrypt(bignum *e,bignum *m){
	bignum key,pt,cx;
	pt=*m;
	key = *e;
	power(&pt,&key,&cx);
	bignum ntemp = n;
	modulo(&cx,&ntemp,&c);
	printf("The number after encryption is :\n");
	print_bignum(&c);
}

void decrypt1() {
	bignum ct,key,mx,message,ctemp=c,ctemp1,ctemp2,ntemp;
	ct = one;
	key = d;
	int_to_bignum(0,&zero);
	bignum keytemp = key;
	while(compare_bignum(&key,&zero) != 0) {
		subtract_bignum(&keytemp,&one,&key);
		multiply_bignum(&ct,&ctemp,&ctemp2);
		ntemp = n;
		modulo(&ctemp2,&ntemp,&ct);
		keytemp = key;
	}
	printf("The number after decryption is :\n");
	print_bignum(&ct);
}
void decrypt2(bignum d1, bignum d2, bignum xp, bignum xq) {
	bignum c1,c2,ptemp,qtemp,ctemp,key,keytemp,vp,vptemp1,vptemp2,c1temp,c2temp,vq,vqtemp1,vqtemp2,op;
	ctemp = c;
	vp = one;
	vq = one;
	ptemp = p;
	modulo(&ctemp,&ptemp,&c1);
	ctemp = c;
	qtemp = q;
	modulo(&ctemp,&qtemp,&c2);
	key = d1;
	int_to_bignum(0,&zero);
	keytemp = key;
	while(compare_bignum(&key,&zero) != 0) {
		subtract_bignum(&keytemp,&one,&key);
		vptemp1 = vp;
		c1temp = c1;
		multiply_bignum(&vptemp1,&c1temp,&vptemp2);
		ptemp = p;
		modulo(&vptemp2,&ptemp,&vp);
		keytemp = key;
	}
	key = d2;
	keytemp = key;
	while(compare_bignum(&key,&zero) != 0) {
		subtract_bignum(&keytemp,&one,&key);
		vqtemp1 = vq;
		c2temp = c2;
		multiply_bignum(&vqtemp1,&c2temp,&vqtemp2);
		qtemp = q;
		modulo(&vqtemp2,&qtemp,&vq);
		keytemp = key;
	}
	vptemp1 = vp;
	vqtemp1 = vq;
	bignum result1,result2,result3;
	multiply_bignum(&xp,&vptemp1,&result2);
	multiply_bignum(&xq,&vqtemp1,&result3);
	add_bignum(&result2,&result3,&result1);
	modulo(&result1,&n,&op);
	printf("\nUsing the 2nd method, the decrypted number is :\n");
	print_bignum(&op);
}

int main() {
	int a;
	bignum n1;
	printf("Please input a number (plaintext) :");
	scanf("%d",&a);
	int_to_bignum(a,&n1);
	int_to_bignum(173,&p);
	int_to_bignum(149,&q);
	int_to_bignum(3,&e);
	int_to_bignum(16971,&d);
	int_to_bignum(1,&one);
	bignum ptemp = p;
	bignum qtemp = q;
	multiply_bignum(&ptemp,&qtemp,&n);
	encrypt(&e,&n1);
	start = clock();
	decrypt1();
	end = clock();
	cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
	printf("The total time taken by 1st method is : %lf s",cpu_time_used);
	bignum d1,d2,xp,xq;
	subtract_bignum(&p,&one,&ptemp);
	subtract_bignum(&q,&one,&qtemp);
	modulo(&d,&ptemp,&d1);
	modulo(&d,&qtemp,&d2);
	bignum modInverse1 = modInverse(q,p);
	bignum modInverse2 = modInverse(p,q);
	multiply_bignum(&q,&modInverse1,&xp);
	multiply_bignum(&p,&modInverse2,&xq);
	start = clock();
	decrypt2(d1,d2,xp,xq);
	end = clock();
	cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
	printf("The total time taken by 2nd method is : %lf s\n",cpu_time_used);
	return 0;
}

