
var fs = require( 'fs' );
var Canvas = require( 'canvas' );
var Image = Canvas.Image;

console.log( "ConvertirImagenRadas");
console.log( "Uso: node convertirImagenRadas.js <path_relativo_imagen_entrada> <path_relativo_imagen_salida>" );
console.log( "" );

var pathImagenEntrada = __dirname + "/" + process.argv[ 2 ];
var pathImagenSalida = __dirname + "/" + process.argv[ 3 ];

fs.readFile( pathImagenEntrada, function( err, buferImagen ) {

    if ( err ) {
        console.error( "Hubo un error al leer la imagen en: " + pathImagen );
        throw err;
    }

    var img = new Image;
    img.src = buferImagen;
    var tx = img.width;
    var ty = img.height;


	var elCanvas = new Canvas( tx, ty );
	var ctx2d = elCanvas.getContext( '2d' );
	ctx2d.drawImage( img, 0, 0 );

	var datosImagen = ctx2d.getImageData( 0, 0, tx, ty );
	var datosPixels = datosImagen.data;

	function getPixelPR( p ) {
		return datosPixels[ p ];
	}

	function getPixelPG( p ) {
		return datosPixels[ p + 1 ];
	}

	function getPixelPB( p ) {
		return datosPixels[ p + 2 ];
	}

	function getPixelPA( p ) {
		return datosPixels[ p + 3 ];
	}

	function getPixelP( x, y ) {
		return 4 * ( Math.round( x ) + Math.round( y ) * tx );
	}

	function setPixel( x, y, r, g, b, a ) {
		var p = getPixelP( x, y );
		datosPixels[ p ] = r;
		datosPixels[ p + 1 ] = g;
		datosPixels[ p + 2 ] = b;
		datosPixels[ p + 3 ] = a;
	}

	function convertirColorRGBARadasRGB332( r, g, b ) {

		// r, g, b de 0 a 255

		var rr1 = ( r & 0xE0 ) >> 5;
		var rr = Math.floor( rr1 * 255 / 7 );
		var gr1 = ( g & 0xE0 ) >> 5;
		var gr = Math.floor( gr1 * 255 / 7 );
		var br1 = ( b & 0xC0 ) >> 6;
		var br = Math.floor( Math.floor( br1 * 7 / 3 ) * 255 / 7 );

		return [ rr, gr, br ];
	}

	for ( var x = 0; x < tx; x ++ ) {
		for ( var y = 0; y < ty; y++ ) {

			var p0 = getPixelP( x, y );
			var colorRadas = convertirColorRGBARadasRGB332( getPixelPR( p0 ), getPixelPG( p0 ), getPixelPB( p0 ) );
			setPixel( x, y, colorRadas[0], colorRadas[1], colorRadas[2], 255 );
			//setPixel( x, y, x, y, 127, 255 );

		}
	}

	ctx2d.putImageData( datosImagen, 0, 0 );

	grabarImagenPNG( elCanvas, pathImagenSalida );

} );

function grabarImagenPNG( canvas, pathImagen ) {

    var out = fs.createWriteStream( pathImagen );
    var stream = canvas.createPNGStream();

    stream.on( 'data', function( chunk ) {
        out.write( chunk );
    } );

}

function grabarFicheroTexto( texto, path, onEnd ) {

    fs.writeFile( path, texto, function( err ) {

        if( err ) {
            console.error( "Error guardando fichero de texto: " + err + " fichero: " + path );
            process.exit( -1 );
        }

        if ( onEnd ) {
            onEnd();
        }

    } );
}
