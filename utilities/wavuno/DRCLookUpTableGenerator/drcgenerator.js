
var fs = require( 'fs' );

console.log( "");

if ( process.argv.length !== 3 ) {
	printUsage();
	process.exit( -1 );
}

var parameterThreshold = process.argv[ 2 ];

if ( isNaN( parseFloat( parameterThreshold ) ) ) {
	printUsage();
	process.exit( -1 );
}

parameterThreshold = Math.floor( parameterThreshold );

if ( parameterThreshold < 0 || parameterThreshold > 255 ) {
	console.log( "Error: threshold must be between 0 and 255" );
	console.log( "" );
	printUsage();
	process.exit( -1 );
}

// Main code

// Find out the value of alpha to match the linear and logarithmic parts of the function
var alphaLower = 2.5;
var alphaHigher = 0;
var alpha = 0;
var precision = 0.01;


var alpha = 5.71144;
parameterThreshold = 128;

// Compute the look up table
var theLookupFunction = [];

for ( var i = 0; i < 256; i++ ) {

	var x = i * 2 / 255;
	
	var f = compressorFunction( x, parameterThreshold / 256, alpha );
	
	theLookupFunction[ i ] = Math.floor( f * 126 ) + 1;
	
	//console.log( "f( " + x + ") = " + theLookupFunction[ i ] );
	console.log( i + " " + x + " " + theLookupFunction[ i ] );

}

// Prepare output
var text = "";
for ( var i = 0; i < 256; i++ ) {
	text += theLookupFunction[ i ].toString( 16 ) + "\n";
}

// Save output file
saveTextFile( text, __dirname + "/output.txt", function() {
	console.log( "Output file written successfully." );
} );

// End of main code


function compressorFunction( x, t, a ) {
	
	if ( x > t ) {
		return Math.sign( x ) * (
			t + ( 1 - t ) * 
			Math.log( 1 + a * ( Math.abs( x ) - t ) / ( 2 - t ) ) /
			Math.log( 1 + a )
		);
	}
	else {
		return x;
	}
	
}

function printUsage() {
	console.log( "Usage: node drcgenerator.js <threshold>" );
	console.log( "    <threshold>: Integer number between 0 and 255" );
	console.log( "" );
}

function saveTextFile( text, path, onEnd ) {

    fs.writeFile( path, text, function( err ) {

        if ( err ) {
            console.error( "Error saving text file: " + err + " file name: " + path );
            process.exit( -1 );
        }

        if ( onEnd ) {
            onEnd();
        }

    } );
}
