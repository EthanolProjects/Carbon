#pragma once
#include "Carbon/Config/Config.hpp"
#include <cstdint>
namespace Carbon {
    using Byte = unsigned char;
    namespace System {
        namespace IO {
            // Stream Control
            struct IInputStream {
                virtual int64_t peek(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int64_t read(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int64_t readSome(Byte* targetBuffer, long long readLengthByBytes) = 0;
            };

            struct IOutputStream {
                virtual int64_t write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                virtual int64_t writeSome(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
            };

            struct IIOStream : IInputStream, IOutputStream {};

            // Synchronized Random Access Control
            enum SRACBasePosition { SRACBegin, SRACCurrent, SRACEnd };

            struct ISynchronizedRandomStreamAccess {
                virtual uint64_t tellp() const = 0;
                virtual uint64_t setp(int64_t offset, SRACBasePosition base = SRACCurrent) = 0;
            };

        }
    }
}