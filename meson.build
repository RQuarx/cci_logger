project('CCI-Logger', 'cpp',
        default_options: [ 'cpp_std=c++20', 'warning_level=3' ],
        version: '1.0.0-alpha',
        license: 'GPL-3.0-or-later',
        license_files: [ 'COPYING' ])

cci_logger = library(
    'cci_logger',
    sources: [ 'cci_logger.cc' ],
    include_directories: include_directories('.'),
    install: true
)

install_headers('cci_logger.hh')


pkg = import('pkgconfig')
pkg.generate(
    name: 'cci_logger',
    description: 'An interactive logger for C++20 and above.',
    version: meson.project_version(),
    libraries: cci_logger,
    subdirs: '.'
)

subdir('test')