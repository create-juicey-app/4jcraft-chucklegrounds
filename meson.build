project(
  '4jcraft',
  ['cpp', 'c'],
  version: '0.1.0',
  meson_version: '>= 1.7',
  default_options: [
    'cpp_std=c++23',
    'warning_level=0',
    'buildtype=debug', # for now
    'unity=on', # merge source files per target
    'unity_size=8', # TODO: mess around with this
    'b_pch=true', # precompiled headers
  ],
)

pymod = import('python')
python = pymod.find_installation('python3', required: true)
fs = import('fs')

cc = meson.get_compiler('cpp', native: false)

global_cpp_defs = [
  '-DSPLIT_SAVES',
  '-D_LARGE_WORLDS',
  '-D_EXTENDED_ACHIEVEMENTS',
  '-D_DEBUG_MENUS_ENABLED',
  '-D_DEBUG',
  '-DDEBUG',
]

if host_machine.system() == 'linux'
  global_cpp_defs += ['-Dlinux', '-D__linux', '-D__linux__']
  global_cpp_defs += ['-DAPP_PLATFORM_LINUX=1', '-DAPP_PLATFORM_ANDROID=0']
elif host_machine.system() == 'android'
  global_cpp_defs += ['-DAPP_PLATFORM_LINUX=0', '-DAPP_PLATFORM_ANDROID=1']
else
  global_cpp_defs += ['-DAPP_PLATFORM_LINUX=0', '-DAPP_PLATFORM_ANDROID=0']
endif

if get_option('renderer') == 'gles'
  global_cpp_defs += ['-DGLES']
  gl_dep = dependency('glesv2', required: false)
  if not gl_dep.found()
    gl_dep = cc.find_library('GLESv2', required: true)
  endif
  glu_dep = dependency('', required: false)
else
  gl_dep = dependency('gl', required: true)
  glu_dep = dependency('glu', required: true)
endif

if get_option('enable_vsync')
  global_cpp_defs += ['-DENABLE_VSYNC']
endif

if get_option('classic_panorama')
    global_cpp_defs += '-DCLASSIC_PANORAMA'
endif

if get_option('enable_frame_profiler')
  global_cpp_defs += ['-DENABLE_FRAME_PROFILER']
endif

if get_option('ui_backend') == 'shiggy'
  global_cpp_defs += ['-D_ENABLEIGGY']
endif

if get_option('ui_backend') == 'java'
  global_cpp_defs += '-DENABLE_JAVA_GUIS'
endif

add_project_arguments(global_cpp_defs, language: ['cpp', 'c'])

global_cpp_args = [
  '-Wshift-count-overflow',
  '-pipe',
]
add_project_arguments(global_cpp_args, language: 'cpp')

sdl2_dep = dependency('sdl2', required: false)
if not sdl2_dep.found() and host_machine.system() == 'android'
  sdl2_lib = meson.project_source_root() / 'android/third_party/SDL2/lib/arm64-v8a/libSDL2.so'
  if fs.exists(sdl2_lib)
    sdl2_inc_dirs = [include_directories('android/third_party/SDL2/include')]
    sdl2_inc2 = meson.project_source_root() / 'android/third_party/SDL2/include/SDL2'
    if fs.exists(sdl2_inc2)
      sdl2_inc_dirs += include_directories('android/third_party/SDL2/include/SDL2')
    endif
    sdl2_dep = declare_dependency(
      include_directories: sdl2_inc_dirs,
      link_args: [sdl2_lib],
    )
  endif
endif
if not sdl2_dep.found()
  error('SDL2 dependency not found. For Android builds, place SDL2 at android/third_party/SDL2 with include/ and lib/arm64-v8a/libSDL2.so')
endif
thread_dep = dependency('threads')
dl_dep = cc.find_library('dl', required: true)

stb = subproject('stb').get_variable('stb_inc')
stb_dep = declare_dependency(include_directories: stb)

miniaudio_dep = dependency('miniaudio')

subdir('targets/console_helpers')
subdir('targets/java')
subdir('targets/nbt')
subdir('targets/platform')

subdir('targets/resources')
subdir('targets/minecraft')
subdir('targets/app')

