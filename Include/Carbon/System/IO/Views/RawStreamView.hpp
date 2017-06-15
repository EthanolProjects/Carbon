#pragma once
#include "../SyncIOConcepts.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            struct IRawIStreamView {
                virtual int64_t peek(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int64_t read(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int64_t readSome(Byte* targetBuffer, long long readLengthByBytes) = 0;
                template <class T>
                IRawIStreamView& operator >> (T& target) { read(&target, sizeof(T)); }
            };

            struct IRawOStreamView {
                virtual int64_t write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                virtual int64_t writeSome(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                template <class T>
                IRawIStreamView& operator << (const T& target) { write(&target, sizeof(T)); }
            };

            struct IRawIOStreamView : IRawIStreamView, IRawOStreamView {};

            struct CARBON_API RawIStreamView: IRawIStreamView {
                RawIStreamView(IInputStream&);
                ~RawIStreamView();
                int64_t peek(Byte*, long long) override;
                int64_t read(Byte*, long long) override;
                int64_t readSome(Byte*, long long) override;
            private:
                IInputStream& mStream;
            };

            struct CARBON_API RawOStreamView : IRawOStreamView {
                RawOStreamView(IOutputStream&);
                ~RawOStreamView();
                int64_t write(const Byte*, long long) override;
                int64_t writeSome(const Byte*, long long) override;
            private:
                IOutputStream& mStream;
            };

            struct CARBON_API RawIOStreamView : IRawIOStreamView {
                RawIOStreamView(IIOStream&);
                ~RawIOStreamView();
                int64_t write(const Byte*, long long) override;
                int64_t writeSome(const Byte*, long long) override;
                int64_t peek(Byte*, long long) override;
                int64_t read(Byte*, long long) override;
                int64_t readSome(Byte*, long long) override;
            private:
                IIOStream& mStream;
            };


        }
    }
}