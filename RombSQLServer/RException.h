/*
    <one line to give the program's name and a brief idea of what it does.>
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
namespace RSQL {
	class RException {
	public:
		virtual string message();
	};

	enum FileError {
	};
class RFileException : public RException {
	FileError code;
public:
	RFileException(FileError fe) {code = fe;};
	string message();
};

enum CellError {
	IllegalType
};
class RCellException : public RException
{
	CellError code;
public:
	RCellException(CellError ce) {code = ce;};
	string message();
};
}
#endif // EXCEPTION_H
