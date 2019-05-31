
var UARTController = require( "./lib/8bnet/uartcontroller.js" );
var path = require( 'path' );
var loadJSONFileSync = require( './lib/yombo/fileUtils/fileUtils.js' ).loadJSONFileSync;

var config;
var uartController;


main();


function main() {

     var title = "8bnet r1.0";
    process.title = title;
    console.log( title );
    
    uartController = new UARTController();

    // Termination signal
    process.on( "SIGINT", function() {
        console.log( " SIGINT Signal Received, shutting down." );
        finish();
    } );

    // Load config
    config = loadConfig();
    if ( config === null ) {
        return;
    }

    uartController.start( config.serial );
    
}

function finish() {

    console.log( "Closing application." );
    
    if ( uartController.started ) {
        uartController.finish();
    }

    process.exit( 0 );

}

 function loadConfig() {

    var p = "./config.json";

    console.log( "Loading config file in: " + p );

    var config = loadJSONFileSync( path.join( __dirname,  p ) );

    if ( ! config ) {
        console.log( "Error while loading config file in: " + p );
        return null;
    }

    return config;

};
