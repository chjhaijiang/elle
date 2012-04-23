#ifndef  ELLE_CONCURRENCY_EVENTSERIALIZER_HXX
# define ELLE_CONCURRENCY_EVENTSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/concurrency/Event.hh>

ELLE_SERIALIZE_SIMPLE(elle::concurrency::Event,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.identifier;

  // XXX
  if (Archive::mode == ArchiveMode::Output)
    value.XXX_OLD_Extract();
}


#endif
