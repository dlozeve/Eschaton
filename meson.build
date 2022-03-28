project(
  'eschaton',
  'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

src = ['src/main.c', 'src/scanner.c']

incdir = include_directories('include')

exe = executable(
  'eschaton',
  src,
  include_directories: incdir
)

