project('', 'c', version : '1.0')

buildtype = get_option('buildtype')

if buildtype == 'debug'
  add_global_arguments('-DDEBUG_PRINT_CODE', language : 'c')
  add_global_arguments('-DDEBUG_TRACE_EXECUTION', language : 'c')
  add_global_arguments('-DDEBUG_BYTECODE_EXECUTION', language : 'c')
  add_global_arguments('-DDEBUG_STRESS_GC', language: 'c')
  add_global_arguments('-DDEBUG_LOG_GC', language: 'c')
endif

executable('clox',
  'main.c',
  'chunk.c',
  'compiler.c',
  'debug.c',
  'memory.c',
  'object.c',
  'scanner.c',
  'table.c',
  'value.c',
  'vm.c'
)
