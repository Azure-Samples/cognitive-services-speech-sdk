// blocking_read_write_buffer.h: Implementation declarations for CSpxBlockingReadWriteBuffer
//
//      The BlockingReadWriteBuffer implements the curiously recurring template pattern (CRTP) extending
//      a "ReadWriteBuffer" implementation with BlockingRead DataAccess.
//
//      In "ReadWriteBuffer" implementations without BlockingRead DataAccess, two forms of DataAccess
//      exist: ExactSize and ReducedSize...
//      (a) ExactSize throws an exception if the specified amount of data is not immediately available.
//      (b) ReducedSize allows the data size available to be smaller than that requested, returning
//          the actual size of the DataAccessed in an output parameter (e.g. size_t* bytesRead).
//
//      For both forms of DataAccess, BlockingRead modifies the standard behavior by blocking the
//      calling thread until the requested amount of data is available instead of throwing an exception or
//      returning less data that requested. Additionally, there is a way to "unblock" the BlockingRead
//      DataAccess triggering the original behavior of the non-BlockingRead DataAccess pattern above with
//      whatever data is already available.
//
//      Thus, ExactSize BlockingRead DataAccess operates as follows:
//      (1) Go to next step if enough data is already available, otherwise wait for data to be available.
//      (2) Once enough data is available, return the data (via CallerSupplied or CalleeManaged DataBuffers).
//      (3) If, while waiting in step #1, someone signals the UnblockTrigger, throw an exception immediately.
//
//      Whereas, ReducedSize BlockingRead DataAccess operates as follows:
//      (1) Go to next step if enough data is already available, otherwise wait for data to be available.
//      (2) Once enough data is available, return the data (via CallerSupplied or CalleeManaged DataBuffers).
//      (3) If, while waiting in step #1, someone signals the UnblockTrigger, return whatever data is
//          already available, returning the actual size of the DataAccessed in the bytesRead output parameter.
//
//      From the BlockingReadWriteBuffer's POV:
//      - UnblockTrigger - means when Caller FromAbove calls ISpxReadWriteBuffer::Write(nullptr, 0)
//
//      NOTE:
//      - ISpxReadWriteBuffer::ReadAtBytePos and ISpxReadWriteBuffer::ReadSharedAtBytePos are unaltered by
//        this BlockingReadWriteBuffer implementation.
//

// ROBCH: Introduced in AUDIO.V3


// read_write_ring_buffer.cpp: Implementation definitions for CSpxReadWriteRingBuffer C++ class
//
//      The ReadWriteRingBuffer (RB) owns and manages a conceptually infinite memory buffer
//      into/from which clients can write/read data.
//
//      Data can be accessed:
//      (1) Sequentially via an internally maintained ReadPtr/ReadPos, or
//      (2) Randomly by caller supplied Pos, if the data is still available in the SlidingWindow.
//      (3) Using CallerSupplied (copies the data) or CalleeManaged (view into DataStorage) DataBuffers.
//      (4) Using ExactSize or ReducedSize DataAccess and DataStorage.
//
//      Additional Caller requirements/guarantees:
//      (1) TwoPhaseInitialization required via ISpxReadWriteBufferInit::SetSize().
//      (2) Optional TwoPhaseDestruction via ISpxObjectInit::Term().
//      (3) Maintains a copy of the data supplied via ISpxReadWriteBuffer::Write().
//      (4) ExactSize DataAccess invocations ThrowExceptions if not enough data available.
//
//      From RB's POV:
//
//      - DataStorage - means ISpxReadWriteBuffer:Write() method.
//      - ExactSize/ReducedSize DataStorage
//        - ExactSize DataStorage is indiciated by null bytesWritten pointer.
//        - ReducedSize DataStorage is indiciated by non-null bytesWritten pointer.
//        - Two different models of DataStorage
//          (a) ExactSize requires that the size of data requested to be written by the caller, must
//              fit inside the SlidingWindow. If it does not, an exception is thrown.
//          (b) ReducedSize removes the exception, and instead always returns successfully, indicating
//              the number of bytes actually written via the non-null bytesWritten pointer supplied
//              in the DataStorage method by the caller. Thus the amount of data actually stored is
//              governed by the max(0, space left in SlidingWindow) and min(amount requested, space left
//              in the SlidingWindow).
//
//      - DataAccess - means ISpxReadWriteBuffer::Read*() methods.
//      - ExactSize/ReducedSize DataAccess
//        - ExactSize DataAccess is indiciated by null bytesRead pointer.
//        - ReducedSize DataAccess is indiciated by non-null bytesRead pointer.
//        - Two different models of DataAccess
//          (a) ExactSize requires the size of data requested by caller to be available; if less
//              data is available, an exception is thrown. If at least that much data is available,
//              the DataAccess method returns exactly that number of bytes to the caller.
//          (b) ReducedSize removes the exception, and instead always returns successfully, indicating
//              the number of bytes actually read via the non-null bytesRead pointer supplied in the
//              DataAccess method by the caller. Thus the amount of data returned is governed by the
//              max(0, amount available) and min(amount available, amount requested).
//
//      - SlidingWindow - data still available in the RB's managed buffer
//        - "to the left of" WritePtr/WritePos
//        - "to the right of" max(InitPos, GetWritePos() - GetSize())
//
//      - CallerSupplied/CalleeManaged DataBuffers
//        (a) CallerSupplied DataBuffers - means ```data``` parameter in Read()/ReadAtBytePos() methods.
//        (b) CalleeManaged DataBuffers - means both
//            - SpxSharedUint8Buffer_Type RingBuffer, and/or
//            - std::shared_ptr<uint8_t> returned from ReadShared*()
//
//      NOTEs:
//      - ReadPtr/ReadPos cannot advance past WritePtr/WritePos
//      - WritePtr/WritePos cannot advance past ReadPtr/ReadPos + GetSize()
//

// ROBCH: Introduced in AUDIO.V3
