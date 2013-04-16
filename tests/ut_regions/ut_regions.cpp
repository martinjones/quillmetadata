/****************************************************************************
**
** Copyright (C) 2009-11 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ut_regions.h"

#define PRECISION 10000

ut_regions::ut_regions()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    imagePath = "/usr/share/libquillmetadata-qt5-tests/images/";
#else
    imagePath = "/usr/share/libquillmetadata-tests/images/";
#endif
}

void ut_regions::initTestCase()
{
    sourceImage = QImage(QSize(8, 2), QImage::Format_RGB32);
    sourceImage.fill(qRgb(255, 255, 255));
}

void ut_regions::cleanupTestCase()
{
}

// To test namespace registration, define NAMESPACE_REGISTRATION_TEST and run "testCreateRegionBag"
// #define NAMESPACE_REGISTRATION_TEST

void ut_regions::init()
{
    metadata	= new QuillMetadata(imagePath + "exif.jpg");
#ifndef NAMESPACE_REGISTRATION_TEST
    region	= new QuillMetadata(imagePath + "mnaa.jpg");
#endif
}

void ut_regions::cleanup()
{
    delete metadata;
#ifndef NAMESPACE_REGISTRATION_TEST

    delete region;
#endif
}


// Rounds to FUZZYACC precision, then asserts that difference is <= FUZZYACC
#define FUZZYACC 1
#define MYQCOMPARE(x, y) do {\
QVERIFY2(abs(((int)(x/FUZZYACC+.5)) -\
	     ((int)(y/FUZZYACC+.5))) <= 1,\
	 (QString("%1 vs. %2").arg(x).arg(y)).toLatin1());} while (0)

void ut_regions::testReadRegions()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regs = data.value<QuillMetadataRegionList>();
    QCOMPARE(regs.count(), 2);
    QCOMPARE(regs.fullImageSize().width(),  4288);
    QCOMPARE(regs.fullImageSize().height(), 2848);
    // Name:
    QCOMPARE(regs[0].name(), QString("Albert Einstein"));
    QCOMPARE(regs[1].name(), QString("Dilbert"));
    // Type:
    QCOMPARE(regs[0].type(), QString("Face"));
    QCOMPARE(regs[1].type(), QString("Face"));
    // Area:
    {
	QRectF area = regs[0].area();
	MYQCOMPARE(area.width(),	0.15*4288);
	MYQCOMPARE(area.height(),	0.17*2848);
	MYQCOMPARE(area.center().x(),	0.3*4288);
	MYQCOMPARE(area.center().y(),	0.4*2848);
    }
    {
	QRectF area = regs[1].area();
	MYQCOMPARE(area.width(),	0.17*4288);
	MYQCOMPARE(area.height(),	0.15*2848);
	MYQCOMPARE(area.center().x(),	0.4*4288);
	MYQCOMPARE(area.center().y(),	0.3*2848);
    }
}

void ut_regions::testEditRegions()
{
    QVERIFY(region->isValid());
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList bag = data.value<QuillMetadataRegionList>();

    bag[0].setName(QString("This is foo name"));
    bag[0].setType(QString("Pet"));
    bag[0].setExtension("dc:Creator", "Donald Duck");
    QRect area;
    area.setWidth(1);
    area.setHeight(2);
    QPoint centerPoint(3, 4);
    area.moveCenter(centerPoint);
    bag[0].setArea(area);
    QVariant entry;
    entry.setValue(bag);
    region->setEntry(QuillMetadata::Tag_Regions,entry);

    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");

    region->write(file.fileName());

    QuillMetadata *region1 = new QuillMetadata(file.fileName());
    QVariant data1 = region1->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data1.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList bag1 = data1.value<QuillMetadataRegionList>();
    QCOMPARE(bag1.count(), 2);
    QCOMPARE(bag1[0].name(), QString("This is foo name"));
    QCOMPARE(bag1[0].type(), QString("Pet"));
    QCOMPARE(bag1[0].extension("dc:Creator"), QVariant("Donald Duck"));
    {
	QRect area = bag1[0].area();
	QCOMPARE(area.width(),	    1);
	QCOMPARE(area.height(),	    2);
	QCOMPARE(area.center().x(), 3);
	QCOMPARE(area.center().y(), 4);
    }

    delete region1;
}

void ut_regions::testRegion()
{
    QuillMetadataRegion region;
    region.setName("this is testing");
    QString name = region.name();
    QCOMPARE(name, QString("this is testing"));

    QString type("face");
    region.setType(type);
    QString type1 = region.type();
    QCOMPARE(type, type1);

    QRect rect(0,0,2,3);
    region.setArea(rect);
    QRect rect1 = region.area();
    QCOMPARE(rect,rect1);

    QString extension("Donald Duck");
    region.setExtension("dc:Creator", extension);
    QString extension1 = region.extension("dc:Creator").toString();
    QCOMPARE(extension,extension1);
}

void ut_regions::testRegionBag()
{
    QuillMetadataRegionList regionBag;

    QSize dimension;
    int width = 5;
    int height = 6;
    dimension = QSize(width, height);
    regionBag.setFullImageSize(dimension);
    QSize dimension1 = regionBag.fullImageSize();
    QCOMPARE(dimension.width(),dimension1.width());
    QCOMPARE(dimension.height(),dimension1.height());
}

void ut_regions::testRegionAssignment()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regionBag = data.value<QuillMetadataRegionList>();
    QuillMetadataRegion region = regionBag[0];

    QuillMetadataRegion region1;

    region1 = region;

    QCOMPARE(region1.type(),  region.type());
    QCOMPARE(region1.area(),	    region.area());
    QCOMPARE(region1.name(),	    region.name());
}


void ut_regions::testRegionBagAssignment()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regionBag = data.value<QuillMetadataRegionList>();

    QuillMetadataRegionList regionBag1;

    regionBag1 = regionBag;

    QCOMPARE(regionBag1.count(), regionBag.count());
    QCOMPARE(regionBag1.fullImageSize(), regionBag.fullImageSize());

    for (int i = 0; i < regionBag.count(); i++)
    {
	QCOMPARE(regionBag1[i].type(),regionBag[i].type());
	QCOMPARE(regionBag1[i].area(),	    regionBag[i].area());
	QCOMPARE(regionBag1[i].name(),	    regionBag[i].name());
    }
}

void ut_regions::testRegionBagAppend()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regs = data.value<QuillMetadataRegionList>();

    regs.append(regs[0]);
    regs[2].setName("appended region");

    QVariant entry;
    entry.setValue(regs);
    region->setEntry(QuillMetadata::Tag_Regions,entry);

    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    region->write(file.fileName());
    QuillMetadata *region1 = new QuillMetadata(file.fileName());
    QVariant data1 = region1->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data1.canConvert<QuillMetadataRegionList>());

    QuillMetadataRegionList regs1 = data1.value<QuillMetadataRegionList>();

    QCOMPARE(regs1.count(), 3);
    QCOMPARE(regs.fullImageSize().width(),  4288);
    QCOMPARE(regs1.fullImageSize().width(),  4288);
    //QCOMPARE(regs1.fullImageSize().height(), 2848);
    // Name:
    QCOMPARE(regs1[0].name(), QString("Albert Einstein"));
    QCOMPARE(regs1[1].name(), QString("Dilbert"));
    QCOMPARE(regs1[2].name(), QString("appended region"));
    // Type:
    QCOMPARE(regs1[0].type(), QString("Face"));
    QCOMPARE(regs1[1].type(), QString("Face"));
    QCOMPARE(regs1[2].type(), QString("Face"));
    // Area:
    for (int i=0; i<3; i+=2){
	QRect area = regs[i].area();
	MYQCOMPARE(area.width(),	0.15*4288);
	MYQCOMPARE(area.height(),	0.17*2848);
	MYQCOMPARE(area.center().x(),	0.3*4288);
	MYQCOMPARE(area.center().y(),	0.4*2848);
    }
    {
	QRectF area = regs[1].area();
	MYQCOMPARE(area.width(),	0.17*4288);
	MYQCOMPARE(area.height(),	0.15*2848);
	MYQCOMPARE(area.center().x(),	0.4*4288);
	MYQCOMPARE(area.center().y(),	0.3*2848);
    }
    delete region1;
}

void ut_regions::testRegionBagRemoveRegion()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regs = data.value<QuillMetadataRegionList>();

    while (regs.count()>0) // Remove, until no regions left.
    {
        regs.removeFirst();

        QVariant entry;
        entry.setValue(regs);
        region->setEntry(QuillMetadata::Tag_Regions,entry);

        QTemporaryFile file;
        file.open();
        sourceImage.save(file.fileName(), "jpg");
        region->write(file.fileName());

        QuillMetadata *region1 = new QuillMetadata(file.fileName());
        QVariant data1 = region1->entry(QuillMetadata::Tag_Regions);
        if (!data1.isNull())
        {
            QVERIFY(data1.canConvert<QuillMetadataRegionList>());
            QuillMetadataRegionList regs1 = data1.value<QuillMetadataRegionList>();
            QCOMPARE(regs1.count(), regs.count());

            QCOMPARE(regs1.fullImageSize().width(),  4288);
            QCOMPARE(regs1.fullImageSize().height(), 2848);
            // Name:
            QCOMPARE(regs1.last().name(), QString("Dilbert"));
            // Type:
            QCOMPARE(regs1.last().type(), QString("Face"));
            // Area:
            {
                QRect area = regs1.last().area();
                MYQCOMPARE(area.width(),	0.17*4288);
                MYQCOMPARE(area.height(),	0.15*2848);
                MYQCOMPARE(area.center().x(),	0.4*4288);
                MYQCOMPARE(area.center().y(),	0.3*2848);
            }
        }
        delete region1;
    }
}

void ut_regions::testCreateRegionBag()
{
    QuillMetadataRegionList regionBag;

    QuillMetadataRegion region;
    region.setName("this is testing");

    QString type("face");
    region.setType(type);

    QRect rect(10,20,30,40);
    region.setArea(rect);

    int width = 50;
    int height = 60;
    QSize dimension = QSize(width, height);
    regionBag.setFullImageSize(dimension);

    regionBag.reserve(1);
    regionBag.append(region);

    QVariant data = metadata->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.isNull());

    QVariant entry;
    entry.setValue(regionBag);
    metadata->setEntry(QuillMetadata::Tag_Regions,entry);
    QVariant data1 = metadata->entry(QuillMetadata::Tag_Regions);


    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");
    metadata->write(file.fileName());

    QuillMetadata *region1 = new QuillMetadata(file.fileName());
    QVariant data2 = region1->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data2.canConvert<QuillMetadataRegionList>());

    QuillMetadataRegionList regs1 = data1.value<QuillMetadataRegionList>();

    QCOMPARE(regs1[0].name(), QString("this is testing"));
    QCOMPARE(regs1[0].type(), QString("face"));
    {
	QRect area = regs1[0].area();
	MYQCOMPARE(area.x(),	    10);
	MYQCOMPARE(area.y(),	    20);
	MYQCOMPARE(area.width(),    30);
	MYQCOMPARE(area.height(),   40);
    }
    delete region1;
}

void ut_regions::testReadExtension()
{
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList regs = data.value<QuillMetadataRegionList>();
    QCOMPARE(regs.count(), 2);
    QCOMPARE(regs[0].extension("xmpRights:UsageTerms"),
             QVariant("copyright Phil Harvey"));
}

void ut_regions::testImplicitSharing()
{
    QuillMetadataRegion region;
    region.setName("Title");
    QuillMetadataRegion region2 = region;
    region2.setName("Title 2");
    QCOMPARE(region.name(), QString("Title"));
}

void ut_regions::testNcoRegions()
{
    QVERIFY(region->isValid());
    QVariant data = region->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList bag = data.value<QuillMetadataRegionList>();

    bag[0].setExtension("nco:PersonContact", "Donald Duck");

    QVariant entry;
    entry.setValue(bag);
    region->setEntry(QuillMetadata::Tag_Regions,entry);

    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");

    region->write(file.fileName());

    QuillMetadata *region1 = new QuillMetadata(file.fileName());
    QVariant data1 = region1->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data1.canConvert<QuillMetadataRegionList>());
    QuillMetadataRegionList bag1 = data1.value<QuillMetadataRegionList>();
    QCOMPARE(bag1.count(), 2);
    QCOMPARE(bag1[0].extension("nco:PersonContact"), QVariant("Donald Duck"));

    delete region1;
}

void ut_regions::testRemoveAllRegionData()
{
    QuillMetadata *region = new QuillMetadata(imagePath + "mnaa.jpg");
    region->removeEntry(QuillMetadata::Tag_Regions);

    QTemporaryFile file;
    file.open();
    sourceImage.save(file.fileName(), "jpg");

    region->write(file.fileName());

    QuillMetadata *region1 = new QuillMetadata(file.fileName());
    QVariant data1 = region1->entry(QuillMetadata::Tag_Regions);
    QVERIFY(data1.isNull());

    delete region;
    delete region1;
}

int main ( int argc, char *argv[] ){
    QCoreApplication app( argc, argv );
    ut_regions test;
    return QTest::qExec( &test, argc, argv );
}
