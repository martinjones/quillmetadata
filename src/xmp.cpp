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

#include <QStringList>
#include <QLocale>
#include <QTextStream>
#include <exempi-2.0/exempi/xmpconsts.h>
#include <math.h>
#include "xmp.h"
#include "quillmetadataregionlist.h"

QHash<QuillMetadata::Tag,XmpTag> Xmp::m_xmpTags;
QHash<Xmp::Tag,XmpRegionTag> Xmp::m_regionXmpTags;

bool Xmp::m_initialized = false;

XmpTag::XmpTag(const QString &schema, const QString &tag, TagType tagType) :
    schema(schema), tag(tag), tagType(tagType)
{
}

XmpTag::XmpTag() :
    schema(""), tag(""), tagType(TagTypeString)
{
}


XmpRegionTag::XmpRegionTag(const QString &schema, const QString &baseTag,
               const QString &tag, TagType tagType) :
XmpTag(schema, tag, tagType), baseTag(baseTag)
{
}

XmpRegionTag::XmpRegionTag() :
    XmpTag(), baseTag("")
{
}

QString XmpRegionTag::getIndexedTag(int zeroBasedIndex)
{
    if (baseTag.isEmpty())
    return tag;
    else
    return baseTag + QString("%1").arg(zeroBasedIndex+1) + tag;
}

Xmp::Xmp()
{
    xmp_init();
    m_xmpPtr = xmp_new_empty();
    initTags();
}

Xmp::Xmp(const QString &fileName)
{
    xmp_init();
    XmpFilePtr xmpFilePtr = xmp_files_open_new(fileName.toLocal8Bit().constData(),
                                               XMP_OPEN_READ);
    m_xmpPtr = xmp_files_get_new_xmp(xmpFilePtr);
    xmp_files_close(xmpFilePtr, XMP_CLOSE_NOOPTION);
    xmp_files_free(xmpFilePtr);

    initTags();
}

Xmp::~Xmp()
{
    xmp_free(m_xmpPtr);
}

bool Xmp::isValid() const
{
    return (m_xmpPtr != 0);
}

bool Xmp::supportsEntry(QuillMetadata::Tag tag) const
{
    return m_xmpTags.contains(tag);
}

bool Xmp::hasEntry(QuillMetadata::Tag tag) const
{
    QList<XmpTag> xmpTags = m_xmpTags.values(tag);
    if (!xmpTags.isEmpty()) {
    foreach (XmpTag tag, xmpTags) {
        if (xmp_has_property(m_xmpPtr,
                 tag.schema.toLatin1().constData(),
                 tag.tag.toLatin1().constData()))
        return true;
    }
    }
    return false;
}

bool Xmp::hasEntry(Xmp::Tag tag, int zeroBasedIndex) const
{
    XmpRegionTag xmpTag = m_regionXmpTags.value(tag);
    if (xmpTag.tag.isEmpty())
    return false;

    return (xmp_has_property(m_xmpPtr,
                 xmpTag.schema.toLatin1().constData(),
                 xmpTag.getIndexedTag(zeroBasedIndex).toLatin1().constData()));
}

QString Xmp::processXmpString(XmpStringPtr xmpString)
{
    return QString(xmp_string_cstr(xmpString)).trimmed();
}

void Xmp::readRegionListItem(const QString & qPropValue,
                 const QString & qPropName,
                 QuillMetadataRegionList & regions) const
{
    QString searchString(m_regionXmpTags.value(Tag_RegionList).tag);
    QRegExp rx("(" + searchString + ".)(\\d+).");
    rx.indexIn(qPropName);

    bool isOk = false;
    int nRegionNumber = rx.cap(2).toInt(&isOk);
    // TODO: Sanity check for number of regions

    if (!isOk)  //RegionList.%d. wasn't found
    return;

    searchString.append(
        "[" + QString::number(nRegionNumber) + "]");

    if (!qPropName.contains(searchString)) //RegionList[nRegionNumber] wasn't found
    return;

    while (regions.size() < nRegionNumber) {
    QuillMetadataRegion region;
    regions.append(region);
    }

    QuillMetadataRegion region = regions[nRegionNumber-1];
    {
    if (qPropName.contains(m_regionXmpTags.value(Tag_RegionArea).tag)) {

        QRectF area = region.areaF();

        if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAreaH).tag)) {
        QPointF center = area.center();
        area.setHeight(qPropValue.toFloat());
        area.moveCenter(center);
        } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAreaW).tag)) {
        QPointF center = area.center();
        area.setWidth(qPropValue.toFloat());
        area.moveCenter(center);
        } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAreaX).tag)) {
        area.moveCenter(
            QPointF(qPropValue.toFloat(), area.center().y()));
        } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAreaY).tag)) {
        area.moveCenter(
            QPointF(area.center().x(), qPropValue.toFloat()));
        }

        region.setAreaF(area);

    } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionName).tag)) {

        region.setName(qPropValue);

    } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionType).tag)) {

        region.setType(qPropValue);

    } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionExtension).tag)) {

            if (!qPropValue.isEmpty()) {
        QString tagName = m_regionXmpTags.value(Tag_RegionExtension).tag;
        QString tag = qPropName.mid(qPropName.indexOf(tagName) + tagName.length() + 1);
                if (!tag.isNull())
                    tag = tag.split("[").first();

                if (!tag.isEmpty()) {
                    region.setExtension(tag, qPropValue);
                }
            }
    }
    }
    regions[nRegionNumber-1] = region;

    return;
}

QVariant Xmp::entry(QuillMetadata::Tag tag) const
{
    if (!supportsEntry(tag))
        return QVariant();

    QList<XmpTag> xmpTags = m_xmpTags.values(tag);

    XmpStringPtr xmpStringPtr = xmp_string_new();

    foreach (XmpTag xmpTag, xmpTags) {
        uint32_t propBits;

    if (xmp_get_property(m_xmpPtr,
                 xmpTag.schema.toLatin1().constData(),
                             xmpTag.tag.toLatin1().constData(),
                             xmpStringPtr,
                             &propBits)) {

            if (XMP_IS_PROP_ARRAY(propBits)) {
        QStringList list;
                int i = 1;
                while (xmp_get_array_item(m_xmpPtr,
                                          xmpTag.schema.toLatin1().constData(),
                                          xmpTag.tag.toLatin1().constData(),
                                          i,
                                          xmpStringPtr,
                                          &propBits)) {
                    QString string = processXmpString(xmpStringPtr);
                    if (!string.isEmpty())
                        list << string;
                    i++;
                }

                if (!list.isEmpty()) {
                    xmp_string_free(xmpStringPtr);
                    return QVariant(list);
                }


        } else if (XMP_IS_PROP_STRUCT(propBits)) {

        XmpIterOptions iterOpts = XMP_ITER_OMITQUALIFIERS;

        XmpIteratorPtr xmpIterPtr = xmp_iterator_new(
            m_xmpPtr, xmpTag.schema.toLatin1().constData(),
            xmpTag.tag.toLatin1().constData(),
            iterOpts);

        XmpStringPtr schema = xmp_string_new();
        XmpStringPtr propName = xmp_string_new();
        XmpStringPtr propValue = xmp_string_new();

        uint32_t options;
        QuillMetadataRegionList regions;

        bool bSuccess = xmp_iterator_next(
            xmpIterPtr, schema, propName,
            propValue, &options);

        while (bSuccess)
        {
            QString qPropValue	= processXmpString(propValue);
            QString qPropName	= processXmpString(propName);

            if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAppliedToDimensions).tag)) {

            if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAppliedToDimensionsH).tag)) {
                regions.setFullImageSize(
                    QSize(regions.fullImageSize().width(), qPropValue.toInt()));
            } else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionAppliedToDimensionsW).tag)) {
                regions.setFullImageSize(
                    QSize(qPropValue.toInt(), regions.fullImageSize().height()));
            }

            }

            else if (qPropName.contains(m_regionXmpTags.value(Tag_RegionList).tag)) {

            this->readRegionListItem(qPropValue, qPropName, regions);

            }

            bSuccess = xmp_iterator_next(
                xmpIterPtr, schema, propName,
                propValue, &options);
        } // while (bSuccess)

        xmp_string_free(schema);
        xmp_string_free(propName);
        xmp_string_free(propValue);

        QVariant var;
        regions.updatePixelCoordinates();
        var.setValue(regions);
        return var;


        } else {
                QString string = processXmpString(xmpStringPtr);
                if (!string.isEmpty()) {
                    xmp_string_free(xmpStringPtr);
                    switch (tag) {
            case QuillMetadata::Tag_GPSLatitude:
            case QuillMetadata::Tag_GPSLongitude: {
                            // Degrees and minutes are separated with a ','
                            QStringList elements = string.split(",");
                            QLocale c(QLocale::C);
                            double value = 0, term = 0;
                            for (int i = 0, power = 1; i < elements.length(); i ++, power *= 60) {
                                term = c.toDouble(elements[i]);
                                if (i == elements.length() - 1) {
                                    term = c.toDouble(elements[i].mid(0, elements[i].length() - 2));
                                }
                                value += term / power;
                            }

                            return QVariant(value);
                        }

            case QuillMetadata::Tag_GPSLatitudeRef:
            case QuillMetadata::Tag_GPSLongitudeRef: {
                            // The 'W', 'E', 'N' or 'S' character is the rightmost character
                            // in the field
                            return QVariant(string.right(1));
                        }

            case QuillMetadata::Tag_GPSImgDirection:
            case QuillMetadata::Tag_GPSAltitude: {
                            const int separator = string.indexOf("/");
                            const int len = string.length();
                            QLocale c(QLocale::C);

                            double numerator = c.toDouble(string.mid(0, separator));
                            double denominator = c.toDouble(string.mid(separator + 1, len - separator - 1));

                            if (denominator && separator != -1) {
                                return QVariant(numerator / denominator);
                            }
                            else {
                                return QVariant(numerator);
                            }
                        }

            default:
            return QVariant(string);
            }
                }
            }
    }
    }

    xmp_string_free(xmpStringPtr);
    return QVariant();
}

void Xmp::setEntry(QuillMetadata::Tag tag, const QVariant &entry)
{
    if (!supportsEntry(tag))
        return;

    if (!m_xmpPtr) {
        m_xmpPtr = xmp_new_empty();
    }

    switch (tag) {
    case QuillMetadata::Tag_GPSAltitude: {
            QString altitude = entry.toString();

            // Check the existence of a slash. If there's not one, append it
            if (!altitude.contains("/")) {
                altitude.append("/1");
            }

            // Sanity check: if the input starts with "-", remove it and update the reference field
            if (altitude.startsWith("-")) {
                setXmpEntry(QuillMetadata::Tag_GPSAltitudeRef, QVariant(1));
                setXmpEntry(tag, QVariant(altitude.mid(1)));
            }
            else {
                setXmpEntry(QuillMetadata::Tag_GPSAltitudeRef, QVariant(0));
                setXmpEntry(tag, QVariant(altitude));
            }

            break;
        }
    case QuillMetadata::Tag_GPSLatitude:
    case QuillMetadata::Tag_GPSLongitude: {
            QString value = entry.toString();
            const QString refPositive(tag == QuillMetadata::Tag_GPSLatitude ? "N" : "E");
            const QString refNegative(tag == QuillMetadata::Tag_GPSLatitude ? "S" : "W");

            if (!value.contains(",")) {
                double val = value.toDouble();
                QString parsedEntry;
                val = fabs(val);
                double remains = (val - trunc(val)) * 3600;

                QTextStream(&parsedEntry) << trunc(val) << ","
                        << trunc(remains / 60) << ","
                        << remains - trunc(remains / 60) * 60;

                if (value.startsWith("-")) {
                    parsedEntry.append(refNegative);
                }
                else {
                    parsedEntry.append(refPositive);
                }

                setXmpEntry(tag, QVariant(parsedEntry));
            }
            else if (value.endsWith(refPositive) || value.endsWith(refNegative)) {
                if (value.startsWith("-")) {
                    value.replace(value.right(1), refNegative);
                    setXmpEntry(tag, QVariant(value.mid(1)));
                }
            } else {
                value.append(value.startsWith("-") ? refNegative : refPositive);
                setXmpEntry(tag, QVariant(value));
            }

            break;
        }
    case QuillMetadata::Tag_GPSImgDirection: {
            QString direction;
            const int slash = entry.toString().indexOf("/");
            double value;

            // Check the existence of a slash. If there's not one, append it
            if (slash == -1) {
                value = entry.toDouble();
                value = fmod(value, 360);
                if (value < 0) {
                    value = 360 + value;
                }
                QTextStream(&direction) << value << "/1";
            }
            else {
                QLocale c(QLocale::C);
                value = c.toDouble(direction.mid(0, slash)) / c.toDouble(direction.mid(slash + 1, direction.length() - slash - 1));
                value = fmod(value, 360);
                if (value < 0) {
                    value = 360 + value;
                }
                QTextStream(&direction) << value << "/1";
            }

            setXmpEntry(tag, QVariant(direction));
            break;
        }
    case QuillMetadata::Tag_Regions: {
        QuillMetadataRegionList regions = entry.value<QuillMetadataRegionList>();

        {
        XmpTag xmpTag = m_xmpTags.value(QuillMetadata::Tag_Regions);
        if (regions.count() == 0) { // No regions to be written: delete all
            removeEntry(QuillMetadata::Tag_Regions);
            break;
        }

        if (regions.count() > 0) { // Regions to be written: create if needed
            if (!hasEntry(QuillMetadata::Tag_Regions)) {
            setXmpEntry(QuillMetadata::Tag_Regions, QVariant(""));
            }
            if (!hasEntry(Xmp::Tag_RegionList)) {
            setXmpEntry(Xmp::Tag_RegionList, QVariant(""));
            }
        }


        // Write all tags of all regions.
        // RegionAppliedToDimensionsH
        setXmpEntry(Xmp::Tag_RegionAppliedToDimensionsH,
                regions.fullImageSize().height());

        // RegionAppliedToDimensionsW
        setXmpEntry(Xmp::Tag_RegionAppliedToDimensionsW,
                regions.fullImageSize().width());
        }

        int nRegion;
        regions.updateRelativeCoordinates();

        for (nRegion = 0; nRegion < regions.count(); nRegion++) {

        if (!hasEntry(Xmp::Tag_RegionListItem, nRegion)) {
            setXmpEntry(Xmp::Tag_RegionListItem, nRegion, "", "");
        }
        if (!hasEntry(Xmp::Tag_RegionArea, nRegion)) {
            setXmpEntry(Xmp::Tag_RegionArea, nRegion, "", "");
        }

        QuillMetadataRegion region = regions[nRegion];

        {

            // RegionAreaX,
            setXmpEntry(Xmp::Tag_RegionAreaX, nRegion, "",
                region.areaF().center().x());
            // RegionAreaY
            setXmpEntry(Xmp::Tag_RegionAreaY, nRegion, "",
                region.areaF().center().y());
            // RegionAreaH
            setXmpEntry(Xmp::Tag_RegionAreaH, nRegion, "",
                region.areaF().height());
            // RegionAreaW
            setXmpEntry(Xmp::Tag_RegionAreaW, nRegion, "",
                region.areaF().width());

        }

        // Region name
        setXmpEntry(Xmp::Tag_RegionName, nRegion, "",
                region.name());
        // Region type
        setXmpEntry(Xmp::Tag_RegionType, nRegion, "",
                region.type());

        // Region extension
        // before we set extensions, we clear all extensions
        // this->removeEntry(Xmp::Tag_RegionExtension, nRegion);

                foreach(QString tag, region.listExtensionTags())
                    if(!region.extension(tag).isNull()) {
                        setXmpEntry(Xmp::Tag_RegionExtension, nRegion,
                                    tag, region.extension(tag));
        }
        }

        // Delete regions that aren't valid anymore
        while (hasEntry(Xmp::Tag_RegionListItem, nRegion)) {
        removeEntry(Xmp::Tag_RegionListItem, nRegion);
        nRegion++;
        }

        break;
    }
    default:
    setXmpEntry(tag, entry);
    break;
    }
}

void Xmp::setXmpEntry(QuillMetadata::Tag tag, const QVariant &entry)
{
    QList<XmpTag> xmpTags = m_xmpTags.values(tag);
    foreach (XmpTag xmpTag, xmpTags) {
    setXmpEntry(xmpTag, entry);
    }
}

void Xmp::setXmpEntry(Xmp::Tag tag, const QVariant &entry)
{
    setXmpEntry(tag, 0, "", entry);
}

void Xmp::setXmpEntry(Xmp::Tag tag, int zeroBasedIndex,
                      const QString &suffix, const QVariant &entry)
{
    XmpRegionTag xmpTag = m_regionXmpTags.value(tag);
    QString fullTag = xmpTag.getIndexedTag(zeroBasedIndex);
    if (!suffix.isEmpty())
        fullTag += "/" + suffix;
    setXmpEntry(XmpTag(xmpTag.schema, fullTag, xmpTag.tagType), entry);
}

void Xmp::setXmpEntry(XmpTag xmpTag, const QVariant &entry)
{
    xmp_delete_property(m_xmpPtr,
            xmpTag.schema.toLatin1().constData(),
            xmpTag.tag.toLatin1().constData());

    if (xmpTag.tagType == XmpTag::TagTypeString){
    xmp_set_property(m_xmpPtr,
             xmpTag.schema.toLatin1().constData(),
             xmpTag.tag.toLatin1().constData(),
             entry.toString().toUtf8().constData(), 0);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeStruct){
    xmp_set_property(m_xmpPtr,
             xmpTag.schema.toLatin1().constData(),
             xmpTag.tag.toLatin1().constData(),
             entry.toString().toUtf8().constData(), XMP_PROP_VALUE_IS_STRUCT);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeArray){
    xmp_set_property(m_xmpPtr,
             xmpTag.schema.toLatin1().constData(),
             xmpTag.tag.toLatin1().constData(),
             entry.toString().toUtf8().constData(), XMP_PROP_VALUE_IS_ARRAY);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeStringList) {
    QStringList list = entry.toStringList();
    foreach (QString string, list)
        xmp_append_array_item(m_xmpPtr,
                  xmpTag.schema.toLatin1().constData(),
                  xmpTag.tag.toLatin1().constData(),
                  XMP_PROP_ARRAY_IS_UNORDERED,
                  string.toUtf8().constData(), 0);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeAltLang) {
    xmp_set_localized_text(m_xmpPtr,
                   xmpTag.schema.toLatin1().constData(),
                   xmpTag.tag.toLatin1().constData(),
                   "", "x-default",
                   entry.toString().toUtf8().constData(), 0);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeReal) {
    xmp_set_property_float(m_xmpPtr,
                   xmpTag.schema.toLatin1().constData(),
                   xmpTag.tag.toLatin1().constData(),
                   entry.toReal(), 0);
    }
    else if (xmpTag.tagType == XmpTag::TagTypeInteger) {
    xmp_set_property_int32(m_xmpPtr,
                   xmpTag.schema.toLatin1().constData(),
                   xmpTag.tag.toLatin1().constData(),
                   entry.toInt(), 0);
    }
}

void Xmp::removeEntry(QuillMetadata::Tag tag)
{
    if (!supportsEntry(tag))
    return;

    if (!m_xmpPtr)
    return;

    QList<XmpTag> xmpTags = m_xmpTags.values(tag);

    foreach (XmpTag xmpTag, xmpTags) {
    xmp_delete_property(m_xmpPtr,
                xmpTag.schema.toLatin1().constData(),
                xmpTag.tag.toLatin1().constData());
    }
}

void Xmp::removeEntry(Xmp::Tag tag, int zeroBasedIndex)
{
    XmpRegionTag xmpTag = m_regionXmpTags.value(tag);
    if (xmpTag.tag.isEmpty())
    return;

    xmp_delete_property(m_xmpPtr,
                 xmpTag.schema.toLatin1().constData(),
                 xmpTag.getIndexedTag(zeroBasedIndex).toLatin1().constData());
}

bool Xmp::write(const QString &fileName) const
{
    XmpPtr ptr = m_xmpPtr;

    if (!ptr)
    ptr = xmp_new_empty();

    XmpFilePtr xmpFilePtr = xmp_files_open_new(fileName.toLocal8Bit().constData(),
                                               XMP_OPEN_FORUPDATE);
    bool result;

    if (xmp_files_can_put_xmp(xmpFilePtr, ptr))
    result = xmp_files_put_xmp(xmpFilePtr, ptr);
    else
    result = false;

    // Crash safety can be ignored here by selecting Nooption since
    // QuillFile already has crash safety measures.
    if (result)
    result = xmp_files_close(xmpFilePtr, XMP_CLOSE_NOOPTION);
    xmp_files_free(xmpFilePtr);

    if (!m_xmpPtr)
    xmp_free(ptr);

    return result;
}

void Xmp::initTags()
{
    if (m_initialized)
    return;

    m_initialized = true;

    const char regionSchema[] = "http://www.metadataworkinggroup.com/schemas/regions/";
    QString regionPrefix("mwg-rs:");
    {

    XmpStringPtr registeredPrefix = xmp_string_new();
    xmp_register_namespace(regionSchema,
                     regionPrefix.toLatin1().constData(),
                     registeredPrefix);
    regionPrefix = processXmpString(registeredPrefix);
    xmp_string_free(registeredPrefix);
    }

    QString xmpAreaPrefix("stArea:");
    {
    const char areaNamespace[] = "http://ns.adobe.com/xmp/sType/Area#";
    XmpStringPtr registeredPrefix = xmp_string_new();
    xmp_register_namespace(areaNamespace,
                      xmpAreaPrefix.toLatin1().constData(),
                      registeredPrefix);
    xmpAreaPrefix = processXmpString(registeredPrefix);
    xmp_string_free(registeredPrefix);
    }

    QString ncoPrefix("nco:");
    {
    const char ncoNamespace[] = "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#";
    XmpStringPtr registeredPrefix = xmp_string_new();
    xmp_register_namespace(ncoNamespace,
                      ncoPrefix.toLatin1().constData(),
                      registeredPrefix);
    ncoPrefix = processXmpString(registeredPrefix);
    xmp_string_free(registeredPrefix);
    }

    m_xmpTags.insertMulti(QuillMetadata::Tag_Creator,
              XmpTag(NS_DC, "creator", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Subject,
              XmpTag(NS_DC, "subject", XmpTag::TagTypeStringList));
    m_xmpTags.insertMulti(QuillMetadata::Tag_City,
              XmpTag(NS_PHOTOSHOP, "City", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Country,
              XmpTag(NS_PHOTOSHOP, "Country", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Location,
              XmpTag(NS_IPTC4XMP, "Location", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_City,
              XmpTag(NS_IPTC4XMP, "LocationShownCity", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Country,
              XmpTag(NS_IPTC4XMP, "LocationShownCountry", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Location,
              XmpTag(NS_IPTC4XMP, "LocationShownSublocation", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Rating,
              XmpTag(NS_XAP, "Rating", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Timestamp,
              XmpTag(NS_XAP, "MetadataDate", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Description,
              XmpTag(NS_DC, "description", XmpTag::TagTypeAltLang));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Orientation,
              XmpTag(NS_EXIF, "Orientation", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Orientation,
              XmpTag(NS_TIFF, "Orientation", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_Title,
              XmpTag(NS_DC, "title", XmpTag::TagTypeAltLang));

    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSLatitude,
              XmpTag(NS_EXIF, "GPSLatitude", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSLongitude,
              XmpTag(NS_EXIF, "GPSLongitude", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSAltitude,
              XmpTag(NS_EXIF, "GPSAltitude", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSAltitudeRef,
              XmpTag(NS_EXIF, "GPSAltitudeRef", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSImgDirection,
              XmpTag(NS_EXIF, "GPSImgDirection", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSImgDirectionRef,
              XmpTag(NS_EXIF, "GPSImgDirectionRef", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSVersionID,
              XmpTag(NS_EXIF, "GPSVersionID", XmpTag::TagTypeString));

    // Workaround for missing reference tags: we'll extract them from the
    // Latitude and Longitude tags
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSLatitudeRef,
              XmpTag(NS_EXIF, "GPSLatitude", XmpTag::TagTypeString));
    m_xmpTags.insertMulti(QuillMetadata::Tag_GPSLongitudeRef,
              XmpTag(NS_EXIF, "GPSLongitude", XmpTag::TagTypeString));


    m_xmpTags.insertMulti(QuillMetadata::Tag_Regions,
              XmpTag(regionSchema,
                 regionPrefix + "Regions", XmpTag::TagTypeStruct));

    /***/

    QString regionsBaseTag(regionPrefix + "Regions/" + regionPrefix); //e.g. "mwg-rs:Regions/mwg-rs:
    m_regionXmpTags.insert(Xmp::Tag_RegionAppliedToDimensions,
               XmpRegionTag(regionSchema,
                    "", regionsBaseTag + "AppliedToDimensions",
                    XmpTag::TagTypeStruct));

    m_regionXmpTags.insert(Xmp::Tag_RegionAppliedToDimensionsH,
               XmpRegionTag(regionSchema,
                    "", regionsBaseTag + "AppliedToDimensions/stDim:h",
                    XmpTag::TagTypeInteger));

    m_regionXmpTags.insert(Xmp::Tag_RegionAppliedToDimensionsW,
               XmpRegionTag(regionSchema,
                    "", regionsBaseTag + "AppliedToDimensions/stDim:w",
                    XmpTag::TagTypeInteger));

    m_regionXmpTags.insert(Xmp::Tag_RegionList,
               XmpRegionTag(regionSchema,
                    "", regionsBaseTag + "RegionList",
                    XmpTag::TagTypeArray));

    QString baseTag(regionsBaseTag + "RegionList["); //e.g. "mwg-rs:Regions/mwg-rs:RegionList["
    m_regionXmpTags.insert(Xmp::Tag_RegionListItem,
               XmpRegionTag(regionSchema, baseTag, "]",
                    XmpTag::TagTypeStruct));


    m_regionXmpTags.insert(Xmp::Tag_RegionExtensionTrackerContact,
               XmpRegionTag(regionSchema,
                    baseTag, QString("]/") + regionPrefix +
                    "Extensions/" + regionPrefix + "TrackerContact",
                    XmpTag::TagTypeString));

    regionPrefix = QString("]/") + regionPrefix; // e.g. ]/mwg-rs:
    m_regionXmpTags.insert(Xmp::Tag_RegionExtension,
               XmpRegionTag(regionSchema,
                    baseTag, regionPrefix + "Extensions",
                    XmpTag::TagTypeString));

    m_regionXmpTags.insert(Xmp::Tag_RegionName,
               XmpRegionTag(regionSchema, baseTag, regionPrefix + "Name",
                    XmpTag::TagTypeString));

    m_regionXmpTags.insert(Xmp::Tag_RegionType,
               XmpRegionTag(regionSchema, baseTag, regionPrefix + "Type",
                    XmpTag::TagTypeString));

    m_regionXmpTags.insert(Xmp::Tag_RegionArea,
               XmpRegionTag(regionSchema, baseTag, regionPrefix + "Area",
                    XmpTag::TagTypeStruct));


    xmpAreaPrefix = regionPrefix + "Area/" + xmpAreaPrefix;
    m_regionXmpTags.insert(Xmp::Tag_RegionAreaH,
               XmpRegionTag(regionSchema, baseTag, xmpAreaPrefix + "h",
                    XmpTag::TagTypeReal));

    m_regionXmpTags.insert(Xmp::Tag_RegionAreaW,
               XmpRegionTag(regionSchema, baseTag, xmpAreaPrefix + "w",
                    XmpTag::TagTypeReal));

    m_regionXmpTags.insert(Xmp::Tag_RegionAreaX,
               XmpRegionTag(regionSchema, baseTag, xmpAreaPrefix + "x",
                    XmpTag::TagTypeReal));

    m_regionXmpTags.insert(Xmp::Tag_RegionAreaY,
               XmpRegionTag(regionSchema, baseTag, xmpAreaPrefix + "y",
                    XmpTag::TagTypeReal));

}
