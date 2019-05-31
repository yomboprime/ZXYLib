/*
 * @author yomboprime
 *
 * 
 * Test coprocesador
*/
#include <stdio.h>
#include <stdlib.h>
#include <input.h>
#include <math.h>

#include "../src/textUtils.h"
#include "../src/zxuno/coprocessor.h"
#include "../src/zxuno/turbo.h"

#define getKey() in_Inkey()

#define DECIMAL_PLACES 7
#define TO_FIXED( x ) ( x << DECIMAL_PLACES )
#define FROM_FIXED( x ) ( x >> DECIMAL_PLACES )

uint32_t getFrames();

void main(void) {

    uint8_t key;
    
    int16_t result1 = 0;
    int16_t result2 = 0;
    int16_t a, b, c;
    
    uint16_t i;
    
    uint32_t t, t2;
    
    float tf;
    
    uint8_t cadena[120];
    
    TURBO_set( ZXUNO_TURBO_X8 );

    textUtils_32ColumnsMode();
    textUtils_cls();

    textUtils_println( "Coprocessor test. Operation = a * b + c" );
    textUtils_println( "" );


    a = TO_FIXED( 100 );
    b = TO_FIXED( 2 );
    c = TO_FIXED( 34 );

    textUtils_print( "a = " );
    itoa( FROM_FIXED( a ), cadena, 10 );
    textUtils_println( cadena );
    
    textUtils_print( "b = " );
    itoa( FROM_FIXED( b ), cadena, 10 );
    textUtils_println( cadena );

    textUtils_print( "c = " );
    itoa( FROM_FIXED( c ), cadena, 10 );
    textUtils_println( cadena );
    
    textUtils_println( "" );
    textUtils_println( "Testing software Z80..." );
    
    result1 = ( ( ((int32_t)( a )) * ((int32_t)( b )) ) >> DECIMAL_PLACES ) + ((int32_t)( c ));
    result1 = FROM_FIXED( result1 );
    
    textUtils_print( "Result = " );
    itoa( result1, cadena, 10 );
    textUtils_println( cadena );
    
    textUtils_println( "" );
    textUtils_println( "Testing coprocessor..." );
    
    //result2 = coprocessorMultAdd( a, b, c ); 
    //result2 = FROM_FIXED( result2 );

    coprocessorMultAddInit();
    result2 = coprocessorMultAddOptimized( c, b, a );
    result2 = FROM_FIXED( result2 );

    textUtils_print( "Result = " );
    itoa( result2, cadena, 10 );
    textUtils_println( cadena );

    if ( result1 == result2 ) {
        textUtils_println( "Results DO match." );
    }
    else {
        textUtils_println( "Results DON'T match." );
    }
    
    textUtils_println( "Testing ops/s..." );
    
    coprocessorMultAddInit();

    #define NUM_OPS 30000
    t = getFrames();
    for ( i = 0; i < NUM_OPS; i ++ ) {

        result2 = coprocessorMultAddOptimized( c, b, a );

    }
    t2 = getFrames();
    
    tf = t2 - t;
    tf = ((float)NUM_OPS) * 50.0 / tf;

    textUtils_print( "Ops/s = " );
    itoa( floor( tf ), cadena, 10 );
    textUtils_println( cadena );

    textUtils_println( "" );
    textUtils_println( "End. Press a key." );
    
    TURBO_set( ZXUNO_TURBO_X1 );
    
    waitKey();
}

uint32_t getFrames() {
    return *((uint32_t *)23672);
}
