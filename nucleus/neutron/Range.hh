#ifndef NUCLEUS_NEUTRON_RANGE_HH
# define NUCLEUS_NEUTRON_RANGE_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <nucleus/neutron/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a set of something.
    ///
    /// a range must be parameterised with a type providing two things:
    ///  1) a type T::S which defines the key type used to differenciate
    ///     items.
    ///  2) a symbol() method which must returns a T::S&, this method
    ///     being used to retrieve the key value of a given item.
    ///
    template <typename T>
    class Range:
      public elle::Printable
    {
    public:
      //
      // constants
      //
      static const T*                   Trash;

      //
      // types
      //
      typedef typename T::Symbol Symbol;

      // XXX[use shared_ptr instead]
      // XXX[should be a vector?]
      typedef std::list<T*> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      //
      // constructors & destructors
      //
      Range();
      Range(elle::Natural32 const size);
      Range(const Range<T>&);

      //
      // methods
      //
      elle::Status      Add(T*);
      /// XXX
      elle::Status
      Add(Range<T> const& other);
      elle::Boolean     Exist(const Symbol&) const;
      elle::Boolean     Lookup(const Symbol&,
                               T const*& = Trash) const;
      elle::Boolean     Lookup(const Symbol&,
                               T*& = Trash) const;
      elle::Status      Remove(const Symbol&);
      elle::Status      Capacity(Size&) const;
      elle::Boolean     Locate(const Symbol&,
                               Scoutor&) const;
      elle::Boolean     Locate(const Symbol&,
                               Iterator&);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Range<T> const& other) const;
      // XXX ELLE_OPERATOR_NEQ_T1(Range<T>);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

#include <nucleus/neutron/Range.hxx>

#endif
