
// For Node only

var fs = require( "fs" );


if ( typeof module !== 'undefined' ) {
    module.exports = {
        loadJSONFileSync: loadJSONFileSync,
        writeJSONFileSync: writeJSONFileSync,
        getObjectSerializedAsString: getObjectSerializedAsString,
        loadTextFileSync: loadTextFileSync,
        writeTextFileSync: writeTextFileSync,
        getFilenameExtension: getFilenameExtension
    };
}

function loadJSONFileSync( path ) {

    return JSON.parse( loadTextFileSync( path ) );

}

function writeJSONFileSync( object, path ) {

    var content = getObjectSerializedAsString( object, true );

    if ( content ) {
        return writeTextFileSync( content, path );
    }

    return false;

}

function getObjectSerializedAsString( objectJSON, beautified ) {

    if ( beautified ) {

        return JSON.stringify( objectJSON, null, 4 );

    }
    else {

        return JSON.stringify( objectJSON );

    }

}

function loadTextFileSync( path ) {

    var fileContent = null;

    try {

        fileContent = fs.readFileSync( path, "utf-8" );

    }
    catch ( e ) {
        if ( e.code === 'ENOENT' ) {
            return null;
        }
        else {
            // TODO how to handle?
            throw e;
        }
    }

    return fileContent;

}

function writeTextFileSync( content, path ) {

    try {

        fs.writeFileSync( path, content );

        return true;

    }
    catch ( e ) {
        return false;
    }

}

function getFilenameExtension( path ) {

    pathLastIndexOfDot = path.lastIndexOf( "." );

    if ( pathLastIndexOfDot > 0 && path.length > pathLastIndexOfDot + 1) {
        return path.substring( pathLastIndexOfDot + 1 );
    }
    else {
        return "";
    }

}
