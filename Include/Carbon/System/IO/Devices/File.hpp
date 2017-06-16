#pragma once
#include "../SyncIOConcepts.hpp"
namespace Carbon {
    namespace System {
        namespace IO {
            class CARBON_API FileStream: public IIOStream, ISynchronizedRandomStreamAccess {
            public:
                FileStream();
                ~FileStream();
                FileStream(FileStream&);
                FileStream& operator = (FileStream&);
                FileStream(const FileStream&) = delete;
                FileStream& operator = (const FileStream&) = delete;
                FileStream(const char* name); // For Testing
                uint64_t tellp() const override;
                uint64_t setp(int64_t offset, SRACBasePosition base = SRACCurrent) override;
                int64_t peek(void* targetBuffer, int64_t readLengthByBytes) override;
                int64_t read(void* targetBuffer, int64_t readLengthByBytes) override;
                int64_t write(const void* targetBuffer, int64_t writeLengthByBytes) override;
                int64_t readSome(void* targetBuffer, int64_t readLengthByBytes) override;
                int64_t writeSome(const void* targetBuffer, int64_t writeLengthByBytes) override;
            private:
#if defined(CARBON_TARGET_WINDOWS)
                void *mFile; // HANDLE mFile;
#endif 
            };
        }
    }
}