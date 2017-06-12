#pragma once 
#include "BinaryIOStreamBase.hpp" 
namespace Carbon {
    namespace System {
        namespace IO {
            class CARBON_API SynchronizedFile : public IIOStream, public ISynchronizedRandomAccessControl {
            public:
                Position tellp() const override;
                Position setp(Position offset, SRACBasePosition base = SRACCurrent) override;
                Byte get() const override;
                void put(const Byte) override;
                void read(Byte* targetBuffer, long long readLengthByBytes) const override;
                void write(const Byte* targetBuffer, long long writeLengthByBytes) override;
                int readSome(Byte* targetBuffer, long long readLengthByBytes) const noexcept override;
                int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept override;
            private:
#ifdef CARBON_TARGET_WINDOWS
                void* mFile; // HANDLE mFile;
#endif
            };
        }
    }
}