/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Alexander Bokovoy <alexander.bokovoy@nokia.com>
**
** This file is part of the Quill package.
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
#include "ut_metadata.h"

ut_metadata::ut_metadata()
{
}

void ut_metadata::initTestCase()
{
}

void ut_metadata::cleanupTestCase()
{
}

void ut_metadata::init()
{
    metadata = new QuillMetadata("/usr/share/libquillmetadata-tests/images/exif.jpg");
    xmp = new QuillMetadata("/usr/share/libquillmetadata-tests/images/xmp.jpg");
    iptc = new QuillMetadata("/usr/share/libquillmetadata-tests/images/iptc.jpg");
}

void ut_metadata::cleanup()
{
    delete metadata;
    delete xmp;
    delete iptc;
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

void ut_metadata::testSubject()
{
    QVERIFY(xmp->isValid());
    QCOMPARE(xmp->entry(QuillMetadata::Tag_Subject).toString(),
             QString("test,quill"));
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


int main ( int argc, char *argv[] ){
    QCoreApplication app( argc, argv );
    ut_metadata test;
    return QTest::qExec( &test, argc, argv );
}