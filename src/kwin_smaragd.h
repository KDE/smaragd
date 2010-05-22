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

extern "C"
{
    typedef struct _window_settings window_settings;
    typedef struct _decor decor_t;
}

namespace Smaragd
{

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

private:
    window_settings *ws;
};

class Decoration : public KCommonDecoration
{
Q_OBJECT

public:
    Decoration(KDecorationBridge *bridge, KDecorationFactory *factory);
    virtual ~Decoration();

public:
    virtual QString visibleName() const;
    virtual bool decorationBehaviour(DecorationBehaviour behaviour) const;
    virtual int layoutMetric(LayoutMetric lm, bool respectWindowState = true, const KCommonDecorationButton *button = 0) const;

    virtual void init();
    virtual KCommonDecorationButton *createButton(ButtonType type);

    virtual void paintEvent(QPaintEvent *event);

private:
    decor_t *d;
};

class DecorationButton : public KCommonDecorationButton
{
Q_OBJECT

public:
    DecorationButton(ButtonType type, KCommonDecoration *parent);
    virtual ~DecorationButton();
    virtual void reset(unsigned long changed);

protected:
    virtual void paintEvent(QPaintEvent *event);
};

}; // namespace Smaragd

#endif

