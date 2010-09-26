/*
 * kwin_smaragd.h - Emerald window decoration for KDE
 *
 * Copyright (c) 2010 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef KWIN_SMARAGD_H
#define KWIN_SMARAGD_H 1

#include <kcommondecoration.h>
#include <kdecorationfactory.h>

#include "shadowengine.h"

#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
#define SMARAGD_NO_ANIMATIONS
#endif

#ifndef SMARAGD_NO_ANIMATIONS
class QPropertyAnimation;
#endif

extern "C"
{
    typedef struct _window_settings window_settings;
}

namespace Smaragd
{

class Config
{
public:
    bool useKWinTextColors;
    bool useKWinShadows;
    int hoverDuration;

    ShadowSettings shadowSettings;
    QImage shadowImage;
};

class DecorationFactory : public KDecorationFactory
{
public:
    DecorationFactory();
    virtual ~DecorationFactory();

public:
    virtual KDecoration *createDecoration(KDecorationBridge *bridge);
    virtual bool reset(unsigned long changed);
    virtual bool supports(Ability ability) const;

public:
    window_settings *windowSettings() const { return ws; }
    const Config *config() const { return &m_config; }

    QRegion cornerShape(KCommonDecoration::WindowCorner corner) const;
    QImage decorationImage(const QSize &size, bool active, int state, const QRect &titleRect = QRect()) const;
    QImage buttonImage(const QSize &size, bool active, int button, int state) const;

private:
    bool readConfig();

private:
    window_settings *ws; // must be first entry because of inline method to access it
    Config m_config;

    QRegion cornerRegion[4];
};

class Decoration : public KCommonDecoration
{
    Q_OBJECT

public:
    Decoration(KDecorationBridge *bridge, KDecorationFactory *factory);
    virtual ~Decoration();

public:
    virtual QString visibleName() const;
    virtual QString defaultButtonsLeft() const;
    virtual QString defaultButtonsRight() const;
    virtual bool decorationBehaviour(DecorationBehaviour behaviour) const;
    virtual int layoutMetric(LayoutMetric lm, bool respectWindowState = true, const KCommonDecorationButton *button = 0) const;
    virtual Position mousePosition(const QPoint &point) const;
    virtual QRegion cornerShape(WindowCorner corner);

    virtual void init();
    virtual KCommonDecorationButton *createButton(ButtonType type);

    virtual void paintEvent(QPaintEvent *event);

public:
    int buttonGlyph(ButtonType type) const;
};

class DecorationButton : public KCommonDecorationButton
{
    Q_OBJECT
#ifndef SMARAGD_NO_ANIMATIONS
    Q_PROPERTY(qreal hoverProgress READ hoverProgress WRITE setHoverProgress);
#endif

public:
    DecorationButton(ButtonType type, KCommonDecoration *parent);
    virtual ~DecorationButton();
    virtual void reset(unsigned long changed);

    qreal hoverProgress() const;
    void setHoverProgress(qreal hoverProgress);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    void startHoverAnimation(qreal endValue);

private:
#ifndef SMARAGD_NO_ANIMATIONS
    QWeakPointer<QPropertyAnimation> m_hoverAnimation;
#endif
    qreal m_hoverProgress;
};

}; // namespace Smaragd

#endif

