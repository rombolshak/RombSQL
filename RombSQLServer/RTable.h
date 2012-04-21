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


#ifndef RTABLE_H
#define RTABLE_H

#include "RTableFile.h"
#include "RException.h"
#include <string>
using namespace std;

namespace RSQL {

class RTableCondition
{
	enum {Operation, Value, Field} nodeType;
	OpCode operation;
	RFieldType retType;
	long l; string s; bool b;
	string fieldName;

	RTableCondition *left, *right;
	void bind(RTableRecord);
	void bind( RFieldType, string str = "", long lon = 0, bool boo = false );
    RTableCondition* runOp ( OpCode operation, RTableCondition* left, RTableCondition* right, RTableRecord rec );
	RTableCondition* like ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* math ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* logic ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* comp ( OpCode operation, RTableCondition* left, RTableCondition* right );
public:
	RTableCondition * check(RTableRecord);
};

class RTable
{
	public:
		static bool CreateTable(string name, RTableDefinition);
		static bool DeleteTable(string name);
		static bool DropTable(string name);
		static bool Insert(string name, RTableRecord);
		static bool Delete(string name, RTableCondition);
		static vector< RTableRecord > Select(string name, vector< string > fields, RTableCondition);
		static bool Update(string name, RTableRecord, RTableCondition);
};
}
#endif // RTABLE_H
