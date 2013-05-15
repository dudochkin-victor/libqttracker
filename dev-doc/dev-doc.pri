doxygen_dev-doc.target = dev-doc/html/index.html
doxygen_dev-doc.commands = cd dev-doc && VERSION=\"$$VERSION\" LIBRARYNAME=\"$$LIBRARYNAME\" doxygen doxygen.cfg
dev-doc.depends = doxygen_dev-doc

#install_dev-doc.files = dev-doc/html
#install_dev-doc.path = $$PREFIX/share/doc/$$PACKAGENAME-doc/dev-doc
#install_dev-doc.CONFIG = no_check_exist
#INSTALLS += install_dev-doc

clean_dev-doc.commands = rm -rf dev-doc/html dev-doc/man
distclean.depends += clean_dev-doc

QMAKE_EXTRA_TARGETS += dev-doc doxygen_dev-doc clean_dev-doc
