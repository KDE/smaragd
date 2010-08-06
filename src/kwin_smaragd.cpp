/*
 * kwin_smaragd.cpp - Emerald window decoration for KDE
 *
 * Copyright (c) 2010 Christoph Feck <christoph@maxiom.de>
 * Copyright (c) 2006 Novell, Inc.
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

#include "kwin_smaragd.h"

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLocale>
#include <kdeversion.h>

#include <QtGui/QPainter>
#include <QtGui/QBitmap>

#include <cairo.h>

extern "C"
{

#include <engine.h>

void pango_layout_get_pixel_size(PangoLayout *layout, int *pwidth, int *pheight)
{
    if (pwidth) {
        *pwidth = layout->bounding_width;
    }
    if (pheight) {
        *pheight = layout->bounding_height;
    }
}

void gdk_color_parse(gchar *s, GdkColor *c)
{
    QString string = QString::fromLocal8Bit(s);
    QColor color(string);
    c->red = qRound(color.redF() * 65535);
    c->green = qRound(color.greenF() * 65535);
    c->blue = qRound(color.blueF() * 65535);
}

void gdk_drawable_get_size(GdkPixmap *pixmap, int *width, int *height)
{
    *width = pixmap->width;
    *height = pixmap->height;
}

extern window_settings *create_settings();
extern void update_settings(window_settings *ws);

extern void legacy_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void line_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void oxygen_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void pixmap_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void truglass_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void vrunner_load_engine_settings(GKeyFile *f, window_settings *ws);
extern void zootreeves_load_engine_settings(GKeyFile *f, window_settings *ws);

extern void legacy_init_engine(window_settings *ws);
extern void line_init_engine(window_settings *ws);
extern void oxygen_init_engine(window_settings *ws);
extern void pixmap_init_engine(window_settings *ws);
extern void truglass_init_engine(window_settings *ws);
extern void vrunner_init_engine(window_settings *ws);
extern void zootreeves_init_engine(window_settings *ws);

extern void legacy_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void line_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void oxygen_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void pixmap_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void truglass_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void vrunner_engine_draw_frame(decor_t * d, cairo_t * cr);
extern void zootreeves_engine_draw_frame(decor_t * d, cairo_t * cr);

static init_engine_proc init_engine;
static draw_frame_proc draw_frame;
static load_settings_proc load_settings;

gboolean load_engine(gchar *engine, window_settings *ws)
{
    if (!engine || !strcmp(engine, "legacy")) {
        init_engine = legacy_init_engine;
        draw_frame = legacy_engine_draw_frame;
        load_settings = legacy_load_engine_settings;
    } else if (!strcmp(engine, "line")) {
        init_engine = line_init_engine;
        draw_frame = line_engine_draw_frame;
        load_settings = line_load_engine_settings;
    } else if (!strcmp(engine, "oxygen")) {
        init_engine = oxygen_init_engine;
        draw_frame = oxygen_engine_draw_frame;
        load_settings = oxygen_load_engine_settings;
    } else if (!strcmp(engine, "pixmap")) {
        init_engine = pixmap_init_engine;
        draw_frame = pixmap_engine_draw_frame;
        load_settings = pixmap_load_engine_settings;
    } else if (!strcmp(engine, "truglass")) {
        init_engine = truglass_init_engine;
        draw_frame = truglass_engine_draw_frame;
        load_settings = truglass_load_engine_settings;
    } else if (!strcmp(engine, "vrunner")) {
        init_engine = vrunner_init_engine;
        draw_frame = vrunner_engine_draw_frame;
        load_settings = vrunner_load_engine_settings;
    } else if (!strcmp(engine, "zootreeves")) {
        init_engine = zootreeves_init_engine;
        draw_frame = zootreeves_engine_draw_frame;
        load_settings = zootreeves_load_engine_settings;
    } else {
        return false;
    }
    init_engine(ws);
    return true;
}


void load_engine_settings(GKeyFile *f, window_settings *ws)
{
    load_settings(f, ws);
}

#define CORNER_REDUCTION 3

int update_shadow(frame_settings * fs)
{
    window_settings *ws = fs->ws;

    int size = ws->shadow_radius * 2 + 2;

    ws->shadow_offset_x = ws->shadow_offset_y = size = 0;

    if (ws->shadow_radius <= 0.0 && ws->shadow_offset_x == 0 &&
        ws->shadow_offset_y == 0)
        size = 0;

    size = size / 2;

    ws->left_space = ws->win_extents.left + size - ws->shadow_offset_x;
    ws->right_space = ws->win_extents.right + size + ws->shadow_offset_x;
    ws->top_space = ws->win_extents.top + size - ws->shadow_offset_y;
    ws->bottom_space = ws->win_extents.bottom + size + ws->shadow_offset_y;


    ws->left_space = MAX(ws->win_extents.left, ws->left_space);
    ws->right_space = MAX(ws->win_extents.right, ws->right_space);
    ws->top_space = MAX(ws->win_extents.top, ws->top_space);
    ws->bottom_space = MAX(ws->win_extents.bottom, ws->bottom_space);

    ws->shadow_left_space = MAX(0, size - ws->shadow_offset_x);
    ws->shadow_right_space = MAX(0, size + ws->shadow_offset_x);
    ws->shadow_top_space = MAX(0, size - ws->shadow_offset_y);
    ws->shadow_bottom_space = MAX(0, size + ws->shadow_offset_y);

    ws->shadow_left_corner_space = MAX(0, size + ws->shadow_offset_x);
    ws->shadow_right_corner_space = MAX(0, size - ws->shadow_offset_x);
    ws->shadow_top_corner_space = MAX(0, size + ws->shadow_offset_y);
    ws->shadow_bottom_corner_space = MAX(0, size - ws->shadow_offset_y);

    ws->left_corner_space =
        MAX(0, ws->shadow_left_corner_space - CORNER_REDUCTION);
    ws->right_corner_space =
        MAX(0, ws->shadow_right_corner_space - CORNER_REDUCTION);
    ws->top_corner_space =
        MAX(0, ws->shadow_top_corner_space - CORNER_REDUCTION);
    ws->bottom_corner_space =
        MAX(0, ws->shadow_bottom_corner_space - CORNER_REDUCTION);

    ws->normal_top_corner_space =
        MAX(0, ws->top_corner_space - ws->titlebar_height);
/*
    d.width =
        ws->left_space + ws->left_corner_space + 1 +
        ws->right_corner_space + ws->right_space;
    d.height =
        ws->top_space + ws->titlebar_height +
        ws->normal_top_corner_space + 2 + ws->bottom_corner_space +
        ws->bottom_space;
*/
    return 1;
}

struct _GdkPixbuf
{
    QImage image;
};

void g_object_unref(void *x)
{
    delete (_GdkPixbuf *) x;
}

int gdk_pixbuf_get_width(GdkPixbuf *pixbuf)
{
    return pixbuf->image.width();
}

int gdk_pixbuf_get_height(GdkPixbuf *pixbuf)
{
    return pixbuf->image.height();
}

GdkPixbuf *gdk_pixbuf_new(GdkColorspace colorspace, gboolean has_alpha, int bits_per_sample, int w, int h)
{
    Q_ASSERT(colorspace == GDK_COLORSPACE_RGB);
    Q_ASSERT(bits_per_sample == 8);

    _GdkPixbuf *pixbuf = new _GdkPixbuf;
    pixbuf->image = QImage(w, h, has_alpha ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);
    pixbuf->image.fill(qRgba(0, 0, 0, 0));
    return pixbuf;
}

GdkPixbuf *gdk_pixbuf_new_from_file(gchar *file, GError **/*error*/)
{
    QImage image = QImage(QString::fromAscii(file));

    if (image.isNull()) {
        return 0;
    }
    _GdkPixbuf *pixbuf = new _GdkPixbuf;
    pixbuf->image = image;
    return pixbuf;
}

GdkPixbuf *gdk_pixbuf_new_subpixbuf(GdkPixbuf *source, int x, int y, int w, int h)
{
    _GdkPixbuf *pixbuf = new _GdkPixbuf;
    pixbuf->image = source->image.copy(x, y, w, h);
    return pixbuf;
}

void gdk_pixbuf_scale(GdkPixbuf *source, GdkPixbuf *dest, int x, int y, int w, int h, double source_x, double source_y, double scale_x, double scale_y, int interp)
{
    QPainter p(&dest->image);
    if (interp == GDK_INTERP_BILINEAR) {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawImage(QRect(x, y, w, h), source->image, QRect(qRound(source_x), qRound(source_y), qRound(w / scale_x), qRound(h / scale_y)));
    p.end();
}

int gdk_pixbuf_get_colorspace(GdkPixbuf */*pixbuf*/)
{
    return GDK_COLORSPACE_RGB;
}

int gdk_pixbuf_get_bits_per_sample(GdkPixbuf */*pixbuf*/)
{
    return 8;
}

}

extern "C" KDE_EXPORT KDecorationFactory *create_factory()
{
    return new Smaragd::DecorationFactory();
}

namespace Smaragd
{

static QRegion findCornerShape(const QImage &image, KCommonDecoration::WindowCorner corner, const QSize &maxSize)
{
    QSize cornerSize = maxSize.boundedTo(image.size());
    QImage cornerImage(cornerSize, QImage::Format_MonoLSB);
    cornerImage.fill(1);

    int xd = 1, yd = 1; // scanning direction
    int sx = 0, sy = 0;
    int cx = 0, cy = 0;
    if (int(corner) & int(KCommonDecoration::WC_TopRight)) {
        xd = -1;
        sx = image.width() - 1;
        cx = cornerImage.width() - 1;
    }
    if (int(corner) & int (KCommonDecoration::WC_BottomLeft)) {
        yd = -1;
        sy = image.height() - 1;
        cy = cornerImage.height() - 1;
    }

    int threshold = qAlpha(QRgb(image.pixel(sx + (cornerSize.width() - 1) * xd, sy))) >> 1;
    for (int y = 0, ys = sy, yc = cy; y < cornerSize.height(); ++y, ys += yd, yc += yd) {
        for (int x = 0, xs = sx, xc = cx; x < cornerSize.width(); ++x, xs += xd, xc += xd) {
            QRgb pixel = QRgb(image.pixel(xs, ys));
            if (qAlpha(pixel) >= threshold) {
                break;
            }
            cornerImage.setPixel(xc, yc, 0);
        }
    }
    return QRegion(QBitmap::fromImage(cornerImage));
}


DecorationFactory::DecorationFactory()
    : KDecorationFactory()
{
    ws = create_settings();
    (void) readConfig();
}

DecorationFactory::~DecorationFactory()
{
    free(ws);
}

KDecoration *DecorationFactory::createDecoration(KDecorationBridge *bridge)
{
    return (new Decoration(bridge, this))->decoration();
}

bool DecorationFactory::readConfig()
{
    QFontMetrics fm(options()->font(true));
    ws->text_height = fm.height();
    update_settings(ws);

    QImage decoImage = decorationImage(QSize(96, 64), true, 0, QRect(32, 8, 32, 8));
    QPainter p(&decoImage);
    QRect rect(0, 0, 96, 64);
    rect.adjust(ws->left_space + ws->left_corner_space, ws->top_space + ws->normal_top_corner_space + ws->titlebar_height,
        -(ws->right_space + ws->right_corner_space), -(ws->bottom_space + ws->bottom_corner_space));
    p.fillRect(rect, Qt::black);
    p.end();
    for (int corner = 0; corner < 4; ++corner) {
        cornerRegion[corner] = findCornerShape(decoImage, KCommonDecoration::WindowCorner(corner), QSize(32, 32));
    }

    KConfig configFile(QLatin1String("kwinsmaragdrc"));
    KConfigGroup configGroup(&configFile, "General");

    m_config.useKWinTextColors = configGroup.readEntry("UseKWinTextColors", false);
    m_config.useKWinShadows = configGroup.readEntry("UseKWinShadows", false);

    if (!m_config.useKWinShadows) {
        m_config.shadowSettings.radius = configGroup.readEntry("ShadowRadius", 5);
        m_config.shadowSettings.color = configGroup.readEntry("ShadowColor", QColor(0, 0, 0));
        m_config.shadowSettings.color.setAlpha(configGroup.readEntry("ShadowAlpha", 180));
        m_config.shadowSettings.offsetX = configGroup.readEntry("ShadowOffsetX", 0);
        m_config.shadowSettings.offsetY = configGroup.readEntry("ShadowOffsetY", 0);
        m_config.shadowSettings.size = configGroup.readEntry("ShadowSize", -3);
        m_config.shadowSettings.linearDecay = configGroup.readEntry("ShadowLinearDecay", 1.0);
        m_config.shadowSettings.exponentialDecay = configGroup.readEntry("ShadowExponentialDecay", 6.0);

        m_config.shadowImage = createShadowImage(m_config.shadowSettings);
    }

    return true;
}

bool DecorationFactory::reset(unsigned long changed)
{
    (void) readConfig();
    resetDecorations(changed);
    return true;
}

bool DecorationFactory::supports(Ability ability) const
{
    switch (ability) {
    case AbilityAnnounceButtons:
    case AbilityButtonMenu:
    case AbilityButtonOnAllDesktops:
    case AbilityButtonSpacer:
    case AbilityButtonHelp:
    case AbilityButtonMinimize:
    case AbilityButtonMaximize:
    case AbilityButtonClose:
    case AbilityButtonAboveOthers:
    case AbilityButtonBelowOthers:
    case AbilityButtonShade:
        return true;
#if KDE_IS_VERSION(4,3,0)
    case AbilityProvidesShadow:
        return !m_config.useKWinShadows;
    case AbilityUsesAlphaChannel:
        return true;
#endif
#if KDE_IS_VERSION(4,4,0)
//    case AbilityExtendIntoClientArea:
//        return true;
#endif
    default:
        return false;
    }
}

QRegion DecorationFactory::cornerShape(KCommonDecoration::WindowCorner corner) const
{
    return cornerRegion[int(corner)];
}


Decoration::Decoration(KDecorationBridge *bridge, KDecorationFactory *factory)
    : KCommonDecoration(bridge, factory)
{
}

Decoration::~Decoration()
{
}

QString Decoration::visibleName() const
{
    return i18n("Smaragd");
}

bool Decoration::decorationBehaviour(DecorationBehaviour behaviour) const
{
    switch (behaviour) {
    case DB_WindowMask:
        return true;
    case DB_MenuClose:
    case DB_ButtonHide:
        return true;
    default:
        return KCommonDecoration::decorationBehaviour(behaviour);
    }
}

QRegion Decoration::cornerShape(WindowCorner corner)
{
    bool border = !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());
    if (!border) {
        return QRegion();
    }

    QRegion region = static_cast<DecorationFactory *>(factory())->cornerShape(corner);
    switch (corner)
    {
    case WC_TopLeft:
        break;
    case WC_TopRight:
        region.translate(width() - 32, 0);
        break;
    case WC_BottomLeft:
        region.translate(0, height() - 32);
        break;
    case WC_BottomRight:
        region.translate(width() - 32, height() - 32);
        break;
    }
#if KDE_IS_VERSION(4,3,0)
    region.translate(layoutMetric(LM_OuterPaddingLeft, true), layoutMetric(LM_OuterPaddingTop, true));
#endif
    return region;
}

int Decoration::buttonGlyph(ButtonType type) const
{
    int y;

    switch (type) {
    case HelpButton:
        y = B_HELP;
        break;
    case MaxButton:
        y = maximizeMode() == MaximizeFull ? B_RESTORE : B_MAXIMIZE;
        break;
    case MinButton:
        y = B_MINIMIZE;
        break;
    case CloseButton:
        y = B_CLOSE;
        break;
    case MenuButton:
        y = B_MENU;
        break;
    case OnAllDesktopsButton:
        y = isOnAllDesktops() ? B_UNSTICK : B_STICK;
        break;
    case AboveButton:
        y = keepAbove() ? B_UNABOVE : B_ABOVE;
        break;
    case BelowButton:
        y = keepBelow() ? B_UNABOVE : B_ABOVE;
        break;
    case ShadeButton:
        y = isShade() ? B_UNSHADE : B_SHADE;
        break;
    default:
        y = B_RESTORE;
        break;
    }
    return y;
}

static Qt::Alignment parseTitleAlignment(char *p)
{
    char c;

    while ((c = *p++) && c != ':') {
        if (c == 'T') {
            return Qt::AlignLeft;
        }
    }
    while ((c = *p++) && c != ':') {
        if (c == 'T') {
            return Qt::AlignHCenter;
        }
    }
    return Qt::AlignRight;
}

static QString parseButtonLayout(char *p, int *leftSpace, int *rightSpace)
{
    QString buttons;
    int s;
    char c;

    *rightSpace = 0;
    while ((c = *p++)) {
        if (c == ':') {
            return buttons;
        }
        if ((s = *rightSpace)) {
            if (s < 100) {
                while (--s >= 0) {
                    buttons += '_';
                }
            }
            *rightSpace = 0;
        }
        switch (c) {
        case 'H': // B_HELP
            buttons += 'H';
            break;
        case 'M':
        case 'I': // B_MENU
            buttons += 'M';
            break;
        case 'N': // B_MINIMIZE
            buttons += 'I';
            break;
        case 'R':
        case 'X': // B_MAXIMIZE
            buttons += 'A';
            break;
        case 'C': // B_CLOSE
            buttons += 'X';
            break;
        case 'U':
        case 'A': // B_ABOVE
            buttons += 'F';
            break;
        case 'D': // B_BELOW
            buttons += 'B';
            break;
        case 'S': // B_SHADE
            buttons += 'L';
            break;
        case 'Y': // B_STICK
            buttons += 'S';
            break;
        case '(': {
            bool negative = false;
            s = 0;
            if (*p == '-') {
                negative = true;
                ++p;
            }
            while (c = *p, c >= '0' && c <= '9') {
                s = s * 10 + c - '0';
                ++p;
            }
            if (c == ')') {
                ++p;
            }
            if (negative) {
                s = -s;
            }
            if (leftSpace) {
                *leftSpace = s;
                s = 0;
            }
            *rightSpace = s;
        }
        default:
            break;
        }
        leftSpace = 0;
    }
    return buttons;
}

QString Decoration::defaultButtonsLeft() const
{
    window_settings *ws = (static_cast<DecorationFactory *>(factory()))->windowSettings();
    if (!ws->tobj_layout) {
        return KDecorationOptions::defaultTitleButtonsLeft();
    }
    int leftSpace = 0, rightSpace = 0;
    return parseButtonLayout(ws->tobj_layout, &leftSpace, &rightSpace);
}

QString Decoration::defaultButtonsRight() const
{
    window_settings *ws = (static_cast<DecorationFactory *>(factory()))->windowSettings();
    if (!ws->tobj_layout) {
        return KDecorationOptions::defaultTitleButtonsRight();
    }
    char *p = ws->tobj_layout;
    while (*p && *p++ != ':') { }
    while (*p && *p++ != ':') { }
    int leftSpace = 0, rightSpace = 0;
    return parseButtonLayout(p, &leftSpace, &rightSpace);
}

static int titleBorderLeft(window_settings *ws)
{
    if (!ws->tobj_layout) {
        return 2;
    }
    int leftSpace = 0, rightSpace = 0;
    parseButtonLayout(ws->tobj_layout, &leftSpace, &rightSpace);
    return 2 + rightSpace;
}

static int titleBorderRight(window_settings *ws)
{
    if (!ws->tobj_layout) {
        return 2;
    }
    char *p = ws->tobj_layout;
    while (*p && *p++ != ':') { }
    while (*p && *p++ != ':') { }
    int leftSpace = 0, rightSpace = 0;
    parseButtonLayout(p, &leftSpace, &rightSpace);
    return 2 + leftSpace;
}

static int titleEdgeLeft(window_settings *ws)
{
    int edge = ws->left_space + ws->button_hoffset;

    if (!ws->tobj_layout) {
        return edge;
    }
    int leftSpace = 0, rightSpace = 0;
    parseButtonLayout(ws->tobj_layout, &leftSpace, &rightSpace);
    return edge + leftSpace;
}

static int titleEdgeRight(window_settings *ws)
{
    int edge = ws->right_space + ws->button_hoffset;

    if (!ws->tobj_layout) {
        return edge;
    }
    char *p = ws->tobj_layout;
    while (*p && *p++ != ':') { }
    while (*p && *p++ != ':') { }
    int leftSpace = 0, rightSpace = 0;
    parseButtonLayout(p, &leftSpace, &rightSpace);
    return edge + rightSpace;
}

int Decoration::layoutMetric(LayoutMetric lm, bool respectWindowState, const KCommonDecorationButton *button) const
{
    window_settings *ws = (static_cast<DecorationFactory *>(factory()))->windowSettings();
    bool border = !respectWindowState || !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());

    switch (lm) {
    case LM_BorderLeft:
        return border ? ws->left_space + ws->left_corner_space : 0;
    case LM_BorderRight:
        return border ? ws->right_space + ws->right_corner_space : 0;
    case LM_BorderBottom:
        return border ? ws->bottom_space + ws->bottom_corner_space : 0;
    case LM_TitleBorderLeft:
        return titleBorderLeft(ws);
    case LM_TitleBorderRight:
        return titleBorderRight(ws);
    case LM_TitleEdgeLeft:
        return border ? titleEdgeLeft(ws) : 0;
    case LM_TitleEdgeRight:
        return border ? titleEdgeRight(ws) : 0;
    case LM_TitleEdgeTop:
        return 0;
    case LM_TitleEdgeBottom:
        return 0;
    case LM_TitleHeight:
        return ws->top_space + ws->normal_top_corner_space + ws->titlebar_height;
    case LM_ButtonHeight:
    case LM_ButtonWidth: {
        if (button->type() == MenuButton) {
            if (lm == LM_ButtonWidth) {
                return 16;
            } else {
                return ws->top_space + ws->normal_top_corner_space + ws->titlebar_height;
            }
        }
        GdkPixbuf *pixbuf = ws->ButtonPix[buttonGlyph(button->type()) * S_COUNT];
        if (pixbuf) {
            if (lm == LM_ButtonWidth) {
                return gdk_pixbuf_get_width(pixbuf);
            } else {
                return gdk_pixbuf_get_height(pixbuf) + ws->button_offset;
            }
        }
        return 0;
    }
    case LM_ButtonSpacing:
        return 0;
    case LM_ExplicitButtonSpacer:
        return 1;
    case LM_ButtonMarginTop:
        return 0;
#if KDE_IS_VERSION(4,3,0)
    case LM_OuterPaddingLeft:
    case LM_OuterPaddingTop:
    case LM_OuterPaddingRight:
    case LM_OuterPaddingBottom:
        return 32;
#endif
    }
    return KCommonDecoration::layoutMetric(lm, respectWindowState, button);
}

KCommonDecoration::Position Decoration::mousePosition(const QPoint & point) const
{
#if KDE_IS_VERSION(4,3,0)
    const int paddingLeft = layoutMetric(LM_OuterPaddingLeft);
    const int paddingTop = layoutMetric(LM_OuterPaddingTop);
#else
    const int paddingLeft = 0;
    const int paddingTop = 0;
#endif
    const int titleHeight = layoutMetric(LM_TitleHeight);
    int pos = PositionCenter;

    if (!isShade()) {
        if (point.x() >= (width() + paddingLeft - qMax(titleHeight, layoutMetric(LM_BorderRight)))) {
            pos |= PositionRight;
        } else if (point.x() <= paddingLeft + qMax(titleHeight, layoutMetric(LM_BorderLeft))) {
            pos |= PositionLeft;
        }
        if (point.y() >= (height() + paddingTop - qMax(titleHeight, layoutMetric(LM_BorderBottom)))) {
            pos |= PositionBottom;
        } else if (point.y() <= paddingTop + (pos == PositionCenter ? 3 : titleHeight)) {
            pos |= PositionTop;
        }
    }
    return Position(pos);
}

KCommonDecorationButton *Decoration::createButton(ButtonType type)
{
    return new DecorationButton(type, this);
}

void Decoration::init()
{
    KCommonDecoration::init();
    widget()->setAutoFillBackground(false);
    widget()->setAttribute(Qt::WA_NoSystemBackground, true);
    widget()->setAttribute(Qt::WA_OpaquePaintEvent, true);
}

QImage DecorationFactory::decorationImage(const QSize &size, bool active, int state, const QRect &titleRect) const
{
    decor_t deco, *d = &deco;
    bzero(d, sizeof(decor_t));

    // ### Title objects position and sizes
    d->tobj_pos[0] = 0; // left
    d->tobj_pos[1] = 0; // mid
    d->tobj_pos[2] = 0; // right
    d->tobj_size[0] = 0;
    d->tobj_size[1] = 0;
    d->tobj_size[2] = 0;

    // ### Buttons
    for (int i = 0; i < 11; ++i) {
        d->tobj_item_pos[i] = 0;
        d->tobj_item_width[i] = 0;
        d->tobj_item_state[i] = 3;
    }

    d->width = size.width();
    d->height = size.height();

    d->client_width = d->width - (
        ws->left_space + ws->left_corner_space +
        ws->right_corner_space + ws->right_space);
    d->client_height = d->height - (
        ws->top_space + ws->titlebar_height +
        ws->normal_top_corner_space + ws->bottom_corner_space +
        ws->bottom_space);

    d->tobj_item_state[TBT_TITLE] = 0;
    d->tobj_item_pos[TBT_TITLE] = titleRect.left() - ws->left_space;
    PangoLayout pangoLayout;
    pangoLayout.bounding_width = titleRect.width();
    pangoLayout.bounding_height = titleRect.height();
    d->layout = &pangoLayout;

    d->state = WnckWindowState(state);

    d->decorated = true;
    d->active = active;

    d->fs = active ? ws->fs_act : ws->fs_inact;

    QSize allocSize(cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, size.width()) / 4, size.height());

    QImage image(allocSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create_for_data(image.bits(), CAIRO_FORMAT_ARGB32, size.width(), image.height(), image.bytesPerLine());
    cr = cairo_create(surface);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_line_width(cr, 1.0);
    draw_frame(d, cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return image;
}

void Decoration::paintEvent(QPaintEvent */*event */)
{
    DecorationFactory *decorationFactory = static_cast<DecorationFactory *>(factory());
    window_settings *ws = decorationFactory->windowSettings();
    const Config *config = decorationFactory->config();
    bool border = !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());
    bool active = isActive();
    QPainter painter(widget());

    QSize size(widget()->size());
#if KDE_IS_VERSION(4,3,0)
    size -= QSize(layoutMetric(LM_OuterPaddingLeft, true) + layoutMetric(LM_OuterPaddingRight, true),
                  layoutMetric(LM_OuterPaddingTop, true) + layoutMetric(LM_OuterPaddingBottom, true));
#endif

    painter.setFont(options()->font(active));
    Qt::Alignment alignment = Qt::AlignHCenter;
    if (ws->tobj_layout) {
        alignment = parseTitleAlignment(ws->tobj_layout);
    }
    QRect labelRect = titleRect().adjusted(0, 0, 1, 1);

    QString text = painter.fontMetrics().elidedText(caption(), Qt::ElideMiddle, labelRect.width());
    int state = 0;
#if 0
    if (maximizeMode() & MaximizeHorizontal) state |= WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY;
    if (maximizeMode() & MaximizeVertical) state |= WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY;
#else
    if (!border) state |= WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY | WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY;
#endif
    if (isShade()) state |= WNCK_WINDOW_STATE_SHADED;
    if (isOnAllDesktops()) state |= WNCK_WINDOW_STATE_STICKY;
    if (keepAbove()) state |= WNCK_WINDOW_STATE_ABOVE;
    if (keepBelow()) state |= WNCK_WINDOW_STATE_BELOW;

    if (!border) {
        size += QSize(layoutMetric(LM_TitleEdgeLeft, false) + layoutMetric(LM_TitleEdgeRight, false), 0);
    }
    QRect titleRect = painter.boundingRect(labelRect, alignment | Qt::AlignVCenter | Qt::TextSingleLine, text);
#if KDE_IS_VERSION(4,3,0)
    titleRect.adjust(-layoutMetric(LM_OuterPaddingLeft, true), 0, -layoutMetric(LM_OuterPaddingLeft, true), 0);
#endif
    if (!border) {
        titleRect.adjust(layoutMetric(LM_TitleEdgeLeft, false), 0, layoutMetric(LM_TitleEdgeLeft, false), 0);
    }
    QImage decoImage = static_cast<DecorationFactory *>(factory())->decorationImage(size, active, state, titleRect);

#if KDE_IS_VERSION(4,3,0)
    const int paddingLeft = layoutMetric(LM_OuterPaddingLeft, true);
    const int paddingTop = layoutMetric(LM_OuterPaddingTop, true);
#else
    const int paddingLeft = 0;
    const int paddingTop = 0;
#endif
    QRect outerRect(paddingLeft, paddingTop, size.width(), size.height());
    QRect innerRect = outerRect.adjusted(layoutMetric(LM_BorderLeft, true), layoutMetric(LM_TitleHeight, true),
        -layoutMetric(LM_BorderRight, true), -layoutMetric(LM_BorderBottom, true));

#if KDE_IS_VERSION(4,3,0)
    if (border && !config->useKWinShadows) {
        paintShadow(&painter, outerRect, config->shadowSettings, config->shadowImage);
    }
#endif

    if (border) {
        painter.drawImage(outerRect.x(), outerRect.y(), decoImage,
            0, 0, outerRect.width(), innerRect.y() - outerRect.y());
        painter.drawImage(outerRect.x(), innerRect.y() + innerRect.height(), decoImage,
            0, outerRect.height() - (outerRect.bottom() - innerRect.bottom()),
            outerRect.width(), outerRect.bottom() - innerRect.bottom());
        painter.drawImage(outerRect.x(), innerRect.y(), decoImage,
            0, innerRect.y() - outerRect.y(),
            innerRect.x() - outerRect.x(), innerRect.height());
        painter.drawImage(innerRect.x() + innerRect.width(), innerRect.y(), decoImage,
            outerRect.width() - (outerRect.right() - innerRect.right()), innerRect.y() - outerRect.y(),
            outerRect.right() - innerRect.right(), innerRect.height());
    } else {
        painter.drawImage(outerRect.x(), outerRect.y(), decoImage,
            layoutMetric(LM_TitleEdgeLeft, false), 0, outerRect.width(), innerRect.y() - outerRect.y());
    }

    frame_settings *fs = active ? ws->fs_act : ws->fs_inact;

    if (config->useKWinTextColors) {
        painter.setPen(QColor(0, 0, 0, 25));
    } else {
        alpha_color &c = fs->text_halo;
        painter.setPen(QColor::fromRgbF(c.color.r, c.color.g, c.color.b, c.alpha));
    }
    painter.drawText(labelRect.adjusted(1, 1, 1, 1), alignment | Qt::AlignVCenter | Qt::TextSingleLine, text);
    if (config->useKWinTextColors) {
        painter.setPen(options()->color(ColorFont, active));
    } else {
        alpha_color &c = fs->text;
        painter.setPen(QColor::fromRgbF(c.color.r, c.color.g, c.color.b, c.alpha));
    }
    painter.drawText(labelRect, alignment | Qt::AlignVCenter | Qt::TextSingleLine, text);

    QList<DecorationButton *> buttons = widget()->findChildren<DecorationButton *>();
    foreach (DecorationButton *button, buttons) {
        QRect rect = button->geometry();

        int x = 0; //state
        if (button->isDown()) {
            x = 2;
        } else if (button->underMouse()) {
            x = 1;
        }
        if (!active) {
            x += 3;
        }

        if (button->type() == MenuButton) {
            icon().paint(&painter, rect);
        } else {
            int y = buttonGlyph(button->type());
            if (ws->use_pixmap_buttons) {
                painter.drawImage(rect.x(), rect.y() + ws->button_offset, ws->ButtonPix[x + y * S_COUNT]->image);
            }
        }
    }
    foreach (DecorationButton *button, buttons) {
        if (button->underMouse() && button->type() != MenuButton) {
            QRect rect = button->geometry();
            int y = buttonGlyph(button->type());
            QImage image;

            if (active && ws->use_button_glow) {
                image = ws->ButtonGlowPix[y]->image;
            } else if (!active && ws->use_button_inactive_glow) {
                image = ws->ButtonInactiveGlowPix[y]->image;
            }
            if (!image.isNull()) {
                painter.drawImage(rect.x() + (rect.width() - ws->c_glow_size.w) / 2, rect.y() + (rect.height() - ws->c_glow_size.h) / 2 + ws->button_offset, image);
            }
        }
    }
}


DecorationButton::DecorationButton(ButtonType type, KCommonDecoration *parent)
    : KCommonDecorationButton(type, parent)
{
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_Hover, true);
}

DecorationButton::~DecorationButton()
{
    /* */
}

void DecorationButton::reset(unsigned long /*changed*/)
{
    /* NOTE: must be implemented, because it is declared pure */
}

void DecorationButton::paintEvent(QPaintEvent */* event */)
{
    /* */
}

void DecorationButton::enterEvent(QEvent *event)
{
    KCommonDecorationButton::enterEvent(event);
    parentWidget()->update(geometry().adjusted(-32, -32, 32, 32));
}

void DecorationButton::leaveEvent(QEvent *event)
{
    KCommonDecorationButton::leaveEvent(event);
    parentWidget()->update(geometry().adjusted(-32, -32, 32, 32));
}

}; // namespace Smaragd

#include "kwin_smaragd.moc"

