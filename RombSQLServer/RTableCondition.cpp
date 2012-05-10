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


#include "RTableCondition.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <string>
namespace RSQL
{

void replace ( string& str, char c1, char c2 )
{
  for ( int i = 0; i < str.size(); ++i )
    if ( str[i]  == c1 )
      str[i] = c2;
}

RTableCondition* RTableCondition::comp ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
  if ( ( left->nodeType != Value ) || ( right->nodeType != Value ) ) throw new RTableException ( IllegType );
  if ( left->retType != right->retType ) throw new RTableException ( NotSameTypes );
  RFieldType t = left->retType;
  long l1, l2;
  string s1, s2;
  bool b1, b2;
  if ( t == TYPE_LONG )
    {
      l1 = left->l;
      l2 = right->l;
    }
  else if ( t == TYPE_TEXT )
    {
      s1 = left->s;
      s2 = right->s;
    }
  else
    {
      b1 = left->b;
      b2 = right->b;
    }
  RTableCondition *res = new RTableCondition();
  res->all = false;
  res->nodeType = Value;
  res->retType = TYPE_BOOL;
  switch ( operation )
    {
    case OpEq:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 == l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 == s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 == b2 );
          break;
        }
      break;
    }
    case OpNeq:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 != l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 != s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 != b2 );
          break;
        }
      break;
    }
    case OpLess:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 < l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 < s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 < b2 );
          break;
        }
      break;
    }
    case OpLessEq:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 <= l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 <= s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 <= b2 );
          break;
        }
      break;
    }
    case OpGreater:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 > l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 > s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 > b2 );
          break;
        }
      break;
    }
    case OpGreaterEq:
    {
      switch ( t )
        {
        case TYPE_LONG:
          res->b = ( l1 >= l2 );
          break;
        case TYPE_TEXT:
          res->b = ( s1 >= s2 );
          break;
        case TYPE_BOOL:
          res->b = ( b1 >= b2 );
          break;
        }
      break;
    }
    default:
      throw new RTableException ( IllegOperation );
    }
  return res;
}
RTableCondition* RTableCondition::like ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
  if ( ( left->nodeType != Value ) || ( right->nodeType != Value ) ) throw new RTableException ( IllegType );
  if ( left->retType != right->retType ) throw new RTableException ( NotSameTypes );
  if ( left->retType != TYPE_TEXT ) throw new RTableException ( NotText );
  string s1 = left->s, s2 = right->s;
  RTableCondition *res = new RTableCondition();
  res->all = false;
  res->nodeType = Value;
  res->retType = TYPE_BOOL;

  // для проверки регулярных выражений вызываем скрипт на bash, который создает файл с именем "1", если строка соответствует выражению, и "0" в противном случае
  replace ( s2, '%', '*' );
  replace ( s2, '_', '?' );
  string q = "./regex.sh " + s2 + " " + s1 + ";";
  cout << q << endl;
  system ( q.c_str() );
  switch ( operation )
    {
    case OpLike:
      res->b = ( std::ifstream ( "1" ) != NULL );
      break;
    case OpNotLike:
      res->b = ( std::ifstream ( "0" ) != NULL );
      break;
    default:
      throw new RTableException ( IllegOperation );
    }
  return res;
}
RTableCondition* RTableCondition::logic ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
  if ( operation != OpNot )
    {
      if ( ( left->nodeType != Value ) || ( right->nodeType != Value ) ) throw new RTableException ( IllegType );
      if ( left->retType != right->retType ) throw new RTableException ( NotSameTypes );
      if ( left->retType != TYPE_BOOL ) throw new RTableException ( NotBool );
      bool b1 = left->b, b2 = right->b;
      switch ( operation )
        {
        case OpAnd:
          return new RTableCondition ( TYPE_BOOL, 0, "", b1 && b2 );
        case OpOr:
          return new RTableCondition ( TYPE_BOOL, 0, "", b1 || b2 );
        default:
          throw new RTableException ( IllegOperation );
        }
    }
  else
    {
      if ( left->nodeType != Value ) throw new RTableException ( IllegType );
      if ( left->retType != TYPE_BOOL ) throw new RTableException ( NotBool );
      return new RTableCondition ( TYPE_BOOL, 0, "", !left->b );
    }
}
RTableCondition* RTableCondition::math ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
  if ( ( left->nodeType != Value ) || ( right->nodeType != Value ) ) throw new RTableException ( IllegType );
  if ( left->retType != right->retType ) throw new RTableException ( NotSameTypes );
  if ( left->retType != TYPE_LONG ) throw new RTableException ( NotLong );
  long l1 = left->l;
  long l2 = right->l;

  switch ( operation )
    {
    case OpAdd:
      return new RTableCondition ( TYPE_LONG, l1 + l2, "", false );
    case OpSub:
      return new RTableCondition ( TYPE_LONG, l1 - l2, "", false );
    case OpMult:
      return new RTableCondition ( TYPE_LONG, l1 * l2, "", false );
    case OpDiv:
      return new RTableCondition ( TYPE_LONG, l1 / l2, "", false );
    case OpMod:
      return new RTableCondition ( TYPE_LONG, l1 % l2, "", false );
    default:
      throw new RTableException ( IllegOperation );
    }
}

RTableCondition* RTableCondition::in ( OpCode operation, RTableCondition* left, RTableCondition* right )
{
  if ( ( left->nodeType != Value ) || ( right->nodeType != List ) ) throw new RTableException ( IllegType );
  if ( left->retType != right->retType ) throw new RTableException ( NotSameTypes );
  switch ( left->retType )
    {
    case TYPE_LONG:
    {
      long l = left->l;
      vector<long> arr = right->longList;
      bool include = false;
      for ( int i = 0; i < arr.size(); ++i )
        {
          cout << "Compare " << l << " with " << arr[i] << endl;
          if ( l == arr[i] )
            {
              include = true;
              break;
            }
        }
      cout << "Result: " << include << endl;
      if ( operation == OpIn ) return new RTableCondition ( TYPE_BOOL, 0, "", include );
      else return new RTableCondition ( TYPE_BOOL, 0, "", !include );
    }
    case TYPE_TEXT:
    {
      string s = left->s;
      vector<string> arr = right->strList;
      bool include = false;
      for ( int i = 0; i < arr.size(); ++i )
        if ( s == arr[i] )
          {
            include = true;
            break;
          }
      if ( operation == OpIn ) return new RTableCondition ( TYPE_BOOL, 0, "", include );
      else return new RTableCondition ( TYPE_BOOL, 0, "", !include );
    }
    }
}

RTableCondition* RTableCondition::runOp ( OpCode operation, RTableCondition* left, RTableCondition* right, RTableRecord rec )
{
  switch ( operation )
    {
    case OpLike:
    case OpNotLike:
      return like ( operation, left->_check ( rec ), right->_check ( rec ) );
    case OpIn:
    case OpNotIn:
      return in ( operation, left->_check ( rec ), right->_check ( rec ) );
    case OpAdd:
    case OpSub:
    case OpMult:
    case OpDiv:
    case OpMod:
      return math ( operation, left->_check ( rec ), right->_check ( rec ) );
    case OpAnd:
    case OpOr:
    case OpNot:
      return logic ( operation, left->_check ( rec ), right->_check ( rec ) );
    case OpEq:
    case OpNeq:
    case OpLess:
    case OpLessEq:
    case OpGreater:
    case OpGreaterEq: // <--- лесенка же!
      return comp ( operation, left->_check ( rec ), right->_check ( rec ) );
    }
  return NULL;
}

void RTableCondition::bind ( RFieldType type , string str, long lon, bool boo )
{
  nodeType = Value;
  retType = type;
  if ( type == TYPE_BOOL ) b = boo;
  else if ( type == TYPE_LONG ) l = lon;
  else s = str;
}
void RTableCondition::bind ( RTableRecord rec )
{
  // связываем текущую запись с текущим листом дерева, подставляем нужные значения нужных полей
  RCell *cell = rec[fieldName];
  bool b;
  long l;
  string s;
  RFieldType t = cell->getType();
  switch ( t )
    {
    case TYPE_BOOL:
      cell->getValue ( b );
      break;
    case TYPE_LONG:
      cell->getValue ( l );
      break;
    case TYPE_TEXT:
      cell->getValue ( s );
      break;
    }
  bind ( t,s,l,b );
}


RTableCondition* RTableCondition::_check ( RTableRecord rec )
{
  if ( all ) return new RTableCondition ( TYPE_BOOL, 0, "", true );
  switch ( nodeType )
    {
    case Operation:
    {
      if ( ( !left ) || ( !right ) ) throw new RTableException ( NullChilds );
      return runOp ( operation, left, right, rec );
    }
    case Value:
    {
      if ( fieldName != "" ) bind ( rec );
      return this;
    }
    case Field:
    {
      bind ( rec );
      return this;
    }
    case List:
    {
      return this;
    }
    }
  return NULL;
}
bool RTableCondition::check ( RTableRecord rec )
{
  RTableCondition * res = _check ( rec );
  if ( res->nodeType == Value )
    if ( res->retType == TYPE_BOOL )
      return res->b;
    else throw new RException ( "Не удалось проверить условие. Неожиданный тип возвращаемого значения" );
}

}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
