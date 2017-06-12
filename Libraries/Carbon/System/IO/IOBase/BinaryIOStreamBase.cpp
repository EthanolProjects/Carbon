#include "Carbon/System/IO/IOBase/BinaryIOStreamBase.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            inline StreamViewBase::StreamViewBase(StreamBase & stream) : mStream(stream) {}

            inline StreamViewBase::~StreamViewBase() {}

            inline RawIStreamView::RawIStreamView(IInputStream & stream) : StreamViewBase(stream) {}

            inline void RawIStreamView::read(Byte * targetBuffer, long long readLengthByBytes) const { getStream<IInputStream>().read(targetBuffer, readLengthByBytes); }

            inline int RawIStreamView::readSome(Byte * targetBuffer, long long readLengthByBytes) const noexcept { return getStream<IInputStream>().readSome(targetBuffer, readLengthByBytes); }

            inline void RawOStreamView::write(const Byte * targetBuffer, long long writeLengthByBytes) { getStream<IOutputStream>().write(targetBuffer, writeLengthByBytes); }

            inline int RawOStreamView::writeSome(const Byte * targetBuffer, long long writeLengthByBytes) noexcept { return getStream<IOutputStream>().writeSome(targetBuffer, writeLengthByBytes); }

            inline RawIOStreamView::RawIOStreamView(IIOStream & stream) : RawIStreamView(stream), RawOStreamView(stream), StreamViewBase(stream) {}

        }
    }
}
