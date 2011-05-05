/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.TXT included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#ifndef SETTINGS_CHECK_H
#define SETTINGS_CHECK_H

#include "coveragebrowserpdef.h"
#include <QList>
#include <QString>

class SettingsCheck
{
    SettingsCheck();
  public:
    static SettingsCheck & object() ;

    void check();
    void clear();

  private:
    struct warning_t
    {
      QString description;
      QString correct_text;
      bool (*warn)() ;
      void (*solve)() ;
    };

    QList<warning_t> warnings;
    bool warn(const warning_t &);

    static bool branch_method_warn();
    static void branch_method_solve();
    static bool test_count_mode_warn();
    static void test_count_mode_solve();
    static bool coverage_level_warn();
    static void coverage_level_solve();
    static bool execution_benefit_mode_warn();
    static void execution_benefit_mode_solve();
    static bool comparaison_mode_warn();
    static void comparaison_mode_solve();
} ;

#endif
