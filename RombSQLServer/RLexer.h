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


#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "RException.h"
using namespace std;

namespace RSQL
{
enum RLexemType
{
    RLEX_ALL,
    RLEX_AND,
    RLEX_COMMA,
    RLEX_CREATE,
    RLEX_DELETE,
    RLEX_DROP,
    RLEX_END,
    RLEX_EQ, // =
    RLEX_FROM,
    RLEX_G, // >
    RLEX_GEQ, // >=
    RLEX_IDENT,
    RLEX_IN,
    RLEX_INSERT,
    RLEX_INTO,
    RLEX_LBR, // (
    RLEX_L, // <
    RLEX_LEQ, // <=
    RLEX_LIKE,
    RLEX_LONG,
    RLEX_MINUS, // -
    RLEX_NEQ, // !=
    RLEX_NOT,
    RLEX_NULL,
    RLEX_NUM,
    RLEX_OR,
    RLEX_PERCENT, // %
    RLEX_PLUS, // +
    RLEX_RBR, // )
    RLEX_SELECT,
    RLEX_SET,
    RLEX_SLASH, // /
    RLEX_STAR, // *
    RLEX_STRING,
    RLEX_TABLE,
    RLEX_TEXT,
    RLEX_TRUNCATE,
    RLEX_UNKNOWN,
    RLEX_UPDATE,
    RLEX_VALUES,
    RLEX_WORD,
    RLEX_WHERE
};

typedef enum {str, lng, err} valType;
struct RLexemValue
{
    long l;
    string s;
    valType type;
    RLexemValue()
    {
        l=0;
        s="";
        type=err;
    }
    RLexemValue ( long x )
    {
        type=lng;
        l = x;
    }
    RLexemValue ( string x )
    {
        type=str;
        s = x;
    }
};

class RLexem
{
public:
    RLexemType type;
    RLexemValue val;
    RLexem ( RLexemType t )
    {
        type=t;
    }
    RLexem ( RLexemType t, RLexemValue v )
    {
        type=t;
        val=v;
    }
    RLexem()
    {
        type=RLEX_NULL;
    }
};


class RLexer
{
    enum Event
    {
        letter, digit, apostr, comma, lbracket, rbracket, plus, minus, star, slash, percent, less, equal, greater, att, def, eot, delim
    };
    string s; // запрос
    int currentState;
    int currentPos;
    string lexValue;
    static vector < map < Event, pair < int, pair < RLexemType, bool > > > > transitionTable; // шедевр, не правда ли?
    /**
     * Добавляет состояния автомату. При изменении лексики ковырять здесь
     */
    static void addTransition ( int from, Event e, int to, RLexemType ret = RLEX_NULL, bool putBack = false );
    /**
     * По текущему символу выдать событие, ему соответствующее
     */
    Event getEvent ( char );
    /**
     * Выполнить переход по таблице состояний
     */
    RLexem performTransition ( Event e );

    /**
     * Пригодится, когда необходимо вернуть лексему во входной поток
     */
    stack<RLexem> _stack;
public:
    RLexer ( string str )
    {
        s=str;
        currentState=currentPos=0;
        lexValue="";
    }
    /**
     * Получить следующую лексему
     */
    RLexem getLexem();
    /**
     * Вернуть лексему обратно. 
     * Она будет выдана при следующем вызове getLexem, продвижения по строке в таком случае не будет
     */
    void putLexem ( RLexem l )
    {
        _stack.push ( l );
    }
    /**
     * Инициализация таблицы. ОБЯЗАТЕЛЬНО вызвать перед использованием любых других методов этого класса.
     */
    static void Init();
};
}
#endif // LEXER_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
