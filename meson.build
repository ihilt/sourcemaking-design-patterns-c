project('design-patterns', 'c',
default_options: [
	'c_std=gnu11',
	'cpp_std=c++17',
	'warning_level=3',
	'buildtype=debug',
	'b_ndebug=if-release',
	'c_args=-fsanitize=undefined,address -Wformat -Wformat-security -Werror=format-security'
])

glib = dependency('glib-2.0')

executable('objectpool', 'objectpool.c', dependencies : glib)
executable('chain-of-responsibility', 'chain-of-responsibility.c', dependencies : glib)
