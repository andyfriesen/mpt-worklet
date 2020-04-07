import { Module } from './mpt-worklet.js';

const BUFFERSIZE = 480;

let bufferSize = BUFFERSIZE;
let leftPtr = Module._malloc(BUFFERSIZE * 2);
let rightPtr = Module._malloc(BUFFERSIZE * 2);

class LibopenmptProcessor extends AudioWorkletProcessor {
    constructor() {
        super();

        this.port.onmessage = (event) => {
            const data = event.data; // ArrayBuffer
            if (data.byteLength == 0) {
                Module.dynCall_vii(loadCb, 0, 0);
            } else {
                const dataPtr = Module._malloc(data.byteLength);
                HEAPU8.set(data, dataPtr);

                Module.dynCall_vii(loadCb, dataPtr, data.byteLength);

                Module._free(dataPtr);
            }
        };
    }

    process(inputs, outputs, parameters) {
        const left = outputs[0][0]; // Float32Array
        const right = outputs[0][1];

        Module.dynCall_viii(processCb, left.length, leftPtr, rightPtr);

        // Also upsample from int16 to float32
        left.set(HEAPU16.subarray(leftPtr >> 1, (leftPtr + left.length) >> 1));
        right.set(HEAPU16.subarray(rightPtr >> 1, (rightPtr + right.length) >> 1));
    }
}

LibopenmptProcessor = LibopenmptProcessor;

registerProcessor('libopenmpt-processor', LibopenmptProcessor);
console.log('registered!');
