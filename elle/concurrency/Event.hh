#ifndef ELLE_CONCURRENCY_EVENT_HH
# define ELLE_CONCURRENCY_EVENT_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

#include <elle/idiom/Close.hh>
# include <openssl/err.h>
# include <reactor/signal.hh>
#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  // XXX
  namespace network
  {
    class Parcel;
  }

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is used to uniquely identify events, network packets and
    /// so on.
    ///
    class Event
      : public Object
    {
    public:
      //
      // constants
      //
      static const Event        Null;

      //
      // constructors & destructors
      //
      Event();

      //
      // methods
      //
      Status            Generate();
      void              Cleanup();

      //
      // interfaces
      //

      // object
      declare(Event);
      Boolean           operator==(const Event&) const;
      Boolean           operator<(const Event&) const;

      void            XXX_OLD_Extract();

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64 Identifier() const;

    public:
      reactor::VSignal<std::shared_ptr<elle::network::Parcel> >& Signal();
    private:
      Natural64                                 _identifier;
      reactor::VSignal<std::shared_ptr<elle::network::Parcel> >*  _signal;
    };

  }
}

#endif
