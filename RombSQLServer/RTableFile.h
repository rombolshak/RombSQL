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

#include "jsoncpp/include/reader.h"
#include "jsoncpp/include/writer.h"
#include "jsoncpp/include/value.h"

using namespace std;
using namespace Json;
namespace RSQL {
	typedef vector< map<string, RFieldType> > RTableDefinition;
	typedef map<string, RCell> RTableRecord;
}

#endif // RTABLE_H
