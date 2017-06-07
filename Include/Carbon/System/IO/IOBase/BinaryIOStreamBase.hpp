#pragma once
#include "Carbon/Config/Config.hpp"
namespace Carbon {
    using Byte = unsigned char;

    class IBStream {
    public:
        virtual ~IBStream() {}
        virtual void put(Byte byte) = 0;
        virtual void write(const Byte* src, unsigned int size) = 0;
        virtual void flush() = 0;
    };

    class OBStream {
    public:
        virtual ~OBStream() {}
        virtual Byte get() const = 0;
        virtual void read(Byte* src, unsigned int size) = 0;
        virtual void flush() = 0;
    };

    class IOBStream : public IBStream, public OBStream {

    };
    
    CARBON_API IBStream& operator << (IBStream&, char);
    CARBON_API IBStream& operator << (IBStream&, short);
    CARBON_API IBStream& operator << (IBStream&, int);
    CARBON_API IBStream& operator << (IBStream&, long);
    CARBON_API IBStream& operator << (IBStream&, long long);
    CARBON_API IBStream& operator << (IBStream&, unsigned char);
    CARBON_API IBStream& operator << (IBStream&, unsigned short);
    CARBON_API IBStream& operator << (IBStream&, unsigned int);
    CARBON_API IBStream& operator << (IBStream&, unsigned long);
    CARBON_API IBStream& operator << (IBStream&, unsigned long long);
    CARBON_API IBStream& operator >> (IBStream&, char&);
    CARBON_API IBStream& operator >> (IBStream&, short&);
    CARBON_API IBStream& operator >> (IBStream&, int&);
    CARBON_API IBStream& operator >> (IBStream&, long&);
    CARBON_API IBStream& operator >> (IBStream&, long long&);
    CARBON_API IBStream& operator >> (IBStream&, unsigned char&);
    CARBON_API IBStream& operator >> (IBStream&, unsigned short&);
    CARBON_API IBStream& operator >> (IBStream&, unsigned int&);
    CARBON_API IBStream& operator >> (IBStream&, unsigned long&);
    CARBON_API IBStream& operator >> (IBStream&, unsigned long long&);

}