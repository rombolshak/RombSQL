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
#include "RTableFile.h"
#include "RException.h"
#include <fstream>

namespace RSQL
{
map<string, pair <_RTable, int> > RTableFile::openedTables;
bool FileExists ( const char *fname )
{
  return std::ifstream ( fname ) != NULL;
}
_RTable& RTableFile::load()
{
  return load(name);
}

_RTable& RTableFile::load ( string name )
{
  _RTable *t = new _RTable();
  RTableDefinition def;
  vector<RTableRecord> data;
  ifstream f ( name.c_str() );
  
  vector<string> names;
  int defSize;
  f >> defSize;
  for ( int i = 0; i < defSize; ++i )
  {
    string name;
    f >> name;
    names.push_back ( name );
    string type;
    f >> type;
    RFieldType t = ( type=="long" ) ?TYPE_LONG: ( ( type=="text" ) ?TYPE_TEXT:TYPE_BOOL );
    def[name] = t;
  }
  while ( !f.eof() )
  {
    RTableRecord rec;
    for ( unsigned int i = 0; i < def.size(); ++i )
    {
      switch ( def[names[i]] )
      {
        case TYPE_LONG:
        {
          long l;
          f >> l;
          rec[names[i]] = new RLongCell ( l );
          break;
        }
        case TYPE_TEXT:
        {
          string s;
          f >> s;
          rec[names[i]] = new RTextCell ( s );
          break;
        }
        case TYPE_BOOL:
        {
          bool b;
          f >> b;
          rec[names[i]] = new RBoolCell ( b );
          break;
        }
      }
    }
    data.push_back ( rec );
  }
  t->first = def;
  t->second = data;
  return *t;
}


RTableFile::RTableFile ( string name )
{
  map<string, pair <_RTable, int> >::iterator it = openedTables.find(name);
  if ( !FileExists ( name.c_str() ) ) throw new RFileException ( NotExists );
  this->name = name;
  current = 0;
  if (it == openedTables.end()) 
    openedTables[name].first = load();
  openedTables[name].second += 1;
  cout << "Open file: " << openedTables[name].second << "users" << endl;
}

RTableFile::~RTableFile()
{
  close ( false );
  if ( --openedTables[name].second == 0 ) ;//openedTables.erase ( name );
  cout << openedTables[name].second << " users" << endl;
}

void RTableFile::close ( bool save )
{
  if (save) cout << "Close with saving" << endl;
  else cout << "Close without saving" << endl;
  if ( save )
    this->save();
  //of.write(wr.write(convertToJson(openedTables[name].first)).c_str(), 1);
}

void RTableFile::save()
{
  _RTable& t = openedTables[name].first;
  write(name, t);
}

void RTableFile::write ( string name, RSQL::_RTable& t )
{
  RTableDefinition& def = t.first;
  vector<RTableRecord>& data = t.second;
  ofstream f ( name.c_str(), ios::out | ios::trunc );
  f << def.size();
  RTableDefinition::iterator it = def.begin();
  vector<string> names;
  while ( it != def.end() )
  {
    names.push_back ( it->first );
    f << endl << it->first;
    f << endl << ((it->second == TYPE_BOOL)?"bool":((it->second == TYPE_LONG)?"long":"text"));
    it++;
  }
  // проход по всем записям
  for ( unsigned int i = 0; i < data.size(); ++i )
  {
    // по каждому полю
    for ( unsigned int j = 0; j < names.size(); ++j )
    {
      switch ( def[names[j]] )
      {
        case TYPE_BOOL:
        {
          bool b;
          data[i][names[j]]->getValue ( b );
          f << endl << b;
          break;
        }
        case TYPE_LONG:
        {
          long l;
          data[i][names[j]]->getValue ( l );
          f << endl << l;
          break;
        }
        case TYPE_TEXT:
        {
          string s;
          data[i][names[j]]->getValue ( s );
          f << endl << s;
          break;
        }
      }
    }
  }
  f.close();
}

void RTableFile::createRecord ( RTableRecord rec )
{
  openedTables[name].first.second.push_back(rec);
}

void RTableFile::create ( string name, RTableDefinition def )
{
  if (FileExists(name.c_str())) throw new RFileException( FileExist );
  _RTable t; vector<RTableRecord> v;
  t.first = def; t.second = v;
  write(name, t);
}

void RTableFile::truncate ( string name )
{
  map< string, pair <_RTable, int> >::iterator it = openedTables.find(name);
  if (it != openedTables.end()) 
  {
    _RTable& t = openedTables[name].first;
    t.second.clear();
    vector<RTableRecord> a;
    t.second = a;
    write(name, t);
  }
  else 
  {
    _RTable& t = load(name);
    t.second.clear();
    vector<RTableRecord> a;
    t.second = a;
    write(name, t);
  }
}

void RTableFile::drop ( string name )
{
  openedTables.erase(name);
  std::remove(name.c_str());
}

RTableFile RTableFile::open ( string name )
{
  return RTableFile(name);
}

RTableDefinition RTableFile::getDefinition()
{
  return openedTables[name].first.first;
}

RTableRecord RTableFile::readCurrentRecord()
{
  const vector<RTableRecord>& v = openedTables[name].first.second;
  if (v.size() <= current) throw new RFileException( OutOfRange );
  return v[current];
}

void RTableFile::deleteCurrentRecord()
{
  vector<RTableRecord>& v = openedTables[name].first.second;
  v.erase(v.begin() + current);
}

void RTableFile::updateCurrentRecord ( pair<string, RCell*> rec )
{
  openedTables[name].first.second[current][rec.first] = rec.second;
}
  
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
