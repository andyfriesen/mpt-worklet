
class MyWorkletProcessor extends AudioWorkletProcessor {
    constructor() {
      super();
    }
  
    process(inputs, outputs, parameters) {
      // audio processing code here.
    }
  }
  
  registerProcessor('libopenmpt-processor', MyWorkletProcessor);
  