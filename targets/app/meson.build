exclude_sources = [
  ' ! -path "*/common/*"',
  ' ! -path "*/linux/*"',
  ' ! -path "*/android/*"',
  ' ! -path "*/windows/*"',
]

# all sources except common/, linux/, windows/
client_sources = run_command(
  'sh',
  '-c', 'find "'
  + meson.current_source_dir()
  + '" \\( -name "*.cpp" -o -name "*.c" \\)'
  + ' '.join(exclude_sources),
  check: true,
).stdout().strip().split('\n')

exclude_platform_common_sources = [
  ' ! -name "UIScene_InGameSaveManagementMenu.cpp"',
]

# all sources in common/
platform_sources = run_command(
  'sh',
  '-c', 'find "'
  + meson.current_source_dir() / 'common'
  + '" \\( -name "*.cpp" -o -name "*.c" \\)'
  + ' '.join(exclude_platform_common_sources),
  check: true,
).stdout().strip().split('\n')

# linux-specific files
if host_machine.system() == 'linux'
  platform_sources += run_command(
    'sh',
    '-c', 'find "'
    + meson.current_source_dir() / 'linux'
    + '" \\( -name "*.cpp" -o -name "*.c" \\) ',
    check: true,
  ).stdout().strip().split('\n')
elif host_machine.system() == 'android'
  platform_sources += run_command(
    'sh',
    '-c', 'find "'
    + meson.current_source_dir() / 'android'
    + '" \\( -name "*.cpp" -o -name "*.c" \\) ',
    check: true,
  ).stdout().strip().split('\n')
  platform_sources += files('linux/Iggy/gdraw/gdraw.c')
endif

client_dependencies = [
  java_dep,
  nbt_dep,
  render_dep,
  input_dep,
  profile_dep,
  storage_dep,
  assets_localisation_dep,
  platform_dep,
  minecraft_dep,
  gl_dep,
  glu_dep,
  thread_dep,
  dl_dep,
  dependency('zlib'),
  miniaudio_dep,
  stb_dep,
  console_helpers_dep,
]

if get_option('ui_backend') == 'shiggy'
  shiggy_dep = dependency(
    'shiggy',
    fallback: ['shiggy', 'shiggy_dep'],
  )
  client_dependencies += shiggy_dep
endif

platform_services_src = files('../platform/PlatformServices.cpp')

target_sources = client_sources + platform_sources + platform_services_src + localisation[1]
target_includes = include_directories('include', '..')
target_cpp_args = global_cpp_args + global_cpp_defs + [
  '-DUNICODE',
  '-D_UNICODE',
]
target_c_args = global_cpp_defs + ['-DUNICODE', '-D_UNICODE']

if host_machine.system() == 'android'
  client = shared_library(
    'main',
    target_sources,
    include_directories: target_includes,
    dependencies: client_dependencies,
    cpp_args: target_cpp_args,
    c_args: target_c_args,
    install: true,
    install_dir: '',
  )
else
  client = executable(
    'Minecraft.Client',
    target_sources,
    include_directories: target_includes,
    dependencies: client_dependencies,
    cpp_args: target_cpp_args,
    c_args: target_c_args,
    install: true,
    install_dir: '',
  )
endif

custom_target(
  'copy_assets_to_client',
  input: [client, media_archive],
  output: 'assets.stamp',
  command: [
    python,
    meson.project_source_root() / 'scripts/copy_assets_to_client.py',
    meson.project_source_root(),
    meson.project_build_root(),
    meson.current_build_dir(),
    '@INPUT1@',
    '@OUTPUT@',
  ],
  build_by_default: true,
)
