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


#include "RParser.h"
#include "RLexer.h"
#include <sstream>
#include <queue>
namespace RSQL
{
ostream& operator << ( ostream& stream, const RLexemValue& val )
{
  if ( val.type==str ) stream << ( val.s );
  else if ( val.type == lng ) stream << val.l;
  else throw new RParserException ( "Обращение к неинициализированной лексеме\n" );
  return stream;
}
RDbCommand* RParser::parse ()
{
  // надеюсь, все в курсе, что мы используем рекурсивный спуск?
  // Грамматика где то у меня на листочке. Сможете её восстановить по коду — молодцы!)
  RLexem lex = lexer.getLexem();
  switch ( lex.type )
    {
    case RLEX_SELECT:
      return _select();
    case RLEX_INSERT:
      return _insert();
    case RLEX_UPDATE:
      return _update();
    case RLEX_DELETE:
      return _delete();
    case RLEX_CREATE:
      return _create();
    case RLEX_DROP:
      return _drop();
    case RLEX_TRUNCATE:
      return _truncate();
    default:
      stringstream s;
      s << "Неожиданное начало команды (" << lex.val << ")\n";
      throw new RParserException ( s.str() );
    }
}

vector< string > RParser::fieldsList()
{
  vector<string> res;
  RLexem lex = lexer.getLexem();
  if ( lex.type == RLEX_STAR ) return res; // звездочка означает, что мы хотим все поля. Поэтому вернем пустой массив. Же(лезн|нск)ая логика :)
  lexer.putLexem ( lex ); // что бы там ни было, сейчас нам не нужно
  
  // небольшое замечание. Я не строго следую своей же грамматике, а реализую, как рекомендует POSIX, а именно
  // выбираю наиболее длинную цепочку, подходящую текущему правилу, за счет чего экономлю на одном состоянии
  // и делаю код куда более ясным для понимания. Enjoy
  do
    {
      string name = fieldName();
      res.push_back ( name );
      lex = lexer.getLexem();
    }
  while ( lex.type == RLEX_COMMA );
  lexer.putLexem ( lex );
  return res;
}

string RParser::fieldName()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_IDENT )
    {
      stringstream s;
      s << "Неожиданная лексема " << lex.val << ". Ожидалось имя поля\n";
      throw new RParserException ( s.str() );
    }
  return lex.val.s;
}

string RParser::tableName()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_IDENT )
    {
      stringstream s;
      s << "Неожиданная лексема " << lex.val << ". Ожидалось имя таблицы\n";
      throw new RParserException ( s.str() );
    }
  return lex.val.s;
}

std::vector< RCell* > RParser::valuesList()
{
  vector< RCell* > res;
  RLexem lex;
  do
    {
      RCell* val = fieldValue();
      res.push_back ( val );
      lex = lexer.getLexem();
    }
  while ( lex.type == RLEX_COMMA );
  lexer.putLexem ( lex );
  return res;
}

RCell* RParser::fieldValue()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type == RLEX_NUM ) return new RLongCell ( lex.val.l );
  if ( lex.type == RLEX_STRING ) return new RTextCell ( lex.val.s );
  stringstream s;
  s << "Ой, это что, какой-то новый тип значений? Я имею в виду это — " << lex.val << endl;
  throw new RParserException ( s.str() );
}

RDbSelectCommand* RParser::_select()
{
  collection.fieldsList = fieldsList();
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_FROM ) throw new RParserException ( "Не найдена лексема FROM\n" );
  collection.tableName = tableName();
  collection.cond = _where();
  return new RDbSelectCommand ( collection.tableName, collection.fieldsList, *collection.cond );
}

RDbInsertCommand* RParser::_insert()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_INTO ) throw new RParserException ( "Пичалько. Кажется, мы потеряли INTO.\n" );
  collection.tableName = tableName();
  lex = lexer.getLexem();
  if ( lex.type == RLEX_LBR )
    {
      collection.fieldsList = fieldsList();
      lex = lexer.getLexem();
      if ( lex.type != RLEX_RBR ) throw new RParserException ( "Хей, а закрывать скобку после списка полей кто будет?\n" );
      lex = lexer.getLexem();
    }
  if ( lex.type != RLEX_VALUES ) throw new RParserException ( "Не, ну я не знаю. Что вставлять, не знаю. VALUES не нашел\n" );
  lex = lexer.getLexem();
  if ( lex.type != RLEX_LBR ) throw new RParserException ( "А может, не стоит так торопиться? Мне было бы приятно, если бы Вы соблюдали правила моей грамматики и не игнорировали открывающую скобку после VALUES самым наглым способом. Спасибо.\n" );
  collection.valuesList = valuesList();
  lex = lexer.getLexem();
  if ( lex.type != RLEX_RBR ) throw new RParserException ( "Закрой скобку, извращенец!" );
  lex = lexer.getLexem();
  if ( lex.type != RLEX_END ) throw new RParserException ( "Слишком многого хотите…" );
  // постройка RTableRecord
  if ( collection.fieldsList.size() != 0 )
    for ( unsigned int i = 0; i < collection.fieldsList.size(); ++i )
      collection.rec[collection.fieldsList[i]] = collection.valuesList[i];
  else return new RDbInsertCommand ( collection.tableName, collection.valuesList );
  return new RDbInsertCommand ( collection.tableName, collection.rec );
}

RDbUpdateCommand* RParser::_update()
{
  RLexem lex;
  collection.tableName = tableName();
  lex = lexer.getLexem();
  if ( lex.type != RLEX_SET ) throw new RParserException ( "Главное — стабильность. Никаких SET! Даешь постоянство записей!" );
  collection.assignmentList = assignmentList();
  collection.cond = _where();
  return new RDbUpdateCommand ( collection.tableName, collection.assignmentList, *collection.cond );
}
vector< RTableAssignment > RParser::assignmentList()
{
  vector<RTableAssignment> res;
  RLexem lex;
  do
    {
      RTableAssignment a = assignment();
      res.push_back ( a );
      lex = lexer.getLexem();
    }
  while ( lex.type == RLEX_COMMA );
  lexer.putLexem ( lex );
  return res;
}
RTableAssignment RParser::assignment()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_IDENT )
    {
      stringstream s;
      s << "Ждал имя поля, ждал… Не дождался. Зато нашел вот это — " << lex.val << endl;
      throw new RParserException ( s.str() );
    }
  string name = lex.val.s;
  lex = lexer.getLexem();
  if ( lex.type != RLEX_EQ ) throw new RParserException ( "Если бы вы знали, как же хочется иногда приравнять чего-нибудь к чему-нибудь\n" );
  RTableCondition* val = expr ( false );
  return make_pair<string, RTableCondition*> ( name, val );
}


RDbDeleteCommand* RParser::_delete()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_FROM ) throw new RParserException ( "Не нашел FROM\n" );
  collection.tableName = tableName();
  collection.cond = _where();
  return new RDbDeleteCommand ( collection.tableName, *collection.cond );
}

RDbCreateCommand* RParser::_create()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_TABLE ) throw new RParserException ( "Быть может, Вы имели в виду CREATE LAMER <lamername>?\n" );
  collection.tableName = tableName();
  lex = lexer.getLexem();
  if ( lex.type != RLEX_LBR ) throw new RParserException ( "Позволю себе побыть занудой. С точки зрения формальности, в этой команде я ожидал увидеть описания полей после имени таблицы. А описание полей, как всем известно, заключаются в круглые скобки. Ну так и где они?!\n" );
  collection.declarationList = declarationList();
  lex = lexer.getLexem();
  if ( lex.type != RLEX_RBR ) throw new RParserException ( "Такое ощущение, что кое-кто в лифте родился. Скобки за собой кто закрывать будет?\n" );
  return new RDbCreateCommand ( collection.tableName, collection.declarationList );
}
RTableDefinition RParser::declarationList()
{
  RTableDefinition def;
  RLexem lex;
  do
    {
      pair<string, RFieldType> dec = declaration();
      def[dec.first] = dec.second;
      lex = lexer.getLexem();
    }
  while ( lex.type == RLEX_COMMA );
  lexer.putLexem ( lex );
  return def;
}
pair< string, RFieldType > RParser::declaration()
{
  string name = fieldName();
  RFieldType type = fieldType();
  return make_pair<string, RFieldType> ( name, type );
}
RFieldType RParser::fieldType()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type == RLEX_LONG ) return TYPE_LONG;
  if ( lex.type == RLEX_TEXT ) return TYPE_TEXT;
  stringstream s;
  s << "Не подскажете, что за тип у этой лексемы — " << lex.val << ". На long непохоже, на text тоже. #какжить\n";
  throw new RParserException ( s.str() );
}

RDbDropCommand* RParser::_drop()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_TABLE ) throw new RParserException ( "Быть может, Вы имели в виду DROP CHAIR <chair_name>?\n" );
  collection.tableName = tableName();
  return new RDbDropCommand ( collection.tableName );
}

RDbTruncateCommand* RParser::_truncate()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type != RLEX_TABLE ) throw new RParserException ( "Быть может, Вы имели в виду TRUNCATE MEMORY <memorize_name>?\n" );
  collection.tableName = tableName();
  return new RDbTruncateCommand ( collection.tableName );
}

RTableCondition* RParser::_where()
{
  RLexem lex = lexer.getLexem();
  if ( lex.type == RLEX_END ) return new RTableCondition();
  if ( lex.type != RLEX_WHERE )
    {
      stringstream s;
      s << "Неожиданная лексема " << lex.val << ". Ожидалось WHERE";
    }
  lex = lexer.getLexem();
  RTableCondition * c = new RTableCondition();
  if ( lex.type != RLEX_ALL )
    {
      delete c;
      lexer.putLexem ( lex );
      c = expr();
      lex = lexer.getLexem();
    }
  else return c;
  if ( lex.type != RLEX_END ) throw new RParserException ( "Мусор в конце\n" );
  return c;
}

/**
 * Эти типы лексем допустимы в <выражение>
 */
bool acceptable ( RLexem l )
{
  switch ( l.type )
    {
    case RLEX_OR:
    case RLEX_AND:
    case RLEX_NOT:
    case RLEX_EQ:
    case RLEX_NEQ:
    case RLEX_G:
    case RLEX_L:
    case RLEX_LEQ:
    case RLEX_GEQ:
    case RLEX_LBR:
    case RLEX_RBR:
    case RLEX_IDENT:
    case RLEX_STRING:
    case RLEX_NUM:
    case RLEX_PLUS:
    case RLEX_MINUS:
    case RLEX_STAR:
    case RLEX_SLASH:
    case RLEX_PERCENT:
    case RLEX_COMMA:
    case RLEX_LIKE:
    case RLEX_IN:
      return true;
    default:
      return false;
    }
}

/**
 * Приоритет операций. Чем выше, тем больше. И наоборот
 */
int opPriority ( RLexemType code )
{
  switch ( code )
    {
    case RLEX_OR:
      return 1;
    case RLEX_AND:
      return 2;
    case RLEX_NOT:
      return 3;
    case RLEX_EQ:
    case RLEX_NEQ:
    case RLEX_L:
    case RLEX_LEQ:
    case RLEX_G:
    case RLEX_GEQ:
      return 4;
    case RLEX_PLUS:
    case RLEX_MINUS:
      return 5;
    case RLEX_STAR:
    case RLEX_SLASH:
    case RLEX_PERCENT:
      return 6;
    }
  return 0;
}
bool isOperator ( RLexemType code )
{
  switch ( code )
    {
    case RLEX_OR:
    case RLEX_AND:
    case RLEX_NOT:
    case RLEX_EQ:
    case RLEX_NEQ:
    case RLEX_L:
    case RLEX_LEQ:
    case RLEX_G:
    case RLEX_GEQ:
    case RLEX_PLUS:
    case RLEX_MINUS:
    case RLEX_STAR:
    case RLEX_SLASH:
    case RLEX_PERCENT:
      return true;
    default:
      return false;
    }
}
bool opLeftAssoc ( RLexemType code )
{
  switch ( code )
    {
    case RLEX_OR:
    case RLEX_AND:
    case RLEX_PLUS:
    case RLEX_MINUS:
    case RLEX_STAR:
    case RLEX_SLASH:
    case RLEX_PERCENT:
      return true;
    case RLEX_NOT:
      return false;
    }
  // для остальных ассоциативность не определена
  return false;
}
OpCode lex2OpCode ( RLexemType type )
{
  switch ( type )
    {
    case RLEX_OR:
      return OpOr;
    case RLEX_AND:
      return OpAnd;
    case RLEX_NOT:
      return OpNot;
    case RLEX_EQ:
      return OpEq;
    case RLEX_NEQ:
      return OpNeq;
    case RLEX_L:
      return OpLess;
    case RLEX_LEQ:
      return OpLessEq;
    case RLEX_G:
      return OpGreater;
    case RLEX_GEQ:
      return OpGreaterEq;
    case RLEX_PLUS:
      return OpAdd;
    case RLEX_MINUS:
      return OpSub;
    case RLEX_STAR:
      return OpMult;
    case RLEX_SLASH:
      return OpDiv;
    case RLEX_PERCENT:
      return OpMod;
    }
}

/**
 * По ПОЛИЗу строим дерево условий (в частном случае дерево выражения)
 * Тот же самый алгоритм, что и вычисление ПОЛИЗа, только не для чисел, а для RTableCondition*
 */
RTableCondition * tree ( queue<RLexem> rpn )
{
  stack<RTableCondition*> st;
  while ( !rpn.empty() )
    {
      RLexem l = rpn.front();
      if ( l.type == RLEX_STRING ) st.push ( new RTableCondition ( TYPE_TEXT, 0, l.val.s, false ) );
      if ( l.type == RLEX_NUM ) st.push ( new RTableCondition ( TYPE_LONG, l.val.l, "", false ) );
      if ( l.type == RLEX_IDENT ) st.push ( new RTableCondition ( l.val.s ) );
      if ( isOperator ( l.type ) )
        {
          if ( l.type != RLEX_NOT )
            {
              if ( st.empty() ) throw new RParserException ( "Нет аргументов операции\n" );
              RTableCondition* right = st.top();
              st.pop();
              if ( st.empty() ) throw new RParserException ( "Не хватает аргументов операции\n" );
              RTableCondition* left = st.top();
              st.pop();
              RTableCondition* c = new RTableCondition ( lex2OpCode ( l.type ), left, right );
              st.push ( c );
            }
          else
            {
              if ( st.empty() ) throw new RParserException ( "Нет аргументов операции\n" );
              RTableCondition* left = st.top();
              st.pop();
              RTableCondition* c = new RTableCondition ( OpNot, left, new RTableCondition() );
              st.push ( c );
            }
        }
      if ( l.type == RLEX_LIKE )
        {
          if ( st.empty() ) throw new RParserException ( "Нет аргументов для LIKE\n" );
          RTableCondition* right = st.top();
          st.pop();
          if ( st.empty() ) throw new RParserException ( "Не хватает аргументов для LIKE\n" );
          RTableCondition* left = st.top();
          st.pop();
          RTableCondition* c = new RTableCondition ( OpLike, left, right );
          st.push ( c );
        }
      if ( l.type == RLEX_IN )
        {
          if ( st.empty() ) throw new RParserException ( "Нет аргументов операции IN\n" );
          RTableCondition *tmp = st.top();
          st.pop();
          if ( tmp->getType() == TYPE_LONG )
            {
              vector<long> arr;
              while ( !tmp->isField() )
                {
                  if ( st.empty() ) throw new RParserException ( "Какая-то ерунда с параметрами для IN. Какое поле проверять то?\n" );
                  long l;
                  RCell* c = tmp->getValue();
                  c->getValue ( l );
                  arr.push_back ( l );
                  tmp = st.top();
                  st.pop();
                }
              RTableCondition* right = new RTableCondition ( arr );
              RTableCondition*c = new RTableCondition ( OpIn, tmp, right );
              st.push ( c );
            }
          else if ( tmp->getType() == TYPE_TEXT )
            {
              vector<string> arr;
              while ( !tmp->isField() )
                {
                  if ( st.empty() ) throw new RParserException ( "Какая-то ерунда с параметрами для IN. Какое поле проверять то?\n" );
                  string s;
                  RCell* c = tmp->getValue();
                  c->getValue ( s );
                  arr.push_back ( s );
                  tmp = st.top();
                  st.pop();
                }
              RTableCondition* right = new RTableCondition ( arr );
              RTableCondition*c = new RTableCondition ( OpIn, tmp, right );
              st.push ( c );
            }
          else throw new RParserException ( "Недопустимый тип параметра операции IN\n" );
        }
      rpn.pop();
    }
  if ( st.empty() ) throw RParserException ( "Некорректное выражение\n" );
  return st.top();
}

/**
 * Самый настоящий ПОЛИЗ. За описанием алгоритма добро пожаловать в Википедию
 */
RTableCondition* RParser::expr ( bool withComma )
{
  stack<RLexem> st;
  queue<RLexem> rpn;
  RLexem lex = lexer.getLexem();
  while ( acceptable ( lex ) )
    {
      if ( ( lex.type == RLEX_COMMA ) && !withComma ) break;
      if ( ( lex.type == RLEX_NUM ) || ( lex.type == RLEX_IDENT ) || ( lex.type == RLEX_STRING ) )
        rpn.push ( lex );
      if ( ( lex.type == RLEX_LIKE ) || ( lex.type == RLEX_IN ) )
        st.push ( lex );
      if ( lex.type == RLEX_COMMA )
        {
          while ( !st.empty() && ( st.top().type != RLEX_LBR ) )
            {
              rpn.push ( st.top() );
              st.pop();
            }
          if ( st.empty() )
            throw new RParserException ( "Пропущена открывающая скобка:(\n" );
        }
      if ( isOperator ( lex.type ) )
        {
          while (
            ( !st.empty() && ( isOperator ( st.top().type ) ) ) &&
            (
              ( opLeftAssoc ( lex.type ) && ( opPriority ( lex.type ) <= opPriority ( st.top().type ) ) ) ||
              ( !opLeftAssoc ( lex.type ) && ( opPriority ( lex.type ) < opPriority ( st.top().type ) ) )
            )
          )
            {
              rpn.push ( st.top() );
              st.pop();
            }
          st.push ( lex );
        }
      if ( lex.type == RLEX_LBR ) st.push ( lex );
      if ( lex.type == RLEX_RBR )
        {
          while ( !st.empty() && ( st.top().type != RLEX_LBR ) )
            {
              rpn.push ( st.top() );
              st.pop();
            }
          if ( st.empty() ) throw new RParserException ( "Пропущена открывающая скобка:(\n" );
          st.pop();
          if ( !st.empty() && ( ( st.top().type == RLEX_IN ) || ( st.top().type == RLEX_LIKE ) ) )
            {
              rpn.push ( st.top() );
              st.pop();
            }
        }
      lex = lexer.getLexem();
    }
  lexer.putLexem ( lex );
  while ( !st.empty() )
    {
      if ( st.top().type == RLEX_LBR ) throw new RParserException ( "В выражении присутствует незакрытая скобка. Мне неуютно\n" );
      rpn.push ( st.top() );
      st.pop();
    }
  return tree ( rpn );
}

}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
