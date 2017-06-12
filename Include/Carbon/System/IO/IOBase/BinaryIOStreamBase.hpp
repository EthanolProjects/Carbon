#pragma once
#include "Carbon/Config/Config.hpp"
namespace Carbon {
    using Byte = unsigned char;
    namespace System {
        namespace IO {
            // Stream Control
            struct StreamBase {
                virtual ~StreamBase() {};
            };

            struct IInputStream : virtual StreamBase {
                virtual Byte get() const = 0;
                virtual void read(Byte* targetBuffer, long long readLengthByBytes) const = 0;
                virtual int readSome(Byte* targetBuffer, long long readLengthByBytes) const noexcept = 0;
            };

            struct IOutputStream : virtual StreamBase {
                virtual void put(const Byte) = 0;
                virtual void write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                virtual int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept = 0;
            };

            struct IIOStream : IInputStream, IOutputStream {};

            class CARBON_API StreamViewBase {
            protected:
                StreamViewBase(StreamBase& stream);
                virtual ~StreamViewBase();
                template <class StreamType>
                constexpr StreamType& getStream() noexcept {
                    return reinterpret_cast<StreamType&>(mStream);
                }
                template <class StreamType>
                constexpr const StreamType& getStream() const noexcept {
                    return reinterpret_cast<StreamType&>(mStream);
                }
            private:
                StreamBase& mStream;
            };

            class CARBON_API RawIStreamView : virtual protected StreamViewBase {
            public:
                RawIStreamView(IInputStream& stream);
                void read(Byte* targetBuffer, long long readLengthByBytes) const;
                int readSome(Byte* targetBuffer, long long readLengthByBytes) const noexcept;
                template <class T>
                RawIStreamView& operator >> (T& in) const { getStream<IInputStream>().read(&in, sizeof(T)); return *this; }
            };

            class CARBON_API RawOStreamView : virtual protected StreamViewBase {
            public:
                RawOStreamView(IOutputStream& stream) : StreamViewBase(stream) {}
                void write(const Byte* targetBuffer, long long writeLengthByBytes);
                int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept;
                template <class T>
                RawOStreamView& operator << (const T& in) { getStream<IOutputStream>().write(&in, sizeof(T)); return *this; }
            };

            class RawIOStreamView : public RawIStreamView, public RawOStreamView {
            public:
                RawIOStreamView(IIOStream& stream);
            };

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