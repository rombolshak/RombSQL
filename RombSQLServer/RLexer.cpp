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


#include "RLexer.h"
#include <cstdlib> // atol
#include <algorithm> // transform
namespace RSQL
{
vector < map < RLexer::Event, pair < int, pair < RLexemType, bool > > > > RLexer::transitionTable;

void RLexer::addTransition ( int from, RSQL::RLexer::Event e, int to, RSQL::RLexemType ret, bool putBack )
{
  transitionTable[from][e] = make_pair<int, pair < RLexemType, bool > > ( to, make_pair<RLexemType, bool> ( ret, putBack ) );
}

void RLexer::Init()
{
  map < Event, pair < int, pair < RLexemType, bool > > > m;
  for ( int i = 0; i < 7; ++i ) // состояния
    transitionTable.push_back ( m );
  addTransition ( 0, letter, 1 );
  addTransition ( 0, apostr, 2 );
  addTransition ( 0, comma, 0, RLEX_COMMA );
  addTransition ( 0, lbracket, 0, RLEX_LBR );
  addTransition ( 0, rbracket, 0, RLEX_RBR );
  addTransition ( 0, plus, 0, RLEX_PLUS );
  addTransition ( 0, minus, 0, RLEX_MINUS );
  addTransition ( 0, star, 0, RLEX_STAR );
  addTransition ( 0, slash, 0, RLEX_SLASH );
  addTransition ( 0, percent, 0, RLEX_PERCENT );
  addTransition ( 0, less, 3 );
  addTransition ( 0, greater, 4 );
  addTransition ( 0, equal, 0, RLEX_EQ );
  addTransition ( 0, att, 5 );
  addTransition ( 0, digit, 6 );
  addTransition ( 0, delim, 0 );
  addTransition ( 0, eot, 0, RLEX_END );
  addTransition ( 0, def, 0, RLEX_UNKNOWN );
  addTransition ( 1, letter, 1 );
  addTransition ( 1, digit, 1 );
  addTransition ( 1, def, 0, RLEX_WORD, true );
  addTransition ( 2, apostr, 0, RLEX_STRING );
  addTransition ( 2, def, 2 );
  addTransition ( 3, equal, 0, RLEX_LEQ );
  addTransition ( 3, def, 0, RLEX_L, true );
  addTransition ( 4, equal, 0, RLEX_GEQ );
  addTransition ( 4, def, 0, RLEX_G, true );
  addTransition ( 5, equal, 0, RLEX_NEQ );
  addTransition ( 5, def, 0, RLEX_UNKNOWN );
  addTransition ( 6, digit, 6 );
  addTransition ( 6, def, 0, RLEX_NUM, true );
}

RLexer::Event RLexer::getEvent ( char c )
{
  if ( ( isalpha ( c ) > 0 ) || ( c == '_' ) ) return letter;
  if ( isdigit ( c ) > 0 ) return digit;
  switch ( c )
    {
    case '\'':
      return apostr;
    case ',':
      return comma;
    case '(':
      return lbracket;
    case ')':
      return rbracket;
    case '+':
      return plus;
    case '-':
      return minus;
    case '*':
      return star;
    case '/':
      return slash;
    case '%':
      return percent;
    case '<':
      return less;
    case '=':
      return equal;
    case '>':
      return greater;
    case '!':
      return att;
    case ';':
    case '\0':
      return eot;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      return delim;
    default:
      return def;
    }
}

RLexem RLexer::performTransition ( Event e )
{
  if ( transitionTable[currentState][e].second.first == RLEX_ALL ) e = def; // если нужного события нет в таблице, идем по умолчанию
  RLexem lex;
  if ( !transitionTable[currentState][e].second.second ) currentPos++; // если не стоит флаг putBack, идти дальше по строке
  else lexValue.erase ( lexValue.size()-1 ); 
  lex.type = transitionTable[currentState][e].second.first;
  currentState = transitionTable[currentState][e].first;
  return lex;
}

#define MATCH(pattern,result) if (word == pattern) return result;
RLexemType lookup ( string word )
{
  int ( *pf ) ( int ) = tolower;
  transform ( word.begin(), word.end(), word.begin(), pf );
  // Упаси тебя Ктулху понаставить пробелов где-либо ниже до return!
  MATCH("all",RLEX_ALL);
  MATCH("and",RLEX_AND);
  MATCH("create",RLEX_CREATE);
  MATCH("delete",RLEX_DELETE);
  MATCH("drop",RLEX_DROP);
  MATCH("from",RLEX_FROM);
  MATCH("insert",RLEX_INSERT);
  MATCH("into",RLEX_INTO);
  MATCH("in",RLEX_IN);
  MATCH("like",RLEX_LIKE);
  MATCH("long",RLEX_LONG);
  MATCH("not",RLEX_NOT);
  MATCH("or",RLEX_OR);
  MATCH("select",RLEX_SELECT);
  MATCH("set",RLEX_SET);
  MATCH("table",RLEX_TABLE);
  MATCH("text",RLEX_TEXT);
  MATCH("truncate",RLEX_TRUNCATE);
  MATCH("update",RLEX_UPDATE);
  MATCH("values",RLEX_VALUES);
  MATCH("where",RLEX_WHERE);
  return RLEX_IDENT;
}

string trim ( string& lexValue )
{
  lexValue = lexValue.erase ( lexValue.find_last_not_of ( ' ' ) + 1 );
  while ( lexValue[0] == ' ' ) lexValue = lexValue.erase ( 0, 1 );
  if ( ( lexValue[0] == '\'' ) && ( lexValue[lexValue.size()-1] == '\'' ) )
    {
      lexValue.erase ( 0,1 );
      lexValue.erase ( lexValue.size()-1 );
    }
  return lexValue;
}

RLexem RLexer::getLexem()
{
  if ( !_stack.empty() )
    {
      RLexem lex = _stack.top();
      _stack.pop();
      return lex;
    }
  RLexem lex;
  if ( currentPos == s.size() )
    {
      lex.type = RLEX_END;
      return lex;
    }
  
  do
    {
      char c = s[currentPos];
      lexValue += c;
      lex = performTransition ( getEvent ( c ) );
      if ( lex.type == RLEX_UNKNOWN ) throw new RLexerException ( s, currentPos );

    }
  while ( lex.type == RLEX_NULL );
  
  switch ( lex.type )
    {
    case RLEX_NUM:
      lex.val = RLexemValue ( atol ( lexValue.c_str() ) );
      break;
    case RLEX_WORD:
      trim ( lexValue );
      lex.type = lookup ( lexValue );
      lex.val = RLexemValue ( lexValue );
      break;
    default:
      lex.val = RLexemValue ( trim ( lexValue ) );
    }
  lexValue = "";
  return lex;
}

}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
