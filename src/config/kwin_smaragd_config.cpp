/*
 * kwin_smaragd_config.cpp - Emerald window decoration for KDE
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

#include "kwin_smaragd_config.h"

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KGlobal>
#include <KDE/KLocale>
#include <kdeversion.h>

extern "C" KDE_EXPORT QObject *allocate_config(KConfig *conf, QWidget *parent)
{
    return new Smaragd::Config(conf, parent);
}

namespace Smaragd
{

Config::Config(KConfig *config, QWidget *parent)
    : QObject(parent)
{
    Q_UNUSED(config);
    smaragdConfig = new KConfig(QLatin1String("kwinsmaragdrc"));
    KGlobal::locale()->insertCatalog(QLatin1String("kwin_clients"));
    ui = new ConfigUi(parent);
#if KDE_IS_VERSION(4,3,0)
    connect(ui->cm_UseKWinShadows, SIGNAL(toggled(bool)), this, SLOT(setShadowTabDisabled(bool)));
#else
    ui->cm_UseKWinShadows->hide();
    ui->tabWidget->removeTab(1);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
    ui->cm_HoverDuration->setEnabled(false);
    ui->cm_HoverDuration->setToolTip(i18n("Animations require KDE SC 4.4"));
#endif
    configManager.addWidgets(ui);
    load(KConfigGroup(smaragdConfig, "General"));
    configManager.connectConfigChanged(this, SLOT(slotSelectionChanged()));
}

Config::~Config()
{
    delete smaragdConfig;
    delete ui;
}

void Config::load(const KConfigGroup &configGroup)
{
    Q_UNUSED(configGroup);
    configManager.load(KConfigGroup(smaragdConfig, "General"));
}

void Config::save(KConfigGroup &configGroup)
{
    Q_UNUSED(configGroup);
    KConfigGroup smaragdConfigGroup(smaragdConfig, "General");
    configManager.save(smaragdConfigGroup);
    smaragdConfig->sync();
}

void Config::defaults()
{
    configManager.defaults();
}

void Config::slotSelectionChanged()
{
    if (configManager.hasChanged()) {
        emit changed();
    }
}

}; // namespace Smaragd

#include "kwin_smaragd_config.moc"

