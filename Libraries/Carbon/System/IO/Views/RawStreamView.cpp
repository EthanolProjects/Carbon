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

            int64_t RawIStreamView::peek(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.peek(targetBuffer, readLengthByBytes);
            }

            int64_t RawIStreamView::read(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.read(targetBuffer, readLengthByBytes);
            }

            int64_t RawIStreamView::readSome(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.readSome(targetBuffer, readLengthByBytes);
            }

            int64_t RawOStreamView::write(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.write(targetBuffer, writeLengthByBytes);
            }

            int64_t RawOStreamView::writeSome(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.writeSome(targetBuffer, writeLengthByBytes);
            }

            int64_t RawIOStreamView::peek(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.peek(targetBuffer, readLengthByBytes);
            }

            int64_t RawIOStreamView::read(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.read(targetBuffer, readLengthByBytes);
            }

            int64_t RawIOStreamView::readSome(Byte * targetBuffer, long long readLengthByBytes) {
                return mStream.readSome(targetBuffer, readLengthByBytes);
            }

            int64_t RawIOStreamView::write(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.write(targetBuffer, writeLengthByBytes);
            }

            int64_t RawIOStreamView::writeSome(const Byte * targetBuffer, long long writeLengthByBytes) {
                return mStream.writeSome(targetBuffer, writeLengthByBytes);
            }
        }
    }
}
