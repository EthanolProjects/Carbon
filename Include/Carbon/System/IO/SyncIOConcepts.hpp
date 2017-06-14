#pragma once
#include "Carbon/Config/Config.hpp"
namespace Carbon {
    using Byte = unsigned char;
    namespace System {
        namespace IO {
            // Stream Control
            struct IInputStream {
                virtual int peek(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int read(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int readSome(Byte* targetBuffer, long long readLengthByBytes) noexcept = 0;
            };

            struct IOutputStream {
                virtual int write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                virtual int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept = 0;
            };

            struct IIOStream : IInputStream, IOutputStream {};

            // Synchronized Random Access Control
            enum SRACBasePosition { SRACBegin, SRACCurrent, SRACEnd };

            struct ISynchronizedRandomAccessControl {
                using Position = unsigned long long;
                virtual Position tellp() const = 0;
                virtual Position setp(Position offset, SRACBasePosition base = SRACCurrent) = 0;
            };

        }
    }
}