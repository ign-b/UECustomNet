#include "BitStream.h"

#include <cassert>
#include <malloc.h>

CBitStream::CBitStream() {
    m_pData = m_aStackData;
    m_nBufferSizeInBits = BUFFER_STACK_ALLOCATION_SIZE * 8;
    m_nWriteOffsetInBits = 0;
    m_nReadOffsetInBits = 0;
    m_bCopyData = true;
}

CBitStream::CBitStream(unsigned char *pBuffer, un::Size const nSizeInBytes, bool bCopyData) {
    if (bCopyData) {
        if (nSizeInBytes > 0) {
            if (nSizeInBytes < BUFFER_STACK_ALLOCATION_SIZE) {
                m_pData = static_cast<unsigned char *>(m_aStackData);
                m_nBufferSizeInBits = BUFFER_STACK_ALLOCATION_SIZE << 3;
            } else {
                m_pData = static_cast<unsigned char *>(malloc(nSizeInBytes));
            }
            memcpy(m_pData, pBuffer, nSizeInBytes);
        } else {
            m_pData = nullptr;
        }
    } else {
        m_pData = pBuffer;
    }

    m_nWriteOffsetInBits = nSizeInBytes << 3;
    m_nReadOffsetInBits = 0;
    m_bCopyData = bCopyData;
    m_nBufferSizeInBits = nSizeInBytes << 3;
}

CBitStream::CBitStream(un::Size const nSizeInBytes) {
    if (nSizeInBytes <= BUFFER_STACK_ALLOCATION_SIZE) {
        m_pData = static_cast<unsigned char *>(m_aStackData);
        m_nBufferSizeInBits = BUFFER_STACK_ALLOCATION_SIZE * 8;
    } else {
        m_pData = static_cast<unsigned char *>(malloc(nSizeInBytes));
        m_nBufferSizeInBits = nSizeInBytes << 3;
    }

    m_nWriteOffsetInBits = 0;
    m_nReadOffsetInBits = 0;
    m_bCopyData = true;
}

CBitStream::~CBitStream() {
    if (m_bCopyData && m_nBufferSizeInBits > BUFFER_STACK_ALLOCATION_SIZE << 3) {
        free(m_pData);
    }
}

void CBitStream::Reset() {
    m_nWriteOffsetInBits = 0;
    m_nReadOffsetInBits = 0;
}

void CBitStream::AddBitsAndReallocate(un::Size const nSizeInBits) {
    un::Size nNewNumberOfBitsAllocated = nSizeInBits + m_nWriteOffsetInBits;

    if (nSizeInBits + m_nWriteOffsetInBits > 0 && (m_nBufferSizeInBits - 1) >> 3 < (nNewNumberOfBitsAllocated - 1) >> 3) {
        assert(m_bCopyData == true);

        nNewNumberOfBitsAllocated = (nSizeInBits + m_nWriteOffsetInBits) * 2;

        if (nNewNumberOfBitsAllocated - (nSizeInBits + m_nWriteOffsetInBits) > 1048576) {
            nNewNumberOfBitsAllocated = nSizeInBits + m_nWriteOffsetInBits + 1048576;
        }

        un::Size nAmountToAllocate = BITS_TO_BYTES(nNewNumberOfBitsAllocated);

        if (m_pData == static_cast<unsigned char *>(m_aStackData)) {
            if (nAmountToAllocate > BUFFER_STACK_ALLOCATION_SIZE) {
                m_pData = static_cast<unsigned char *>(malloc(nAmountToAllocate));
                memcpy(m_pData, m_aStackData, BITS_TO_BYTES(m_nBufferSizeInBits));
            }
        } else {
            m_pData = static_cast<unsigned char *>(realloc(m_pData, nAmountToAllocate));
        }
    }

    if (nNewNumberOfBitsAllocated > m_nBufferSizeInBits) {
        m_nBufferSizeInBits = nNewNumberOfBitsAllocated;
    }
}

void CBitStream::ResetReadPointer() {
    m_nReadOffsetInBits = 0;
}

void CBitStream::ResetWritePointer() {
    m_nWriteOffsetInBits = 0;
}

void CBitStream::Write(CString const &sIn) {
    un::Size const nLen = sIn.Length();

    Write(nLen);
    Write(*sIn, nLen);
}

void CBitStream::Write(CVector3f const &vecIn) {
    Write(vecIn.GetX());
    Write(vecIn.GetY());
    Write(vecIn.GetZ());
}

void CBitStream::Write(CQuaternionf const &quatIn) {
    Write(quatIn.GetW());
    Write(quatIn.GetX());
    Write(quatIn.GetY());
    Write(quatIn.GetZ());
}

void CBitStream::Write(SPlacementContext const &InContext) {
    Write(InContext.vecPosition);
    Write(InContext.quatRotation);
}

void CBitStream::Write(SMovementContext const &InContext) {
    Write(InContext.vecVelocity);
    Write(InContext.fGroundSpeed);
    Write(InContext.bShouldMove);
    Write(InContext.bIsFalling);
}

bool CBitStream::Read(CString &sOut) {
    sOut.Clear();

    un::Size nLen = 0;
    bool bReturn = Read(nLen);

    if (bReturn && nLen > 0) {
        sOut.Resize(nLen);
        bReturn = Read(*sOut, nLen);
    }

    return bReturn;
}

bool CBitStream::Read(CVector3f &vecOut) {
    if (!Read(vecOut.GetX())) return false;
    if (!Read(vecOut.GetY())) return false;
    if (!Read(vecOut.GetZ())) return false;
    return true;
}

bool CBitStream::Read(CQuaternionf &quatOut) {
    if (!Read(quatOut.GetW())) return false;
    if (!Read(quatOut.GetX())) return false;
    if (!Read(quatOut.GetY())) return false;
    if (!Read(quatOut.GetZ())) return false;
    return true;
}

bool CBitStream::Read(SPlacementContext &OutContext) {
    if (!Read(OutContext.vecPosition)) return false;
    if (!Read(OutContext.quatRotation)) return false;
    return true;
}

bool CBitStream::Read(SMovementContext &InContext) {
    if (!Read(InContext.vecVelocity)) return false;
    if (!Read(InContext.fGroundSpeed)) return false;
    if (!Read(InContext.bShouldMove)) return false;
    if (!Read(InContext.bIsFalling)) return false;
    return true;
}

void CBitStream::Write(const char *pInputByteArray, un::Size const nNumberOfBytes) {
    if (nNumberOfBytes == 0) {
        return;
    }

    if (MUL_OF_8(m_nWriteOffsetInBits)) {
        AddBitsAndReallocate(BYTES_TO_BITS(nNumberOfBytes));
        memcpy(m_pData + BITS_TO_BYTES(m_nWriteOffsetInBits), pInputByteArray, nNumberOfBytes);
        m_nWriteOffsetInBits += BYTES_TO_BITS(nNumberOfBytes);
    } else {
        WriteBits((unsigned char *) pInputByteArray, nNumberOfBytes * 8);
    }
}

bool CBitStream::Read(char *pOutputByteArray, un::Size const nNumberOfBytes) {
    if (nNumberOfBytes == 0) {
        return false;
    }

    if (MUL_OF_8(m_nReadOffsetInBits)) {
        if (m_nReadOffsetInBits + (nNumberOfBytes << 3) > m_nWriteOffsetInBits) {
            return false;
        }

        memcpy(pOutputByteArray, m_pData + (m_nReadOffsetInBits >> 3), nNumberOfBytes);

        m_nReadOffsetInBits += nNumberOfBytes << 3;
        return true;
    } else {
        return ReadBits(reinterpret_cast<unsigned char *>(pOutputByteArray), nNumberOfBytes * 8);
    }
}

void CBitStream::WriteCompressed(const unsigned char *pInByteArray, un::Size const nSize, const bool bUnsignedData) {
    un::Size nCurrentByte = (nSize >> 3) - 1;
    unsigned char bMatch = bUnsignedData ? 0x00 : 0xFF;

    while (nCurrentByte > 0) {
        if (pInByteArray[nCurrentByte] == bMatch) {
            bool b = true;
            Write(b);
        } else {
            bool b = false;
            Write(b);
            WriteBits(pInByteArray, (nCurrentByte + 1) << 3, true);
            return;
        }

        nCurrentByte--;
    }

    if ((bUnsignedData && ((*(pInByteArray + nCurrentByte)) & 0xF0) == 0x00) ||
        (!bUnsignedData && ((*(pInByteArray + nCurrentByte)) & 0xF0) == 0xF0)) {
        bool b = true;
        Write(b);
        WriteBits(pInByteArray + nCurrentByte, 4, true);
    } else {
        bool b = false;
        Write(b);
        WriteBits(pInByteArray + nCurrentByte, 8, true);
    }
}

bool CBitStream::ReadCompressed(unsigned char *pInOutByteArray, const un::Size nSize, const bool bUnsignedData) {
    un::Size nCurrentByte = (nSize >> 3) - 1;
    unsigned char bMatch = bUnsignedData ? 0x00 : 0xFF;
    unsigned char bHalfMatch = bUnsignedData ? 0x00 : 0xF0;

    while (nCurrentByte > 0) {
        bool b = false;
        if (!Read(b)) {
            return false;
        }

        if (b) {
            pInOutByteArray[nCurrentByte] = bMatch;
            nCurrentByte--;
        } else {
            if (!ReadBits(pInOutByteArray, (nCurrentByte + 1) << 3)) {
                return false;
            }

            return true;
        }
    }

    if (m_nReadOffsetInBits + 1 > m_nWriteOffsetInBits) {
        return false;
    }

    bool b = false;
    if (!Read(b)) {
        return false;
    }

    if (b) {
        if (!ReadBits(pInOutByteArray + nCurrentByte, 4)) {
            return false;
        }

        pInOutByteArray[nCurrentByte] |= bHalfMatch;
    } else {
        if (!ReadBits(pInOutByteArray + nCurrentByte, 8)) {
            return false;
        }
    }

    return true;
}

void CBitStream::WriteBits(const unsigned char *pInByteArray, un::Size nNumberOfBitsToWrite,
                           bool const bAlignBitsToRight) {
    if (nNumberOfBitsToWrite == 0) {
        return;
    }

    AddBitsAndReallocate(nNumberOfBitsToWrite);

    if (MUL_OF_8(m_nWriteOffsetInBits) && MUL_OF_8(nNumberOfBitsToWrite)) {
        memcpy(m_pData + (m_nWriteOffsetInBits >> 3), pInByteArray, nNumberOfBitsToWrite >> 3);
        m_nWriteOffsetInBits += nNumberOfBitsToWrite;
        return;
    }

    const unsigned int nWriteOffsetMod8 = m_nWriteOffsetInBits & 7;
    const unsigned char *pInput = pInByteArray;

    while (nNumberOfBitsToWrite > 0) {
        unsigned char bDataByte = *pInput++;

        if (nNumberOfBitsToWrite < 8 && bAlignBitsToRight) {
            bDataByte <<= 8 - nNumberOfBitsToWrite;
        }

        if (nWriteOffsetMod8 == 0) {
            *(m_pData + (m_nWriteOffsetInBits >> 3)) = bDataByte;
        } else {
            *(m_pData + (m_nWriteOffsetInBits >> 3)) |= bDataByte >> nWriteOffsetMod8;

            if (8 - nWriteOffsetMod8 < 8 && 8 - nWriteOffsetMod8 < nNumberOfBitsToWrite) {
                *(m_pData + (m_nWriteOffsetInBits >> 3) + 1) = static_cast<unsigned char>(
                    (bDataByte << 8) - nWriteOffsetMod8);
            }
        }

        if (nNumberOfBitsToWrite >= 8) {
            m_nWriteOffsetInBits += 8;
            nNumberOfBitsToWrite -= 8;
        } else {
            m_nWriteOffsetInBits += nNumberOfBitsToWrite;
            nNumberOfBitsToWrite = 0;
        }
    }
}

bool CBitStream::ReadBits(unsigned char *pInOutByteArray, un::Size nNumberOfBitsToRead, bool const bAlignBitsToRight) {
    if (nNumberOfBitsToRead == 0) {
        return false;
    }

    if (m_nReadOffsetInBits + nNumberOfBitsToRead > m_nWriteOffsetInBits) {
        return false;
    }

    if (MUL_OF_8(m_nReadOffsetInBits) && MUL_OF_8(nNumberOfBitsToRead)) {
        memcpy(pInOutByteArray, m_pData + (m_nReadOffsetInBits >> 3), nNumberOfBitsToRead >> 3);
        m_nReadOffsetInBits += nNumberOfBitsToRead;
        return true;
    }

    const unsigned int nReadOffsetMod8 = m_nReadOffsetInBits & 7;
    unsigned int nOffset = 0;
    memset(pInOutByteArray, 0, BITS_TO_BYTES(nNumberOfBitsToRead));

    while (nNumberOfBitsToRead > 0) {
        *(pInOutByteArray + nOffset) |= *(m_pData + (m_nReadOffsetInBits >> 3)) << nReadOffsetMod8;

        if (nReadOffsetMod8 > 0 && nNumberOfBitsToRead > 8 - nReadOffsetMod8) {
            *(pInOutByteArray + nOffset) |= *(m_pData + (m_nReadOffsetInBits >> 3) + 1) >> (8 - (nReadOffsetMod8));
        }

        if (nNumberOfBitsToRead >= 8) {
            nNumberOfBitsToRead -= 8;
            m_nReadOffsetInBits += 8;
            nOffset++;
        } else {
            if (int const neg = static_cast<int>(nNumberOfBitsToRead - 8); neg < 0) {
                if (bAlignBitsToRight) {
                    *(pInOutByteArray + nOffset) >>= -neg;
                }

                m_nReadOffsetInBits += 8 + neg;
            } else {
                m_nReadOffsetInBits += 8;
            }

            nOffset++;
            nNumberOfBitsToRead = 0;
        }
    }

    return true;
}

void CBitStream::WriteBit(bool const bState) {
    if (bState) {
        Write1();
    } else {
        Write0();
    }
}

void CBitStream::Write0() {
    AddBitsAndReallocate(1);

    if (MUL_OF_8(m_nWriteOffsetInBits)) {
        m_pData[m_nWriteOffsetInBits >> 3] = 0;
    }

    m_nWriteOffsetInBits++;
}

void CBitStream::Write1() {
    AddBitsAndReallocate(1);

    if (unsigned int const nWriteOffsetMod8 = m_nWriteOffsetInBits & 7; nWriteOffsetMod8 == 0) {
        m_pData[m_nWriteOffsetInBits >> 3] = 0x80;
    } else {
        m_pData[m_nWriteOffsetInBits >> 3] |= 0x80 >> (nWriteOffsetMod8);
    }

    m_nWriteOffsetInBits++;
}

bool CBitStream::ReadBit() {
    bool const bResult = (m_pData[m_nReadOffsetInBits >> 3] & 0x80 >> (m_nReadOffsetInBits & 7)) != 0;
    m_nReadOffsetInBits++;
    return bResult;
}

void CBitStream::PadWithZeroToByteLength(un::Size const nSizeInBytes) {
    if (GetNumberOfBytesUsed() < nSizeInBytes) {
        AlignWriteToByteBoundary();
        un::Size const nNumToWrite = nSizeInBytes - GetNumberOfBytesUsed();
        AddBitsAndReallocate(BYTES_TO_BITS(nNumToWrite));
        memset(m_pData + BITS_TO_BYTES(m_nWriteOffsetInBits), 0, nNumToWrite);
        m_nWriteOffsetInBits += BYTES_TO_BITS(nNumToWrite);
    }
}
