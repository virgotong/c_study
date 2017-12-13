#ifndef _COMMON_UTILITY_H_
#define _COMMON_UTILITY_H_

#define SWAP_VALUES(a, b)	do{(a) -= (b); (b) += (a); (a) = (b) - (a);}	\
	while(0);

typedef unsigned char 	BOOL;
#define INLINE 			inline

/* my_is_digit -- check if the char is digit */
static INLINE BOOL my_is_digit( int c )
{
	return	( c >= '0' && c <= '9' );
}

/* my_is_hex_digit -- check if the char is hex digit */
static INLINE BOOL my_is_hex_digit( int c )
{
	return	( c >= '0' && c <= '9' ) ||
			( c >= 'a' && c <= 'f' ) ||
			( c >= 'A' && c <= 'F' );
}

/* my_is_alpha -- check if the char is alpha */
static INLINE BOOL my_is_alpha( int c )
{
	return	( c >= 'a' && c <= 'z' ) ||
			( c >= 'A' && c <= 'Z' );
}

/* my_is_alpha_digit -- check if the char is alpha or digit */
static INLINE BOOL my_is_alpha_digit( int c )
{
	return	( c >= '0' && c <= '9' ) ||
			( c >= 'a' && c <= 'z' ) ||
			( c >= 'A' && c <= 'Z' );
}

/* my_locase -- convert character to lower case */
static INLINE int my_locase( int c )
{
	return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

/* my_upcase -- convert character to upcase case */
static INLINE int my_upcase( int c )
{
	return c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c;
}

/* my_hex_value -- get hex value of the char */
static INLINE int my_hex_value( int c )
{
	if( c >= '0' && c <= '9' )
		return ( int )( c - '0' );
	else if( c >= 'a' && c <= 'z' )
		return ( int )( c - ( 'a' - 10 ) );
	else if( c >= 'A' && c <= 'Z' )
		return ( int )( c - ( 'A' - 10 ) );
	else
		return -1;
}

#endif

