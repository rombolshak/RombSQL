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


#include "RException.h"
#include <iostream>

#include <sstream>
using namespace std;
namespace RSQL
{
string RFileException::message()
{
  switch ( code )
    {
    case NotExists:
      return "Файл не существует";
    case FileExist:
      return "Файл уже существует";
    case OutOfRange:
      return "Выход за границы массива";
    default:
      return "Неужели я должен помнить все коды ошибок?";
    }
}
string RTableException::message()
{
  switch ( code )
    {
    case NullChilds:
      return "Недостаточно аргументов для передачи в операцию";
    case IllegType:
      return "Не является типом LONG или TEXT";
    case NotSameTypes:
      return "Типы аргументов не совпадают";
    case NotBool:
      return "Значение не является bool";
    case NotLong:
      return "Значение не является long";
    case NotText:
      return "Значение не является text";
    case IllegOperation:
      return "Недопустимое значение операции";
    case ColumnsNumber:
      return "Не совпадает число колонок у вставляемой записи";
    default:
      return "Программа аварийно завершилась по соображениям сохранения психического здоровья";
    }
}
string RServerException::message()
{
  switch ( code )
    {
    case SocketFail:
      return "Не удалось создать сокет";
    case BindFail:
      return "Не удалось связать сокет с локальным адресом";
    case ListenFail:
      return "Ошибка при вызове listen()";
    default:
      return "Случайный баг";
    }
}
string RCellException::message()
{
  return RSQL::RException::message();
}
string RLexerException::message()
{
  int start = p - 4;
  if ( start < 0 ) start = 0;
  stringstream ss;
  ss << "Ошибка в синтаксисе около '" << s.substr ( start, 10 ) << "'" << endl;
  return  ss.str();
}


string RException::message()
{
  return s;
}

}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
