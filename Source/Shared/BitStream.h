#pragma once

#include "Common.h"
#include "Containers/String.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "NetTypes.h"

#define BUFFER_STACK_ALLOCATION_SIZE 256

#ifndef BITS_TO_BYTES
#define BITS_TO_BYTES(x) (((x) + 7) >> 3)
#endif

#ifndef BYTES_TO_BITS
#define BYTES_TO_BITS(x) ((x) << 3)
#endif

#define MUL_OF_8(x) (((x) & 7) == 0)

#define READ_TEMPLATE(type, size, out) \
    return ReadBits((unsigned char *)&out, (size * 8));

#define READ_COMPRESSED_TEMPLATE(type, size, out) \
    return ReadCompressed((unsigned char *)&out, (size * 8), true);

#define WRITE_TEMPLATE(type, size, in) \
    WriteBits((unsigned char *)&in, (size * 8), true);

#define WRITE_COMPRESSED_TEMPLATE(type, size, in) \
    WriteCompressed((unsigned char *)&in, (size * 8), true);

class UNET_API CBitStream {
protected:
    unsigned char *m_pData;
    un::Size m_nBufferSizeInBits;
    un::Size m_nWriteOffsetInBits;
    un::Size m_nReadOffsetInBits;
    bool m_bCopyData;
    unsigned char m_aStackData[BUFFER_STACK_ALLOCATION_SIZE]{};

public:
    CBitStream();

    CBitStream(unsigned char *pBuffer, un::Size nSizeInBytes, bool bCopyData);

    explicit CBitStream(un::Size nSizeInBytes);

    ~CBitStream();

    void Reset();

    void AddBitsAndReallocate(un::Size nSizeInBits);

    void ResetReadPointer();

    void ResetWritePointer();

    void Write(const bool &bIn) { WRITE_TEMPLATE(bool, sizeof(bool), bIn); }
    void Write(const char &cIn) { WRITE_TEMPLATE(char, sizeof(char), cIn); }
    void Write(const unsigned char &ucIn) { WRITE_TEMPLATE(unsigned char, sizeof(unsigned char), ucIn); }
    void Write(const short &sIn) { WRITE_TEMPLATE(short, sizeof(short), sIn); }
    void Write(const unsigned short &usIn) { WRITE_TEMPLATE(unsigned short, sizeof(unsigned short), usIn); }
    void Write(const int &iIn) { WRITE_TEMPLATE(int, sizeof(int), iIn); }
    void Write(const unsigned int &uiIn) { WRITE_TEMPLATE(unsigned int, sizeof(unsigned int), uiIn); }
    void Write(const long &lIn) { WRITE_TEMPLATE(long, sizeof(long), lIn); }
    void Write(const unsigned long &ulIn) { WRITE_TEMPLATE(unsigned long, sizeof(unsigned long), ulIn); }
    void Write(const __int64 &nIn) { WRITE_TEMPLATE(__int64, sizeof(__int64), nIn); }
    void Write(const unsigned __int64 &unIn) { WRITE_TEMPLATE(unsigned __int64, sizeof(unsigned __int64), unIn); }
    void Write(const float &fIn) { WRITE_TEMPLATE(float, sizeof(float), fIn); }
    void Write(const double &dIn) { WRITE_TEMPLATE(double, sizeof(double), dIn); }

    void Write(CString const &sIn);

    void Write(CVector3f const &vecIn);

    void Write(CQuaternionf const &quatIn);

    void Write(SPlacementContext const &InContext);

    void Write(SMovementContext const &InContext);

    void WriteCompressed(const bool &bIn) { WRITE_COMPRESSED_TEMPLATE(bool, sizeof(bool), bIn); }
    void WriteCompressed(const char &cIn) { WRITE_COMPRESSED_TEMPLATE(char, sizeof(char), cIn); }

    void WriteCompressed(const unsigned char &ucIn) {
        WRITE_COMPRESSED_TEMPLATE(unsigned char, sizeof(unsigned char), ucIn);
    }

    void WriteCompressed(const short &sIn) { WRITE_COMPRESSED_TEMPLATE(short, sizeof(short), sIn); }

    void WriteCompressed(const unsigned short &usIn) {
        WRITE_COMPRESSED_TEMPLATE(unsigned short, sizeof(unsigned short), usIn);
    }

    void WriteCompressed(const int &iIn) { WRITE_COMPRESSED_TEMPLATE(int, sizeof(int), iIn); }

    void WriteCompressed(const unsigned int &uiIn) {
        WRITE_COMPRESSED_TEMPLATE(unsigned int, sizeof(unsigned int), uiIn);
    }

    void WriteCompressed(const long &lIn) { WRITE_COMPRESSED_TEMPLATE(long, sizeof(long), lIn); }

    void WriteCompressed(const unsigned long &ulIn) {
        WRITE_COMPRESSED_TEMPLATE(unsigned long, sizeof(unsigned long), ulIn);
    }

    void WriteCompressed(const __int64 &nIn) { WRITE_COMPRESSED_TEMPLATE(__int64, sizeof(__int64), nIn); }

    void WriteCompressed(const unsigned __int64 &unIn) {
        WRITE_COMPRESSED_TEMPLATE(unsigned __int64, sizeof(unsigned __int64), unIn);
    }

    void WriteCompressed(const float &fIn) { WRITE_COMPRESSED_TEMPLATE(float, sizeof(float), fIn); }
    void WriteCompressed(const double &dIn) { WRITE_COMPRESSED_TEMPLATE(double, sizeof(double), dIn); }

    bool Read(bool &bOut) { READ_TEMPLATE(bool, sizeof(bool), bOut); }
    bool Read(char &cOut) { READ_TEMPLATE(char, sizeof(char), cOut); }
    bool Read(unsigned char &ucOut) { READ_TEMPLATE(unsigned char, sizeof(unsigned char), ucOut); }
    bool Read(short &sOut) { READ_TEMPLATE(short, sizeof(short), sOut); }
    bool Read(unsigned short &usOut) { READ_TEMPLATE(unsigned short, sizeof(unsigned short), usOut); }
    bool Read(int &iOut) { READ_TEMPLATE(int, sizeof(int), iOut); }
    bool Read(unsigned int &uiOut) { READ_TEMPLATE(unsigned int, sizeof(unsigned int), uiOut); }
    bool Read(long &lOut) { READ_TEMPLATE(long, sizeof(long), lOut); }
    bool Read(unsigned long &ulOut) { READ_TEMPLATE(unsigned long, sizeof(unsigned long), ulOut); }
    bool Read(__int64 &nOut) { READ_TEMPLATE(__int64, sizeof(__int64), nOut); }
    bool Read(unsigned __int64 &unOut) { READ_TEMPLATE(unsigned __int64, sizeof(unsigned __int64), unOut); }
    bool Read(float &fOut) { READ_TEMPLATE(float, sizeof(float), fOut); }
    bool Read(double &dOut) { READ_TEMPLATE(double, sizeof(double), dOut); }

    bool Read(CString &sOut);

    bool Read(CVector3f &vecOut);

    bool Read(CQuaternionf &quatOut);

    bool Read(SPlacementContext &OutContext);

    bool Read(SMovementContext &InContext);

    bool ReadCompressed(bool &bOut) { READ_COMPRESSED_TEMPLATE(bool, sizeof(bool), bOut); }
    bool ReadCompressed(char &cOut) { READ_COMPRESSED_TEMPLATE(char, sizeof(char), cOut); }
    bool ReadCompressed(unsigned char &ucOut) { READ_COMPRESSED_TEMPLATE(unsigned char, sizeof(unsigned char), ucOut); }
    bool ReadCompressed(short &sOut) { READ_COMPRESSED_TEMPLATE(short, sizeof(short), sOut); }

    bool ReadCompressed(unsigned short &usOut) {
        READ_COMPRESSED_TEMPLATE(unsigned short, sizeof(unsigned short), usOut);
    }

    bool ReadCompressed(int &iOut) { READ_COMPRESSED_TEMPLATE(int, sizeof(int), iOut); }
    bool ReadCompressed(unsigned int &uiOut) { READ_COMPRESSED_TEMPLATE(unsigned int, sizeof(unsigned int), uiOut); }
    bool ReadCompressed(long &lOut) { READ_COMPRESSED_TEMPLATE(long, sizeof(long), lOut); }
    bool ReadCompressed(unsigned long &ulOut) { READ_COMPRESSED_TEMPLATE(unsigned long, sizeof(unsigned long), ulOut); }
    bool ReadCompressed(__int64 &nOut) { READ_COMPRESSED_TEMPLATE(__int64, sizeof(__int64), nOut); }

    bool ReadCompressed(unsigned __int64 &unOut) {
        READ_COMPRESSED_TEMPLATE(unsigned __int64, sizeof(unsigned __int64), unOut);
    }

    bool ReadCompressed(float &fOut) { READ_COMPRESSED_TEMPLATE(float, sizeof(float), fOut); }
    bool ReadCompressed(double &dOut) { READ_COMPRESSED_TEMPLATE(double, sizeof(double), dOut); }

    void Write(const char *pInputByteArray, un::Size nNumberOfBytes);

    bool Read(char *pOutputByteArray, un::Size nNumberOfBytes);

protected:
    void WriteCompressed(const unsigned char *pInByteArray, un::Size nSize, bool bUnsignedData);

    bool ReadCompressed(unsigned char *pInOutByteArray, un::Size nSize, bool bUnsignedData);

public:
    void WriteBits(const unsigned char *pInByteArray, un::Size nNumberOfBitsToWrite, bool bAlignBitsToRight = true);

    bool ReadBits(unsigned char *pInOutByteArray, un::Size nNumberOfBitsToRead, bool bAlignBitsToRight = true);

    void WriteBit(bool bState);

    void Write0();

    void Write1();

    bool ReadBit();

    [[nodiscard]] unsigned char *GetData() const { return m_pData; }

    [[nodiscard]] un::Size GetNumberOfBitsUsed() const { return m_nWriteOffsetInBits; }
    [[nodiscard]] un::Size GetNumberOfBytesUsed() const { return BITS_TO_BYTES(m_nWriteOffsetInBits); }
    [[nodiscard]] un::Size GetNumberOfUnreadBits() const { return (m_nWriteOffsetInBits - m_nReadOffsetInBits); }

    [[nodiscard]] un::Size GetNumberOfUnreadBytes() const {
        return BITS_TO_BYTES(m_nWriteOffsetInBits - m_nReadOffsetInBits);
    }

    void IgnoreBits(const un::Size nSizeInBits) { m_nReadOffsetInBits += nSizeInBits; }
    void IgnoreBytes(const un::Size nSizeInBytes) { m_nReadOffsetInBits += BYTES_TO_BITS(nSizeInBytes); }

    void PadWithZeroToByteLength(un::Size nSizeInBytes);

    void AlignWriteToByteBoundary() { m_nWriteOffsetInBits += 8 - (((m_nWriteOffsetInBits - 1) & 7) + 1); }
};
