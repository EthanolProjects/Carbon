#include "Carbon/System/IO/Views/RawStreamView.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            RawIStreamView::RawIStreamView(IInputStream & s) : mStream(s) {}

            RawIStreamView::~RawIStreamView() = default;

            RawOStreamView::RawOStreamView(IOutputStream & s) : mStream(s) {}

            RawOStreamView::~RawOStreamView() = default;

            RawIOStreamView::RawIOStreamView(IIOStream & s) : mStream(s) {}

            RawIOStreamView::~RawIOStreamView() = default;

            int RawIStreamView::peek(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.peek(targetBuffer, readLengthByBytes);
            }

            int RawIStreamView::read(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.read(targetBuffer, readLengthByBytes);
            }

            int RawIStreamView::readSome(Byte * targetBuffer, long long readLengthByBytes) noexcept {
                return mStream.readSome(targetBuffer, readLengthByBytes);
            }

            int RawOStreamView::write(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.write(targetBuffer, writeLengthByBytes);
            }

            int RawOStreamView::writeSome(const Byte * targetBuffer, long long writeLengthByBytes) noexcept {
                return mStream.writeSome(targetBuffer, writeLengthByBytes);
            }

            int RawIOStreamView::peek(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.peek(targetBuffer, readLengthByBytes);
            }

            int RawIOStreamView::read(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.read(targetBuffer, readLengthByBytes);
            }

            int RawIOStreamView::readSome(Byte * targetBuffer, long long readLengthByBytes) noexcept {
                return mStream.readSome(targetBuffer, readLengthByBytes);
            }

            int RawIOStreamView::write(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.write(targetBuffer, writeLengthByBytes);
            }

            int RawIOStreamView::writeSome(const Byte * targetBuffer, long long writeLengthByBytes) noexcept {
                return mStream.writeSome(targetBuffer, writeLengthByBytes);
            }
        }
    }
}
