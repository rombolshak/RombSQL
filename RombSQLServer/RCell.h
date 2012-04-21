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

#ifndef RCELL_H
#define RCELL_H

#include <iostream>
#include <string>
#include "RException.h"
using namespace std;

namespace RSQL
{
enum RFieldType
{
    TYPE_TEXT,
    TYPE_LONG,
    TYPE_BOOL
};
enum OpCode
{
    OpLike,
    OpNotLike,
    OpAdd,
    OpSub,
    OpNegate,
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

class RCell
{
public:
    virtual RFieldType getType() = 0;
    virtual RCell * exec  ( OpCode, RCell * right );
    virtual void getValue ( long& ) = 0;
    virtual void getValue ( string& ) = 0;
    virtual void getValue ( bool& ) = 0;
    virtual ~RCell() {cout << "RCell destructor" << endl;}
};
class RLongCell : public RCell
{
    long l;
public:
    RLongCell(long val) : l(val) {}
    RFieldType getType() {return TYPE_LONG;}
    RCell * exec (OpCode, RCell*);
    void getValue(long& val) {val = l;}
    void getValue(string&) {throw RCellException(IllegalType);}
    void getValue(bool&) {throw RCellException(IllegalType);}
};
class RTextCell : public RCell
{
    string s;
public:
    RTextCell(string val) : s(val) {}
    RFieldType getType() {return TYPE_TEXT;}
    RCell * exec (OpCode, RCell*);
    void getValue(string& val) {val = s;}
    void getValue(long&) {throw RCellException(IllegalType);}
    void getValue(bool&) {throw RCellException(IllegalType);}
};
class RBoolCell : public RCell
{
    bool b;
public:
    RBoolCell(bool val) : b(val) {}
    RFieldType getType() {return TYPE_BOOL;}
    RCell * exec (OpCode, RCell*);
    void getValue(long&) {throw RCellException(IllegalType);}
    void getValue(string&) {throw RCellException(IllegalType);}
    void getValue(bool& val) {val = b;}
};
}

#endif // RCELLVALUE_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
