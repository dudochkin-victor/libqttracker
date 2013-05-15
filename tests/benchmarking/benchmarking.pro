include(../../shared.pri)

TEMPLATE = subdirs

SUBDIRS = gallery rtcom music contacts email calls im content location

runall.files = runall.py
runall.path = $$PREFIX/share/libqttracker-tests/benchmarking/
INSTALLS += runall
