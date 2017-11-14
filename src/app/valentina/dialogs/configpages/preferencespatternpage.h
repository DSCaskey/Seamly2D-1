/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   http://github.com/fashionfreedom/seamly2d                             *
 *                                                                         *
 ***************************************************************************
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 **************************************************************************

 ************************************************************************
 **
 **  @file   preferencespatternpage.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 4, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
 **  <https: /github.com/valentina-project/vpo2> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#ifndef PREFERENCESPATTERNPAGE_H
#define PREFERENCESPATTERNPAGE_H

#include <QWidget>

namespace Ui
{
    class PreferencesPatternPage;
}

class QComboBox;

class PreferencesPatternPage : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPatternPage(QWidget *parent = nullptr);
    virtual ~PreferencesPatternPage();

    void Apply();
    void InitDefaultSeamAllowance();

private slots:
    void EditDateTimeFormats();

private:
    Q_DISABLE_COPY(PreferencesPatternPage)
    Ui::PreferencesPatternPage *ui;

    void InitLabelDateTimeFormats();
    void InitComboBoxFormats(QComboBox *box, const QStringList &items, const QString &currentFormat);

    template <typename T>
    void CallDateTimeFormatEditor(const T &type, const QStringList &predefinedFormats,
                                  const QStringList &userDefinedFormats, QComboBox *box);
};

#endif // PREFERENCESPATTERNPAGE_H
