#include "Carbon/System/IO/Views/RawStreamView.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            RawIStreamView::RawIStreamView(IInputStream & s) : mStream(s) {}

            RawIStreamView::~RawIStreamView() = default;

            RawOStreamView::RawOStreamView(IOutputStream & s) : mStream(s) {}

            RawOStreamView::~RawOStreamView() = default;

            RawIOStreamView::RawIOStreamView(IIOStream & s) : RawIStreamView(s), RawOStreamView(s) {}

            RawIOStreamView::~RawIOStreamView() = default;

            int64_t RawIStreamView::peek(void* targetBuffer, int64_t readLengthByBytes) {
                return mStream.peek(targetBuffer, readLengthByBytes);
            }

            int64_t RawIStreamView::read(void* targetBuffer, int64_t readLengthByBytes) {
                return mStream.read(targetBuffer, readLengthByBytes);
            }

            int64_t RawIStreamView::readSome(void* targetBuffer, int64_t readLengthByBytes) {
                return mStream.readSome(targetBuffer, readLengthByBytes);
            }

            int64_t RawOStreamView::write(const void* targetBuffer, int64_t writeLengthByBytes) {
                return mStream.write(targetBuffer, writeLengthByBytes);
            }

            int64_t RawOStreamView::writeSome(const void* targetBuffer, int64_t writeLengthByBytes) {
                return mStream.writeSome(targetBuffer, writeLengthByBytes);
            }

        }
    }
}
