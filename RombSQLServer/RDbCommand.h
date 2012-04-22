/*
    <one line to give the program's name and a brief idea of what it does.>
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
class RDbResult
{
public:
    enum {ERROR, SUCCESS} status;
    string errMessage() {return status==ERROR?e.message():"";}
    vector< RTableRecord > result() {if (!select) throw 403; return selectRes;}
private:
    friend class RDbSelectCommand;
    friend class RDbInsertCommand;
    friend class RDbUpdateCommand;
    friend class RDbDeleteRecordCommand;
    friend class RDbDeleteTableCommand;
    friend class RDbDropCommand;
    friend class RDbCreateCommand;
    RException e;
    bool select;
    vector< RTableRecord > selectRes;
    RDbResult(bool success) {select = false; status = success?SUCCESS:ERROR;}
    RDbResult(RException ex) {status = ERROR; e = ex;}
    RDbResult(vector< RTableRecord > res) {status = SUCCESS; select = true; selectRes = res;}
};

class RDbCommand
{
protected:
    string tableName;
public:
    virtual RDbResult execute() = 0;
    static RDbCommand * parseFromString(string query);
};

class RDbInsertCommand : public RDbCommand
{
    RTableRecord rec;
public:
    virtual RDbResult execute();
};

class RDbSelectCommand : public RDbCommand
{
    vector<string> fields;
    RTableCondition cond;
public:
    virtual RDbResult execute();
};
class RDbUpdateCommand : public RDbCommand
{
    RTableRecord rec;
    RTableCondition cond;
public:
    virtual RDbResult execute();
};
class RDbDeleteRecordCommand : public RDbCommand
{
    RTableCondition cond;
public:
    virtual RDbResult execute();
};
class RDbDeleteTableCommand : public RDbCommand
{
public:
    virtual RDbResult execute();
};
class RDbCreateCommand : public RDbCommand
{
    RTableDefinition def;
public:
    virtual RDbResult execute();
};
class RDbDropCommand : public RDbCommand
{
public:
    virtual RDbResult execute();
};
}
#endif // RDBCOMMAND_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
