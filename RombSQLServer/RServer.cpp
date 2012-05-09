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


#include "RServer.h"
#include "RException.h"
#include "RDbCommand.h"
#include "RParser.h"
/* Порт на котором сервер будет принимать соединения */
#define LPORT 4567
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
// да, дохера (sorry) включений, но мне крайне лень разбираться, что из этого уже не нужно

namespace RSQL
{
void getNames ( RTableDefinition d, vector<string>& names )
{
  map<string, RFieldType>::iterator it = d.begin();
  while ( it != d.end() )
    {
      names.push_back ( it->first );
      it++;
    }
}
int intSize ( long l )
{
  int res = 1;
  while ( l != 0 )
    {
      l = l / 10;
      if ( l != 0 ) res++;
    }
  return res;
}
void getValues ( vector<RTableRecord>& v, vector < vector< string > >& values )
{
  for ( unsigned int i = 0; i < v.size(); ++i )
    {
      vector<string> tmp;
      map<string, RCell*>::iterator it = v[i].begin();
      while ( it != v[i].end() )
        {
          long l;
          switch ( it->second->getType() )
            {
            case TYPE_LONG:
            {
              stringstream s;
              it->second->getValue ( l );
              s << l;
              l = s.str().size();
              tmp.push_back ( s.str() );
              break;
            }
            case TYPE_TEXT:
            {
              string s;
              it->second->getValue ( s );
              l = s.size();
              tmp.push_back ( s );
              break;
            }
            default:
              l = 0;
            }

          it++;
        }
      values.push_back ( tmp );
    }
}

/**
 * Мы же хотим выводить красивую табличку на экран? А для этого нам нужно знать максимальные размеры элементов
 * Записывает в массив максимальную ширину каждого столбца
 */
void getSizes ( vector<int>& res, const vector< vector<string> >& vals )
{
  for ( unsigned int i = 0; i < vals[0].size(); ++i )
    {
      unsigned int max = 0;
      for ( unsigned int j = 0; j < vals.size(); ++j )
        if ( vals[j][i].size() > max )
          max = vals[j][i].size();
      res.push_back ( max );
    }
}

/**
 * Красивую табличку нужно красиво выводить
 */
string out ( _RTable t )
{
  stringstream ss;
  vector<string> names;
  vector< vector<string> > values;
  getNames ( t.first, names );
  values.push_back ( names );
  getValues ( t.second, values );

  vector<int> sizes;
  getSizes ( sizes, values );

  unsigned int totalW;
  for ( unsigned int i = 0; i < sizes.size(); ++i )
    {
      totalW += ( 3 + sizes[i] );
    }
  totalW += 1;
  for ( unsigned int i = 0; i < totalW; ++i ) ss << "*";
  ss << endl;
// 		for (unsigned int j = 0; j < values[0].size(); ++j)
// 			ss << "* " << setw(sizes[j]) << right << "" << " ";
// 		ss << "*" << endl;
  for ( unsigned int j = 0; j < values[0].size(); ++j )
    ss << "* " << setw ( sizes[j] ) << left << values[0][j] << " ";
  ss << "*" << endl;
  for ( unsigned int i = 0; i < totalW; ++i ) ss << "*";
  ss << endl;
  for ( unsigned int i = 1; i < values.size(); ++i )
    {
      for ( unsigned int j = 0; j < values[0].size(); ++j )
        ss << "* " << setw ( sizes[j] ) << right << values[i][j] << " ";
      ss << "*" << endl;
    }
// 		for (unsigned int j = 0; j < values[0].size(); ++j)
// 			ss << "* " << setw(sizes[j]) << right << "" << " ";
// 		ss << "*" << endl;
  for ( unsigned int i = 0; i < totalW; ++i ) ss << "*";
  ss << endl;
  return ss.str();
}

void client_thread ( int * sock )
{
  send ( *sock, "Welcome!\n", 9, 0 );
  char *buf = new char[32768];
  while ( true )
    {
// 			vector<string> commands;
// 			commands.push_back("SELECT * FROM S;");
// 			commands.push_back("DELETE FROM S WHERE age=4;");
// 			commands.push_back("SELECT * FROM S;");
// 			commands.push_back("UPDATE S SET id=2 WHERE age=13;");
// 			commands.push_back("SELECT * FROM S;");
// 			for (int i = 0; i < commands.size(); ++i)
// 			{
      try
        {
          //string query = commands[i];
          recv ( *sock, buf, 32767, 0 );
          string query ( buf );
          cout << query << endl;
          RDbResult res = RParser::Parse ( query )->execute();
          if ( res.status == SUCCESS )
            {
              // т.к. мы не в курсе, исполняли ли мы select или что, пытаемся показать табличку, если что, нам кинется 403 исключение
              try
                {
                  string s = out ( res.result() );
                  send ( *sock, s.c_str(), strlen ( s.c_str() ), 0 );
                }
              catch ( int i )
                {
                  if ( i == 403 )
                    {
                      string ok = "OK\n";
                      send ( *sock, ok.c_str(), strlen ( ok.c_str() ), 0 );
                    }
                }
            }
          else
            {
              send ( *sock, res.errMessage().c_str(), strlen ( res.errMessage().c_str() ), 0 );
            }

        }
      catch ( RException* ex )
        {
          send ( *sock, ex->message().c_str(), strlen ( ex->message().c_str() ), 0 );
        }
      bzero ( buf, 32768 );
      //}
      //break;
    }
  string ok = "Bye-bye\n";
  write ( *sock, ok.c_str(), strlen ( ok.c_str() ) );
  close ( *sock );
  delete sock;
}
void RServer::Start()
{
  int lsock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  if ( lsock <= 0 ) throw new RServerException ( SocketFail );

  struct sockaddr_in addr;
  addr.sin_port        = htons ( LPORT );
  addr.sin_addr.s_addr = htonl ( INADDR_ANY );
  addr.sin_family      = AF_INET;


  int stat;
  stat = bind ( lsock, ( struct sockaddr* ) &addr, sizeof ( addr ) );
  if ( stat != 0 ) throw new RServerException ( BindFail );

  stat = listen ( lsock, 3 );
  if ( stat != 0 ) throw new RServerException ( ListenFail );


  int tmp_sock = 0;
  pthread_t tid;

  do
    {
      tmp_sock = accept ( lsock, NULL, NULL );
      if ( tmp_sock > 0 )
        {
          int * client_socket = new int ( tmp_sock );
          pthread_create ( &tid, NULL, ( void * ( * ) ( void* ) ) &client_thread, ( void* ) client_socket );
        }
      sleep ( 1 );
    }
  while ( true );
}

}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
