const fs = require("fs");
const path = require("path");

const tilesCols = 32;
const mapCols = 32;
const tileSize = 8;
const stageTilesOffset = [0, 3, 5, 9, 12];
const mapBufferBytesPerTile = 2;

const objectsIds = {
    blob: 1,
    bat: 2,
    bat_rev: 14,
    bat_wave: 3,
    knight: 4,
    cloud: 5,
    demon_blue: 6,
    skeleton: 7,
    demon: 8,
    death_ghost: 9,
    stone_monster: 10,
    ghost: 11,
    yellow_thing: 12,
    red_thing: 13,

    weapon: 29,
    power_up: 30,
    block: 31,
};

const blockTypes = {
    rook: 1,
    knight: 2,
    queen: 3,
    king: 4,
    barrier: 5,
    bridge: 7,
};

function generateMapBinary(stage) {
    const mapData = require(`./map_stage${stage}.json`);

    const groundBuffer = Buffer.from(mapData.layers[0].data, "base64");
    const solidBuffer = Buffer.from(mapData.layers[1].data, "base64");
    const mapBuffer = Buffer.alloc(groundBuffer.length / mapBufferBytesPerTile);

    const tileOffset = stageTilesOffset[stage - 1] * tilesCols;

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
     * |  |      ||-------> 7 bits: Tile Id
     * |  |      |--------> 1 bit : Solid bit
     * |  |---------------> 5 bits: Objects, enemies, power-ups
     * |------------------> 3 bits: Object properties
     */
    for (let i = 0; i < groundBuffer.length; i += 4) {
        // 96 tiles per stage
        let groundValue = groundBuffer.readUInt32LE(i);
        let solidValue = solidBuffer.readUInt32LE(i);
        const objectData = tileObjects[i / 4];

        // Calculate tile offset
        if (solidValue) {
            solidValue -= tileOffset + 1;
            if (solidValue < 0) panic(`Solid tile ID is < 0\n- Stage ${stage}\n- Offset: ${tileOffset}\n- Index ${i}\n- ID ${solidValue}`);
        } else {
            groundValue -= tileOffset + 1;
            if (groundValue < 0) panic(`Ground tile ID is < 0\n- Stage ${stage}\n- Offset: ${tileOffset}\n- Index ${i}\n- ID ${groundValue}`);
        }
        // Tile value + solid flag
        let value = solidValue ? solidValue | 0x80 : groundValue;

        // Object Id
        if (objectData) {
            value |= objectData.id << 8;
            value |= getObjectPropertyValue(objectData) << 13;
        }
        mapBuffer.writeUInt16BE(value & 0xFFFF, i / mapBufferBytesPerTile);
    };

    //fs.writeFileSync("ground.bin", groundBuffer);
    //fs.writeFileSync("solid.bin", solidBuffer);
    fs.writeFileSync(path.join(__dirname, `../../maps/stage${stage}.map`), mapBuffer);
}

function getObjectPropertyValue(objectData) {
    let propertyValue = 0;
    (objectData.properties ?? []).every(propertyData => {
        if (objectData.type === "block") {
            propertyValue = blockTypes[propertyData.value];
        } else {
            propertyValue = parseInt(propertyData.value, 10);
            if (propertyData.name === "Count") {
                propertyValue = propertyValue > 0 ? propertyValue - 1 : 0
            }
        }
        return false;
    });
    return propertyValue;
}

function panic(message) {
    console.error(`PANIC: ${message}`);
    process.exit(-1);
}

for(let stage = 1; stage <= 5; ++stage) {
    generateMapBinary(stage);
}
