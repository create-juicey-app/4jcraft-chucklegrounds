platform_inc = include_directories('.')

platform_dep = declare_dependency(
  include_directories: platform_inc,
)

# SDL2-based platform implementations (formerly 4J.* modules)
_sdl2 = sdl2_dep
_threads = thread_dep
_glm = dependency('glm', required: false)
if not _glm.found() and host_machine.system() == 'android'
  glm_header = meson.project_source_root() / 'android/third_party/glm/glm/glm.hpp'
  if fs.exists(glm_header)
    _glm = declare_dependency(include_directories: include_directories('../../android/third_party/glm'))
  else
    error('GLM dependency not found!')
  endif
endif
if not _glm.found()
  error('GLM dependency not found')
endif
_defs = []

if get_option('renderer') == 'gles'
  _gl = gl_dep
  _defs += ['-DGLES']
else
  _gl = gl_dep
endif

sdl2_sources = files(
  'sdl2/Input.cpp',
  'sdl2/Profile.cpp',
  'sdl2/Render.cpp',
  'sdl2/render_stubs.cpp',
  'sdl2/Storage.cpp',
)

lib_platform_sdl2 = static_library('platform_sdl2',
  sdl2_sources,
  include_directories: [platform_inc, include_directories('sdl2')],
  dependencies: [_sdl2, _gl, _threads, _glm, stb_dep],
  cpp_args: _defs + global_cpp_args + global_cpp_defs,
)

# Combined dependency: interfaces + SDL2 implementations
# Replaces the old render_dep, input_dep, profile_dep, storage_dep
render_dep = declare_dependency(
  link_with: lib_platform_sdl2,
  include_directories: [platform_inc, include_directories('sdl2')],
  dependencies: [_sdl2, _gl, _threads, _glm],
)
input_dep = render_dep
profile_dep = render_dep
storage_dep = render_dep
