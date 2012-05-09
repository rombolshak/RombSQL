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


#ifndef REXCEPTION_H
#define REXCEPTION_H
#include <string>
using namespace std;
namespace RSQL
{
class RException
{
    string s;
public:
    RException()
    {
        s = "Крайне информативное сообщение об ошибке. Удачной диагностики ;)\n";
    }
    RException ( string s )
    {
        this->s = s;
    }
    virtual string message();
};

enum TableError
{
    NullChilds,
    IllegType,
    NotSameTypes,
    NotBool,
    NotLong,
    NotText,
    IllegOperation,
    ColumnsNumber
};
class RTableException : public RException
{
    TableError code;
public:
    RTableException ( TableError te )
    {
        code = te;
    }
    string message();
};

enum FileError
{
    NotExists,
    FileExist,
    OutOfRange
};
class RFileException : public RException
{
    FileError code;
public:
    RFileException ( FileError fe )
    {
        code = fe;
    };
    string message();
};

enum CellError
{
    IllegalType
};
class RCellException : public RException
{
    CellError code;
public:
    RCellException ( CellError ce )
    {
        code = ce;
    };
    string message();
};

class RLexerException : public RException
{
    string s;
    int p;
public:
    RLexerException ( string str, int pos )
    {
        s=str;
        p=pos;
    };
    string message();
};

class RParserException : public RException
{
    string s;
public:
    RParserException ( string e )
    {
        s=e;
    }
    string message()
    {
        return s;
    }
};

enum ServerError
{
    SocketFail,
    BindFail,
    ListenFail
};
class RServerException : public RException
{
    ServerError code;
public:
    RServerException ( ServerError ce )
    {
        code = ce;
    };
    string message();
};
}
#endif // EXCEPTION_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
