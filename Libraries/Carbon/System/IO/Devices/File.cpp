#include "Carbon/System/IO/Devices/File.hpp"
#include <stdexcept>

#if defined CARBON_TARGET_WINDOWS
#include <Windows.h>
namespace Carbon {
    namespace System {
        namespace IO {
            inline void _YieldBrokenStreamException() {
                throw std::runtime_error("Broken FileStream");
            }

            inline void _CheckStreamValidation(void* file) {
                if (file == INVALID_HANDLE_VALUE)
                    _YieldBrokenStreamException();
            }

            void _FileIOErrorHandlerWin32() {
                LPVOID lpMsgBuf;
                DWORD dw = GetLastError();
                FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)&lpMsgBuf,
                    0, NULL);
                const char* msgTable[] = {
                    ""
                };
                const char* msg = msgTable[0];
                throw std::runtime_error((LPSTR)lpMsgBuf);
            }

            FileStream::FileStream() : mFile(INVALID_HANDLE_VALUE) {
            }

            FileStream::~FileStream() {
                if (mFile != INVALID_HANDLE_VALUE)
                    CloseHandle(reinterpret_cast<HANDLE>(mFile));
            }

            FileStream::FileStream(FileStream & rhs) {
                mFile = rhs.mFile;
                rhs.mFile = INVALID_HANDLE_VALUE;
            }

            FileStream & FileStream::operator=(FileStream & rhs) {
                mFile = rhs.mFile;
                rhs.mFile = INVALID_HANDLE_VALUE;
                return *this;
            }

            FileStream::FileStream(const char * name) {
                mFile = CreateFileA(name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                _CheckStreamValidation(mFile);
            }

            uint64_t FileStream::tellp() const {
                _CheckStreamValidation(mFile);
                LARGE_INTEGER in, out;
                in.QuadPart = 0;
                if (!SetFilePointerEx(reinterpret_cast<HANDLE>(mFile), in, &out, FILE_CURRENT))
                    _FileIOErrorHandlerWin32();
                return out.QuadPart;
            }

            uint64_t FileStream::setp(int64_t offset, SRACBasePosition base) {
                _CheckStreamValidation(mFile);
                LARGE_INTEGER in, out;
                in.QuadPart = offset;
                if (!SetFilePointerEx(reinterpret_cast<HANDLE>(mFile), in, &out, base))
                    _FileIOErrorHandlerWin32();
                return out.QuadPart;
            }

            int64_t FileStream::peek(void* targetBuffer, int64_t readLengthByBytes) {
                setp(-read(targetBuffer, readLengthByBytes));
            }

            int64_t FileStream::read(void* targetBuffer, int64_t readLengthByBytes) {
                DWORD read;
                _CheckStreamValidation(mFile);
                if (!ReadFile(reinterpret_cast<HANDLE>(mFile), targetBuffer, readLengthByBytes, &read, nullptr))
                    _FileIOErrorHandlerWin32();
                return read;
            }

            int64_t FileStream::write(const void* targetBuffer, int64_t writeLengthByBytes) {
                DWORD written;
                _CheckStreamValidation(mFile);
                if (WriteFile(reinterpret_cast<HANDLE>(mFile), targetBuffer, writeLengthByBytes, &written, nullptr))
                    _FileIOErrorHandlerWin32();
                return written;
            }

            int64_t FileStream::readSome(void* targetBuffer, int64_t readLengthByBytes) {
                return read(targetBuffer, readLengthByBytes);
            }

            int64_t FileStream::writeSome(const void* targetBuffer, int64_t writeLengthByBytes) {
                return write(targetBuffer, writeLengthByBytes);
            }

        }
    }
}
#endif 