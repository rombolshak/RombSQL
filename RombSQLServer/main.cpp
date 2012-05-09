#include "RServer.h"
#include "RException.h"
#include "RParser.h"
using namespace RSQL;

int main ( void )
{
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
