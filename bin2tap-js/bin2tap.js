
// Se acepta un parametro, el path relativo de un fichero binario para exportar como fichero TAP de código.
// El fichero puede tener entre 1 y 65535 bytes de longitud


// ***** Librerias *****

var fs = require( 'fs' );
var TAP = require( './TAP.js' );





console.log( "bin2tap-js v1.0.0");
console.log( "Uso: node bin2tap.js <path_relativo_fichero_binario>" );
console.log( "" );

var pathFicheroEntrada = __dirname + "/" + process.argv[ 2 ];

console.log( "Leyendo fichero binario en: " + pathFicheroEntrada );

fs.readFile( pathFicheroEntrada, function( err, datosFichero ) {

    if ( err ) {
        console.error( "Hubo un error al leer el fichero en: " + pathFicheroEntrada );
        process.exit( -1 );
    }

    console.log( "Fichero cargado. Procesando..." );

    var tap = TAP.crearTAPVacio();

    var error = TAP.nuevoBloque( tap, TAP.TIPO_BLOQUE.CODE, "code", datosFichero, 0, datosFichero.length, 32768, 32768 );

    if ( error ) {
        console.error( error );
        process.exit( -1 );
    }

    var nombreTAP = pathFicheroEntrada;
    var lastIndexOfDot = nombreTAP.lastIndexOf( '.' );
    if ( lastIndexOfDot > 0 && nombreTAP.lastIndexOf( '/' ) < lastIndexOfDot ) {
        nombreTAP = nombreTAP.substring( 0, lastIndexOfDot ) + ".tap";
    }
    else {
        nombreTAP += ".tap";
    }

    console.log( "Guardando fichero TAP en " + nombreTAP );

    TAP.grabarAFichero( tap, undefined, nombreTAP, function() {
        console.log( "Fichero TAP guardado." );
    }, function( err ) {
        console.error( err );
        process.exit( -1 );
    } );


} );
