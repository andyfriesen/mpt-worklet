import sys
import os
import os.path

openmptdir = '../openmpt'

OPENMPT_SRC = [os.path.join(openmptdir, x) for x in Split('''
    common/ComponentManager.cpp
    common/FileReader.cpp
    common/Logging.cpp
    common/Profiler.cpp
    common/misc_util.cpp
    common/mptAlloc.cpp
    common/mptCPU.cpp
    common/mptFileIO.cpp
    common/mptIO.cpp
    common/mptLibrary.cpp
    common/mptOS.cpp
    common/mptPathString.cpp
    common/mptRandom.cpp
    common/mptString.cpp
    common/mptStringBuffer.cpp
    common/mptStringFormat.cpp
    common/mptStringParse.cpp
    common/mptTime.cpp
    common/mptUUID.cpp
    common/mptWine.cpp
    common/serialization_utils.cpp
    common/version.cpp
    soundbase/Dither.cpp
    soundlib/AudioCriticalSection.cpp
    soundlib/ContainerMMCMP.cpp
    soundlib/ContainerPP20.cpp
    soundlib/ContainerUMX.cpp
    soundlib/ContainerXPK.cpp
    soundlib/Dlsbank.cpp
    soundlib/Fastmix.cpp
    soundlib/ITCompression.cpp
    soundlib/ITTools.cpp
    soundlib/InstrumentExtensions.cpp
    soundlib/Load_669.cpp
    soundlib/Load_amf.cpp
    soundlib/Load_ams.cpp
    soundlib/Load_c67.cpp
    soundlib/Load_dbm.cpp
    soundlib/Load_digi.cpp
    soundlib/Load_dmf.cpp
    soundlib/Load_dsm.cpp
    soundlib/Load_dtm.cpp
    soundlib/Load_far.cpp
    soundlib/Load_gdm.cpp
    soundlib/Load_imf.cpp
    soundlib/Load_it.cpp
    soundlib/Load_itp.cpp
    soundlib/Load_mdl.cpp
    soundlib/Load_med.cpp
    soundlib/Load_mid.cpp
    soundlib/Load_mo3.cpp
    soundlib/Load_mod.cpp
    soundlib/Load_mt2.cpp
    soundlib/Load_mtm.cpp
    soundlib/Load_okt.cpp
    soundlib/Load_plm.cpp
    soundlib/Load_psm.cpp
    soundlib/Load_ptm.cpp
    soundlib/Load_s3m.cpp
    soundlib/Load_sfx.cpp
    soundlib/Load_stm.cpp
    soundlib/Load_stp.cpp
    soundlib/Load_uax.cpp
    soundlib/Load_ult.cpp
    soundlib/Load_wav.cpp
    soundlib/Load_xm.cpp
    soundlib/MIDIEvents.cpp
    soundlib/MIDIMacros.cpp
    soundlib/MPEGFrame.cpp
    soundlib/Message.cpp
    soundlib/MixFuncTable.cpp
    soundlib/MixerLoops.cpp
    soundlib/MixerSettings.cpp
    soundlib/ModChannel.cpp
    soundlib/ModInstrument.cpp
    soundlib/ModSample.cpp
    soundlib/ModSequence.cpp
    soundlib/OPL.cpp
    soundlib/OggStream.cpp
    soundlib/Paula.cpp
    soundlib/RowVisitor.cpp
    soundlib/S3MTools.cpp
    soundlib/SampleFormatFLAC.cpp
    soundlib/SampleFormatMP3.cpp
    soundlib/SampleFormatMediaFoundation.cpp
    soundlib/SampleFormatOpus.cpp
    soundlib/SampleFormatSFZ.cpp
    soundlib/SampleFormatVorbis.cpp
    soundlib/SampleFormats.cpp
    soundlib/SampleIO.cpp
    soundlib/Snd_flt.cpp
    soundlib/Snd_fx.cpp
    soundlib/Sndfile.cpp
    soundlib/Sndmix.cpp
    soundlib/SoundFilePlayConfig.cpp
    soundlib/Tables.cpp
    soundlib/Tagging.cpp
    soundlib/TinyFFT.cpp
    soundlib/UMXTools.cpp
    soundlib/UpgradeModule.cpp
    soundlib/WAVTools.cpp
    soundlib/WindowedFIR.cpp
    soundlib/XMTools.cpp
    soundlib/load_j2b.cpp
    soundlib/mod_specifications.cpp
    soundlib/modcommand.cpp
    soundlib/modsmp_ctrl.cpp
    soundlib/pattern.cpp
    soundlib/patternContainer.cpp
    soundlib/tuning.cpp
    soundlib/tuningCollection.cpp
    soundlib/plugins/DigiBoosterEcho.cpp
    soundlib/plugins/LFOPlugin.cpp
    soundlib/plugins/PlugInterface.cpp
    soundlib/plugins/PluginManager.cpp
    soundlib/plugins/dmo/Chorus.cpp
    soundlib/plugins/dmo/Compressor.cpp
    soundlib/plugins/dmo/DMOPlugin.cpp
    soundlib/plugins/dmo/Distortion.cpp
    soundlib/plugins/dmo/Echo.cpp
    soundlib/plugins/dmo/Flanger.cpp
    soundlib/plugins/dmo/Gargle.cpp
    soundlib/plugins/dmo/I3DL2Reverb.cpp
    soundlib/plugins/dmo/ParamEq.cpp
    soundlib/plugins/dmo/WavesReverb.cpp
    sounddsp/AGC.cpp
    sounddsp/DSP.cpp
    sounddsp/EQ.cpp
    sounddsp/Reverb.cpp
    libopenmpt/libopenmpt_c.cpp
    libopenmpt/libopenmpt_cxx.cpp
    libopenmpt/libopenmpt_impl.cpp
    libopenmpt/libopenmpt_ext_impl.cpp
    include/miniz/miniz.c
    include/minimp3/minimp3.c
    include/stb_vorbis/stb_vorbis.c
''')]

def EmscriptenEnvironment():
    # Find emcc
    emcc = None
    path = os.environ['PATH'].split(os.path.pathsep)
    for p in path:
        p2 = os.path.join(p, 'emcc')
        if os.path.exists(p2):
            emcc = p2
            break

    assert emcc is not None, 'Could not find emcc'

    if sys.platform == 'windows':
        env_dict = {
            'path': os.environ['PATH']
        }

        for key in ['HOME', 'USERPROFILE', 'EM_CONFIG']:
            value = os.environ.get(key)
            if value is not None:
                env_dict[key] = value

        env = Environment(ENV=env_dict, TOOLS=['mingw'])
    else:
        env = Environment()

    env['CC'] = emcc
    env['CXX'] = emcc

    emscriptenOpts = [
        '-s', 'ALLOW_MEMORY_GROWTH=1',
        '-s', 'ENVIRONMENT=worker',
        '-s', 'SINGLE_FILE=1',
        '-s', 'EXPORTED_FUNCTIONS=[\'_load\',\'_process1\',\'_process2\',\'_process4\']',
        '-s', 'EXTRA_EXPORTED_RUNTIME_METHODS=[\'cwrap\']',
        '-s', 'DISABLE_EXCEPTION_CATCHING=0',
    ]

    cflags = ['-fcolor-diagnostics']

    debug = ARGUMENTS.get('debug', 0)
    asan = ARGUMENTS.get('asan', 0)
    ubsan = ARGUMENTS.get('ubsan', 0)

    if debug:
        if not asan:
            emscriptenOpts += [
                '-s', 'SAFE_HEAP=1',
            ]

        emscriptenOpts += [
            '-s', 'ASSERTIONS=1',
            '-s', 'STACK_OVERFLOW_CHECK=1',
            '-s', 'DEMANGLE_SUPPORT=1',
        ]

        cflags.append('-g')

        env.Append(LINKFLAGS=[
            '-g4',
            '--source-map-base', 'http://localhost:8000/',
        ])

    else:
        cflags.append('-O3')

    if asan:
        cflags.append('-fsanitize=address')
        env.Append(LINKFLAGS=['-fsanitize=address'])
    if ubsan:
        cflags.append('-fsanitize=undefined')
        env.Append(LINKFLAGS=['-fsanitize=undefined'])

    cflags.extend([
        '-MMD',
        '-Wno-parentheses',
        '-Wno-long-long',
        '-Wno-dangling-else',
        '-Wno-writable-strings',
    ])

    cflags.extend(emscriptenOpts)

    env.Append(CFLAGS=cflags)
    env.Append(CXXFLAGS=cflags)

    env.Append(CXXFLAGS=[
        '-std=c++17',
    ])

    env.Append(LINKFLAGS=[
        '-lidbfs.js',
    ] + emscriptenOpts)

    return env

env = EmscriptenEnvironment()

env.Append(CPPDEFINES={
    'LIBOPENMPT_BUILD': None,
    'MPT_WITH_MINIZ': None,
    'MPT_WITH_MINIMP3': None,
    'MPT_WITH_STBVORBIS': None,
    'STB_VORBIS_NO_PULLDATA_API': None,
    'STB_VORBIS_NO_STDIO': None,

    'MPT_SVNURL': '\\"https://source.openmpt.org/svn/openmpt/trunk/OpenMPT\\"',
    'MPT_SVNVERSION': '\\"12583\\"',
    'MPT_SVNDATE': '\\"2020-03-15T22:22:00Z\\"',
},
CPPPATH=[
    openmptdir,
    os.path.join(openmptdir, 'common'),
    os.path.join(openmptdir, 'include'),
    os.path.join(openmptdir, 'include/modplug/include'),
    os.path.join(openmptdir, 'build/svn_version'),
],
LINKFLAGS=[
    '--pre-js', 'pre.js',
    '--post-js', 'post.js',
    '--source-map-base', 'http://localhost/',
])

worklet = env.Program('mpt-worklet.js', OPENMPT_SRC + ['worklet.cpp'])
env.Depends(worklet, ['pre.js', 'post.js'])
