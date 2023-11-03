const fs = require("fs");

function generateMapBinary(stage) {
    const mapData = require(`./map_stage${stage}.json`);

    const groundBuffer = Buffer.from(mapData.layers[0].data, 'base64');
    const solidBuffer = Buffer.from(mapData.layers[1].data, 'base64');

    const mapBufferBytesPerTile = 2;
    const mapBuffer = Buffer.alloc(groundBuffer.length / mapBufferBytesPerTile);

    const mapCols=32;
    const tileSize=8;

    const objectsIds = {
        blob: 1,
        bat: 2,
        "bat-wave": 3,
    };

    /**
     * Indexes objects by tile index
     */
    const tileObjects = mapData.layers[2].objects.reduce(
        (data, object) => {
            const higherX = Math.ceil(object.x);
            const higherY = Math.ceil(object.y);
            const normX = higherX - (higherX % tileSize);
            const normY = higherY - (higherY % tileSize);
            const index = (normY / tileSize) * mapCols + (normX / tileSize);

            data[index] = Object.assign({}, object, { id: objectsIds[object.type] });
            return data;
        }, {}
    );

    /**
     * 2 Bytes per tile
     * 0000 0000 0000 0000
     * |  |    | |--------> 1 byte: Tile Id
     * |  |    |----------> 1 bit: Solid bit
     * |  |---------------> 4 bits: Objects, enemies, power-ups
     * |------------------> 4 bits: Object properties
     */
    for (let i = 0; i < groundBuffer.length; i += 4) {
        const groundValue = groundBuffer.readUInt32LE(i)
        const solidValue = solidBuffer.readUInt32LE(i)
        const objectData = tileObjects[i / 4];

        // Solid flag
        let value = solidValue ? solidValue | 0x100 : groundValue;
        // Object Id
        if (objectData) {
            value |= objectData.id << 9;
            value |= getObjectPropertyValue(objectData) << 13;
        }
        mapBuffer.writeUInt16BE(value & 0xFFFF, i / mapBufferBytesPerTile);
    };

    //fs.writeFileSync("ground.bin", groundBuffer);
    //fs.writeFileSync("solid.bin", solidBuffer);
    fs.writeFileSync(`../maps/stage${stage}.map`, mapBuffer);
}

function getObjectPropertyValue(objectData) {
    let propertyValue = 0;
    (objectData.properties ?? []).every(propertyData => {
        propertyValue = parseInt(propertyData.value, 10);
        return false;
    });
    return propertyValue > 0 ? propertyValue - 1 : 0;
}

for(let stage = 1; stage <= 2; ++stage) {
    generateMapBinary(stage);
}
