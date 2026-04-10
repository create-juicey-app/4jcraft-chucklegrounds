platform_inc = include_directories('.')
_threads = dependency('threads')

lib_platform_core = static_library('platform_core',
  files('C4JThread.cpp', 'ShutdownManager.cpp'),
  include_directories: [platform_inc, include_directories('..')],
  dependencies: [_threads],
  cpp_args: global_cpp_args + global_cpp_defs,
)

platform_dep = declare_dependency(
  link_with: lib_platform_core,
  include_directories: platform_inc,
)

# SDL2-based platform implementations (formerly 4J.* modules)
_sdl2 = dependency('sdl2')
_defs = []

if get_option('renderer') == 'gles'
  _gl = dependency('glesv2', required: true)
  _defs += ['-DGLES']
else
  _gl = dependency('gl', required: true)
endif

sdl2_sources = files(
  'input/sdl2/SDL2Input.cpp',
  'profile/stub/StubProfile.cpp',
  'storage/stub/StubStorage.cpp',
  'renderer/gl/GLRenderer.cpp',
  'renderer/gl/render_stubs.cpp',
  'fs/std/StdFilesystem.cpp',
)

lib_platform_sdl2 = static_library('platform_sdl2',
  sdl2_sources,
  include_directories: [platform_inc],
  dependencies: [_sdl2, _gl, _threads, glm_dep, stb_dep, java_dep],
  cpp_args: _defs + global_cpp_args + global_cpp_defs,
)

# Combined dependency: interfaces + SDL2 implementations
# Replaces the old render_dep, input_dep, profile_dep, storage_dep
render_dep = declare_dependency(
  link_with: lib_platform_sdl2,
  include_directories: [platform_inc],
  dependencies: [_sdl2, _gl, _threads, glm_dep],
)
input_dep = render_dep
profile_dep = render_dep
storage_dep = render_dep
