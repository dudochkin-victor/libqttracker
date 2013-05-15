include(../shared.pri)

TEMPLATE = lib
CONFIG += shared create_pc create_prl link_pkgconfig
QT += dbus
PKGCONFIG += uuid

TARGET = qttracker
DESTDIR = $$top_srcdir/lib
OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_LFLAGS += -Wl,-z,nodelete
CONFIG += link_pkgconfig
PKGCONFIG += QtSparql

INCLUDEPATH += .
DEFINES += QTTRACKER_SUPPORT_VER=$$SUPPORT_VER
DEFINES += MAKE_SOPRANO_LIB

HEADERS = \
	backends/local/local_p.h \
	backends/mafw/mafw_p.h \
	backends/tracker/common_p.h \
	backends/tracker/graph_signals_p.h \
	backends/tracker/id2iricache_p.h \
	backends/tracker/rdftrackerselect_p.h \
	backends/tracker/rdftrackerupdate_p.h \
	backends/tracker/signals_p.h \
	backends/tracker/tracker_p.h \
	backends/tracker/trackerutil.h \
	backends/tracker/trackeraccess.h \
	backends/tracker/trackeraccessqsparql.h \
	backends/tracker/dbus/asyncdbusinterface.h \
	backends/tracker/dbus/tracker_resources.h \
	backends/tracker/dbus/dbus_types.h \
	backends/tracker/dbus/trackeraccessdbus.h \
	soprano/vocabulary/rdf.h \
	soprano/vocabulary/xsd.h \
	utils/visualizer.h \
	compositemodule_p.h \
	derived_columns_p.h \
	live_p.h \
	livenodemodelbase_p.h \
	livenodemodelbridge_p.h \
	liveresource_p.h \
	lnm_transitionstrategy_p.h \
	qfifoset.h \
	qhasproperty.h \
	qsharedset_p.h \
	rawdatatypes_p.h \
	rdfcache_p.h \
	rdfderivedselect_p.h \
	rdfservice_p.h \
	rdfservicebase_p.h \
	rdfunbound_p.h \
	rowstoreblock_p.h \
	rowstoremodel_p.h \
	sequencer.h \
	sequencer_p.h \
	sparsevector_p.h \
	subrowstoremodel_p.h \
	transaction_p.h \
	triplemodel_p.h \
	tripleupdates_p.h \
	updatermodule_p.h \
	util_p.h \
	include/QtMafw/mafw.h \
	include/QtMafw/mafw_common.h \
	include/QtMafw/dummyresource.h \
	include/QtTracker/ontologies/dc.h \
	include/QtTracker/ontologies/fts.h \
	include/QtTracker/ontologies/maemo.h \
	include/QtTracker/ontologies/mfo.h \
	include/QtTracker/ontologies/mlo.h \
	include/QtTracker/ontologies/mto.h \
	include/QtTracker/ontologies/nao.h \
	include/QtTracker/ontologies/ncal.h \
	include/QtTracker/ontologies/nco.h \
	include/QtTracker/ontologies/nfo.h \
	include/QtTracker/ontologies/nid3.h \
	include/QtTracker/ontologies/nie.h \
	include/QtTracker/ontologies/nmm.h \
	include/QtTracker/ontologies/nmo.h \
	include/QtTracker/ontologies/nrl.h \
	include/QtTracker/ontologies/poi.h \
	include/QtTracker/ontologies/rdf.h \
	include/QtTracker/ontologies/rdfs.h \
	include/QtTracker/ontologies/scal.h \
	include/QtTracker/ontologies/slo.h \
	include/QtTracker/ontologies/tracker.h \
	include/QtTracker/ontologies/xsd.h \
	include/QtTracker/domains/tags.h \
	include/QtTracker/tracker.h \
	include/QtTracker/tracker_common.h \
	include/soprano/datetime.h \
	include/soprano/languagetag.h \
	include/soprano/literalvalue.h \
	include/soprano/node.h \
	include/soprano/soprano_export.h \
	include/soprano/sopranomacros.h \
	include/sopranolive/ontologies/core/dc.h \
	include/sopranolive/ontologies/core/nrl.h \
	include/sopranolive/ontologies/core/rdf.h \
	include/sopranolive/ontologies/core/rdfs.h \
	include/sopranolive/ontologies/core/xsd.h \
	include/sopranolive/live.h \
	include/sopranolive/livebag.h \
	include/sopranolive/liveautogenerationbase.h \
	include/sopranolive/livenode.h \
	include/sopranolive/livenodemodel.h \
	include/sopranolive/liveresource.h \
	include/sopranolive/liveresourcebridge.h \
	include/sopranolive/local_common.h \
	include/sopranolive/mostderived.h \
	include/sopranolive/qsharedobject.h \
	include/sopranolive/rdfchain.h \
	include/sopranolive/rdfservice.h \
	include/sopranolive/rdfunbound.h \
	include/sopranolive/transitionmodel.h \
	include/sopranolive/unbound/rdfderivedselect.h \
	include/sopranolive/unbound/rdfderivedselect.h \

SOURCES = \
	backends/local/local_p.cpp \
	backends/mafw/mafw_p.cpp \
	backends/mafw/dummyresource_p.cpp \
	backends/tracker/graph_signals_p.cpp \
	backends/tracker/id2iricache_p.cpp \
	backends/tracker/rdftrackerselect_p.cpp \
	backends/tracker/rdftrackerupdate_p.cpp \
	backends/tracker/signals_p.cpp \
	backends/tracker/tracker_p.cpp \
	backends/tracker/trackeraccess.cpp \
	backends/tracker/trackeraccessqsparql.cpp \
	backends/tracker/trackerutil.cpp \
	backends/tracker/dbus/asyncdbusinterface.cpp \
	backends/tracker/dbus/tracker_resources.cpp \
	backends/tracker/dbus/dbus_types.cpp \
	backends/tracker/dbus/trackeraccessdbus.cpp \
	domains/tags.cpp \
	soprano/vocabulary/rdf.cpp \
	soprano/vocabulary/xsd.cpp \
	soprano/datetime.cpp \
	soprano/languagetag.cpp \
	soprano/literalvalue.cpp \
	soprano/node.cpp \
	utils/visualizer_p.cpp \
	derived_columns_p.cpp \
	live_p.cpp \
	livenodemodelbase_p.cpp \
	livenodemodelbridge_p.cpp \
	liveresource_p.cpp \
	lnm_transitionstrategy_p.cpp \
	pending_abi_break.cpp \
	qsharedobject_p.cpp \
	rdfcache_p.cpp \
	rdfderivedselect_p.cpp \
	rdfservice_p.cpp \
	rdfservicebase_p.cpp \
	rdfunbound_p.cpp \
	rdfvariable_p.cpp \
	rowstoreblock_p.cpp \
	rowstoremodel_p.cpp \
	sequencer_p.cpp \
	subrowstoremodel_p.cpp \
	transaction_p.cpp \
	triplemodel_p.cpp \
	tripleupdates_p.cpp \
	updatermodule_p.cpp \
	util_p.cpp

headers_dir = $$PREFIX/include/QtTracker

soprano_headers.path = $$headers_dir
soprano_headers.files = include/soprano/*.h
sopranolive_headers.path = $$headers_dir
sopranolive_headers.files = include/sopranolive/*.h \
                            include/sopranolive/ontologies

qttracker_headers.path = $$headers_dir
qttracker_headers.files = include/QtTracker/*.h \
                          include/QtTracker/ontologies \
                          include/QtTracker/domains

mafw_headers.path = $$headers_dir
mafw_headers.files = include/QtMafw/*.h

forwarder_headers.path = $$headers_dir
forwarder_headers.files = include/install/QtTracker/*

target.path = $$PREFIX/lib

INSTALLS += \
	target \
	soprano_headers \
	sopranolive_headers \
	qttracker_headers \
	mafw_headers \
	forwarder_headers

QMAKE_PKGCONFIG_REQUIRES = QtCore
QMAKE_PKGCONFIG_DESTDIR = $$DESTDIR/pkgconfig

coverage {
	LIBS += -lgcov
	QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
	QMAKE_EXTRA_TARGETS += coverage-report
	coverage-report.commands  = lcov -d .obj --capture --output-file all.cov -b . &&
	coverage-report.commands += lcov -e all.cov '*/*/src/*.cpp' -e all.cov '*/*/src/*.h' -o src.cov &&
	coverage-report.commands += genhtml -o coverage_report src.cov
}

timestamps {
	DEFINES += PROFILE_TIMESTAMPS
}

profile {
	CONFIG += static
	CONFIG -= shared
	QMAKE_CXXFLAGS += -pg
}

visualize {
	QMAKE_CXXFLAGS += -DVISUALIZER_ENABLE
}
check.CONFIG = recursive
check.commands = cat /dev/null
