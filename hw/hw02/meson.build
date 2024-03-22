project('esw-epoll', 'c',
  default_options: [
    'warning_level=3',
    'werror=true',
    'optimization=2',
    'debug=true'
])
inc_dirs = include_directories('incl')
src_files = ['src/main.c', 'src/all.c', 'src/stdin.c', 'src/tcp_server.c', 'src/timer.c']
executable('epoll_server', src_files, include_directories: inc_dirs)
test('first run', find_program('t.sh'), is_parallel : false)
test('second run', find_program('t.sh'), is_parallel : false)
