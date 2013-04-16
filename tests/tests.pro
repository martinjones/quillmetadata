
TEMPLATE = subdirs

DEPENDPATH += .
INCLUDEPATH += .

CONFIG += ordered

SUBDIRS += ut_metadata \
	    ut_regions


# --- install
tatam.files = tests.xml
equals(QT_MAJOR_VERSION, 4): tatam.path  = $$(DESTDIR)/usr/share/libquillmetadata-tests/
equals(QT_MAJOR_VERSION, 5): tatam.path  = $$(DESTDIR)/usr/share/libquillmetadata-qt5-tests/

tatamimages.files += images/exif.jpg
tatamimages.files += images/xmp.jpg
tatamimages.files += images/iptc.jpg
tatamimages.files += images/gps.jpg
tatamimages.files += images/mnaa.jpg

tatamimages.path  = $${tatam.path}/images/

INSTALLS += tatam tatamimages
