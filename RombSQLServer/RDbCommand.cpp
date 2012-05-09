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


#include "RDbCommand.h"
#include "RParser.h"

namespace RSQL
{
RDbResult RDbCreateCommand::execute()
{
  try
    {
      RTable::CreateTable ( tableName, def );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbDeleteCommand::execute()
{
  try
    {
      RTable::Delete ( tableName, cond );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbDropCommand::execute()
{
  try
    {
      RTable::DropTable ( tableName );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbTruncateCommand::execute()
{
  try
    {
      RTable::TruncateTable ( tableName );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbInsertCommand::execute()
{
  try
    {
      if ( cells.size() == 0 ) // если задан массив ячеек вместо записи
        RTable::Insert ( tableName, rec ); // да, мне впадлу было тут преобразовывать, пусть таблица этим занимается
      else RTable::Insert ( tableName, cells );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbUpdateCommand::execute()
{
  try
    {
      RTable::Update ( tableName, rec, cond );
      return RDbResult ( true );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}
RDbResult RDbSelectCommand::execute()
{
  try
    {
      _RTable res = RTable::Select ( tableName, fields, cond );
      return RDbResult ( res );
    }
  catch ( RException e )
    {
      return RDbResult ( e );
    }
}

}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
