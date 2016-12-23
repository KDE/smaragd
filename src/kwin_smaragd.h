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

#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationButton>

#include <QVariantList>

#include "shadowengine.h"

class QPropertyAnimation;

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

class DecorationFactory
{
public:
    DecorationFactory();
    ~DecorationFactory();

public:
    window_settings *windowSettings() const { return ws; }
    const Config *config() const { return &m_config; }

    QRegion cornerShape(int corner) const;
    QImage decorationImage(const QSize &size, bool active, int state, const QRect &titleRect = QRect()) const;
    QImage buttonImage(const QSize &size, bool active, int button, int state) const;

    void setFontHeight(int fontHeight);

private:
    window_settings *ws; // must be first entry because of inline method to access it
    Config m_config;

    QRegion cornerRegion[4];
};

class Decoration : public KDecoration2::Decoration
{
    Q_OBJECT

public:
    explicit Decoration(QObject *parent = Q_NULLPTR, const QVariantList &args = QVariantList());
    ~Decoration() Q_DECL_OVERRIDE;

public:
    DecorationFactory *factory() { return &m_factory; }
    void init() Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QRect &repaintArea) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void updateLayout();

public:
    int buttonGlyph(KDecoration2::DecorationButtonType type) const;

private:
    DecorationFactory m_factory;
};

#if 0
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
#endif
class DecorationButton : public KDecoration2::DecorationButton
{
    Q_OBJECT
    Q_PROPERTY(qreal hoverProgress READ hoverProgress WRITE setHoverProgress);

public:
    DecorationButton(KDecoration2::DecorationButtonType type, Decoration *parent);
    ~DecorationButton();

    qreal hoverProgress() const;
    void setHoverProgress(qreal hoverProgress);

protected:
    void paint(QPainter *painter, const QRect &repaintArea) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;

private:
    void startHoverAnimation(qreal endValue);

private:
    QPointer<QPropertyAnimation> m_hoverAnimation;
    qreal m_hoverProgress;
};

}; // namespace Smaragd

#endif

