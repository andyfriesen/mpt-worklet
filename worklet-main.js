import Module from './mpt-worklet.js';

let load_;
let process_;
let bufferSize;
let leftPtr;
let rightPtr;

Module.onRuntimeInitialized = function() {
    console.log('onRuntimeInitialized');
    const BUFFERSIZE = 480;

    bufferSize = BUFFERSIZE;
    leftPtr = Module._malloc(BUFFERSIZE * 2);
    rightPtr = Module._malloc(BUFFERSIZE * 2);

    load_ = Module.cwrap('load', 'void', ['number', 'number']);
    process_ = Module.cwrap('process', 'void', ['number', 'number', 'number']);
    console.log('registered');
}

class LibopenmptProcessor extends AudioWorkletProcessor {
    constructor() {
        super();

        this.port.onmessage = (event) => {
            const data = event.data; // ArrayBuffer
            if (data.byteLength == 0) {
                load_(0, 0);
            } else {
                const dataPtr = Module._malloc(data.byteLength);
                Module.HEAPU8.set(data, dataPtr);

                load_(dataPtr, data.byteLength);

                Module._free(dataPtr);
            }
        };

        this.port.postMessage('ready');
    }

    process(inputs, outputs, parameters) {
        const left = outputs[0][0]; // Float32Array
        // const right = outputs[0][1];

        process_(left.length, leftPtr, rightPtr);

        // Also upsample from int16 to float32
        left.set(Module.HEAPU16.subarray(leftPtr >> 1, (leftPtr + left.length) >> 1));
        // right.set(Module.HEAPU16.subarray(rightPtr >> 1, (rightPtr + right.length) >> 1));
    }
}

registerProcessor('libopenmpt-processor', LibopenmptProcessor);
