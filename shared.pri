LIBRARYNAME = QtTracker
LIBRARYNAME_LC = qttracker
PACKAGENAME = libqttracker

VERSION = 1~6.12.6

# use format VVBBMM where VV = major version, BB = abi/prerelease version, MM = feature version.
# Minor/bugfix versions are ignored.
# drop leading zeroes
# 0.3.0 -> 0003 -> 3
# 0+2.0.1 -> 000200 -> 200
# 1~4.1.3 -> 000401 -> 401
# 2~1.0.3 -> 010100 -> 10100
# 2.1.0 -> 200001 -> 20001

LIBRARY_VER = 604
SUPPORT_VER = 400

# internals below this line

isEmpty(PREFIX): PREFIX = /usr/local
top_srcdir = $$PWD
INCLUDEPATH = $$top_srcdir/src/include $$top_srcdir/src/include/qt4
CONFIG += release qt depend_includepath warn_on nostrip
unix:CONFIG += hide_symbols
QT = core
QMAKE_LIBDIR = $$top_srcdir/lib

noopt {
	CONFIG -= release
	CONFIG += debug
}
