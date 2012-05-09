/*
    RombSQL Server
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


#ifndef RTABLEFILE_H
#define RTABLEFILE_H

#include <string>
#include <vector>
#include <map>

#include "RCell.h"

using namespace std;
namespace RSQL {
	typedef map<string, RFieldType> RTableDefinition;
	typedef map<string, RCell*> RTableRecord;
	typedef pair<RTableDefinition, vector<RTableRecord> > _RTable;
	
	class RTableFile 
	{
		string name;
		static map<string, pair <_RTable, int> > openedTables;
		int current;
		_RTable& load();
		static _RTable& load(string name);
		static void write ( string name, RSQL::_RTable& t );
		RTableFile(string name);
	public:
		RTableDefinition getDefinition();
		static void create(string name, RTableDefinition);
		static void drop(string name);
		static void truncate(string name);
		static RTableFile open(string name);
		void createRecord(RTableRecord);
		RTableRecord readCurrentRecord();
		void deleteCurrentRecord();
		void updateCurrentRecord(pair<string, RCell*>);
		void moveNext() {++current;}
		void movePrev() {--current;}
		void save();
		void close(bool save = true);
		~RTableFile();
	};
}

#endif // RTABLE_H
