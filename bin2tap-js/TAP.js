
// ***** Libreria para trabajar con ficheros TAP de ZX Spectrum *****

if ( typeof module !== 'undefined' ) {
    module.exports = {
        crearTAPVacio: crearTAPVacio,
        cargarTAPDesdeFichero: cargarTAPDesdeFichero,
        cargarTAPDesdeBufer: cargarTAPDesdeBufer,
        nuevoBloque: nuevoBloque,
        grabarAFichero: grabarAFichero,
        grabarABufer: grabarABufer,
        TIPO_BLOQUE: getTipoBloque()
    };
}

// ***** Librerias *****

var fs = require( 'fs' );

// ***** API Publica *****

function getTipoBloque() {
    return {
        BASIC: 0,
        CODE: 1,
        SCR: 2,
        MAX: 3
    };
}

var TIPO_BLOQUE = getTipoBloque();

function crearTAPVacio() {
    return {
        bloques: []
    };
}

function cargarTAPDesdeFichero( path ) {

    // TODO

    // cargarTAPDesdeBufer( bufer )

    var tap = crearTAPVacio();

    return tap;

}

function cargarTAPDesdeBufer( bufer ) {

    // TODO

    var tap = crearTAPVacio();

    return tap;

}

function nuevoBloque( tap, tipoBloque, nombre, buferDatos, offset, longitud, param1, param2 ) {

    // Devuelve una cadena con el error, o null si se ha añadido el bloque correctamente.

    if ( nombre.length > 10 ) {
        nombre = nombre.substring( 0, 10 );
    }
    else {
        while ( nombre.length < 10 ) {
            nombre += " ";
        }
    }

    if ( ! Number.isInteger( tipoBloque ) || tipoBloque < 0 || tipoBloque >= TIPO_BLOQUE.MAX ) {
        return "Error: Tipo de bloque inválido: " + tipoBloque;
    }

    if ( ! Number.isInteger( param1 ) || param1 < 0 || param1 > 65535 ) {
        return "Error: param1 fuera de rango, tiene que estar entre 0 y 65535 inclusives. Valor param1=" + param1;
    }

    if ( ! Number.isInteger( param2 ) || param2 < 0 || param2 > 65535 ) {
        return "Error: param2 fuera de rango, tiene que estar entre 0 y 65535 inclusives. Valor param2=" + param2;
    }

    if ( longitud > 65535 ) {
        return "Error: Longitud del bloque demasiado larga ( > 65535 bytes ). Longitud: " + buferDatos.length;
    }

    if ( ! Number.isInteger( offset ) || offset < 0 ||
         ! Number.isInteger( longitud ) || longitud <= 0 ||
         offset + longitud > buferDatos.length ) {
        return "Error: Offset/longitud/tamaño de búfer inválidos.";
    }

    if ( tipoBloque === TIPO_BLOQUE.SCR ) {
        if ( param1 !== 16384 ) {
            console.log( "Aviso: Bloque SCR no tiene param1=16384 (valor param1=" + param1 + "), el valor ha sido corregido." );
            param1 = 16384;
        }
    }
    if ( tipoBloque === TIPO_BLOQUE.CODE || tipoBloque === TIPO_BLOQUE.SCR ) {
        // En este caso param2 siempre es 32768.
        // Si es BLOQUE_CODE el param1 es la direccion de memoria donde se carga el bloque.
        if ( param2 !== 32768 ) {
            console.log( "Aviso: Bloque SCR o CODE no tiene param2=32768 (valor param2=" + param2 + "), el valor ha sido corregido." );
            param2 = 32768;
        }
    }

    //if ( tipo == BLOQUE_BASIC ) {
        // En este caso param1  es el la linea de inicio, de 0 a 32767, o >= 32768 si no estaba especificada.
        // Param2 es el inicio de las variables, normalmente al final del bloque del programa (param2 === longitud)
    //}

    var buferBloqueDatos = new Buffer( longitud );

    // Copia datos al bufer del bloque
    for ( var i = 0; i < longitud; i++ ) {
        buferBloqueDatos[ i ] = buferDatos[ i + offset ];
    }

    var buferBloqueCabecera = new Buffer( 17 );

    var bloque = {
        nombre: nombre,
        tipoBloque: tipoBloque,
        param1: param1,
        param2: param2,
        buferBloqueCabecera: buferBloqueCabecera,
        buferBloqueDatos: buferBloqueDatos,
        // Buferes de salida que incluyen bytes de longitud, flag y checksum:
        buferBloqueCabeceraSalida: null,
        buferBloqueDatosSalida: null
    };

    // Rellena la cabecera
    var error = rellenarCabecera( bloque );
    if ( error ) {
        return error;
    }

    // Genera los bufers de salida
    error = generarBufersSalidaBloque( bloque );
    if ( error ) {
        return error;
    }

    tap.bloques.push( bloque );

    return null;

}

function grabarAFichero( tap, indices, path, onEnd, onErr ) {

    // onErr recibe parametro de cadena con el error

    var bufer = grabarABufer( tap, indices );

    if ( bufer === null ) {
        if ( onErr ) {
            onErr( "Error guardando fichero TAP: no hay contenido que guardar" );
        }
    }

    fs.writeFile( path, bufer, function( err ) {

        if( err ) {
            if ( onErr ) {
                onErr( "Error guardando fichero TAP: " + err + " path del fichero: " + path );
            }
        }
        else {
            if ( onEnd ) {
                onEnd();
            }
        }
    } );

}

function grabarABufer( tap, indices ) {

    // Devuelve un nuevo búfer de bytes con el contenido del tap, o null si el tap está vacío.
    // indices es un array con los indices de los bloques a grabar. Si es undefined se graban todos los bloques.

    var numBytes = 0;
    var numBloques = tap.bloques.length;

    for ( var i = 0; i < numBloques; i++ ) {
        if ( indices === undefined || i in indices ) {
            var bloque = tap.bloques[ i ];
            numBytes += bloque.buferBloqueCabeceraSalida.length;
            numBytes += bloque.buferBloqueDatosSalida.length;
        }
    }

    if ( numBytes === 0 ) {
        return null;
    }

    var bufer = new Buffer( numBytes );

    var pos =  0;
    function addBufer( b ) {
        for ( var j = 0, l = b.length; j < l; j++ ) {
            bufer[ pos++ ] = b[ j ];
        }
    }

    for ( var i = 0; i < numBloques; i++ ) {
        if ( indices === undefined || i in indices ) {
            var bloque = tap.bloques[ i ];
            addBufer( bloque.buferBloqueCabeceraSalida );
            addBufer( bloque.buferBloqueDatosSalida );
        }
    }

    return bufer;

}

// ***** API Privada *****

var OFFSETS = {
    INICIO_CABECERA: 0,
    INICIO_NOMBRE: 1,
    INICIO_LONGITUD: 11,
    INICIO_PARAM1: 13,
    INICIO_PARAM2: 15
};

function rellenarCabecera( bloque ) {

    // Devuelve cadena de error o null si ok

    switch ( bloque.tipoBloque ) {
        case TIPO_BLOQUE.BASIC:
            bloque.buferBloqueCabecera[ OFFSETS.INICIO_CABECERA ] = 0;
            break;
        case TIPO_BLOQUE.CODE:
        case TIPO_BLOQUE.SCR:
            bloque.buferBloqueCabecera[ OFFSETS.INICIO_CABECERA ] = 3;
            break;
        default:
            return "Error interno al rellenar cabecera: Valor de tipoBloque no valido. tipoBloque=" + bloque.tipoBloque;
    }

    // Rellena nombre del bloque (hasta 10 bytes)
    if ( bloque.nombre.length !== 10 ) {
        return "Error interno: el nombre del bloque no tiene 10 caracteres. Nombre=" + bloque.nombre;
    }
    for ( var i = 0; i < 10; i++ ) {
        var b = bloque.nombre.charCodeAt( i );
        if ( ! Number.isInteger( b ) || b < 0x20 || b >= 0x80 ) {
            b = 0x20;
        }
        bloque.buferBloqueCabecera[ OFFSETS.INICIO_NOMBRE + i ] = b;
    }

    // Rellena longitud del bloque de datos
    var longitud = bloque.buferBloqueDatos.length;
    var longitudLow = longitud & 0xFF;
    var longitudHigh = ( longitud & 0xFF00 ) >>> 8;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_LONGITUD ] = longitudLow;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_LONGITUD + 1 ] = longitudHigh;

    // Rellena parametros
    var param1Low = bloque.param1 & 0xFF;
    var param1High = ( bloque.param1 & 0xFF00 ) >>> 8;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_PARAM1 ] = param1Low;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_PARAM1 + 1] = param1High;

    var param2Low = bloque.param2 & 0xFF;
    var param2High = ( bloque.param2 & 0xFF00 ) >>> 8;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_PARAM2 ] = param2Low;
    bloque.buferBloqueCabecera[ OFFSETS.INICIO_PARAM2 + 1] = param2High;

    return null;

}

function generarBufersSalidaBloque( bloque ) {

    bloque.buferBloqueCabeceraSalida = generarBuferSalida( 0x00, bloque.buferBloqueCabecera );
    bloque.buferBloqueDatosSalida = generarBuferSalida( 0xFF, bloque.buferBloqueDatos );

}

function generarBuferSalida( flag, buferBytes ) {

    // flag: 0x00 para el bloque de cabecera, 0xFF para el bloque de datos

    var longitud = buferBytes.length;
    var numBytes = longitud + 2;

    var buferSalida = new Buffer( longitud + 4 );

    var numBytesLow = numBytes & 0xFF;
    var numBytesHigh = ( numBytes & 0xFF00 ) >>> 8;

    var pos = 0;

    // Escribe dos bytes con el numero de bytes
    buferSalida[ pos++ ] = numBytesLow;
    buferSalida[ pos++ ] = numBytesHigh;

    // Escribe byte de flag
    buferSalida[ pos++ ] = flag;

    // Escribe datos a la vez que calcula byte de checksum
    var checksum = flag;
    for ( var i = 0; i < longitud; i++ ) {
        var b = buferBytes[ i ];
        buferSalida[ pos++ ] = b;
        checksum ^= b;
    }

    // Escribe byte de checksum
    buferSalida[ pos++ ] = checksum;
    
    return buferSalida;
}
