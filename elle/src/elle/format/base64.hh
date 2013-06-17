#ifndef ELLE_FORMAT_BASE64_HH
# define ELLE_FORMAT_BASE64_HH

# include <elle/types.hh>
# include <elle/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace base64
    {
      class StreamBuffer:
        public elle::StreamBuffer,
        public elle::Printable
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        StreamBuffer(std::iostream& stream);

      /*-------------------.
      | elle::StreamBuffer |
      `-------------------*/
      public:
        virtual
        WeakBuffer
        read_buffer();
        virtual
        WeakBuffer
        write_buffer();
        virtual
        void
        flush(Size size);
        void
        finalize();
      private:
        friend class Stream;
        std::iostream& _stream;
        int _remaining_write;
        char _buffer_write[1 << 12];
        int _remaining_read;
        char _buffer_read[1 << 12];

      /*----------.
      | Printable |
      `----------*/
      public:
        virtual
        void
        print(std::ostream& output) const;
      };

      class Stream:
        public elle::IOStream
      {
      public:
        Stream(std::iostream& underlying);
        ~Stream();

      private:
        friend class StreamBuffer;
        ELLE_ATTRIBUTE(StreamBuffer*, buffer);
        ELLE_ATTRIBUTE_R(std::iostream&, underlying);
      };

      /// Encode to base64.
      Buffer
      encode(WeakBuffer clear);

      /// The size of the encoded input.
      size_t
      encoded_size(WeakBuffer input);

      /// Decode from base64.
      Buffer
      decode(WeakBuffer input);

      /// The size of the decoded input.
      size_t
      decoded_size(WeakBuffer input);
    }
  }
}

#endif
