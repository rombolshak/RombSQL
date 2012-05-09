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


#ifndef RDBCOMMAND_H
#define RDBCOMMAND_H
#include <string>
#include <vector>
#include "RTable.h"

using namespace std;
namespace RSQL
{
typedef enum {ERROR, SUCCESS} statusResult;

/**
 * Результат выполнения команды
 */
class RDbResult
{
public:
    statusResult status;
    string errMessage()
    {
        return status==ERROR?e.message() :"";
    }
    /**
     * Вернет таблицу, если тип выполненной команды — SELECT
     */
    _RTable result()
    {
        if ( !select ) throw 403;
        return selectRes;
    }
private:
    friend class RDbSelectCommand;
    friend class RDbInsertCommand;
    friend class RDbUpdateCommand;
    friend class RDbDeleteCommand;
    friend class RDbDropCommand;
    friend class RDbTruncateCommand;
    friend class RDbCreateCommand;
    RException e;
    bool select;
    _RTable selectRes;
    RDbResult ( bool success )
    {
        select = false;
        status = success?SUCCESS:ERROR;
    }
    RDbResult ( RException ex )
    {
        status = ERROR;
        e = ex;
    }
    RDbResult ( _RTable res )
    {
        status = SUCCESS;
        select = true;
        selectRes = res;
    }
};

class RDbCommand
{
protected:
    string tableName;
    RDbCommand ( string name ) : tableName ( name ) {}
public:
    virtual RDbResult execute() = 0;
};

class RDbInsertCommand : public RDbCommand
{
    RTableRecord rec; // вставляем либо готовую запись
    vector<RCell*> cells; // либо массив ячеек приводим к виду записи
public:
    RDbInsertCommand ( string name, RTableRecord r ) : RDbCommand ( name ), rec ( r ) {}
    RDbInsertCommand ( string name, vector<RCell*> c ) : RDbCommand ( name ), cells ( c ) {}
    virtual RDbResult execute();
};

class RDbSelectCommand : public RDbCommand
{
    vector<string> fields;
    RTableCondition cond;
public:
    RDbSelectCommand ( string name, vector<string> f, RTableCondition c ) : RDbCommand ( name ), fields ( f ), cond ( c ) {}
    virtual RDbResult execute();
};
class RDbUpdateCommand : public RDbCommand
{
    vector<RTableAssignment> rec; // список присваиваний. Присваивание есть <имя>=<выражение>, поэтому синтаксический не будет ругаться на что-то вида id=(age+13)AND(name='qwe'), но присвоить значение типа bool невозможно (см. грамматику)
    RTableCondition cond;
public:
    RDbUpdateCommand ( string name, vector<RTableAssignment> r, RTableCondition c ) : RDbCommand ( name ), rec ( r ), cond ( c ) {}
    virtual RDbResult execute();
};
class RDbDeleteCommand : public RDbCommand
{
    RTableCondition cond;
public:
    RDbDeleteCommand ( string name, RTableCondition c ) : RDbCommand ( name ), cond ( c ) {}
    virtual RDbResult execute();
};
class RDbDropCommand : public RDbCommand
{
public:
    RDbDropCommand ( string name ) : RDbCommand ( name ) {}
    virtual RDbResult execute();
};
class RDbCreateCommand : public RDbCommand
{
    RTableDefinition def;
public:
    RDbCreateCommand ( string name, RTableDefinition d ) : RDbCommand ( name ), def ( d ) {}
    virtual RDbResult execute();
};
class RDbTruncateCommand : public RDbCommand
{
public:
    RDbTruncateCommand ( string name ) : RDbCommand ( name ) {}
    virtual RDbResult execute();
};
}
#endif // RDBCOMMAND_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
