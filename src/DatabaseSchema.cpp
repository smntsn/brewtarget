/*
 * DatabaseSchema.cpp is part of Brewtarget, and is Copyright the following
 * authors 2019-2024
 * - Mik Firestone <mikfire@fastmail.com>
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QDebug>
#include <QString>
#include <QStringBuilder>

#include "TableSchema.h"
#include "TableSchemaConst.h"
#include "DatabaseSchema.h"

// These HAVE to be in the same order as they are listed in
// Brewtarget::DBTable
static QStringList dbTableToName  = QStringList() <<
   QString("none") <<  // need to handle the NOTABLE index
   ktableMeta <<
   ktableSettings <<
   ktableEquipment <<
   ktableFermentable <<
   ktableHop <<
   ktableMisc <<
   ktableStyle <<
   ktableYeast <<
   ktableWater <<
   ktableMash <<
   ktableMashStep <<
   ktableRecipe <<
   ktableBrewnote <<
   ktableInstruction <<
// Now for BT internal tables
   ktableBtEquipment <<
   ktableBtFermentable <<
   ktableBtHop <<
   ktableBtMisc <<
   ktableBtStyle <<
   ktableBtYeast <<
   ktableBtWater <<
// Now the in_recipe tables
   ktableFermInRec <<
   ktableHopInRec <<
   ktableMiscInRec <<
   ktableWaterInRec <<
   ktableYeastInRec <<
   ktableInsInRec <<
// child tables next
   ktableEquipChildren <<
   ktableFermChildren <<
   ktableHopChildren <<
   ktableMiscChildren <<
   ktableRecChildren <<
   ktableStyleChildren <<
   ktableWaterChildren <<
   ktableYeastChildren <<
// inventory tables last
   ktableFermInventory <<
   ktableHopInventory <<
   ktableMiscInventory <<
   ktableYeastInventory;

DatabaseSchema::DatabaseSchema()
{
   loadTables();
   m_type = Brewtarget::dbType();
}

void DatabaseSchema::loadTables()
{
   int it;

   for ( it = Brewtarget::NOTABLE; it <= Brewtarget::YEASTINVTABLE; it++ ) {
      Brewtarget::DBTable tab = static_cast<Brewtarget::DBTable>(it);
      TableSchema* tmp = new TableSchema(tab);
      m_tables.append(tmp);
   }
}

TableSchema *DatabaseSchema::table(Brewtarget::DBTable table)
{
   if ( table > Brewtarget::NOTABLE && table < m_tables.size() ) {
      return m_tables.at(table);
   }

   return nullptr;
}

TableSchema *DatabaseSchema::table(QString tableName)
{
   if ( dbTableToName.contains(tableName) ) {
      return m_tables.value( static_cast<Brewtarget::DBTable>(dbTableToName.indexOf(tableName)));
   }
   return nullptr;
}

QString DatabaseSchema::tableName(Brewtarget::DBTable table)
{
   if ( table > Brewtarget::NOTABLE && table < m_tables.size() ) {
      return m_tables.at(table)->tableName();
   }

   return QString("");
}

// I believe one method replaces EVERY create_ method in DatabaseSchemaHelper.
// It is so beautiful, it must be evil.
const QString DatabaseSchema::generateCreateTable(Brewtarget::DBTable table, QString name)
{
   if ( table <= Brewtarget::NOTABLE || table > m_tables.size() ) {
      return QString();
   }

   TableSchema* tSchema = m_tables.at(table);

   return tSchema->generateCreateTable(m_type);
}

// these two basically just pass the call to the proper table
const QString DatabaseSchema::generateInsertRow(Brewtarget::DBTable table)
{
   TableSchema* tSchema = m_tables.value(table);
   return tSchema->generateInsertRow(m_type);
}

const QString DatabaseSchema::generateCopyTable(Brewtarget::DBTable src, QString dest, Brewtarget::DBTypes type)
{
   TableSchema* tSchema = m_tables.value(src);
   return tSchema->generateCopyTable(dest,type);
}

const QString DatabaseSchema::generateUpdateRow(Brewtarget::DBTable table, int key)
{
   TableSchema* tSchema = m_tables.value(table);
   return tSchema->generateUpdateRow(key, m_type);
}

Brewtarget::DBTable DatabaseSchema::classNameToTable(QString className) const
{
   Brewtarget::DBTable retval = Brewtarget::NOTABLE;

   foreach( TableSchema* here, m_tables ) {
      if ( here->className() == className ) {
          retval = here->dbTable();
          break;
      }
   }
   return retval;
}

const QString DatabaseSchema::classNameToTableName(QString className) const
{
   QString retval;

   foreach( TableSchema* here, m_tables ) {
      if ( here->className() == className ) {
          retval = here->tableName();
          break;
      }
   }
   return retval;
}

QVector<TableSchema*> DatabaseSchema::inventoryTables()
{
    QVector<TableSchema*> retVal;

    foreach( TableSchema* here, m_tables ) {
        if ( here->isInventoryTable() ) {
            retVal.append(here);
        }
    }

    return retVal;
}

QVector<TableSchema*> DatabaseSchema::childTables()
{
    QVector<TableSchema*> retVal;

    foreach( TableSchema* here, m_tables ) {
        if ( here->isChildTable() ) {
            retVal.append(here);
        }
    }

    return retVal;
}

QVector<TableSchema*> DatabaseSchema::inRecipeTables()
{
    QVector<TableSchema*> retVal;

    foreach( TableSchema* here, m_tables ) {
        if ( here->isInRecTable() ) {
            retVal.append(here);
        }
    }

    return retVal;
}

QVector<TableSchema*> DatabaseSchema::baseTables()
{
    QVector<TableSchema*> retVal;

    foreach( TableSchema* here, m_tables ) {
        if ( here->isBaseTable() ) {
            retVal.append(here);
        }
    }

    return retVal;
}

QVector<TableSchema*> DatabaseSchema::btTables()
{
    QVector<TableSchema*> retVal;

    foreach( TableSchema* here, m_tables ) {
        if ( here->isBtTable() ) {
            retVal.append(here);
        }
    }

    return retVal;
}

QVector<TableSchema*>  DatabaseSchema::allTables()
{
    QVector<TableSchema*> retval;

    for( int i = 1; i < dbTableToName.size(); ++i ) {
        retval.append( m_tables.value( static_cast<Brewtarget::DBTable>(i)) );
    }
    return retval;
}

TableSchema* DatabaseSchema::childTable(Brewtarget::DBTable dbTable)
{
    TableSchema* tbl = table(dbTable);
    TableSchema* retVal = nullptr;

    if ( tbl != nullptr ) {
       Brewtarget::DBTable idx = tbl->childTable();
       retVal = table( idx );
    }
    return retVal;
}

TableSchema* DatabaseSchema::inRecTable(Brewtarget::DBTable dbTable)
{
    TableSchema* tbl = table(dbTable);
    TableSchema* retVal = nullptr;

    if ( tbl != nullptr ) {
       Brewtarget::DBTable idx = tbl->inRecTable();
       retVal = table( idx );
    }
    return retVal;
}

TableSchema* DatabaseSchema::invTable(Brewtarget::DBTable dbTable)
{
    TableSchema* tbl = table(dbTable);
    TableSchema* retVal = nullptr;

    if ( tbl != nullptr ) {
       Brewtarget::DBTable idx = tbl->invTable();
       retVal = table( idx );
    }
    return retVal;
}

TableSchema* DatabaseSchema::btTable(Brewtarget::DBTable dbTable)
{
    TableSchema* tbl = table(dbTable);
    TableSchema* retVal = nullptr;

    if ( tbl != nullptr ) {
       Brewtarget::DBTable idx = tbl->btTable();
       retVal = table( idx );
    }
    return retVal;
}

const QString DatabaseSchema::childTableName(Brewtarget::DBTable dbTable)
{
    TableSchema* chld = childTable(dbTable);

    return chld == nullptr ? QString() : chld->tableName();
}

const QString DatabaseSchema::inRecTableName(Brewtarget::DBTable dbTable)
{
    TableSchema* chld = inRecTable(dbTable);

    return chld == nullptr ? QString() : chld->tableName();
}

const QString DatabaseSchema::invTableName(Brewtarget::DBTable dbTable)
{
    TableSchema* chld = invTable(dbTable);

    return chld == nullptr ? QString() : chld->tableName();
}

const QString DatabaseSchema::btTableName(Brewtarget::DBTable dbTable)
{
   TableSchema* chld = btTable(dbTable);

   return chld == nullptr ? QString() : chld->tableName();
}
