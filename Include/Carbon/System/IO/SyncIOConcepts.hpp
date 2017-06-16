#pragma once
#include "Carbon/Config/Config.hpp"
#include <cstdint>
namespace Carbon {
    namespace System {
        namespace IO {
            // Agents
            namespace StreamAgent {
                class Agent {};

                struct CARBON_API IBasics {
                protected:
                    virtual uint64_t ngets(void*, uint64_t);
                    virtual uint64_t nputs(const void*, uint64_t);
                    virtual uint64_t tgets(void*, uint64_t) noexcept;
                    virtual uint64_t tputs(const void*, uint64_t) noexcept;

                };

                class CARBON_API IRandomAccessable {
                protected:
                    virtual uint64_t tellp() const;
                    virtual uint64_t setp(int64_t offset, int base);
                };

                class CARBON_API IBufferable {

                };
            }
            
            // Stream Control
            struct IInputStream {
                virtual int64_t peek(void* targetBuffer, int64_t readLengthByBytes) = 0;
                virtual int64_t read(void* targetBuffer, int64_t readLengthByBytes) = 0;
                virtual int64_t readSome(void* targetBuffer, int64_t readLengthByBytes) = 0;
            };

            struct IOutputStream {
                virtual int64_t write(const void* targetBuffer, int64_t writeLengthByBytes) = 0;
                virtual int64_t writeSome(const void* targetBuffer, int64_t writeLengthByBytes) = 0;
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