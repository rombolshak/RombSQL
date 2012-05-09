/*
    RombSQL Server. Simple SQL server w/ simple SQL commands supported
    Copyright (C) 2012  Большаков Роман <rombolshak@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "RTable.h"
#include "RTableFile.h"
#include "RException.h"
#include "RTableCondition.h"
#include <fstream>
#include <cstdlib>

#include <sstream>

namespace RSQL
{

bool RTable::CreateTable ( string name, RTableDefinition def )
{
  RTableFile::create ( name, def );
  return true;
}
bool RTable::DropTable ( string name )
{
  RTableFile::drop ( name );
  return true;
}
bool RTable::TruncateTable ( string name )
{
  RTableFile::truncate ( name );
  return true;
}
bool RTable::Insert ( string name, RTableRecord rec )
{
  RTableFile f = RTableFile::open ( name );
  RTableDefinition def = f.getDefinition();
  if ( def.size() != rec.size() ) throw new RTableException ( ColumnsNumber );
  bool valid = true;
  // проверка соответствия типов
  RTableDefinition::iterator it = def.begin();
  while ( it != def.end() )
    if ( rec[it->first]->getType() != it->second )
      {
        valid = false;
        break;
      }
    else it++;
  if ( valid )
    f.createRecord ( rec );
  else
    {
      stringstream s;
      s << "Плохой тип у " << it->first << endl;
      throw new RException ( s.str() );
    }
  f.close();
  return true;
}
bool RTable::Insert ( string name, std::vector< RCell* > cells )
{
  // то самое преобразование к записи
  RTableFile f = RTableFile::open ( name );
  RTableDefinition def = f.getDefinition();
  f.close();
  RTableRecord rec;
  map<string, RFieldType>::iterator it = def.begin();
  int i = 0;
  while ( ( it != def.end() ) && ( i < cells.size() ) )
    rec[it++->first] = cells[i++];
  if ( ( it != def.end() ) || ( i < cells.size() ) ) throw new RTableException ( ColumnsNumber );
  return Insert ( name, rec );
}


bool RTable::Delete ( string name, RTableCondition cond )
{
  RTableFile f = RTableFile::open ( name );
  RTableRecord rec;
  while ( true )
    {
      try
        {
          rec = f.readCurrentRecord();
        }
      catch ( RFileException* e )
        {
          break;
        }
      if ( cond.check ( rec ) ) f.deleteCurrentRecord();
      else f.moveNext();
    }
  f.close();
  return true;
}
bool RTable::Update ( string name, vector<RTableAssignment> newVal, RTableCondition cond )
{
  RTableFile f = RTableFile::open ( name );
  RTableRecord rec;
  while ( true )
    {
      try
        {
          rec = f.readCurrentRecord();
        }
      catch ( RFileException* e )
        {
          break;
        }
      if ( cond.check ( rec ) )
        for ( int i = 0; i < newVal.size(); ++i )
          {
            bool valid = true;
            // тоже нужно проверить, а вдруг на бяку хотим заменить
            RTableDefinition def = f.getDefinition();
            RTableDefinition::iterator it = def.begin();
            while ( it != def.end() )
              if ( rec[it->first]->getType() != it->second )
                {
                  valid = false;
                  break;
                }
              else it++;
            if ( valid )
              f.updateCurrentRecord (
                make_pair<string, RCell*> (
                  newVal[i].first,
                  newVal[i].second->_check ( rec )->getValue() )
              );
            else
              {
                stringstream s;
                s << "Плохой тип у " << it->first << endl;
                throw new RException ( s.str() );
              }
          }
      f.moveNext();
    }
  f.close();
  return true;
}
_RTable RTable::Select ( string name, vector< string > fields, RTableCondition cond )
{
  vector<RTableRecord> res;
  RTableDefinition def;
  RTableRecord tmp;

  RTableFile f = RTableFile::open ( name );
  def = f.getDefinition();
  if ( fields.size() != 0 )
    {
      // оставляем только те поля, которые требуются
      // в этом куске происходят какие то странные вещи, но программа не падает, и даже не ругается. Ну и отлично
      RTableDefinition::iterator it = def.begin();
      while ( it != def.end() )
        {
          bool contains = false;
          for ( int i = 0; i < fields.size(); ++i )
            if ( it->first == fields[i] )
              {
                contains = true;
                break;
              }
          if ( !contains ) def.erase ( it->first );
          it++;
        }
    } // здесь магия кончается и все течет своим чередом. Ну или как там говорится
  RTableRecord rec;
  while ( true )
    {
      try
        {
          rec = f.readCurrentRecord();
        }
      catch ( RFileException* e )
        {
          break;
        }
      if ( cond.check ( rec ) )
        {
          if ( fields.size() == 0 ) tmp = rec;
          else
            for ( uint i = 0; i < fields.size(); ++i )
              tmp[fields[i]] = rec[fields[i]];
          res.push_back ( tmp );
          tmp.clear();
        }
      f.moveNext();
    }
  f.close();

  return make_pair<RTableDefinition, vector<RTableRecord> > ( def, res );
}
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
