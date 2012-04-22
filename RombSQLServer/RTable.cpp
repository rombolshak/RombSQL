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


#include "RTable.h"
#include "RTableFile.h"
#include "RException.h"
#include <fstream>
#include <cstdlib>

namespace RSQL
{
RTableCondition* RTableCondition::comp ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
	if ((left->nodeType != Value) || (right->nodeType != Value)) throw RTableException(IllegType);
	if (left->retType != right->retType) throw RTableException(NotSameTypes);
	RFieldType t = left->retType;
	long l1, l2; string s1, s2; bool b1, b2;
	if (t == TYPE_LONG) {l1 = left->l; l2 = right->l;}
	else if (t == TYPE_TEXT) {s1 = left->s; s2 = right->s;}
	else {b1 = left->b; b2 = right->b;}
	RTableCondition *res = new RTableCondition(); res->nodeType = Value; res->retType = TYPE_BOOL;
	switch (operation)
	{
		case OpEq:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 == l2); break;
				case TYPE_TEXT: res->b = (s1 == s2); break;
				case TYPE_BOOL: res->b = (b1 == b2); break;
			}
		}
		case OpNeq:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 != l2); break;
				case TYPE_TEXT: res->b = (s1 != s2); break;
				case TYPE_BOOL: res->b = (b1 != b2); break;
			}
		}
		case OpLess:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 < l2); break;
				case TYPE_TEXT: res->b = (s1 < s2); break;
				case TYPE_BOOL: res->b = (b1 < b2); break;
			}
		}
		case OpLessEq:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 <= l2); break;
				case TYPE_TEXT: res->b = (s1 <= s2); break;
				case TYPE_BOOL: res->b = (b1 <= b2); break;
			}
		}
		case OpGreater:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 > l2); break;
				case TYPE_TEXT: res->b = (s1 > s2); break;
				case TYPE_BOOL: res->b = (b1 > b2); break;
			}
		}
		case OpGreaterEq:
		{
			switch (t)
			{
				case TYPE_LONG: res->b = (l1 >= l2); break;
				case TYPE_TEXT: res->b = (s1 >= s2); break;
				case TYPE_BOOL: res->b = (b1 >= b2); break;
			}
		}
				default: throw RTableException(IllegOperation);
	}
	return res;
}
RTableCondition* RTableCondition::like ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
	if ((left->nodeType != Value) || (right->nodeType != Value)) throw RTableException(IllegType);
	if (left->retType != right->retType) throw RTableException(NotSameTypes);
	if (left->retType != TYPE_TEXT) throw RTableException(NotText);
	string s1 = left->s, s2 = right->s;
	RTableCondition *res = new RTableCondition(); res->nodeType = Value; res->retType = TYPE_BOOL;
	string q = "./regex.sh " + s2 + s1;
	system(q.c_str());
	switch (operation)
	{
		case OpLike: res->b = (std::ifstream ( "1" ) != NULL);
		case OpNotLike: res->b = (std::ifstream ( "0" ) != NULL);
		default: throw RTableException(IllegOperation);
	}
	return res;
}
RTableCondition* RTableCondition::logic ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
	if ((left->nodeType != Value) || (right->nodeType != Value)) throw RTableException(IllegType);
	if (left->retType != right->retType) throw RTableException(NotSameTypes);
	if (left->retType != TYPE_BOOL) throw RTableException(NotBool);
	bool b1 = left->b, b2 = right->b;
	RTableCondition *res = new RTableCondition(); res->nodeType = Value; res->retType = TYPE_BOOL;
	switch (operation)
	{
		case OpAnd: res->b = (b1 && b2);
		case OpOr: res->b = (b1 || b2);
		case OpNot: res->b = (b1 != b2);
		default: throw RTableException(IllegOperation);
	}
	return res;
}
RTableCondition* RTableCondition::math ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
	if ((left->nodeType != Value) || (right->nodeType != Value)) throw RTableException(IllegType);
	if (left->retType != right->retType) throw RTableException(NotSameTypes);
	if (left->retType != TYPE_LONG) throw RTableException(NotLong);
	long l1 = left->l; long l2 = right->l;
	RTableCondition *res = new RTableCondition(); res->nodeType = Value; res->retType = TYPE_LONG;
	switch (operation)
	{
		case OpAdd: res->l = l1 + l2;
		case OpSub: res->l = l1 - l2;
		case OpMult:res->l = l1 * l2;
		case OpDiv: res->l = l1 / l2;
		case OpMod: res->l = l1 % l2;
		default: throw RTableException(IllegOperation);
	}
	return res;
}


RTableCondition* RTableCondition::runOp ( OpCode operation, RTableCondition* left, RTableCondition* right, RTableRecord rec )
{
	switch (operation)
	{
		case OpLike:
		case OpNotLike: return like(operation, left->check(rec), right->check(rec));
		case OpAdd:
		case OpSub:
		case OpMult:
		case OpDiv:
		case OpMod: return math(operation, left->check(rec), right->check(rec));
		case OpAnd:
		case OpOr:
		case OpNot: return logic(operation, left->check(rec), right->check(rec));
		case OpEq:
		case OpNeq:
		case OpLess:
		case OpLessEq:
		case OpGreater:
		case OpGreaterEq: return comp(operation, left->check(rec), right->check(rec));
	}
	return NULL;
}
void RTableCondition::bind ( RFieldType type , string str, long lon, bool boo )
{
	nodeType = Value;
	retType = type;
	if (type == TYPE_BOOL) b = boo;
	else if (type == TYPE_LONG) l = lon;
	else s = str;
}
void RTableCondition::bind ( RTableRecord rec )
{
	RCell *cell = rec[fieldName];
	bool b; long l; string s; RFieldType t = cell->getType();
	switch (t)
	{
		case TYPE_BOOL: cell->getValue(b); break;
		case TYPE_LONG: cell->getValue(l); break;
		case TYPE_TEXT: cell->getValue(s); break;
	}
	bind(t,s,l,b);
}


RTableCondition* RTableCondition::check ( RTableRecord rec )
{
	switch (nodeType)
	{
		case Operation:
		{
			if ((!left) || (!right)) throw RTableException(NullChilds);
			return runOp(operation, left, right, rec);
		}
		case Value:
		{
			return this;
		}
		case Field:
		{
			bind(rec);
			return this;
		}
	}
	return NULL;
}

bool RTable::CreateTable ( string name, RTableDefinition def )
{
	try 
	{
		RTableFile::create(name, def);
		return true;
	}
	catch (RFileException) {throw;}
}
bool RTable::DeleteTable ( string name )
{
	try 
	{
		RTableFile::delet(name);
		return true;
	}
	catch(RFileException) {throw;}
}
bool RTable::DropTable ( string name )
{
	try
	{
		RTableFile::drop(name);
		return true;
	}
	catch (RFileException) {throw;}
}

bool RTable::Insert ( string name, RTableRecord rec )
{
	try 
	{
		RTableFile f = RTableFile::open(name);
		f.createRecord(rec);
		f.close();
		return true;
	}
	catch (...) {throw;}
}

bool RTable::Delete ( string name, RTableCondition cond )
{
	try 
	{
		RTableFile f = RTableFile::open(name);
		RTableRecord rec;
		while (true) {
		try {
		rec = f.readCurrentRecord();
		}
		catch (RFileException e) {break;}
		if (cond.check(rec)) f.deleteCurrentRecord();
		f.moveNext();
		}
		return true;
	}
	catch (...) {throw;}
}
bool RTable::Update ( string name, RTableRecord newRec, RTableCondition cond )
{
	try 
	{
		RTableFile f = RTableFile::open(name);
		RTableRecord rec;
		while (true) {
			try {
				rec = f.readCurrentRecord();
			}
			catch (RFileException e) {break;}
			if (cond.check(rec)) f.updateCurrentRecord(newRec);
			f.moveNext();
		}
		return true;
	}
	catch (...) {throw;}
}
vector< RTableRecord > RTable::Select ( string name, vector< string > fields, RTableCondition cond )
{
	vector<RTableRecord> res;
	RTableRecord tmp;
	try 
	{
		RTableFile f = RTableFile::open(name);
		RTableRecord rec;
		while (true) {
			try {
				rec = f.readCurrentRecord();
			}
			catch (RFileException e) {break;}
			if (cond.check(rec))
			{
				for (uint i = 0; i < fields.size(); ++i)
					tmp[fields[i]] = rec[fields[i]];
				res.push_back(tmp);
			}
			f.moveNext();
		}
	}
	catch (...) {throw;}
	return res;
}


	
}
