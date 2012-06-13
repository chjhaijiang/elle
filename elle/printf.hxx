#ifndef  ELLE_PRINTF_HXX
# define ELLE_PRINTF_HXX

# include <cstddef> // size_t
# include <iostream>
# include <sstream>
# include <typeinfo>
# include <type_traits> // remove_cv, remove_reference

# include <boost/format.hpp>

# include <elle/printf.hh>

namespace elle
{
  namespace detail
  {


    template<typename _OStream, typename _T> struct IsPrintable
    {
    private:
      template<typename K>
        struct Clean
        {
          typedef typename std::remove_cv<
            typename std::remove_reference<K>::type
          >::type type;
        };
      typedef typename Clean<_OStream>::type  OStream;
      typedef typename Clean<_T>::type        T;
      typedef char No;
      typedef struct { No _[2];} Yes;
      static No f(...);
      template<size_t> struct Helper {};
      template<typename U>
        static Yes f(U*,  Helper<sizeof(*((OStream*) 0) << *((U*)0))>* sfinae = 0);
    public:
      static bool const value = (sizeof f((T*)0) == sizeof(Yes));
    };

    // Select the right feeder.
    // Feeder for "printable" objects.
    template<bool __b> struct FeedItSwissMotherFuckingCheese
    {
      template<size_t> struct Helper{};

      // Default overload :
      // - the dummy int parameter make call unambiguous
      // - the last parameter uses sfinae to fall into the fallback method
      //   when boost::format just don't work (nucleus::proton::Version
      //   with boost 1.49 in c++0x).
      template<typename T>
      static void feed_it(boost::format& fmt, T const& value, int,
          Helper<sizeof( static_cast<boost::format&>(*((boost::format* const)0)) % static_cast<T const&>(*((T const* const)0)) )>* = 0)
      {
        fmt % value;
      }

      // This is the fallback method using a classic stringstream, and
      // format a plain string instead of the value.
      template<typename T>
      static void feed_it(boost::format& fmt, T const& value, unsigned int)
      {
        std::stringstream ss;
        ss << value;
        fmt % ss.str();
      }
    };

    // Feeder for "non printable" objects.
    template<> struct FeedItSwissMotherFuckingCheese<false>
    {
      template<typename T>
      static void feed_it(boost::format& fmt, T const& value, int)
      {
        std::stringstream ss;
        ss << '<'<< typeid(T).name() << "instance at 0x"
           << std::hex << static_cast<void const*>(&value) << '>';
        fmt % ss.str();
      }
    };

    inline void
    format_feed(boost::format&)
    {}

    // feed printable values
    template<typename T, typename... K>
    void
    format_feed(boost::format& fmt, T const& front, K const&... tail)
    {
      FeedItSwissMotherFuckingCheese<
          IsPrintable<std::ostream, T>::value
      >::feed_it(fmt, front, 42);
      format_feed(fmt, tail...);
    }


    // XXX virer les const& et utiliser les bon traits
    template<typename... T>
    std::string
    format(char const* fmt, T const&... values)
    {
      try
        {
          boost::format boost_fmt{fmt};
          format_feed(boost_fmt, values...);
          return boost_fmt.str();
        }
      catch (std::exception const& e)
        {
          // FIXME
          std::cerr << e.what() << ": " << fmt << std::endl;
          std::abort();
        }
    }
  }


  template<typename... T>
    size_t fprintf(std::ostream& out, char const* fmt, T const&... values)
    {
      std::string res = elle::detail::format(fmt, values...);
      out << res;
      return res.size();
    }

  template<typename... T>
    size_t printf(char const* fmt, T const&... values)
    {
      return fprintf(std::cout, fmt, values...);
    }

  template<typename... T>
    std::string sprintf(char const* fmt, T const&... values)
    {
      return elle::detail::format(fmt, values...);
    }

}

#endif
