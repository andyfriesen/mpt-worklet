# mpt-worklet
Run libopenmpt in an AudioWorklet.

This is much nicer than ScriptProcessorNode because the music won't hitch just because
something sits on the main thread too long.  This is interesting to me personally because
lately I am porting old terrible DOS codebases to run in WASM.

# Important limitations

1. AudioWorklet is [mostly standard](https://caniuse.com/#search=audioworklet) at this point, but it is not truly totally ubiquitous.
2. It only works over https.  localhost also works as a special exception.
3. (this one is on me!) This is barely more than a proof of concept just now.  Buyer beware!

# How to use it

The worklet is contained in the single JS file `mpt-worklet.js`.  It's all you need.

Check index.html for a working example.  tldr:

```javascript
ctx = new AudioContext();

await ctx.audioWorklet.addModule('mpt-worklet.js');

node = new AudioWorkletNode(ctx, 'libopenmpt-processor', {
    numberOfInputs: 0,
    numberOfOutputs: 1,
    outputChannelCount: [2] // stereo is the only supported configuration right now
});
node.connect(ctx.destination);

// Load some mod file into an ArrayBuffer
const response = await fetch("milksun.it");
const songData = await response.arrayBuffer();

node.port.postMessage({songData: songData, setRepeatCount: -1});
```

# How to build

I use WSL on Windows.  Ubuntu and Mac should be fine?

Get SCons and Emscripten.

Put libopenmpt in a sibling directory to this one.

```shell
scons -j4
```

This will compile libopenmpt and the worklet asm.js all in one go.

# To do

Add hooks to control playback:

* Pause and resume
* get duration
