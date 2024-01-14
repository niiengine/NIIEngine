#include "OISConfig.h"
#ifdef OIS_WIN32_WIIMOTE_SUPPORT
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
#ifndef OIS_WiiMoteRingBuffer_H
#define OIS_WiiMoteRingBuffer_H

#include "OISPrereqs.h"

namespace OIS
{
	struct WiiMoteEvent
	{
		//! (7 buttons) If a button was just pressed, the bit will be set
		unsigned int pushedButtons;

		//! (7 buttons) If a button was just released, the bit will be set
		unsigned int releasedButtons;

		//! Will be true if POV changed this event
		bool povChanged;

		//! Will be valid if povChanged = true
		unsigned int povDirection;

		//! Will be valid if a movement just occurred on main motion sensing
		bool movement;

		//Values of main orientation vector
		float x, y, z;

		//! Will be valid if a movement just occurred on main motion sensing
		bool movementChuck;

		//Values of main orientation vector
		float nunChuckx, nunChucky, nunChuckz;

		//Used to flag when a Nunchuck axis moved
		bool nunChuckXAxisMoved, nunChuckYAxisMoved;

		//Values of NunChuck JoyStick
		int nunChuckXAxis, nunChuckYAxis;

		//! clear initial state
		void clear()
		{
			pushedButtons = releasedButtons = 0;
			povChanged = false;
			povDirection = 0;

			movement = false;
			x = y = z = 0.0f;

			nunChuckx = nunChucky = nunChuckz = 0;
			movementChuck = false;

			nunChuckXAxisMoved = nunChuckYAxisMoved = false;
			nunChuckXAxis = nunChuckYAxis = 0;
		}
	};

	/// <summary>
	/// Ring Buffer (fifo) used to store 16bit pcm data
	/// </summary>
	class WiiMoteRingBuffer
	{
	private:
		//! Number of bytes in FIFO. Power of 2. Set by RingBuffer_Init
		int bufferSize;
		//! Used for wrapping indices with extra bit to distinguish full/empty.
		int bigMask;
		// Used for fitting indices to buffer.
		int smallMask;

		// Buffer holding the actual event buffers
		WiiMoteEvent *buffer;

		//! Index of next writable byte. Set by RingBuffer_AdvanceWriteIndex.
		volatile int writeIndex; 
		
		//! Index of next readable byte. Set by RingBuffer_AdvanceReadIndex.
		volatile int readIndex;	

	public:
		WiiMoteRingBuffer(unsigned int numEntries)
		{
			numEntries = RoundUpToNextPowerOf2(numEntries);

			//2 bytes per short
			bufferSize = (int)numEntries;
			buffer = new WiiMoteEvent[numEntries];
		
			Flush();
		
			bigMask = (int)(numEntries*2)-1;
			smallMask = (int)(numEntries)-1;
		}

		~WiiMoteRingBuffer()
		{
			delete buffer;
		}

		unsigned int RoundUpToNextPowerOf2( unsigned int n )
		{
			int numBits = 0;
			if( ((n-1) & n) == 0) 
			return n; //Already Power of two.

			while( n > 0 )
			{
				n= n>>1;
				numBits++;
			}
			return (unsigned int)(1<<numBits);
		}


		int GetReadAvailable( )
		{
			return ( (writeIndex - readIndex) & bigMask );
		}


		int GetWriteAvailable( )
		{
			return ( bufferSize - GetReadAvailable());
		}


		int Write( WiiMoteEvent *data, int numEntries )
		{
			int size1 = 0, size2 = 0, numWritten;
			int data1Ptr = 0, data2Ptr = 0;
			
			numWritten = GetWriteRegions( numEntries, data1Ptr, size1, data2Ptr, size2 );

			if( size2 > 0 )
			{
				//copy to two parts
				memcpy( &buffer[data1Ptr], data, sizeof(WiiMoteEvent) * size1 );
				//Array.Copy( data, offsetPtr, buffer, data1Ptr, size1 );
				memcpy( &buffer[data2Ptr], &data[size1], sizeof(WiiMoteEvent) * size2 );
				//Array.Copy( data, offsetPtr + size1, buffer, data2Ptr, size2 );
			}
			else
			{	//Copy all continous
				memcpy( &buffer[data1Ptr], data, sizeof(WiiMoteEvent) * size1 );
				//Array.Copy( data, offsetPtr, buffer, data1Ptr, size1 );
			}
			AdvanceWriteIndex( numWritten );
			return numWritten;
		}


		/// <summary>
		/// Reads requested number of entries into sent array.
		/// Returns number written
		/// </summary>
		int Read( WiiMoteEvent *data, int numEntries )
		{
			int size1 = 0, size2 = 0, numRead, data1Ptr = 0, data2Ptr = 0;
			
			numRead = GetReadRegions( numEntries, data1Ptr, size1, data2Ptr, size2 );
			
			if( size2 > 0 )
			{
				memcpy( data, &buffer[data1Ptr], sizeof(WiiMoteEvent) * size1 );
				//Array.Copy( buffer, data1Ptr, data, 0, size1 );
				memcpy( &data[size1], &buffer[data2Ptr], sizeof(WiiMoteEvent) * size2 );
				//Array.Copy( buffer, data2Ptr, data, size1, size2 );
			}
			else
				memcpy( data, &buffer[data1Ptr], sizeof(WiiMoteEvent) * size1 );
				//Array.Copy( buffer, data1Ptr, data, 0, size1 );

			AdvanceReadIndex( numRead );
			return numRead;
		}

	private:

		int GetWriteRegions( int numEntries, int &dataPtr1, int &sizePtr1,
							 int &dataPtr2, int &sizePtr2 )
		{
			int   index;
			int   available = GetWriteAvailable();
			if( numEntries > available ) 
				numEntries = available;
		
			//Check to see if write is not contiguous.
			index = writeIndex & smallMask;
			if( (index + numEntries) > bufferSize )
			{
				//Write data in two blocks that wrap the buffer.
				int   firstHalf = bufferSize - index;
				dataPtr1 = index;//&buffer[index];
				sizePtr1 = firstHalf;
				dataPtr2 = 0;//&buffer[0];
				sizePtr2 = numEntries - firstHalf;
			}
			else
			{
				dataPtr1 = index;//&buffer[index];
				sizePtr1 = numEntries;
				dataPtr2 = 0;
				sizePtr2 = 0;
			}
			return numEntries;
		}
	

		int GetReadRegions( int numEntries, int &dataPtr1, int &sizePtr1, int &dataPtr2, int &sizePtr2 )
		{
			int   index;
			int   available = GetReadAvailable( );
			if( numEntries > available ) 
				numEntries = available;
			
			// Check to see if read is not contiguous
			index = readIndex & smallMask;
			if( (index + numEntries) > bufferSize )
			{
				// Write data in two blocks that wrap the buffer
				int firstHalf = bufferSize - index;
				dataPtr1 = index;//&buffer[index];
				sizePtr1 = firstHalf;
				dataPtr2 = 0;//&buffer[0];
				sizePtr2 = numEntries - firstHalf;
			}
			else
			{
				dataPtr1 = index;//&buffer[index];
				sizePtr1 = numEntries;
				dataPtr2 = 0;
				sizePtr2 = 0;
			}
			return numEntries;
		}


		int AdvanceWriteIndex( int numEntries )
		{
			 return writeIndex = (writeIndex + numEntries) & bigMask;
		}


		int AdvanceReadIndex( int numEntries )
		{
			return readIndex = (readIndex + numEntries) & bigMask;
		}


		void Flush( )
		{
			writeIndex = readIndex = 0;
		}
	};
}
#endif //#define OIS_WiiMoteRingBuffer_H
#endif
