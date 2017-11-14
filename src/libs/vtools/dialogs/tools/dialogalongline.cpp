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
 **  @file   dialogalongline.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
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

#include "dialogalongline.h"

#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLatin1String>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QSharedPointer>
#include <QToolButton>
#include <new>

#include "../../visualization/line/vistoolalongline.h"
#include "../../visualization/visualization.h"
#include "../ifc/xml/vabstractpattern.h"
#include "../ifc/xml/vdomdocument.h"
#include "../support/dialogeditwrongformula.h"
#include "../vgeometry/../ifc/ifcdef.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vcommonsettings.h"
#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/variables/vlinelength.h"
#include "../vpatterndb/vtranslatevars.h"
#include "ui_dialogalongline.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief DialogAlongLine create dialog
 * @param data container with data
 * @param parent parent widget
 */
DialogAlongLine::DialogAlongLine(const VContainer *data, const quint32 &toolId, QWidget *parent)
    :DialogTool(data, toolId, parent), ui(new Ui::DialogAlongLine),
      formula(QString()), formulaBaseHeight(0), buildMidpoint(false)
{
    ui->setupUi(this);

    ui->lineEditNamePoint->setClearButtonEnabled(true);

    InitFormulaUI(ui);
    ui->lineEditNamePoint->setText(qApp->getCurrentDocument()->GenerateLabel(LabelType::NewLabel));
    labelEditNamePoint = ui->labelEditNamePoint;

    this->formulaBaseHeight = ui->plainTextEditFormula->height();
    ui->plainTextEditFormula->installEventFilter(this);

    InitOkCancelApply(ui);
    flagFormula = false;
    DialogTool::CheckState();

    FillComboBoxPoints(ui->comboBoxFirstPoint);
    FillComboBoxPoints(ui->comboBoxSecondPoint);
    FillComboBoxTypeLine(ui->comboBoxLineType, LineStylesPics());
    FillComboBoxLineColors(ui->comboBoxLineColor);

    connect(ui->toolButtonExprLength, &QPushButton::clicked, this, &DialogAlongLine::FXLength);
    connect(ui->lineEditNamePoint, &QLineEdit::textChanged, this, &DialogAlongLine::NamePointChanged);
    connect(ui->plainTextEditFormula, &QPlainTextEdit::textChanged, this, &DialogAlongLine::FormulaTextChanged);
    connect(ui->pushButtonGrowLength, &QPushButton::clicked, this, &DialogAlongLine::DeployFormulaTextEdit);
    connect(ui->comboBoxFirstPoint, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            this, &DialogAlongLine::PointChanged);
    connect(ui->comboBoxSecondPoint, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            this, &DialogAlongLine::PointChanged);

    vis = new VisToolAlongLine(data);

    // Call after initialization vis!!!!
    SetTypeLine(TypeLineNone);//By default don't show line
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::FormulaTextChanged()
{
    this->FormulaChangedPlainText();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::PointChanged()
{
    QColor color = okColor;
    if (GetFirstPointId() == GetSecondPointId())
    {
        flagError = false;
        color = errorColor;
    }
    else
    {
        flagError = true;
        color = okColor;
    }
    SetCurrentLength();
    ChangeColor(ui->labelFirstPoint, color);
    ChangeColor(ui->labelSecondPoint, color);
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::FXLength()
{
    DialogEditWrongFormula *dialog = new DialogEditWrongFormula(data, toolId, this);
    dialog->setWindowTitle(tr("Edit length"));
    dialog->SetFormula(GetFormula());
    dialog->setPostfix(UnitsToStr(qApp->patternUnit(), true));
    if (dialog->exec() == QDialog::Accepted)
    {
        SetFormula(dialog->GetFormula());
    }
    delete dialog;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::ShowVisualization()
{
    AddVisualization<VisToolAlongLine>();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::DeployFormulaTextEdit()
{
    DeployFormula(ui->plainTextEditFormula, ui->pushButtonGrowLength, formulaBaseHeight);
}

//---------------------------------------------------------------------------------------------------------------------
DialogAlongLine::~DialogAlongLine()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ChoosedObject gets id and type of selected object. Save right data and ignore wrong.
 * @param id id of point or detail
 * @param type type of object
 */
void DialogAlongLine::ChosenObject(quint32 id, const SceneObject &type)
{
    if (prepare == false)// After first choose we ignore all objects
    {
        if (type == SceneObject::Point)
        {
            VisToolAlongLine *line = qobject_cast<VisToolAlongLine *>(vis);
            SCASSERT(line != nullptr)

            const QString toolTip = tr("Select second point of line");
            switch (number)
            {
                case 0:
                    if (SetObject(id, ui->comboBoxFirstPoint, toolTip))
                    {
                        number++;
                        line->VisualMode(id);
                    }
                    break;
                case 1:
                    if (SetObject(id, ui->comboBoxSecondPoint, ""))
                    {
                        if (flagError)
                        {
                            line->setObject2Id(id);
                            line->RefreshGeometry();
                            if (buildMidpoint)
                            {
                                SetFormula(currentLength + QLatin1String("/2"));
                            }
                            prepare = true;
                            this->setModal(true);
                            this->show();
                        }
                        else
                        {
                            emit ToolTip(toolTip);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::SaveData()
{
    pointName = ui->lineEditNamePoint->text();

    formula = ui->plainTextEditFormula->toPlainText();
    formula.replace("\n", " ");

    VisToolAlongLine *line = qobject_cast<VisToolAlongLine *>(vis);
    SCASSERT(line != nullptr)

    line->setObject1Id(GetFirstPointId());
    line->setObject2Id(GetSecondPointId());
    line->setLength(formula);
    line->setLineStyle(LineStyleToPenStyle(GetTypeLine()));
    line->RefreshGeometry();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::closeEvent(QCloseEvent *event)
{
    ui->plainTextEditFormula->blockSignals(true);
    DialogTool::closeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::SetCurrentLength()
{
    const QSharedPointer<VPointF> p1 = data->GeometricObject<VPointF>(GetFirstPointId());
    const QSharedPointer<VPointF> p2 = data->GeometricObject<VPointF>(GetSecondPointId());

    VLengthLine *length = new VLengthLine(p1.data(), GetFirstPointId(), p2.data(),
                                          GetSecondPointId(), *data->GetPatternUnit());
    length->SetName(currentLength);

    VContainer *locData = const_cast<VContainer *> (data);
    locData->AddVariable(currentLength, length);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetSecondPointId set id second point of line
 * @param value id
 */
void DialogAlongLine::SetSecondPointId(const quint32 &value)
{
    setCurrentPointId(ui->comboBoxSecondPoint, value);

    VisToolAlongLine *line = qobject_cast<VisToolAlongLine *>(vis);
    SCASSERT(line != nullptr)
    line->setObject2Id(value);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::Build(const Tool &type)
{
    if (type == Tool::Midpoint)
    {
        buildMidpoint = true;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetFirstPointId set id first point of line
 * @param value id
 */
void DialogAlongLine::SetFirstPointId(const quint32 &value)
{
    setCurrentPointId(ui->comboBoxFirstPoint, value);

    VisToolAlongLine *line = qobject_cast<VisToolAlongLine *>(vis);
    SCASSERT(line != nullptr)
    line->setObject1Id(value);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetFormula set string of formula
 * @param value formula
 */
void DialogAlongLine::SetFormula(const QString &value)
{
    formula = qApp->TrVars()->FormulaToUser(value, qApp->Settings()->GetOsSeparator());
    // increase height if needed.
    if (formula.length() > 80)
    {
        this->DeployFormulaTextEdit();
    }
    ui->plainTextEditFormula->setPlainText(formula);

    VisToolAlongLine *line = qobject_cast<VisToolAlongLine *>(vis);
    SCASSERT(line != nullptr)
    line->setLength(formula);

    MoveCursorToEnd(ui->plainTextEditFormula);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetTypeLine set type of line
 * @param value type
 */
void DialogAlongLine::SetTypeLine(const QString &value)
{
    ChangeCurrentData(ui->comboBoxLineType, value);
    vis->setLineStyle(LineStyleToPenStyle(value));
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogAlongLine::GetLineColor() const
{
    return GetComboBoxCurrentData(ui->comboBoxLineColor, ColorBlack);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogAlongLine::SetLineColor(const QString &value)
{
    ChangeCurrentData(ui->comboBoxLineColor, value);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetPointName set name of point
 * @param value name
 */
void DialogAlongLine::SetPointName(const QString &value)
{
    pointName = value;
    ui->lineEditNamePoint->setText(pointName);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetTypeLine return type of line
 * @return type
 */
QString DialogAlongLine::GetTypeLine() const
{
    return GetComboBoxCurrentData(ui->comboBoxLineType, TypeLineLine);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetFormula return string of formula
 * @return formula
 */
QString DialogAlongLine::GetFormula() const
{
    return qApp->TrVars()->TryFormulaFromUser(formula, qApp->Settings()->GetOsSeparator());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetFirstPointId return id first point of line
 * @return id
 */
quint32 DialogAlongLine::GetFirstPointId() const
{
    return getCurrentObjectId(ui->comboBoxFirstPoint);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetSecondPointId return id second point of line
 * @return id
 */
quint32 DialogAlongLine::GetSecondPointId() const
{
    return getCurrentObjectId(ui->comboBoxSecondPoint);
}
