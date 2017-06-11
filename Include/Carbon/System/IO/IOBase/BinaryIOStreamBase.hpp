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
                virtual void read(Byte* targetBuffer, long long readLengthByBytes) const = 0;
                virtual int readSome(Byte* targetBuffer, long long readLengthByBytes) const noexcept = 0;
            };

            struct IOutputStream : virtual StreamBase {
                virtual Byte put() const noexcept = 0;
                virtual void write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
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

            class RawIStreamView : virtual protected StreamViewBase {
            public:
                RawIStreamView(IInputStream& stream) : StreamViewBase(stream) {}
                void read(Byte* targetBuffer, long long readLengthByBytes) const { getStream<IInputStream>().read(targetBuffer, readLengthByBytes); }
                int readSome(Byte* targetBuffer, long long readLengthByBytes) const noexcept { getStream<IInputStream>().readSome(targetBuffer, readLengthByBytes); }
                template <class T>
                RawIStreamView& operator >> (T& in) const { getStream<IInputStream>().read(&in, sizeof(T)); return *this; }
            };

            class RawOStreamView : virtual protected StreamViewBase {
            public:
                RawOStreamView(IOutputStream& stream) : StreamViewBase(stream) {}
                void write(const Byte* targetBuffer, long long writeLengthByBytes) { getStream<IOutputStream>().write(targetBuffer, writeLengthByBytes); }
                int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept { getStream<IOutputStream>().writeSome(targetBuffer, writeLengthByBytes); }
                template <class T>
                RawOStreamView& operator << (const T& in) { getStream<IOutputStream>().write(&in, sizeof(T)); return *this; }
            };

            class RawIOStreamView : public RawIStreamView, public RawOStreamView {
            public:
                RawIOStreamView(IIOStream& stream) : RawIStreamView(stream), RawOStreamView(stream), StreamViewBase(stream) {}
            };

            struct RAMBase {};

            struct IReadOnlyRAM {
                virtual Byte getAt(unsigned long long at) const = 0;
                virtual void readAt(Byte* targetBuffer, unsigned long long at, unsigned long long size) const = 0;
            };

            struct IWriteOnlyRAM {
                virtual void putAt(const Byte in, unsigned long long at) = 0;
                virtual void writeAt(const Byte* fromBuffer, unsigned long long at, unsigned long long size) = 0;
            };

        }
    }
}