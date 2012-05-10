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


#ifndef RPARSER_H
#define RPARSER_H
#include "RDbCommand.h"
#include "RLexer.h"
#include "RTableCondition.h"
namespace RSQL
{
class RParser
{
    RLexer lexer;
    RParser ( string q ) : lexer ( q ) {}
    RDbCommand * parse();
    struct commandFullCollection // скачать бесплатно без смс без регистрации
    {
        RTableCondition * cond;
        vector<string> fieldsList;
        string tableName;
        vector <RCell*> valuesList;
        vector<RTableAssignment> assignmentList;
        RTableDefinition declarationList;
        RTableRecord rec;
    } collection;
public:
    /**
     * Возвращает полностью построенную команду во внутреннем представлении
     */
    static RDbCommand * Parse ( string q )
    {
        RParser p ( q );
        return p.parse();
    }
private:

// #region рекурсивный спуск
    RDbSelectCommand* _select();
    RDbInsertCommand* _insert();
    RDbUpdateCommand* _update();
    RDbDeleteCommand* _delete();
    RDbCreateCommand* _create();
    RDbDropCommand* _drop();
    RDbTruncateCommand* _truncate();

    RTableCondition* _where();
    /**
     * Если withComma == false, то запятой будет считаться разделителем.
     * Используется в UPDATE (в списке присваиваний)
     */
    RTableCondition* expr ( bool withComma = true );

    vector<string> fieldsList();
    vector <RCell*> valuesList();
    vector<RTableAssignment> assignmentList();
    RTableDefinition declarationList();

    string tableName();
    string fieldName();
    RCell* fieldValue();
    RFieldType fieldType();
    RTableAssignment assignment();
    pair<string, RFieldType> declaration();
};
}
#endif // RPARSER_H
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
