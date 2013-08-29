#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <CoreServices/CoreServices.h>
#include "mach_override.h"

// A simple extension to assert that can print expectation information:
#define assertf(CONDITION, FORMAT, ...) \
	if (!(CONDITION)) { \
		printf((FORMAT), ##__VA_ARGS__); \
		assert(CONDITION); \
	}

#define string_equals(s1, s2) \
	({ \
		strcmp(s1, s2) == 0; \
	})

#define	assertStrEqual(EXPECTED, ACTUAL) \
	assertf(string_equals(EXPECTED, ACTUAL), \
		"EXPECTED: %s\nACTUAL: %s\n", \
		EXPECTED, \
		ACTUAL);

#define	assertStr2Equal(EXPECTED1, EXPECTED2, ACTUAL) \
	assertf(string_equals(EXPECTED1, ACTUAL) || string_equals(EXPECTED2, ACTUAL), \
		"EXPECTED: %s OR %s\nACTUAL: %s\n", \
		EXPECTED1, \
		EXPECTED2, \
		ACTUAL);

#define	assertIntEqual(EXPECTED, ACTUAL) \
	assertf(EXPECTED == ACTUAL, \
		"EXPECTED: %d\nACTUAL: %d\n", \
		EXPECTED, \
		ACTUAL);

//------------------------------------------------------------------------------
#pragma mark Test Local Override by Pointer

const char* localFunction() {
	asm("nop;nop;nop;nop;");
	return __FUNCTION__;
}
const char* (*localOriginalPtr)() = localFunction;

void testLocalFunctionOverrideByPointer() {
	//	Test original.
	assertStrEqual( "localFunction", localOriginalPtr() );

	//	Override local function by pointer.
	kern_return_t err;
	
	MACH_OVERRIDE( const char*, localFunction, (), err ) {
		//	Test calling through the reentry island back into the original
		//	implementation.
		assertStrEqual( "localFunction", localFunction_reenter() );
		
		return "localFunctionOverride";
	} END_MACH_OVERRIDE(localFunction);
	assert( !err );
	
	//	Test override took effect.
	assertStrEqual( "localFunctionOverride", localOriginalPtr() );
}

//------------------------------------------------------------------------------
#pragma mark Test System Override by Pointer

char* (*strerrorPtr)(int) = strerror;
const char* strerrReturnValueOn10_6 = "Unknown error: 0";
const char* strerrReturnValueOn10_7 = "Undefined error: 0";

void testSystemFunctionOverrideByPointer() {
	//	Test original.
	assertStr2Equal(strerrReturnValueOn10_6,
		strerrReturnValueOn10_7,
		strerrorPtr(0));
	
	//	Override system function by pointer.
	kern_return_t err;
	MACH_OVERRIDE(char*, strerror, (int errnum), err) {
		//	Test calling through the reentry island back into the original
		//	implementation.
		assertStr2Equal(strerrReturnValueOn10_6,
			strerrReturnValueOn10_7,
			strerror_reenter(0));
		
		return (char*)"strerrorOverride";
	} END_MACH_OVERRIDE(strerror);
	assert(!err);
	
	//	Test override took effect.
	assertStrEqual("strerrorOverride", strerrorPtr(0));
}

//------------------------------------------------------------------------------
#pragma mark Test System Override by Name

/* The following is commented out because it does not compile.
int strerror_rOverride( int errnum, char *strerrbuf, size_t buflen );
int (*strerror_rPtr)( int, char*, size_t ) = strerror_r;
int (*gReentry_strerror_r)( int, char*, size_t );

void testSystemFunctionOverrideByName() {
	//	Test original.
	assertIntEqual( ERANGE, strerror_rPtr( 0, NULL, 0 ) );
	
	//	Override local function by pointer.
	kern_return_t err = mach_override( (char*)"_strerror_r",
									   NULL,
									   (void*)&strerror_rOverride,
									   (void**)&gReentry_strerror_r );
	
	//	Test override took effect.
	assertIntEqual( 0, strerror_rPtr( 0, NULL, 0 ) );
}

int strerror_rOverride( int errnum, char *strerrbuf, size_t buflen ) {
	assertIntEqual( ERANGE, gReentry_strerror_r( 0, NULL, 0 ) );
	
	return 0;
}
*/

//------------------------------------------------------------------------------
#pragma mark main

int main( int argc, const char *argv[] ) {
	testLocalFunctionOverrideByPointer();
	testSystemFunctionOverrideByPointer();
	//testSystemFunctionOverrideByName();
	
	printf( "success\n" );
	return 0;
}
