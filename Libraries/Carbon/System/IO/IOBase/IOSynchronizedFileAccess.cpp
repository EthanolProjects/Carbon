#include "Carbon/System/IO/IOBase/IOSynchronizedFileAccess.hpp"
#include <stdexcept>

#if defined CARBON_TARGET_WINDOWS
#include <Windows.h>
namespace Carbon {
    namespace System {
        namespace IO {
            ISynchronizedRandomAccessControl::Position SynchronizedFile::tellp() const {
                LARGE_INTEGER in, out;
                in.QuadPart = 0;
                if (SetFilePointerEx(reinterpret_cast<HANDLE>(mFile), in, &out, FILE_CURRENT)) {
                    throw std::runtime_error("");
                }
                return out.QuadPart;
            }

            ISynchronizedRandomAccessControl::Position SynchronizedFile::setp(
                ISynchronizedRandomAccessControl::Position offset,
                SRACBasePosition base) {
                LARGE_INTEGER in, out;
                in.QuadPart = offset;
                if (SetFilePointerEx(reinterpret_cast<HANDLE>(mFile), in, &out, base)) {
                    throw std::runtime_error("");
                }
                return out.QuadPart;
            }

            Byte SynchronizedFile::get() const {
                Byte res;
                read(&res, 1);
                return res;
            }

            void SynchronizedFile::put(const Byte in) {
                write(&in, 1);
            }

            void SynchronizedFile::read(Byte* targetBuffer, long long readLengthByBytes) const {

            }
        }
    }
}
#endif 