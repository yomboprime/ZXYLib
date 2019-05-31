
// For browser and Node

var objectUtils = {
    copyObject: copyObject,
    iterateProperties: iterateProperties,
    countProperties: countProperties,
    extendObject: extendObject,
    isObject: isObject,
    isArray: isArray,
    isArrayBuffer: isArrayBuffer,
    isFunction: isFunction,
    isString: isString
};

if ( typeof module !== 'undefined' ) {
    module.exports = {
        copyObject: copyObject,
        iterateProperties: iterateProperties,
        countProperties: countProperties,
        extendObject: extendObject,
        isObject: isObject,
        isArray: isArray,
        isArrayBuffer: isArrayBuffer,
        isFunction: isFunction,
        isString: isString
    };
}

function copyObject( objectToCopy ) {

    if ( isObject( objectToCopy ) ) {
        return extendObject( { }, objectToCopy );
    }
    else if ( isArray( objectToCopy ) ) {
        return extendObject( [], objectToCopy );
    }
    else {
        return objectToCopy;
    }

}

function iterateProperties( object, callback ) {
    
    for ( var key in object ) {

        if ( object.hasOwnProperty( key ) ) {
        
            callback( object[ key ] );

        }
        
    }
}

function countProperties( object ) {
    
    var count = 0;

    iterateProperties( object, function( property ) {
        count++;
    } );
    
    return count;
    
}

function extendObject( original, context ) {

    // Extends original with context properties. Returns original.

    for ( var key in context ) {

        if ( context.hasOwnProperty( key ) ) {

            if ( isObject( context[ key ] ) ) {

                original[ key ] = extendObject( original[ key ] || { }, context[ key ] );

            }
            else if ( isArray( context[ key ] )  ) {

                original[ key ] = extendObject( original[ key ] || [ ], context[ key ] );

            }
            else {

                original[ key ] = context[ key ];

            }
        }
    }

    return original;

}

function isObject( object ) {
    return Object.prototype.toString.call( object ) === '[object Object]';
}

function isArray( object ) {
    return Object.prototype.toString.call( object ) === '[object Array]';
}

function isArrayBuffer( object ) {
    return Object.prototype.toString.call( object ) === '[object ArrayBuffer]';
}

function isFunction( theFunction ) {
    return Object.prototype.toString.call( theFunction ) === '[object Function]';
}

function isString( object ) {
    return Object.prototype.toString.call( object ) === '[object String]';
}