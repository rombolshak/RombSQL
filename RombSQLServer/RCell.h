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

class RCell
{
public:
    virtual RFieldType getType() = 0;
    virtual void getValue ( long& ) = 0;
    virtual void getValue ( string& ) = 0;
    virtual void getValue ( bool& ) = 0;
};
class RLongCell : public RCell
{
    long l;
public:
    RLongCell ( long val ) : l ( val ) {}
    RFieldType getType()
    {
        return TYPE_LONG;
    }
    void getValue ( long& val )
    {
        val = l;
    }
    void getValue ( string& )
    {
        throw new RCellException ( IllegalType );
    }
    void getValue ( bool& )
    {
        throw new RCellException ( IllegalType );
    }
};
class RTextCell : public RCell
{
    string s;
public:
    RTextCell ( string val ) : s ( val ) {}
    RFieldType getType()
    {
        return TYPE_TEXT;
    }
    void getValue ( string& val )
    {
        val = s;
    }
    void getValue ( long& )
    {
        throw new RCellException ( IllegalType );
    }
    void getValue ( bool& )
    {
        throw new RCellException ( IllegalType );
    }
};
class RBoolCell : public RCell
{
    bool b;
public:
    RBoolCell ( bool val ) : b ( val ) {}
    RFieldType getType()
    {
        return TYPE_BOOL;
    }
    void getValue ( long& )
    {
        throw new RCellException ( IllegalType );
    }
    void getValue ( string& )
    {
        throw new RCellException ( IllegalType );
    }
    void getValue ( bool& val )
    {
        val = b;
    }
};
}

#endif // RCELLVALUE_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
