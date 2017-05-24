#pragma once
namespace Carbon {
    using Byte = unsigned char;

    class IBStream {
    public:
        virtual ~IBStream() {}
        virtual void put(Byte byte) = 0;
        virtual void write(const Byte* src, unsigned int size) = 0;
    };

    class OBStream {
    public:
        virtual ~OBStream() {}
        virtual Byte get() const = 0;
        virtual void read(Byte* src, unsigned int size) = 0;
    };

    template <class T>
    IBStream& operator << (IBStream& stream, const T& rhs) {
        stream.write(&rhs, sizeof(T));
        return stream;
    }

    template <class T>
    IBStream& operator >> (IBStream& stream, T& rhs) {
        stream.read(&rhs, sizeof(T));
        return stream;
    }

    class IOBStream : public IBStream, public OBStream {

    };
}