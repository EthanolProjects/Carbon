#pragma once
#include "../SyncIOConcepts.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            struct CARBON_API RawIStreamView {
                RawIStreamView(IInputStream&);
                ~RawIStreamView();
                int64_t peek(void*, int64_t);
                int64_t read(void*, int64_t);
                int64_t readSome(void*, int64_t);
            private:
                IInputStream& mStream;
            };

            struct CARBON_API RawOStreamView{
                RawOStreamView(IOutputStream&);
                ~RawOStreamView();
                int64_t write(const void*, int64_t);
                int64_t writeSome(const void*, int64_t);
            private:
                IOutputStream& mStream;
            };

            struct CARBON_API RawIOStreamView : RawIStreamView, RawOStreamView {
                RawIOStreamView(IIOStream&);
                ~RawIOStreamView();
            };

        }
    }
}