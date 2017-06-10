#pragma once
#include "Carbon/Config/Config.hpp"
namespace Carbon {
    using Byte = unsigned char;
    namespace System {
        namespace IO {
            struct StreamBase {
                virtual ~StreamBase() {};
            };

            struct IInputStream : virtual StreamBase {
                virtual Byte get() const noexcept = 0;
                virtual void read(Byte* targetBuffer, long long readLengthByBytes) const noexcept = 0;
                virtual int readSome(const Byte* targetBuffer, long long readLengthByBytes) const noexcept = 0;
            };

            struct IOutputStream : virtual StreamBase {
                virtual Byte put() const noexcept = 0;
                virtual void write(const Byte* targetBuffer, long long writeLengthByBytes) noexcept = 0;
                virtual int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept = 0;
            };

            struct IIOStream : IInputStream, IOutputStream {};

            class StreamViewBase {
            protected:
                constexpr StreamViewBase(StreamBase& stream) : mStream(stream) {}
                virtual ~StreamViewBase() {}
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

            class RawIStreamView : StreamViewBase {
            public:
                RawIStreamView(IInputStream& stream) : StreamViewBase(stream) {}
                void read(Byte* targetBuffer, long long readLengthByBytes) const noexcept { getStream<IInputStream>().read(targetBuffer, readLengthByBytes); }
                int readSome(const Byte* targetBuffer, long long readLengthByBytes) const noexcept { getStream<IInputStream>().readSome(targetBuffer, readLengthByBytes); }
                template <class T>
                RawIStreamView& operator >> (T& in) { getStream<IInputStream>().read(&in, sizeof(T)); return *this; }
            };

        }
    }
}