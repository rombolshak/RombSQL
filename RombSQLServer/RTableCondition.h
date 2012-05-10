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


#ifndef RTABLECONDITION_H
#define RTABLECONDITION_H
#include "RTableFile.h"
namespace RSQL {
	enum OpCode
	{
		OpLike,
		OpNotLike,
		OpIn,
		OpNotIn,
		OpAdd,
		OpSub,
		OpMult,
		OpDiv,
		OpMod,
		OpAnd,
		OpOr,
		OpNot,
		OpEq,
		OpNeq,
		OpLess,
		OpGreater,
		OpLessEq,
		OpGreaterEq
	};

class RTableCondition
{// мой любимый класс
	enum {Operation, Value, Field, List} nodeType;
	OpCode operation; // если nodeType == Operation
	RFieldType retType; // nodeType == Value
	long l; string s; bool b; 
	vector<string> strList; vector<long> longList; // nodeType == List
	string fieldName;// nodeType == Field
	bool all;// WHERE ALL
	RTableCondition *left, *right;
	void bind(RTableRecord);
	void bind( RFieldType, string str = "", long lon = 0, bool boo = false );
	RTableCondition* runOp ( OpCode operation, RTableCondition* left, RTableCondition* right, RTableRecord rec );
	RTableCondition* like ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* in ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* math ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* logic ( OpCode operation, RTableCondition* left, RTableCondition* right );
	RTableCondition* comp/*compare*/ ( OpCode operation, RTableCondition* left, RTableCondition* right );
	
public:
	RTableCondition * _check(RTableRecord);
	bool check(RTableRecord);
	RTableCondition(OpCode oc, RTableCondition* left, RTableCondition* right)
	{
		nodeType = Operation; operation = oc; this->left = left; this->right = right; fieldName = ""; all = false;
	}
	RTableCondition(RFieldType type, long lng, string str, bool boo)
	{
		nodeType = Value; retType = type; l = lng; s = str; b = boo; fieldName = ""; all = false;
	}
	RTableCondition(string name)
	{
		nodeType = Field; fieldName = name; all = false;
	}
	RTableCondition(vector<string> arr)
	{
		nodeType = List; retType = TYPE_TEXT; strList = arr; fieldName = ""; all = false;
	}
	RTableCondition(vector<long> arr)
	{
		nodeType = List; retType = TYPE_LONG; longList = arr; fieldName = ""; all = false;
	}
	RTableCondition() {all = true;}
	RFieldType getType() {return retType;/* АХТУНГ!! Нет никаких проверок, так что аккуратнее пользуйтесь*/}
	RCell* getValue() {if (nodeType == Value) switch(retType) {
		case TYPE_LONG: return new RLongCell(l);
		case TYPE_TEXT: return new RTextCell(s);
		case TYPE_BOOL: return new RBoolCell(b);
	} else throw new RParserException("Неверное выражение\n");};
	bool isField() {return nodeType==Field;}
};
}
#endif // RTABLECONDITION_H
