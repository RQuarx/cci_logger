test_bin = executable(
    'logger_test',
    'test.cc',
    include_directories: include_directories('..'),
    link_with: cci_logger
)


test('CCI-Logger unit test.', test_bin)