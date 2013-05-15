TEMPLATE = subdirs

SUBDIRS = unit-tests util-suite mafw perf-tests benchmarking

check.CONFIG = recursive
check.recurse = unit-tests util-suite mafw
performance_tests.CONFIG = recursive
performance_tests.recurse = perf-tests

QMAKE_EXTRA_TARGETS += check performance_tests
