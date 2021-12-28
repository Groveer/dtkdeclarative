/*
 * Copyright (C) 2021 UnionTech Technology Co., Ltd.
 *
 * Author:     JiDe Zhang <zhangjide@deepin.org>
 *
 * Maintainer: JiDe Zhang <zhangjide@deepin.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DQUICKCONTROLPALETTE_P_H
#define DQUICKCONTROLPALETTE_P_H

#include <dtkdeclarative_global.h>
#include <DQMLGlobalObject>

#include <QColor>
#include <QObject>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QQmlEngine>
#include <QVector>

QT_BEGIN_NAMESPACE
class QQuickItem;
class QQuickWindow;
QT_END_NAMESPACE

DQUICK_BEGIN_NAMESPACE
class DQuickControlColor
{
    friend class DQuickControlPalette;
    Q_GADGET
    Q_PROPERTY(QColor common READ common WRITE setCommon FINAL)
    Q_PROPERTY(QColor crystal READ crystal WRITE setCrystal FINAL)

public:
    DQuickControlColor();
    DQuickControlColor(const QColor &color);
    DQuickControlColor(DQuickControlColor &&other)
        {data = std::move(other.data); other.data = nullptr;
         changed = std::move(other.changed);
         isSingleColor = std::move(other.isSingleColor);}
    DQuickControlColor(const DQuickControlColor &other) {
        isSingleColor = other.isSingleColor;
        if (other.isSingleColor) {
            data = new QColor(*other.data);
        } else {
            data = other.data;
            changed = other.changed;
        }
    }
    inline DQuickControlColor &operator=(const DQuickControlColor &other) {
        if (isSingleColor) {
            if (other.isSingleColor) {
                *data = *other.data;
            } else {
                delete data;
                data = other.data;
                isSingleColor = false;
            }
        } else {
            if (other.isSingleColor) {
                if (data) {
                    setCommon(*other.data);
                } else {
                    data = new QColor(*other.data);
                }
            } else {
                data = other.data;
            }
        }
        changed = other.changed;
        return *this;
    }
    inline DQuickControlColor &operator=(DQuickControlColor &&other) {
        if (data && isSingleColor) {
            delete data;
        }

        data = std::move(other.data);
        other.data = nullptr;
        changed = other.changed;
        isSingleColor = other.isSingleColor;
        return *this;
    }
    ~DQuickControlColor();

    const QColor &common() const;
    void setCommon(const QColor &newCommon);
    const QColor &crystal() const;
    void setCrystal(const QColor &newCrystal);

private:
    DQuickControlColor(QColor *colors);

    QColor *data = nullptr;
    bool changed = false;
    bool isSingleColor = false;
};
DQUICK_END_NAMESPACE
Q_DECLARE_METATYPE(DTK_QUICK_NAMESPACE::DQuickControlColor)

DQUICK_BEGIN_NAMESPACE

class DQuickControlPalette : public QObject
{
    friend class DQuickControlColorSelector;
    Q_OBJECT
    Q_DISABLE_COPY(DQuickControlPalette)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor normalDark READ normalDark WRITE setNormalDark NOTIFY normalDarkChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor hoveredDark READ hoveredDark WRITE setHoveredDark NOTIFY hoveredDarkChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor pressed READ pressed WRITE setPressed NOTIFY pressedChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor pressedDark READ pressedDark WRITE setPressedDark NOTIFY pressedDarkChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor disabled READ disabled WRITE setDisabled NOTIFY disabledChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQuickControlColor disabledDark READ disabledDark WRITE setDisabledDark NOTIFY disabledDarkChanged)

public:
    enum ColorFamily {
        CommonColor = 0,
        CrystalColor = 1,
        ColorFamilyCount
    };
    Q_ENUM(ColorFamily)

    enum ColorGroup {
        Light = 0,
        Dark = 1 * ColorFamilyCount,
        ThemeTypeCount = 2,

        Normal = 0,
        Hovered = 1 * (ThemeTypeCount + ColorFamilyCount),
        Pressed = 2 * (ThemeTypeCount + ColorFamilyCount),
        Disabled = 3 * (ThemeTypeCount + ColorFamilyCount),
        ColorTypeCount = 4
    };

    explicit DQuickControlPalette(QObject *parent = nullptr);
    ~DQuickControlPalette();

    bool enabled() const;
    void setEnabled(bool newEnabled);

    inline QColor *colorPointer(int colorPropertyIndex) {
        return colors.data() + colorPropertyIndex * ColorFamilyCount;
    }
    inline bool setTo(const DQuickControlColor &color, int colorPropertyIndex) {
        auto colors = colorPointer(colorPropertyIndex);
        Q_ASSERT(colors);
        if (color.data == colors)
            return color.changed;
        bool changed = false;
        for (int i = 0; i < ColorFamilyCount; ++i) {
            if (color.data[i] == colors[i])
                continue;
            colors[i] = color.data[i];
            changed = true;
        }
        return changed;
    }
    void setNormal(const DQuickControlColor &color) {
        if (!setTo(color, 0))
            return;
        Q_EMIT normalChanged();
        Q_EMIT changed();
    }
    DQuickControlColor normal() {
        return DQuickControlColor(colorPointer(0));
    }
    void setNormalDark(const DQuickControlColor &color) {
        if (!setTo(color, 1))
            return;
        Q_EMIT normalDarkChanged();
        Q_EMIT changed();
    }
    DQuickControlColor normalDark() {
        return DQuickControlColor(colorPointer(1));
    }
    void setHovered(const DQuickControlColor &color) {
        if (!setTo(color, 2))
            return;
        Q_EMIT hoveredChanged();
        Q_EMIT changed();
    }
    DQuickControlColor hovered() {
        return DQuickControlColor(colorPointer(2));
    }
    void setHoveredDark(const DQuickControlColor &color) {
        if (!setTo(color, 3))
            return;
        Q_EMIT hoveredDarkChanged();
        Q_EMIT changed();
    }
    DQuickControlColor hoveredDark() {
        return DQuickControlColor(colorPointer(3));
    }
    void setPressed(const DQuickControlColor &color) {
        if (!setTo(color, 4))
            return;
        Q_EMIT pressedChanged();
        Q_EMIT changed();
    }
    DQuickControlColor pressed() {
        return DQuickControlColor(colorPointer(4));
    }
    void setPressedDark(const DQuickControlColor &color) {
        if (!setTo(color, 5))
            return;
        Q_EMIT pressedDarkChanged();
        Q_EMIT changed();
    }
    DQuickControlColor pressedDark() {
        return DQuickControlColor(colorPointer(5));
    }
    void setDisabled(const DQuickControlColor &color) {
        if (!setTo(color, 6))
            return;
        Q_EMIT disabledChanged();
        Q_EMIT changed();
    }
    DQuickControlColor disabled() {
        return DQuickControlColor(colorPointer(6));
    }
    void setDisabledDark(const DQuickControlColor &color) {
        if (!setTo(color, 7))
            return;
        Q_EMIT disabledDarkChanged();
        Q_EMIT changed();
    }
    DQuickControlColor disabledDark() {
        return DQuickControlColor(colorPointer(7));
    }

Q_SIGNALS:
    void enabledChanged();
    void normalChanged();
    void normalDarkChanged();
    void hoveredChanged();
    void hoveredDarkChanged();
    void pressedChanged();
    void pressedDarkChanged();
    void disabledChanged();
    void disabledDarkChanged();
    void changed();

public:
    QVector<QColor> colors;

private:
    bool m_enabled = true;
};
DQUICK_END_NAMESPACE
QML_DECLARE_TYPE(DTK_QUICK_NAMESPACE::DQuickControlPalette)

DQUICK_BEGIN_NAMESPACE
class CustomMetaObject;
class DQuickControlColorSelector : public QObject, public QQmlParserStatus
{
    friend class CustomMetaObject;
    Q_OBJECT
    Q_DISABLE_COPY(DQuickControlColorSelector)
public:
    Q_PROPERTY(QQuickItem *control READ control WRITE setControl NOTIFY controlChanged)
    Q_PROPERTY(DTK_GUI_NAMESPACE::DGuiApplicationHelper::ColorType controlTheme READ controlTheme NOTIFY controlThemeChanged)
    Q_PROPERTY(DTK_QUICK_NAMESPACE::DQMLGlobalObject::ControlState controlState READ controlState NOTIFY controlStateChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered RESET resetHovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ pressed WRITE setPressed RESET resetPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool disabled READ disabled WRITE setDisabled RESET resetDisabled NOTIFY disabledChanged)
    Q_PROPERTY(bool inactived READ inactived WRITE setInactived RESET resetInactived NOTIFY inactivedChanged)
    Q_PROPERTY(QQmlListProperty<DTK_QUICK_NAMESPACE::DQuickControlPalette> palettes READ palettes NOTIFY palettesChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "palettes")
    Q_INTERFACES(QQmlParserStatus)

    explicit DQuickControlColorSelector(QObject *parent = nullptr);
    ~DQuickControlColorSelector();

    static DQuickControlColorSelector *qmlAttachedProperties(QObject *object);

    QQuickItem *control() const;
    void setControl(QQuickItem *newControl);

    DQMLGlobalObject::ControlState controlState() const;
    DGuiApplicationHelper::ColorType controlTheme() const;

    bool hovered() const;
    void setHovered(bool newHovered);
    void resetHovered();

    bool pressed() const;
    void setPressed(bool newPressed);
    void resetPressed();

    bool disabled() const;
    void setDisabled(bool newDisabled);
    void resetDisabled();

    bool inactived() const;
    void setInactived(bool newInactived);
    void resetInactived();

    QQmlListProperty<DQuickControlPalette> palettes();

Q_SIGNALS:
    void controlThemeChanged();
    void controlStateChanged();
    void controlChanged();
    void hoveredChanged();
    void pressedChanged();
    void disabledChanged();
    void inactivedChanged();
    void palettesChanged();

private:
    void classBegin() override;
    void componentComplete() override;

    QColor getColorOf(const DQuickControlPalette *palette,
                      DGuiApplicationHelper::ColorType theme,
                      DQMLGlobalObject::ControlState state);
    QColor getColorOf(const QString &propertyName,
                      DGuiApplicationHelper::ColorType theme,
                      DQMLGlobalObject::ControlState state);
    void ensureMetaObject();
    bool updatePropertyForPalette(const DQuickControlPalette *palette, bool force = false);
    void removePropertyForPalette(const DQuickControlPalette *palette);
    Q_SLOT void updateControlTheme();
    Q_SLOT void updateControlState();
    Q_SLOT void updateProperties();
    Q_SLOT void onPaletteChanged();
    Q_SLOT void onControlWindowChanged();

    void setControlTheme(DGuiApplicationHelper::ColorType theme);
    void setControlState(DQMLGlobalObject::ControlState state);

    void destroyPalette(DQuickControlPalette *palette);
    static void palette_append(QQmlListProperty<DQuickControlPalette> *property, DQuickControlPalette *value);
    static int palette_count(QQmlListProperty<DQuickControlPalette> *property);
    static DQuickControlPalette *palette_at(QQmlListProperty<DQuickControlPalette> *property, int index);
    static void palette_clear(QQmlListProperty<DQuickControlPalette> *property);
    void palette_replace(int index, DQuickControlPalette *newValue, bool updateProperty);
    static void palette_replace(QQmlListProperty<DQuickControlPalette> *property, int index,
                                DQuickControlPalette *newValue);
    static void palette_remove_last(QQmlListProperty<DQuickControlPalette> *property);

    QQuickItem *m_control = nullptr;
    QQuickWindow *m_controlWindow = nullptr;
    QList<DQuickControlPalette*> m_palettes;
    CustomMetaObject *m_metaObject = nullptr;
    DGuiApplicationHelper::ColorType m_controlTheme = DGuiApplicationHelper::LightType;
    DQMLGlobalObject::ControlState m_controlState = DQMLGlobalObject::NormalState;
    uint m_componentCompleted:1;
    uint m_hovered:1;
    uint m_hoveredValueValid:1;
    uint m_pressed:1;
    uint m_pressedValueValid:1;
    uint m_disabled:1;
    uint m_disabledValueValid:1;
    uint m_inactived:1;
    uint m_inactivedValueValid:1;
};

DQUICK_END_NAMESPACE
QML_DECLARE_TYPE(DTK_QUICK_NAMESPACE::DQuickControlColorSelector)
QML_DECLARE_TYPEINFO(DTK_QUICK_NAMESPACE::DQuickControlColorSelector, QML_HAS_ATTACHED_PROPERTIES)

#endif // DQUICKCONTROLPALETTE_P_H