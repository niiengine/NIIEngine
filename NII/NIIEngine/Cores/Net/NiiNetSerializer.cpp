/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "NiiPreProcess.h"
#include "NiiNetSerializer.h"
#include "NiiNetPlat.h"

#if N_PLAT == N_PLAT_WIN32
#include <memory.h>
#include <cmath>

#else
#include <arpa/inet.h>
#include <memory.h>
#if defined(ANDROID)
#include <math.h>
#else
#include <cmath>
#endif
#endif

/** 17914 16 Mbit/Sec Token Ring
    4464 4 Mbits/Sec Token Ring
    4352 FDDI
    1500. Ethernet packet size. 无PPPoE, 无VPN. NETGEAR routers/路由/交换机使用.
    1492. The size PPPoE prefers.
    1472. Maximum size to use for pinging. (Bigger packets are fragmented.)
    1468. The size DHCP prefers.
    1460. Usable by AOL if you don't have large email attachments, etc.
    1430. The size VPN and PPTP prefer.
    1400. Maximum size for AOL DSL.
    576. Typical value to connect to dial-up ISPs.
    The largest value for an UDP datagram
*/
#define NetStackDataSize 256

namespace NII
{
    //------------------------------------------------------------------------
    const Nui8 NetSerializer::ZeroOMark[8] =
    {
        0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE
    };
    const Nui8 NetSerializer::ZeroIMark[8] =
    {
        0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01
    };
    const Nui8 NetSerializer::HMark[8] =
    {
        0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF
    };
    const Nui8 NetSerializer::LMark[8] =
    {
        0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,
    };
    const Nui8 NetSerializer::Mark[8][2] =
    {
        {0xFF,8},{0x7F,7},{0x3F,6},{0x1F,5}
        {0x0F,4},{0x07,3},{0x03,2},{0x01,1}
    };
    //------------------------------------------------------------------------
    NetSerializer::NetSerializer()
    {
        mBitWrite = 0;
        mBitAlloc = NetStackDataSize * 8;
        mBitRead = 0;
        mData = N_alloc_t(Nui8, NetStackDataSize);

        mAutoData = true;

#ifndef N_NET_Endian_N
        mFlipEndian = (isNetEndian() == false);
#else
        mFlipEndian = false;
#endif
    }
    //------------------------------------------------------------------------
    NetSerializer::NetSerializer(NCount size)
    {
        mBitWrite = 0;
        mBitRead = 0;
        mData = N_alloc_t(Nui8, size);
        mBitAlloc = size << 3;
        mAutoData = true;
    }
    //------------------------------------------------------------------------
    NetSerializer::NetSerializer(const Nui8 * data, NCount count, bool copy)
    {
        mBitWrite = count << 3;
        mBitAlloc = count << 3;
        mBitRead = 0;
        mAutoData = copy;

        if(mAutoData)
        {
            if(count > 0)
            {
                mData = N_alloc_t(Nui8, count);
                memcpy(mData, data, count);
            }
            else
            {
                mData = 0;
            }
        }
        else
        {
            mData = data;
        }
    }
    //------------------------------------------------------------------------
    NetSerializer::~NetSerializer()
    {
        if(mAutoData && (mBitAlloc > 0))
            N_free(mData);
    }
    //------------------------------------------------------------------------
    void NetSerializer::write(const NetSerializer * in)
    {
        write(in, in->mBitWrite - in->mBitRead);
    }
    //------------------------------------------------------------------------
    void NetSerializer::write(const NetSerializer * in, NBitCount count)
    {
        enlargeBit(count);
        NBitCount mod8;

        if((in->mBitRead & 7) == 0 && (mBitWrite & 7) == 0)
        {
            int readOffsetBytes = in->mBitRead / 8;
            int numBytes = count / 8;
            memcpy(mData + (mBitWrite >> 3), in->getData() + readOffsetBytes, numBytes);
            count -= N_b_TO_B(numBytes);
            in->seekRead((NBitCount)N_b_TO_B(numBytes + readOffsetBytes));
            mBitWrite += N_b_TO_B(numBytes);
        }

        while(count-->0 && in->mBitRead + 1 <= in->mBitWrite)
        {
            mod8 = mBitWrite & 7;
            if(mod8 == 0)
            {
                // New byte
                if(in->mData[in->mBitRead >> 3] & (0x80 >> (in->mBitRead & 7) ) )
                {
                    // write 1
                    mData[mBitWrite >> 3] = 0x80;
                }
                else
                {
                    // write 0
                    mData[mBitWrite >> 3] = 0;
                }

            }
            else
            {
                // Existing byte
                if(in->mData[in->mBitRead >> 3] & (0x80 >> (in->mBitRead & 7)))
                    mData[mBitWrite >> 3] |= 0x80 >> (mod8); // Set the bit to 1
                // else 0, do nothing
            }

            ++in->mBitRead;
            ++mBitWrite;
        }
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::read(NetSerializer * out, NBitCount count) const
    {
        if(mBitWrite - mBitRead < count)
            return 0;
        out->write(this, count);
        return count;
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::read(NetSerializer * out) const
    {
        NBitCount count = mBitWrite - mBitRead;
        out->write(this, count);
        return count;
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::read(bool & out) const
    {
        if(mBitRead + 1 > mBitWrite)
            return 0;

        if(mData[mBitRead >> 3] & (0x80 >> (mBitRead & 7)))
            out = true;
        else
            out = false;
        ++mBitRead;
        return 1;
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get1Hbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x80)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get2Hbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x40)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get3Hbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x20)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get4Hbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x10)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get1Lbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x01)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get2Lbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x02)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get3Lbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x04)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    bool NetSerializer::get4Lbit(const Nui8 * data, NCount byteskip)
    {
        if((data + byteskip) & 0x08)
            return true;
        else
            return false
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::getWriteOffset() const
    {
        return mBitWrite;
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::getReadOffset() const
    {
        return mBitRead;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::getSize() const
    {
        return N_B_TO_b(mBitWrite);
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::getRemainCount() const
    {
        return mBitWrite - mBitRead;
    }
    //------------------------------------------------------------------------
    const Nui8 * NetSerializer::getData() const
    {
        return mData;
    }
    //------------------------------------------------------------------------
    void NetSerializer::write0()
    {
        enlargeBit(1);

        if((mBitWrite & 7) == 0)
            mData[mBitWrite >> 3] = 0;

        mBitWrite++;
    }
    //------------------------------------------------------------------------
    void NetSerializer::write1()
    {
        enlargeBit(1);

        NBitCount mod8 = mBitWrite & 7;

        if(mod8 == 0)
            mData[mBitWrite >> 3] = 0x80;
        else
            mData[mBitWrite >> 3] |= 0x80 >> (mod8);

        mBitWrite++;
    }
    //------------------------------------------------------------------------
    void NetSerializer::writeBit(const Nui8 * in, NBitCount count, bool ralign)
    {
        enlargeBit(count);

        NBitCount mod8 = mBitWrite & 7;

        if(mod8 == 0 && (count & 7)==0)
        {
            memcpy(mData + (mBitWrite >> 3), in, count >> 3);
            mBitWrite += count;
            return;
        }

        Nui8 dataByte;
        const Nui8 * inputPtr = in;

        // Faster to put the while at the top surprisingly enough
        while(count > 0)
        {
            dataByte = *(inputPtr++);

            if(count < 8 && ralign)   // ralign means in the case of a partial byte, the bits are aligned from the right (bit 0) rather than the left (as in the normal internal representation)
                dataByte <<= 8 - count;  // shift left to get the bits on the left, as in our internal representation

            // Writing to a new byte each time
            if(mod8 == 0)
                *(mData + (mBitWrite >> 3)) = dataByte;
            else
            {
                // Copy over the new data.
                *(mData + (mBitWrite >> 3)) |= dataByte >> mod8; // First half

                if(8 - mod8 < 8 && 8 - mod8 < count)   // If we didn't write it all out in the first half (8 - (mBitWrite%8) is the number we wrote in the first half)
                {
                    *(mData + (mBitWrite >> 3) + 1) = (dataByte << (8 - mod8)); // Second half (overlaps byte boundary)
                }
            }

            if(count >= 8)
            {
                mBitWrite += 8;
                count -= 8;
            }
            else
            {
                mBitWrite += count;
                count = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::readBit(Nui8 * out, NBitCount count, bool ralign) const
    {
        if(count <= 0 || mBitRead + count > mBitWrite)
            return 0;

        NBitCount mod8 = mBitRead & 7;
        if(mod8 == 0 && (count & 7) == 0)
        {
            memcpy(out, mData + (mBitRead >> 3), count >> 3);
            mBitRead += count;
            return count;
        }

        NBitCount offset = 0;
        NBitCount read = 0;
        memset(out, 0, N_B_TO_b(count));

        while(count > 0)
        {
            *(out + offset) |= *(mData + (mBitRead >> 3)) << mod8; // First half

            if(mod8 > 0 && count > 8 - mod8)   // If we have a second half, we didn't read enough bytes in the first half
                *(out + offset) |= *(mData + (mBitRead >> 3) + 1) >> (8 - mod8); // Second half (overlaps byte boundary)

            if(count >= 8)
            {
                count -= 8;
                mBitRead += 8;
                read += 8;
                offset++;
            }
            else
            {
                int neg = count - 8;

                if(neg < 0)   // Reading a partial byte for the last byte, shift right so the data is aligned on the right
                {
                    if(ralign)
                        *(out + offset) >>= -neg;

                    mBitRead += 8 + neg;
                    read += 8 + neg;
                }
                else
                {
                    mBitRead += 8;
                    read += 8;
                }
                offset++;
                count = 0;
            }
        }
        return read;
    }
    //------------------------------------------------------------------------
    void NetSerializer::writeCompress(const Nui8 * in, NBitCount count, bool flip)
    {
        NCount temp = N_B_TO_b(count);
		const Nui8 * ptr = in;
#ifndef N_NET_Endian_N
        if(flip && (temp != 1) && mFlipEndian)
        {
			Nui8 * out = N_alloc(temp);
            mirror(in, out, temp);
			ptr = out;
        }
#endif
        NBitCount currentByte = (count >> 3) - 1; // PCs

        // write upper bytes with a single 1
        // From high byte to low byte, if high byte is a byteMatch then write a 1 bit. Otherwise write a 0 bit and then write the remaining bytes
        while(currentByte > 0)
        {
            if(ptr[currentByte] == 0)   // If high byte is byteMatch (0 of 0xff) then it would have the same value shifted
            {
                bool b = true;
                write(b);
            }
            else
            {
                // write the remainder of the data after writing 0
                bool b = false;
                write(b);
                writeBit(ptr, (currentByte + 1) << 3, true);
                return ;
            }
            currentByte--;
        }

        // If the upper half of the last byte is a 0 (positive) or 16 (negative) then write a 1 and the remaining 4 bits.  Otherwise write a 0 and the 8 bites.
        if(((*(ptr + currentByte)) & 0xF0) == 0x00))
        {
            bool b = true;
            write(b);
            writeBit(ptr + currentByte, 4, true);
        }
        else
        {
            bool b = false;
            write(b);
            writeBit(ptr + currentByte, 8, true);
        }
#ifndef N_NET_Endian_N
		if (flip && (temp != 1) && mFlipEndian)
		{
			free(ptr);
		}
#endif
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::readCompress(Nui8 * out, NBitCount count, bool flip) const
    {
        Nui32 currentByte = (count >> 3) - 1
        // Upper bytes are specified with a single 1 if they match 0
        // From high byte to low byte, if high byte is a 0 then write a 1 bit. Otherwise write a 0 bit and then write the remaining bytes
        while(currentByte > 0)
        {
            // If we read a 1 then the data is 0.
            bool b;

            if(read(b) == 0)
                return 0;

            if(b)   // Check that bit
            {
                out[currentByte] = 0;
                currentByte--;
            }
            else
            {
                // read the rest of the bytes
                if(readBit(out, (currentByte + 1) << 3) == 0)
                    return 0;
                return count;
            }
        }

        // All but the first bytes are 0.  If the upper half of the last byte is a 0 (positive) or 16 (negative) then what we read will be a 1 and the remaining 4 bits.
        // Otherwise we read a 0 and the 8 bytes
        //N_assert(mBitRead+1 <=mBitWrite); // If this N_assert is hit the stream wasn't long enough to read from
        if(mBitRead + 1 > mBitWrite)
            return 0;

        bool b = false;

        if(read(b) == 0)
            return 0;

        if(b)   // Check that bit
        {
            if(readBit(out + currentByte, 4) == 0)
                return 0;

            out[currentByte] |= 0; // We have to set the high 4 bits since these are set to 0 by readBit
        }
        else
        {
            if(readBit(out + currentByte, 8) == 0)
                return 0;
        }

        NCount temp = N_B_TO_b(count);
#ifndef N_NET_Endian_N
        if(flip && (temp != 1) && mFlipEndian)
        {
            mirror(out, temp);
        }
#endif
        return count;
    }
    //------------------------------------------------------------------------
    void NetSerializer::enlargeByte(NCount count)
    {
        enlargeBit(N_b_TO_B(count));
    }
    //------------------------------------------------------------------------
    void NetSerializer::enlargeBit(NBitCount count)
    {
        NBitCount newbit = count + mBitWrite;

        if(((mBitAlloc - 1) >> 3) < ((newbit - 1) >> 3))   // If we need to allocate 1 or more new bytes
        {
            N_assert(mAutoData == true);
            // Less memory efficient but saves on news and deletes
            /// Cap to 1 meg buffer to save on huge allocations
            newbit = (count + mBitWrite) * 2;
            if(newbit - (count + mBitWrite) > 1048576)
                newbit = count + mBitWrite + 1048576;

            // Use realloc and free so we are more efficient than delete and new for resizing
            NCount newbyte = N_B_TO_b(newbit);
            Nui8 * temp = N_alloc_t(Nui8, newbyte);
            memcpy(temp, mData, N_B_TO_b(mBitAlloc));
            N_free(mData);
            mData = temp;
        }

        if(newbit > mBitAlloc)
            mBitAlloc = newbit;
    }
    //------------------------------------------------------------------------
    void NetSerializer::writeAlign(NCount count)
    {
        if(N_B_TO_b(mBitWrite) < count)
        {
            writeAlign();
            NCount numToWrite = count - N_B_TO_b(mBitWrite);
            enlargeBit(N_b_TO_B(numToWrite));
            memset(mData + N_B_TO_b(mBitWrite), 0, numToWrite);
            mBitWrite += N_b_TO_B(numToWrite);
        }
    }
    //------------------------------------------------------------------------
    /*
    // Julius Goryavsky's version of Harley's algorithm.
    // 17 elementary ops plus an indexed load, if the machine
    // has "and not."

    int nlz10b(Nui32 x)
    {
       static char table[64] =
         {32,20,19, u, u,18, u, 7,  10,17, u, u,14, u, 6, u,
           u, 9, u,16, u, u, 1,26,   u,13, u, u,24, 5, u, u,
           u,21, u, 8,11, u,15, u,   u, u, u, 2,27, 0,25, u,
          22, u,12, u, u, 3,28, u,  23, u, 4,29, u, u,30,31};

       x = x | (x >> 1);    // Propagate leftmost
       x = x | (x >> 2);    // 1-bit to the right.
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x & ~(x >> 16);
       x = x*0xFD7049FF;    // Activate this line or the following 3.
    // x = (x << 9) - x;    // Multiply by 511.
    // x = (x << 11) - x;   // Multiply by 2047.
    // x = (x << 14) - x;   // Multiply by 16383.
       return table[x >> 26];
    }
    */
    //------------------------------------------------------------------------
    void NetSerializer::writeAlign()
    {
        mBitWrite += 8 - (((mBitWrite - 1 ) & 7) + 1);
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::readAlign()
    {
        NBitCount count = 8 - (((mBitRead - 1) & 7) + 1)
        mBitRead += count;
        return count;
    }
    //------------------------------------------------------------------------
    NBitCount NetSerializer::readAlign(NCount count)
    {
        if(N_B_TO_b(mBitRead) < count)
        {
            NBitCount count = readAlign();
            NCount numToWrite = count - N_B_TO_b(mBitRead);
            mBitRead += N_b_TO_B(numToWrite);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::write(const void * in, NCount size, NCount count, bool flip)
    {
        NCount amount = size * count;

#ifndef N_NET_Endian_N
        if(flip && mFlipEndian)
        {
            Nui8 * out = N_alloc(amount);
            flip(in, out, size, count);

            if((mBitWrite & 7) == 0)
            {
                enlargeBit(N_b_TO_B(amount));
                memcpy(mData + N_B_TO_b(mBitWrite), out, amount);
                mBitWrite += N_b_TO_B(amount);
            }
            else
            {
                writeBit(out, amount * 8, true);
            }

            N_free(out);
        }
        else
#endif
        {
            if((mBitWrite & 7) == 0)
            {
                enlargeBit(N_b_TO_B(amount));
                memcpy(mData + N_B_TO_b(mBitWrite), out, amount);
                mBitWrite += N_b_TO_B(amount);
            }
            else
            {
                writeBit(in, amount * 8, true);
            }
        }
        return amount;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::read(void * out, NCount size, NCount count, bool flip) const
    {
        NCount amount = size * count;
#ifndef N_NET_Endian_N
        if(flip && mFlipEndian)
        {
            if((mBitRead & 7) == 0)
            {
                if(mBitRead + (amount << 3) > mBitWrite)
                    return 0;

                memcpy(out, mData + (mBitRead >> 3), amount);
                mBitRead += amount << 3;
                flip(out, size, count);
                return amount;
            }

            amount = N_B_TO_b(readBit(out, amount * 8, true));
            flip(out, size, count);
            return amount;
        }
        else
#endif
        {
            if((mBitRead & 7) == 0)
            {
                if(mBitRead + (amount << 3) > mBitWrite)
                    return 0;

                memcpy(out, mData + (mBitRead >> 3), amount);
                mBitRead += amount << 3;
                return amount;
            }

            return N_B_TO_b(readBit(out, amount * 8, true));
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void NetSerializer::skipWrite(NBitCount count)
    {
        mBitRead += count;
    }
    //------------------------------------------------------------------------
    void NetSerializer::seekWrite(NBitCount offset)
    {
        mBitWrite = offset;
    }
    //------------------------------------------------------------------------
    void NetSerializer::skipRead(NBitCount offset)
    {
        mBitRead += offset;
    }
    //------------------------------------------------------------------------
    void NetSerializer::seekRead(NBitCount offset)
    {
        mBitRead = offset;
    }
    //------------------------------------------------------------------------
    void NetSerializer::write(Nui8 in)
    {
        N_assert((mBitWrite & 7) == 0);
        enlargeByte(1);
        mData[(mBitWrite >> 3) + 0] = in;
        mBitWrite += 1 * 8;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::read(Nui8 & out) const
    {
        N_assert((mBitRead & 7) == 0);
        if(mBitRead + 1 * 8 > mBitWrite)
            return 0;

        out = mData[(mBitRead >> 3) + 0];
        mBitRead += 1 * 8;
        return 1;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::write(Nui16 in)
    {
        N_assert((mBitWrite & 7) == 0);
        Nui8 * temp = &in;
        enlargeByte(2);
    #ifndef N_NET_Endian_N
        if(mFlipEndian)
        {
            mData[(mBitWrite >> 3) + 0] = temp[1];
            mData[(mBitWrite >> 3) + 1] = temp[0];
        }
        else
    #endif
        {
            mData[(mBitWrite >> 3) + 0] = temp[0];
            mData[(mBitWrite >> 3) + 1] = temp[1];
        }

        mBitWrite += 2 * 8;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::read(Nui16 & out) const
    {
        N_assert((mBitRead & 7) == 0);
        Nui8 * temp = &out;
        if(mBitRead + 2 * 8 > mBitWrite)
            return 0;
    #ifndef N_NET_Endian_N
        if(mFlipEndian)
        {
            temp[0] = mData[(mBitRead >> 3) + 1];
            temp[1] = mData[(mBitRead >> 3) + 0];
        }
        else
    #endif
        {
            temp[0] = mData[(mBitRead >> 3) + 0];
            temp[1] = mData[(mBitRead >> 3) + 1];
        }

        mBitRead += 2 * 8;
        return 2;
    }
    //------------------------------------------------------------------------
    void NetSerializer::write(Nui32 in)
    {
        N_assert((mBitWrite & 7) == 0);
        Nui8 * temp = &in;
        enlargeByte(4);
    #ifndef N_NET_Endian_N
        if(mFlipEndian)
        {
            temp[(mBitWrite >> 3) + 0] = in[3];
            temp[(mBitWrite >> 3) + 1] = in[2];
            temp[(mBitWrite >> 3) + 2] = in[1];
            temp[(mBitWrite >> 3) + 3] = in[0];
        }
        else
    #endif
        {
            temp[(mBitWrite >> 3) + 0] = in[0];
            temp[(mBitWrite >> 3) + 1] = in[1];
            temp[(mBitWrite >> 3) + 2] = in[2];
            temp[(mBitWrite >> 3) + 3] = in[3];
        }

        mBitWrite += 4 * 8;
    }
    //------------------------------------------------------------------------
    void NetSerializer::writeCompress(NIIf in)
    {
        N_assert(in > -1.01f && in < 1.01f);
        NIIf varCopy=in;
        if(varCopy < -1.0f)
            varCopy = -1.0f;
        if(varCopy > 1.0f)
            varCopy = 1.0f;
        write((Nui16)((varCopy + 1.0f) * 32767.5f));
    }
    //------------------------------------------------------------------------
    void NetSerializer::writeCompress(NIId in)
    {
        N_assert(in > -1.01 && in < 1.01);
        NIId varCopy=in;
        if(varCopy < -1.0f)
            varCopy = -1.0f;
        if(varCopy > 1.0f)
            varCopy = 1.0f;
        write((Nui32)((varCopy + 1.0) * 2147483648.0));
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::read(Nui32 & out) const
    {
        N_assert((mBitRead & 7) == 0);
        Nui8 * temp = &out;
        if(mBitRead + 4 * 8 > mBitWrite)
            return 0;
    #ifndef N_NET_Endian_N
        if(mFlipEndian)
        {
            temp[0] = mData[(mBitRead >> 3) + 3];
            temp[1] = mData[(mBitRead >> 3) + 2];
            temp[2] = mData[(mBitRead >> 3) + 1];
            temp[3] = mData[(mBitRead >> 3) + 0];
        }
        else
    #endif
        {
            temp[0] = mData[(mBitRead >> 3) + 0];
            temp[1] = mData[(mBitRead >> 3) + 1];
            temp[2] = mData[(mBitRead >> 3) + 2];
            temp[3] = mData[(mBitRead >> 3) + 3];
        }

        mBitRead += 4 * 8;
        return 4;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::readCompress(NIIf & out) const
    {
        Nui16 compressedFloat;
        if(read(compressedFloat))
        {
            out = ((NIIf)compressedFloat / 32767.5f - 1.0f);
            return 2;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount NetSerializer::readCompress(NIId & out) const
    {
        Nui32 compressedFloat;
        if(read(compressedFloat))
        {
            out = ((NIId)compressedFloat / 2147483648.0 - 1.0);
            return 4;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool NetSerializer::isFlipData()
    {
#ifndef N_NET_Endian_N
        return isNetEndian() == false;
#else
        return false;
#endif
    }
    //------------------------------------------------------------------------
    bool NetSerializer::isNetEndian()
    {
        static const bool isNetworkOrder = (htonl(12345) == 12345);
        return isNetworkOrder;
    }
    //------------------------------------------------------------------------
}