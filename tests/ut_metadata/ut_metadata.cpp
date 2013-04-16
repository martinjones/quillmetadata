/****************************************************************************
**
** Copyright (C) 2010-11 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Pekka Marjola <pekka.marjola@nokia.com>
**
** This file is part of the Quill Metadata package.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QVariant>
#include <QtTest/QtTest>

#include "quillmetadata.h"
#include "quillmetadataregionlist.h"
#include "ut_metadata.h"

#define PRECISION 10000

ut_metadata::ut_metadata()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    imagePath = "/usr/share/libquillmetadata-qt5-tests/images/";
#else
    imagePath = "/usr/share/libquillmetadata-tests/images/";
#endif
}

void ut_metadata::initTestCase()
{
    sourceImage = QImage(QSize(8, 2), QImage::Format_RGB32);
    sourceImage.fill(qRgb(255, 255, 255));
}

void ut_metadata::cleanupTestCase()
{
}

// To test namespace registration, define NAMESPACE_REGISTRATION_TEST and run "testCreateRegionBag"
// #define NAMESPACE_REGISTRATION_TEST

void ut_metadata::init()
{
    metadata	= new QuillMetadata(imagePath + "exif.jpg");
#ifndef NAMESPACE_REGISTRATION_TEST
    xmp		= new QuillMetadata(imagePath + "xmp.jpg");
    iptc	= new QuillMetadata(imagePath + "iptc.jpg");
    gps		= new QuillMetadata(imagePath + "gps.jpg");

    region	= new QuillMetadata(imagePath + "mnaa.jpg");
#endif
}

void ut_metadata::cleanup()
{
    delete metadata;
#ifndef NAMESPACE_REGISTRATION_TEST
    delete xmp;
    delete iptc;
    delete gps;

    delete region;
#endif
}


void ut_metadata::testCameraMake()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_Make).toString(), QString("Quill"));
}

void ut_metadata::testCameraModel()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_Model).toString(), QString("Q100125"));
}

void ut_metadata::testImageWidth()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_ImageWidth).toInt(), 2);
}

void ut_metadata::testImageHeight()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_ImageHeight).toInt(), 2);
}

void ut_metadata::testFocalLength()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(QString::number(metadata->entry(QuillMetadata::Tag_FocalLength).toDouble()), QString("9.9"));
}

void ut_metadata::testExposureTime()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(QString::number(metadata->entry(QuillMetadata::Tag_ExposureTime).toDouble()), QString("0.005"));
}

void ut_metadata::testTimestampOriginal()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_TimestampOriginal).toString(),
             QString("2010:01:25 15:00:00"));
}

void ut_metadata::testOrientation()
{
    QVERIFY(metadata->isValid());
    QCOMPARE(metadata->entry(QuillMetadata::Tag_Orientation).toString(),
             QString("3"));
}

void ut_metadata::testSubject()
{
    QVERIFY(xmp->isValid());
    QStringList reference;
    reference << "test" << "quill";
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Subject).toStringList(),
             reference);
}

void ut_metadata::testCity()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_City).toString(),
             QString("Tapiola"));
}

void ut_metadata::testCountry()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Country).toString(),
             QString("Finland"));
}

void ut_metadata::testRating()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Rating).toInt(),
             5);
}

void ut_metadata::testCreator()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Creator).toString(),
             QString("John Q"));
}

void ut_metadata::testDescription()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Description).toString(),
             QString("Snowman warming up"));
}

void ut_metadata::testTitle()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Title).toString(),
             QString("Funny snowman"));
}

void ut_metadata::testCityIptc()
{
    QVERIFY(iptc->isValid());
    QCOMPARE(iptc->entry(QuillMetadata::Tag_City).toString(),
             QString("Tapiola"));
}

void ut_metadata::testCountryIptc()
{
    QVERIFY(iptc->isValid());
    QCOMPARE(iptc->entry(QuillMetadata::Tag_Country).toString(),
             QString("Finland"));
}

void ut_metadata::testWriteSubject()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    xmp->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QStringList reference;
    reference << "test" << "quill";
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Subject).toStringList(),
             reference);
}

void ut_metadata::testWriteCity()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    xmp->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_City).toString(),
             QString("Tapiola"));
}

void ut_metadata::testWriteCameraMake()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    metadata->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Make).toString(),
             QString("Quill"));
}

void ut_metadata::testWriteDescription()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    xmp->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Description).toString(),
             QString("Snowman warming up"));
}

void ut_metadata::testEditCameraMake()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_Make, QString("Quill"));
    empty.write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Make).toString(),
             QString("Quill"));
}

void ut_metadata::testEditOrientation()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_Orientation, QVariant(7));
    QVERIFY(empty.write(file.fileName()));

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Orientation).toString(),
             QString("7"));
}

void ut_metadata::testEditCity()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_City, QString("Tapiola"));
    empty.write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_City).toString(),
             QString("Tapiola"));
}

void ut_metadata::testEditKeywords()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    QStringList list;
    list << "aquarium" << "Neon Tetra" << "Paracheirodon innesi";

    empty.setEntry(QuillMetadata::Tag_Subject, list);
    empty.write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QStringList resultList =
        writtenMetadata.entry(QuillMetadata::Tag_Subject).toStringList();
    QCOMPARE(resultList, list);
}

void ut_metadata::testDoubleEditKeywords()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    QStringList list, list2;
    list << "aquarium" << "Neon Tetra" << "Paracheirodon innesi";
    list2 << "aquarium" << "Colombian Tetra" << "Hyphessobrycon columbianus";

    empty.setEntry(QuillMetadata::Tag_Subject, list);
    empty.setEntry(QuillMetadata::Tag_Subject, list2);
    empty.write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QStringList resultList =
        writtenMetadata.entry(QuillMetadata::Tag_Subject).toStringList();
    QCOMPARE(resultList, list2);
}

void ut_metadata::testEditDescription()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_Description,
                   QString("Fish"));
    QVERIFY(empty.write(file.fileName()));

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Description).toString(),
             QString("Fish"));
}

void ut_metadata::testEditTitle()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_Title,
                   QString("The Big Fish"));
    QVERIFY(empty.write(file.fileName()));

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Title).toString(),
             QString("The Big Fish"));
}

void ut_metadata::testCrossEdit()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    metadata->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    writtenMetadata.setEntry(QuillMetadata::Tag_Creator, QString("John Quill"));
    QVERIFY(writtenMetadata.write(file.fileName()));

    QuillMetadata writtenMetadata2(file.fileName());
    QVERIFY(writtenMetadata2.isValid());

    QCOMPARE(writtenMetadata2.entry(QuillMetadata::Tag_Creator).toString(),
             QString("John Quill"));
}

void ut_metadata::testCrossEdit2()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    xmp->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    writtenMetadata.setEntry(QuillMetadata::Tag_Make, QString("Quill"));
    QVERIFY(writtenMetadata.write(file.fileName()));

    QuillMetadata writtenMetadata2(file.fileName());
    QVERIFY(writtenMetadata2.isValid());

    QCOMPARE(writtenMetadata2.entry(QuillMetadata::Tag_Make).toString(),
             QString("Quill"));
}

void ut_metadata::testGps()
{
    QVERIFY(gps->isValid());
    QCOMPARE(gps->entry(QuillMetadata::Tag_GPSLatitude).toString(), QString("65"));
    QCOMPARE(gps->entry(QuillMetadata::Tag_GPSLongitude).toString(), QString("30"));
    QCOMPARE(gps->entry(QuillMetadata::Tag_GPSAltitude).toString(), QString("85"));
}

void ut_metadata::testWriteGps()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    gps->write(file.fileName());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSLatitude).toString(), QString("65"));
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSLongitude).toString(), QString("30"));
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSAltitude).toString(), QString("85"));
}

void ut_metadata::testGps_XmpExif()
{
    // TODO: Stop skipping the test once that it becomes possible to only read XMP data
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QSKIP("Reading only XMP data is still unsupported");
#else
    QSKIP("Reading only XMP data is still unsupported", SkipSingle);
#endif

    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    gps->write(file.fileName());

    QuillMetadata editMetadataEXIF(file.fileName(), QuillMetadata::ExifFormat);
    QuillMetadata editMetadataXMP(file.fileName(), QuillMetadata::XmpFormat);

    QVERIFY(editMetadataEXIF.isValid());
    QVERIFY(editMetadataXMP.isValid());

    // Latitude
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSLatitude).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSLatitude).toString());

    // Latitude REF
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSLatitudeRef).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSLatitudeRef).toString());

    // Longitude
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSLongitude).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSLongitude).toString());

    // Longitude REF
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSLongitudeRef).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSLongitudeRef).toString());

    // Altitude
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSAltitude).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSAltitude).toString());

    // Altitude REF
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSAltitudeRef).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSAltitudeRef).toString());

    // Direction
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSImgDirection).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSImgDirection).toString());

    // Direction REF
    QCOMPARE(editMetadataEXIF.entry(QuillMetadata::Tag_GPSImgDirectionRef).toString(),
             editMetadataXMP.entry(QuillMetadata::Tag_GPSImgDirectionRef).toString());
}

void ut_metadata::testEditGps_latitude()
{
    QuillMetadata editMetadata;
    double expected = 60.1639;
    double actual;

    QVERIFY(editMetadata.isValid());

    // Setting a latitude
    editMetadata.setEntry(QuillMetadata::Tag_GPSLatitude, QVariant(expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSLatitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLatitudeRef).toString(), QString("N"));

    // Setting a negative latitude (that is: the latitude reference should be now "S"outh)
    editMetadata.setEntry(QuillMetadata::Tag_GPSLatitude, QVariant(-expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSLatitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLatitudeRef).toString(), QString("S"));

    // 0 degrees latitude is "N"orth
    editMetadata.setEntry(QuillMetadata::Tag_GPSLatitude, QVariant(double(0)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLatitude).toString(), QString("0"));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLatitudeRef).toString(), QString("N"));

    // Manually setting the latitude reference
    editMetadata.setEntry(QuillMetadata::Tag_GPSLatitudeRef, QVariant(QString("S")));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLatitudeRef).toString(), QString("S"));
}

void ut_metadata::testEditGps_longitude()
{
    QuillMetadata editMetadata;
    double expected = 60.1639;
    double actual;

    QVERIFY(editMetadata.isValid());

    // Setting a setEntry
    editMetadata.setEntry(QuillMetadata::Tag_GPSLongitude, QVariant(expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSLongitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLongitudeRef).toString(), QString("E"));

    // Setting a negative longitude (that is: the longitude reference should be now "W"est)
    editMetadata.setEntry(QuillMetadata::Tag_GPSLongitude, QVariant(-expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSLongitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLongitudeRef).toString(), QString("W"));

    // 0 degrees longitude is "E"ast
    editMetadata.setEntry(QuillMetadata::Tag_GPSLongitude, QVariant(double(0)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLongitude).toString(), QString("0"));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLongitudeRef).toString(), QString("E"));

    // Manually setting the longitude reference
    editMetadata.setEntry(QuillMetadata::Tag_GPSLongitudeRef, QVariant(QString("W")));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSLongitudeRef).toString(), QString("W"));
}

void ut_metadata::testEditGps_altitude()
{
    QuillMetadata editMetadata;
    double expected = 60.1639;
    double actual;

    QVERIFY(editMetadata.isValid());

    // Setting a latitude
    editMetadata.setEntry(QuillMetadata::Tag_GPSAltitude, QVariant(expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSAltitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSAltitudeRef).toString(), QString("0"));

    // Setting a negative altitude (that is: the altitude reference should be now "1" (below sea level)
    editMetadata.setEntry(QuillMetadata::Tag_GPSAltitude, QVariant(-expected));
    actual = editMetadata.entry(QuillMetadata::Tag_GPSAltitude).toDouble();
    QCOMPARE(round(actual * PRECISION), round(expected * PRECISION));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSAltitudeRef).toString(), QString("1"));

    // 0 meters altitude is over sea level
    editMetadata.setEntry(QuillMetadata::Tag_GPSAltitude, QVariant(double(0)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSAltitude).toString(), QString("0"));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSAltitudeRef).toString(), QString("0"));

    // Manually setting the altitude reference
    editMetadata.setEntry(QuillMetadata::Tag_GPSAltitudeRef, QVariant(int(1)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSAltitudeRef).toString(), QString("1"));
}

void ut_metadata::testEditGps_direction()
{
    QuillMetadata editMetadata;

    QVERIFY(editMetadata.isValid());

    // Setting image direction between 0.00 and 359.99
    editMetadata.setEntry(QuillMetadata::Tag_GPSImgDirection, QVariant(double(25.25)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSImgDirection).toString(), QString("25.25"));

    // Setting image under 0.00
    editMetadata.setEntry(QuillMetadata::Tag_GPSImgDirection, QVariant(double(-15)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSImgDirection).toString(), QString("345"));

    // Setting image over 359.99
    editMetadata.setEntry(QuillMetadata::Tag_GPSImgDirection, QVariant(double(400)));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSImgDirection).toString(), QString("40"));

    // Manually setting the direction reference
    editMetadata.setEntry(QuillMetadata::Tag_GPSImgDirectionRef, QVariant(QString("T")));
    QCOMPARE(editMetadata.entry(QuillMetadata::Tag_GPSImgDirectionRef).toString(), QString("T"));
}

void ut_metadata::testClearGps()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");

    QuillMetadata gps(imagePath + "gps.jpg");
    gps.removeEntries(QuillMetadata::TagGroup_GPS);
    QVERIFY(gps.write(file.fileName()));

    QCOMPARE(gps.entry(QuillMetadata::Tag_GPSLatitude).toString(), QString());
    QCOMPARE(gps.entry(QuillMetadata::Tag_GPSLongitude).toString(), QString());
    QCOMPARE(gps.entry(QuillMetadata::Tag_GPSAltitude).toString(), QString());

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());

    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSLatitude).toString(), QString(""));
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSLongitude).toString(), QString(""));
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_GPSAltitude).toString(), QString(""));
}

void ut_metadata::testCopyOrientation()
{
    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");

    QuillMetadata empty;
    empty.setEntry(QuillMetadata::Tag_Orientation,
                   metadata->entry(QuillMetadata::Tag_Orientation));
    QVERIFY(empty.write(file.fileName()));

    QuillMetadata writtenMetadata(file.fileName());
    QVERIFY(writtenMetadata.isValid());
    QCOMPARE(writtenMetadata.entry(QuillMetadata::Tag_Orientation).toString(),
             QString("3"));
}

void ut_metadata::testCanRead()
{
    QVERIFY(QuillMetadata::canRead(imagePath+"exif.jpg"));
    QVERIFY(QuillMetadata::canRead(imagePath+"iptc.jpg"));
    QVERIFY(QuillMetadata::canRead(imagePath+"xmp.jpg"));
    QVERIFY(QuillMetadata::canRead(imagePath+"gps.jpg"));

    QTemporaryFile file;
    file.open();
    QVERIFY(!QuillMetadata::canRead(file.fileName()));
}

//we add the case to test dump function by creating medatedata object with file name from other team.
void ut_metadata::testSetOrientationTag()
{
    QVERIFY(QuillMetadata::canRead(imagePath+"exif.jpg"));
    metadata->setEntry(QuillMetadata::Tag_Orientation, QVariant(short(7)));
    QByteArray result = metadata->dump(QuillMetadata::ExifFormat);
    QVERIFY(!result.isNull());
}

void ut_metadata::testOrientationTagSpeedup()
{
    QString str(imagePath+"16_color_palette.png");

    for (int i = 0; i<=8; i++){
        QTemporaryFile file;
        file.open();
        sourceImage.save(file.fileName(), "jpg");

        QuillMetadata empty;
        empty.setEntry(QuillMetadata::Tag_Orientation, QVariant(i));
        QVERIFY(empty.write(file.fileName()));

        int orientation1, orientation2;
        {
            QuillMetadata *metadata = new QuillMetadata(file.fileName(),
                                                        QuillMetadata::ExifFormat);
            QVERIFY(metadata->isValid());
            QCOMPARE(metadata->entry(QuillMetadata::Tag_Orientation).toInt(), i);

            orientation1 = metadata->entry(QuillMetadata::Tag_Orientation).toInt();

            delete metadata;
        }
        {
            QuillMetadata *metadata = new QuillMetadata(file.fileName(),
                                                        QuillMetadata::ExifFormat,
                                                        QuillMetadata::Tag_Orientation);
            QVERIFY(metadata->isValid());
            QCOMPARE(metadata->entry(QuillMetadata::Tag_Orientation).toInt(), i);
            orientation2 = metadata->entry(QuillMetadata::Tag_Orientation).toInt();

            delete metadata;
        }
        QCOMPARE(orientation1, orientation2);
    }
}


int main ( int argc, char *argv[] ){
    QCoreApplication app( argc, argv );
    ut_metadata test;
    return QTest::qExec( &test, argc, argv );
}
