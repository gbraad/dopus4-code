


#ifndef I64_MATH
#define I64_MATH

typedef struct {
    unsigned hi;  
    unsigned lo;  
} bigint;



int i64_cmp(bigint n1, bigint n2);

#define I64_GREATER 1
#define I64_LESS -1
#define I64_EQUAL 0


int i64_sgn(bigint n);


bigint i64_lshift(bigint n, int b);


bigint i64_urshift(bigint n, int b);


bigint i64_srshift(bigint n, int b);


bigint i64_inv(bigint n);


bigint i64_add(bigint n1, bigint n2);


bigint i64_sub(bigint n1, bigint n2);


bigint i64_mul(bigint multiplicand, bigint multiplier);


void i64_div(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder);


void i64_udiv(bigint dividend, bigint divisor, bigint *quotient, bigint *remainder);


bigint i64_atoi(const char *str);


void i64_itoa(bigint n, char *str, int strSize);





bigint i64_set( long n1 );


bigint i64_uset( unsigned long n1 );


bigint i64_uumul( unsigned long n1, unsigned long n2 );

#endif
