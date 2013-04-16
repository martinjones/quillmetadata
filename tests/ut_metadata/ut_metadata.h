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

#include <QObject>
#include <QImage>

#ifndef TEST_LIBQUILL_METADATA_H
#define TEST_LIBQUILL_METADATA_H

class QuillMetadata;

class ut_metadata : public QObject {
Q_OBJECT
public:
    ut_metadata();

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();
    void testCameraMake();
    void testCameraModel();
    void testImageWidth();
    void testImageHeight();
    void testFocalLength();
    void testExposureTime();
    void testTimestampOriginal();
    void testSubject();
    void testCity();
    void testCountry();
    void testRating();
    void testCreator();
    void testCityIptc();
    void testCountryIptc();
    void testDescription();
    void testTitle();
    void testOrientation();

    // Unit tests for metadata writing

    void testWriteSubject();
    void testWriteCity();
    void testWriteCameraMake();
    void testWriteDescription();

    // Unit tests for metadata editing

    void testEditCameraMake();
    void testEditOrientation();
    void testOrientationTagSpeedup();
    void testEditCity();
    void testEditKeywords();
    void testDoubleEditKeywords();
    void testEditDescription();
    void testEditTitle();

    void testCrossEdit();
    void testCrossEdit2();

    // GPS metadata unittests

    void testGps();
    void testGps_XmpExif();
    void testEditGps_latitude();
    void testEditGps_longitude();
    void testEditGps_altitude();
    void testEditGps_direction();
    void testWriteGps();
    void testClearGps();

    // Unit tests for orientation preservation

    void testCopyOrientation();
    // Unit tests for format detection

    void testCanRead();
    void testSetOrientationTag();

private:
    QImage sourceImage;

    QuillMetadata *metadata;
    QuillMetadata *xmp;
    QuillMetadata *iptc;
    QuillMetadata *gps;

    QuillMetadata *region;

    QString imagePath;
};

#endif  // TEST_LIBQUILL_METADATA_H
