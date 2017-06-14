#pragma once
#include "../SyncIOConcepts.hpp"

namespace Carbon {
    namespace System {
        namespace IO {
            struct IRawIStreamView {
                virtual int peek(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int read(Byte* targetBuffer, long long readLengthByBytes) = 0;
                virtual int readSome(Byte* targetBuffer, long long readLengthByBytes) noexcept = 0;
                template <class T>
                IRawIStreamView& operator >> (T& target) { read(&target, sizeof(T)); }
            };

            struct IRawOStreamView {
                virtual int write(const Byte* targetBuffer, long long writeLengthByBytes) = 0;
                virtual int writeSome(const Byte* targetBuffer, long long writeLengthByBytes) noexcept = 0;
                template <class T>
                IRawIStreamView& operator << (const T& target) { write(&target, sizeof(T)); }
            };

            struct IRawIOStreamView : IRawIStreamView, IRawOStreamView {};

            struct CARBON_API RawIStreamView: IRawIStreamView {
                RawIStreamView(IInputStream&);
                ~RawIStreamView();
                int peek(Byte*, long long) override;
                int read(Byte*, long long) override;
                int readSome(Byte*, long long) noexcept override;
            private:
                IInputStream& mStream;
            };

            struct CARBON_API RawOStreamView : IRawOStreamView {
                RawOStreamView(IOutputStream&);
                ~RawOStreamView();
                int write(const Byte*, long long) override;
                int writeSome(const Byte*, long long) noexcept override;
            private:
                IOutputStream& mStream;
            };

            struct CARBON_API RawIOStreamView : IRawIOStreamView {
                RawIOStreamView(IIOStream&);
                ~RawIOStreamView();
                int write(const Byte*, long long) override;
                int writeSome(const Byte*, long long) noexcept override;
                int peek(Byte*, long long) override;
                int read(Byte*, long long) override;
                int readSome(Byte*, long long) noexcept override;
            private:
                IIOStream& mStream;
            };


        }
    }
}