#include "RServer.h"
#include "RException.h"
#include "RParser.h"
using namespace RSQL;

int main ( void )
{
  cout << "RombSQL Copyright © 2012 Большаков Роман <rombolshak@gmail.com> " << endl << 
  "This program comes with ABSOLUTELY NO WARRANTY" << endl <<
  "This is free software, and you are welcome to redistribute it under certain conditions; see source for details." << endl;
  RLexer::Init();
  try
    {
      RServer::Start();
    }
  catch ( RException* e )
    {
      cout << e->message();
    }
  return 0;
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 
