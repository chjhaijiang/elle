//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/gate/Client.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [wed may 25 18:03:45 2011]
//

#ifndef ELLE_TEST_NETWORK_GATE_CLIENT_HH
#define ELLE_TEST_NETWORK_GATE_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/gate/Manifest.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Client
    {
    public:
      //
      // constructors & destructors
      //
      Client();

      //
      // methods
      //
      Status		Setup(const String&);
      Status		Run();

      //
      // callbacks
      //
      Status		Challenge(const String&);

      //
      // attributes
      //
      Address		address;
      Gate		gate;
    };

  }
}

#endif
