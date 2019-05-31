
var UARTController = function() {

    // Libraries
    this.SerialPort = require( 'serialport' );
    

    this.config = null;

    this.started = false;
    this.serialPortIsOpen = false;
    this.serialPort = null;


};

// ***** Exports *****

if ( typeof module !== 'undefined' ) {

    module.exports = UARTController;

}

// ***** UARTController class *****

UARTController.prototype = {

    constructor: UARTController

};

UARTController.prototype.start = function( config ) {
    
    if ( this.started ) {
        return false;
    }
    
    this.config = config;
    
    this.started = true;
    
    this.openSerialPort();
    
    return true;

};

UARTController.prototype.finish = function( config ) {
    if ( this.started && this.serialPortIsOpen ) {
        this.serialPort.close();
    }
};

UARTController.prototype.openSerialPort = function() {
    
    if ( this.serialPort || this.serialPortIsOpen ) {
        return false;
    }

    var scope = this;
    this.serialPort = new this.SerialPort( this.config.deviceName, {
        baudRate: this.config.bauds
    }, function (err) {

        if ( err ) {
            console.log( "Error opening serial port: " + scope.config.deviceName + ", Error: " + err );
            // TODO Trying to open in ... seconds
            //scope.finish();
            return;
        }

        scope.serialPortIsOpen = true;

        scope.serialPort.on( 'close', function() {
            console.log( "Serial port closed: " + scope.config.deviceName );
            scope.serialPortIsOpen = false;
            // TODO Trying to open in ... seconds
            //scope.finish();
        } );

        console.log( "Serial port opened: " + scope.config.deviceName );

    } );

    var parser = new this.SerialPort.parsers.ByteLength( { length: 1 } );
    this.serialPort.pipe( parser );
    parser.on( 'data', function( data ) {
        
        console.log( "Data: " + data );
        
        aqui se lee un paquete y se envia al servicehub donde se llama al servicio registrado y se devuelve el paquete de respuesta a enviar
        
    } );
    
    return true;
    
};
