let load_ = () => 0;
let unload_ = () => {};
let process1_ = () => {};
let process2_ = () => {};
let process4_ = () => {};
let setRepeatCount_ = (rc) => {};
let setPosition_ = (p) => {};
let getPosition_ = () => 0;

let initResolver = null;
const init = new Promise((resolve) => { initResolver = resolve; });

Module.onRuntimeInitialized = function() {
    load_ = Module.cwrap('load', 'number', ['number', 'number']);
    unload_ = Module.cwrap('unload', 'void', ['number']);
    process1_ = Module.cwrap('process1', 'void', ['number', 'number', 'number']);
    process2_ = Module.cwrap('process2', 'void', ['number', 'number', 'number', 'number']);
    process4_ = Module.cwrap('process4', 'void', ['number', 'number', 'number', 'number', 'number', 'number']);
    setRepeatCount_ = Module.cwrap('setRepeatCount', 'void', ['number']);
    setPosition_ = Module.cwrap('setPosition', 'void', ['number']);
    getPosition_ = Module.cwrap('getPosition', 'number', []);

    initResolver();
}

const BUFFERSIZE = 480;

class LibopenmptProcessor extends AudioWorkletProcessor {
    constructor() {
        super();
        
        this.bufferSize = BUFFERSIZE;
        this.initialized = false;
        this.leftPtr = null;
        this.rightPtr = null;        
        this.mod = 0;

        this.port.onmessage = async (event) => {
            await init;
            if (!this.initialized) {
                this.init();
            }

            const {songData, setRepeatCount, setPosition, getPosition} = event.data;

            if (songData != null) { // ArrayBuffer
                if (this.mod != 0) {
                    unload_(this.mod);
                    this.mod = 0;
                }

                if (songData.byteLength != 0) {
                    const srcArray = new Uint8Array(songData);

                    const dataPtr = Module._malloc(srcArray.length);
                    Module.HEAPU8.set(srcArray, dataPtr);

                    this.mod = load_(dataPtr, srcArray.length);

                    Module._free(dataPtr);
                }
            }

            if (setRepeatCount != null) {
                setRepeatCount_(this.mod, setRepeatCount);
            }

            if (setPosition != null) {
                setPosition_(this.mod, setPosition);
            }

            if (getPosition != null) {
                this.port.postMessage(this.mod, {position: getPosition_()});
            }
        };
    }

    init() {
        const leftPtr = Module._malloc(BUFFERSIZE * 4);; 
        this.leftPtr = leftPtr;
        this.leftArray = Module.HEAPF32.subarray(leftPtr >> 2, (leftPtr >> 2) + BUFFERSIZE);
        this.leftArray.fill(0);

        const rightPtr = Module._malloc(BUFFERSIZE * 4);
        this.rightPtr = rightPtr;
        this.rightArray = Module.HEAPF32.subarray(rightPtr >> 2, (rightPtr >> 2) + BUFFERSIZE);
        this.rightArray.fill(0);
        
        this.initialized = true;
    }

    process(inputs, outputs, parameters) {
        const leftPtr = this.leftPtr;
        const rightPtr = this.rightPtr;
        if (!leftPtr || !rightPtr) {
            return true;
        }

        const left = outputs[0][0]; // Float32Array
        const right = outputs[0][1];

        process2_(this.mod, sampleRate, left.length, leftPtr, rightPtr);

        left.set(Module.HEAPF32.subarray(leftPtr >> 2, (leftPtr >> 2) + left.length));
        right.set(Module.HEAPF32.subarray(rightPtr >> 2, (rightPtr >> 2) + right.length));

        return true;
    }
}

registerProcessor('libopenmpt-processor', LibopenmptProcessor);
