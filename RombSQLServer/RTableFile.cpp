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
  _RTable t;
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
    for ( int i = 0; i < def.size(); ++i )
    {
      RTableRecord rec;
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
      data.push_back ( rec );
    }
  }
  t.first = def;
  t.second = data;
  return t;
}


RTableFile::RTableFile ( string name )
{
  if ( !FileExists ( name.c_str() ) ) throw RFileException ( NotExists );
  this->name = name;
  current = 0;
  openedTables[name].first = load();
  openedTables[name].second += 1;
  cout << "Users of this table: " << openedTables[name].second << endl;
}

RTableFile::~RTableFile()
{
  close ( false );
}

void RTableFile::close ( bool save )
{
  if ( save )
    this->save();
  //of.write(wr.write(convertToJson(openedTables[name].first)).c_str(), 1);
  if ( --openedTables[name].second == 0 ) openedTables.erase ( name );
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
  f << def.size() << endl;
  RTableDefinition::iterator it = def.begin();
  vector<string> names;
  while ( it != def.end() )
  {
    names.push_back ( it->first );
    f << it->first;
    f << ( it->second == TYPE_BOOL ) ?"bool": ( ( it->second == TYPE_LONG ) ?"long":"text" );
    it++;
  }
  // проход по всем записям
  for ( int i = 0; i < data.size(); ++i )
  {
    // по каждому полю
    for ( int j = 0; j < names.size(); ++i )
    {
      switch ( def[names[j]] )
      {
        case TYPE_BOOL:
        {
          bool b;
          reinterpret_cast<RBoolCell*> ( data[i][names[j]] )->getValue ( b );
          f << b;
          break;
        }
        case TYPE_LONG:
        {
          long l;
          data[i][names[j]]->getValue ( l );
          f << l;
          break;
        }
        case TYPE_TEXT:
        {
          string s;
          data[i][names[j]]->getValue ( s );
          f << s;
          break;
        }
      }
    }
  }
  f.close();
}


void RTableFile::create ( string name, RTableDefinition def )
{
  if (FileExists(name.c_str())) throw RFileException( FileExist );
  _RTable t; vector<RTableRecord> v;
  t.first = def; t.second = v;
  write(name, t);
}

void RTableFile::drop ( string name )
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

void RTableFile::delet ( string name )
{
  openedTables.erase(name);
  std::remove(name.c_str());
}

RTableFile RTableFile::open ( string name )
{
  return RTableFile(name);
}

RTableRecord RTableFile::readRecordAndGoToNext()
{
  vector<RTableRecord> v = openedTables[name].first.second;
  if (v.size() <= current) throw RFileException( OutOfRange );
  return v[current++];
}

void RTableFile::deleteCurrentRecord()
{
  vector<RTableRecord> v = openedTables[name].first.second;
  v.erase(v.begin() + current);
}

void RTableFile::updateCurrentRecordAndGoToNext ( RTableRecord rec )
{
  openedTables[name].first.second[current++] = rec;
}

}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
