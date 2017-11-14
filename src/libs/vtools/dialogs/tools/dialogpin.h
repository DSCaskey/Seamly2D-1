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
 **  @file   dialogpin.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   31 1, 2017
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

#ifndef DIALOGPIN_H
#define DIALOGPIN_H

#include "dialogtool.h"

namespace Ui
{
    class DialogPin;
}

class DialogPin : public DialogTool
{
    Q_OBJECT
public:
    explicit DialogPin(const VContainer *data, quint32 toolId, QWidget *parent = nullptr);
    virtual ~DialogPin();

    void EnbleShowMode(bool disable);

    quint32 GetPieceId() const;
    void    SetPieceId(quint32 id);

    quint32 GetPointId() const;
    void    SetPointId(quint32 id);

    virtual void SetPiecesList(const QVector<quint32> &list) Q_DECL_OVERRIDE;

public slots:
    virtual void ChosenObject(quint32 id, const SceneObject &type) Q_DECL_OVERRIDE;

protected:
    virtual void CheckState() Q_DECL_FINAL;
    virtual void ShowVisualization() Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(DialogPin)
    Ui::DialogPin *ui;
    bool  m_showMode;
    bool  m_flagPoint;

    void CheckPieces();
    void CheckPoint();
};

#endif // DIALOGPIN_H
