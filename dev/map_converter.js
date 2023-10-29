const fs = require("fs");
const mapData = require("./map_stage1.json");

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

        data[index] = objectsIds[object.type];
        return data;
    }, {}
);

/**
 * 2 Bytes per tile
 * 0000 0000 0000 0000
 *    |    | |--------> 1 byte: Tile Id
 *    |    |----------> 1 bit: Solid bit
 *    |---------------> 4 bits: Objects, enemies, power-ups
 */
for (let i = 0; i < groundBuffer.length; i += 4) {
    const groundValue = groundBuffer.readUInt32LE(i)
    const solidValue = solidBuffer.readUInt32LE(i)
    const objectId = tileObjects[i / 4];

    // Solid flag
    let value = solidValue ? solidValue | 0x100 : groundValue;
    // Object Id
    if (objectId) {
        value |= objectId << 9
    }
    mapBuffer.writeUInt16BE(value & 0xFFFF, i / mapBufferBytesPerTile);
};

//fs.writeFileSync("ground.bin", groundBuffer);
//fs.writeFileSync("solid.bin", solidBuffer);
fs.writeFileSync("../maps/stage1.map", mapBuffer);
