const fs = require("fs");
const mapData = require("./map_stage1.json");

const groundBuffer = Buffer.from(mapData.layers[0].data, 'base64');
const solidBuffer = Buffer.from(mapData.layers[1].data, 'base64');

const mapBufferBytesPerTile = 2;
const mapBuffer = Buffer.alloc(groundBuffer.length / mapBufferBytesPerTile);

/**
 * 2 Bytes per tile
 * 0000 0000 0000 0000
 *         | |--------> 1 byte: Tile Id 
 *         |----------> 1 bit: Solid bit
 */

for (let i = 0; i < groundBuffer.length; i += 4) {
    const groundValue = groundBuffer.readUInt32LE(i)
    const solidValue = solidBuffer.readUInt32LE(i)
    
    const value = solidValue ? solidValue | 0x100 : groundValue;
    mapBuffer.writeUInt16BE(value & 0xFFFF, i / mapBufferBytesPerTile);
};

//fs.writeFileSync("ground.bin", groundBuffer);
//fs.writeFileSync("solid.bin", solidBuffer);
fs.writeFileSync("stage1.map", mapBuffer);