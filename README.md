# mpt-worklet
Run libopenmpt in an AudioWorklet.

This is much nicer than ScriptProcessorNode because the music won't hitch just because
something sits on the main thread too long.  This is interesting to me personally because
lately I am porting old terrible DOS codebases to run in WASM.

# Important limitations

1. AudioWorklet is set to become a standard, but it only works in Chrome derivatives right now.  No Firefox. :/
2. It only works over https.  localhost also works as a special exception.
3. (this one is on me!) This is barely more than a proof of concept just now.  Buyer beware!

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
* get/set Repeat count
* get/set position (aka seeking)
* get duration