/************************************************************************
 **
 **  @file   calculator.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
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

#include "calculator.h"
#include <QDebug>
#include "../widgets/vapplication.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Calculator class constructor.
 * @param data pointer to a variable container.
 */
Calculator::Calculator(const VContainer *data)
    :QmuParser(), vVarVal(nullptr)
{
    //String with all unique symbols for supported alpabets.
    // See script alphabets.py for generation and more information.
    const QString symbols = QStringLiteral("ցЀĆЈVӧĎАғΕĖӅИқΝĞơРңњΥĦШҫ̆جگĮаҳѕεشԶиһνԾрυلՆӝшËՎҔPÓՖXӛӟŞӣզhëծpóӞնxßվāŁЃֆĉЋCŬđ"
                                           "ҐГΒęҘЛΚŘġҠУGاհЫدԱҰгβطԹõлκKՁÀуςهՉÈыvیՑÐSOřӘћաőcӐթèkàѓżűðsķչøӥӔĀփїІĈЎґĐΗЖҙĘȚ"
                                           "ΟОҡĠآΧЦتЮұİزηжԸغοоÁՀقχцÉՈيюÑՐђӋіәťӆўáŠĺѐfөըnñŰӤӨӹոľЁրăЉŭċБӸēłΔҖЙŤěΜӜDСձģΤӰ"
                                           "ЩīņحҮбưԳصδHйԻŇμӲӴсՃمτƠщՋєLQŹՓŕÖYśÞaգĽæiŽիӓîqճöyջþĂօЄӦĊЌΑĒДҗјΙȘĚМΡéĵĢФūӚΩبĪ"
                                           "ЬүќαذԲдҷιظԺмρՂфÇωوՊьÏՒTŚĻJբdçժlïӪղtպӫAւąЇčŃЏĕӯЗΖEțŮĝПΞأĥĹЧΦثÆӳЯIسŲԵзζԽпξكՅ"
                                           "ÄчφNMՍӌяӢՕÔWÎŝÜџёźեägխoӒյôwĶBžսüЂĄև̈ЊČƏљΓВҕĔӮΛКĜΣТҥĤکЪƯخγвŅԴŪضλкԼĴσтÅՄنъÍՌR"
                                           "ӕՔZÝŜbåդﻩjíլļrӵմzýռپêЅքćچЍďӱҒЕůėژșΘØҚНğńءΠFҢХħΨҪЭųįҶرҲеԷňعθҺнԿفπÂхՇψÊэšՏÒU"
                                           "əÚѝŻşҤӑâeէŐımկòuշÕúտŔ");

    // Defining identifier character sets
    DefineNameChars(QStringLiteral("0123456789_") + symbols);
    DefineOprtChars(symbols + QStringLiteral("+-*^/?<>=#!$%&|~'_"));

    // Add variables
    InitVariables(data);

    // Add unary operators
    DefinePostfixOprt(cm_Oprt, CmUnit);
    DefinePostfixOprt(mm_Oprt, MmUnit);
    DefinePostfixOprt(in_Oprt, InchUnit);
}

Calculator::~Calculator()
{
    delete [] vVarVal;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief eval calculate formula.
 * @param formula string of formula.
 * @return value of formula.
 */
qreal Calculator::EvalFormula(const QString &formula)
{
    SetExpr(formula);
    return Eval();
}

//---------------------------------------------------------------------------------------------------------------------
void Calculator::InitVariables(const VContainer *data)
{
    int num = 0;
    if (qApp->patternType() == Pattern::Standard)
    {
        num +=2;
    }

    const QHash<QString, qreal> *lengthLines = data->DataLengthLines();
    num += lengthLines->size();

    const QHash<QString, qreal> *lengthSplines = data->DataLengthSplines();
    num += lengthSplines->size();

    const QHash<QString, qreal> *lengthArcs = data->DataLengthArcs();
    num += lengthArcs->size();

    const QHash<QString, qreal> *lineAngles = data->DataLineAngles();
    num += lineAngles->size();

    const QHash<QString, VMeasurement> *measurements = data->DataMeasurements();
    num += measurements->size();

    const QHash<QString, VIncrement> *increments = data->DataIncrements();
    num += increments->size();

    vVarVal = new qreal[num];
    int j = 0;

    if (qApp->patternType() == Pattern::Standard)
    {
        vVarVal[j] = data->size();
        DefineVar(data->SizeName(), &vVarVal[j]);
        ++j;

        vVarVal[j] = data->height();
        DefineVar(data->HeightName(), &vVarVal[j]);
        ++j;
    }

    {
        QHash<QString, qreal>::const_iterator i = lengthLines->constBegin();
        while (i != lengthLines->constEnd())
        {
            vVarVal[j] = i.value();
            DefineVar(i.key(), &vVarVal[j]);
            ++j;
            ++i;
        }
    }

    {
        QHash<QString, qreal>::const_iterator i = lengthSplines->constBegin();
        while (i != lengthSplines->constEnd())
        {
            vVarVal[j] = i.value();
            DefineVar(i.key(), &vVarVal[j]);
            ++j;
            ++i;
        }
    }

    {
        QHash<QString, qreal>::const_iterator i = lengthArcs->constBegin();
        while (i != lengthArcs->constEnd())
        {
            vVarVal[j] = i.value();
            DefineVar(i.key(), &vVarVal[j]);
            ++j;
            ++i;
        }
    }

    {
        QHash<QString, qreal>::const_iterator i = lineAngles->constBegin();
        while (i != lineAngles->constEnd())
        {
            vVarVal[j] = i.value();
            DefineVar(i.key(), &vVarVal[j]);
            ++j;
            ++i;
        }
    }

    {
        QHash<QString, VMeasurement>::const_iterator i = measurements->constBegin();
        while (i != measurements->constEnd())
        {
            if (qApp->patternType() == Pattern::Standard)
            {
                vVarVal[j] = i.value().GetValue(data->size(), data->height());
                DefineVar(i.key(), &vVarVal[j]);
                ++j;
            }
            else
            {
                vVarVal[j] = i.value().GetValue();
                DefineVar(i.key(), &vVarVal[j]);
                ++j;
            }
            ++i;
        }
    }

    {
        QHash<QString, VIncrement>::const_iterator i = increments->constBegin();
        while (i != increments->constEnd())
        {
            if (qApp->patternType() == Pattern::Standard)
            {
                vVarVal[j] = i.value().GetValue(data->size(), data->height());
                DefineVar(i.key(), &vVarVal[j]);
                ++j;
            }
            else
            {
                vVarVal[j] = i.value().GetValue();
                DefineVar(i.key(), &vVarVal[j]);
                ++j;
            }
            ++i;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
qreal Calculator::CmUnit(qreal val)
{
    qreal unit = val;
    switch(qApp->patternUnit())
    {
        case Valentina::Mm:
            unit = val * 10.0;
            break;
        case Valentina::Cm:
            break;
        case Valentina::Inch:
            unit = val / 2.54;
            break;
        default:
            break;
    }

    return unit;
}

//---------------------------------------------------------------------------------------------------------------------
qreal Calculator::MmUnit(qreal val)
{
    qreal unit = val;
    switch(qApp->patternUnit())
    {
        case Valentina::Mm:
            break;
        case Valentina::Cm:
            unit = val / 10.0;
            break;
        case Valentina::Inch:
            unit = val / 25.4;
            break;
        default:
            break;
    }

    return unit;
}

//---------------------------------------------------------------------------------------------------------------------
qreal Calculator::InchUnit(qreal val)
{
    qreal unit = val;
    switch(qApp->patternUnit())
    {
        case Valentina::Mm:
            unit = val * 25.4;
            break;
        case Valentina::Cm:
            unit = val * 2.54;
            break;
        case Valentina::Inch:
            break;
        default:
            break;
    }

    return unit;
}
