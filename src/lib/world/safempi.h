/*
  This file is part of MADNESS.

  Copyright (C) 2007,2010 Oak Ridge National Laboratory

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  For more information please contact:

  Robert J. Harrison
  Oak Ridge National Laboratory
  One Bethel Valley Road
  P.O. Box 2008, MS-6367

  email: harrisonrj@ornl.gov
  tel:   865-241-3937
  fax:   865-572-0680

  $Id$
*/
#ifndef MADNESS_WORLD_SAFEMPI_H__INCLUDED
#define MADNESS_WORLD_SAFEMPI_H__INCLUDED

/// \file safempi.h
/// \brief Serializes calls to MPI in case it does not support THREAD_MULTIPLE

#include "madness_config.h"

//  Jeff's original comments:
//  It is not safe to undefine this because the MPI mutex protects static variables.
//  One needs thread-local storage or something similar if MPI_THREAD_MULTIPLE is to be used.
//  Jeff's new comments:
//  I can't remember where the static stuff is that scared me but it must be found and properly
//  protected.  We really need to be able to use MPI_THREAD_MULTIPLE on BGQ.
#if MADNESS_MPI_THREAD_LEVEL == MPI_THREAD_SERIALIZED
#  define MADNESS_SERIALIZES_MPI
#endif

#ifdef STUBOUTMPI
#include <world/stubmpi.h>
#else
#include <mpi.h>
#endif

#include <world/worldmutex.h>
#include <world/typestuff.h>
#include <world/enable_if.h>
#include <world/scopedptr.h>
#include <iostream>
#include <cstring>

#define MPI_THREAD_STRING(level)  \
        ( level==MPI_THREAD_SERIALIZED ? "THREAD_SERIALIZED" : \
            ( level==MPI_THREAD_MULTIPLE ? "THREAD_MULTIPLE" : \
                ( level==MPI_THREAD_FUNNELED ? "THREAD_FUNNELED" : \
                    ( level==MPI_THREAD_SINGLE ? "THREAD_SINGLE" : "THREAD_UNKNOWN" ) ) ) )

#define MADNESS_MPI_TEST(condition) \
    { \
        int mpi_error_code = condition; \
        if(mpi_error_code != MPI_SUCCESS) throw ::SafeMPI::Exception(mpi_error_code); \
    }

namespace SafeMPI {

#ifdef MADNESS_SERIALIZES_MPI
    extern madness::SCALABLE_MUTEX_TYPE charon;      // Inside safempi.cc
#define SAFE_MPI_GLOBAL_MUTEX madness::ScopedMutex<madness::SCALABLE_MUTEX_TYPE> obolus(SafeMPI::charon)
#else
#define SAFE_MPI_GLOBAL_MUTEX
#endif

    /// tags in [1,999] ... allocated once by unique_reserved_tag
    ///
    /// tags in [1000,1023] ... statically assigned here
    ///
    /// tags in [1024,4095] ... allocated round-robin by unique_tag
    ///
    /// tags in [4096,MPI::TAG_UB] ... not used/managed by madness

    static const int RMI_TAG = 1023;
    static const int RMI_HUGE_ACK_TAG = 1022;
    static const int RMI_HUGE_DAT_TAG = 1021;
    static const int MPIAR_TAG = 1001;
    static const int DEFAULT_SEND_RECV_TAG = 1000;

    /**
     * analogous to MPI_Init_thread
     * @param argc the number of arguments in argv
     * @param argv the vector of command-line arguments
     * @param requested the desired thread level
     * @return provided thread level
     */
    inline int Init_thread(int & argc, char **& argv, int requested);
    /**
     * analogous to MPI_Init_thread
     * @param requested the desired thread level
     * @return provided thread level
     */
    inline int Init_thread(int requested);
    /**
     * analogous to MPI_Query_thread
     * @return the MPI thread level provided by SafeMPI::Init_thread()
     */
    inline int Query_thread();
    /**
     * analogous to MPI_Init
     */
    inline void Init();
    /**
     * analogous to MPI_Init
     * @param argc the number of arguments in argv
     * @param argv the vector of command-line arguments
     */
    inline void Init(int &argc, char **&argv);
    /**
     * analogous to MPI_Finalize. This returns status rather than throw an exception upon failure because
     * this is a "destructor", and throwing from destructors is evil.
     * @return 0 if successful, nonzero otherwise (see MPI_Finalize() for the return codes).
     */
    inline int Finalize();
    /**
     * analogous to MPI_Finalized()
     * @return true is SafeMPI::Finalize() has been called, false otherwise
     */
    inline bool Is_finalized();

    class Exception : public std::exception {
    private:
        char mpi_error_string_[MPI_MAX_ERROR_STRING];

    public:

        Exception(const int mpi_error) throw() {
            int len;
            if(MPI_Error_string(mpi_error, mpi_error_string_, &len) != MPI_SUCCESS)
                std::strncpy(mpi_error_string_, "UNKNOWN MPI ERROR!", MPI_MAX_ERROR_STRING);
        }

        Exception(const Exception& other) throw() {
            std::strncpy(mpi_error_string_, other.mpi_error_string_, MPI_MAX_ERROR_STRING);
        }

        Exception& operator=(const Exception& other) {
            std::strncpy(mpi_error_string_, other.mpi_error_string_, MPI_MAX_ERROR_STRING);
            return *this;
        }

        virtual ~Exception() throw() { }

        virtual const char* what() const throw() {
            return mpi_error_string_;
        }

        friend std::ostream& operator<<(std::ostream& os, const Exception& e) {
            os << e.what();
            return os;
        }
    }; // class Exception


    class Status {
    private:
        MPI_Status status_;

    public:
        // Constructors
        Status(void) : status_() { }
        Status(const Status &other) : status_(other.status_) { }
        Status(MPI_Status other) : status_(other) { }

        // Assignment operators
        Status& operator=(const Status &other) {
            status_ = other.status_;
            return *this;
        }

        Status& operator=(const MPI_Status other) {
            status_ = other;
            return *this;
        }

        // C/C++ cast and assignment
        operator MPI_Status*() { return &status_; }

        operator MPI_Status() const { return status_; }

//        bool Is_cancelled() const {
//            int flag = 0;
//            MADNESS_MPI_TEST(MPI_Test_cancelled(const_cast<MPI_Status*>(&status_), &flag));
//            return flag != 0;
//        }
//
//        int Get_elements(const MPI_Datatype datatype) const {
//            int elements = 0;
//            MADNESS_MPI_TEST(MPI_Get_elements(const_cast<MPI_Status*>(&status_), datatype, &elements));
//            return elements;
//        }

        int Get_count(const MPI_Datatype datatype) const {
            int count = 0;
            MADNESS_MPI_TEST(MPI_Get_count(const_cast<MPI_Status*>(&status_), datatype, &count));
            return count;
        }

//        void Set_cancelled(bool flag) {
//            MADNESS_MPI_TEST(MPI_Status_set_cancelled(&status_, flag));
//        }
//
//        void Set_elements( const MPI_Datatype &v2, int v3 ) {
//            MADNESS_MPI_TEST(MPI_Status_set_elements(&status_, v2, v3 ));
//        }

        int Get_source() const { return status_.MPI_SOURCE; }

        int Get_tag() const { return status_.MPI_TAG; }

        int Get_error() const { return status_.MPI_ERROR; }

        void Set_source(int source) { status_.MPI_SOURCE = source; }

        void Set_tag(int tag) { status_.MPI_TAG = tag; }

        void Set_error(int error) { status_.MPI_ERROR = error; }
    }; // class Status

    class Request {
        // Note: This class was previously derived from MPI::Request, but this
        // was changed with the removal of the MPI C++ bindings. Now this class
        // only implements the minumum functionality required by MADNESS. Feel
        // free to add more functionality as needed.

    private:
        MPI_Request request_;

    public:

        // Constructors
        Request() : request_(MPI_REQUEST_NULL) { }
        Request(MPI_Request other) : request_(other) { }
        Request(const Request& other) : request_(other.request_) { }

        // Assignment operators
        Request& operator=(const Request &other) {
            request_ = other.request_;
            return *this;
        }

        Request& operator=(const MPI_Request& other) {
            request_ = other;
            return *this;
        }

        // logical
        bool operator==(const Request &other) { return (request_ == other.request_); }
        bool operator!=(const Request &other) { return (request_ != other.request_); }

        // C/C++ cast and assignment
        operator MPI_Request*() { return &request_; }
        operator MPI_Request() const { return request_; }

        static bool Testany(int count, Request* requests, int& index, Status& status) {
            MADNESS_ASSERT(requests != NULL);
            int flag;
            madness::ScopedArray<MPI_Request> mpi_requests(new MPI_Request[count]);

            // Copy requests to an array that can be used by MPI
            for(int i = 0; i < count; ++i)
                mpi_requests[i] = requests[i].request_;
            {
                SAFE_MPI_GLOBAL_MUTEX;
                MADNESS_MPI_TEST(MPI_Testany(count, mpi_requests.get(), &index, &flag, status));
            }
            // Copy results from MPI back to the original array
            for(int i = 0; i < count; ++i)
                requests[i].request_ = mpi_requests[i];
            return flag != 0;
        }

        static bool Testany(int count, Request* requests, int& index) {
            MADNESS_ASSERT(requests != NULL);
            int flag;
            madness::ScopedArray<MPI_Request> mpi_requests(new MPI_Request[count]);

            // Copy requests to an array that can be used by MPI
            for(int i = 0; i < count; ++i)
                mpi_requests[i] = requests[i].request_;
            {
                SAFE_MPI_GLOBAL_MUTEX;
                MADNESS_MPI_TEST(MPI_Testany(count, mpi_requests.get(), &index, &flag, MPI_STATUS_IGNORE));
            }
            // Copy results from MPI back to the original array
            for(int i = 0; i < count; ++i)
                requests[i] = mpi_requests[i];
            return flag != 0;
        }

        static int Testsome(int incount, Request* requests, int* indices, Status* statuses) {
            MADNESS_ASSERT(requests != NULL);
            MADNESS_ASSERT(indices != NULL);
            MADNESS_ASSERT(statuses != NULL);

            int outcount = 0;
            madness::ScopedArray<MPI_Request> mpi_requests(new MPI_Request[incount]);
            madness::ScopedArray<MPI_Status> mpi_statuses(new MPI_Status[incount]);
            for(int i = 0; i < incount; ++i)
                mpi_requests[i] = requests[i].request_;
            {
                SAFE_MPI_GLOBAL_MUTEX;
                MADNESS_MPI_TEST( MPI_Testsome( incount, mpi_requests.get(), &outcount, indices, mpi_statuses.get()));
            }
            for(int i = 0; i < incount; ++i) {
                requests[i] = mpi_requests[i];
                statuses[i] = mpi_statuses[i];
            }
            return outcount;
        }

        static int Testsome(int incount, Request* requests, int* indices) {
            int outcount = 0;
            madness::ScopedArray<MPI_Request> mpi_requests(new MPI_Request[incount]);
            for(int i = 0; i < incount; ++i)
                mpi_requests[i] = requests[i].request_;
            {
                SAFE_MPI_GLOBAL_MUTEX;
                MADNESS_MPI_TEST( MPI_Testsome( incount, mpi_requests.get(), &outcount, indices, MPI_STATUSES_IGNORE));
            }
            for(int i = 0; i < incount; ++i)
                requests[i] = mpi_requests[i];
            return outcount;
        }


        bool Test_got_lock_already(MPI_Status& status) {
            int flag;
            MADNESS_MPI_TEST(MPI_Test(&request_, &flag, &status));
            return flag != 0;
        }

        bool Test(MPI_Status& status) {
            SAFE_MPI_GLOBAL_MUTEX;
            return Test_got_lock_already(status);
        }

        bool Test_got_lock_already() {
            int flag;
            MADNESS_MPI_TEST(MPI_Test(&request_, &flag, MPI_STATUS_IGNORE));
            return flag != 0;
        }

        bool Test() {
            SAFE_MPI_GLOBAL_MUTEX;
            return Test_got_lock_already();
        }
    }; // class Request

    ///  Wrapper around MPI_Group. Has a shallow copy constructor. Usually deep copy is not needed, but can be created
    ///  via Group::Incl().
    class Group {
      public:
        Group Incl(int n, const int* ranks) const {
          // MPI <3 interface lacks explicit const sanitation
          Group result(std::shared_ptr<Impl>(new Impl(*impl, n, const_cast<int*>(ranks))));
          return result;
        }

        void Translate_ranks(int nproc, const int* ranks1, const Group& grp2, int* ranks2) const {
          // MPI <3 interface lacks explicit const sanitation
          MADNESS_MPI_TEST(MPI_Group_translate_ranks(this->impl->group, nproc, const_cast<int*>(ranks1),
                                                     grp2.impl->group, ranks2));
        }

        MPI_Group group() const {
          return impl->group;
        }

      private:

        struct Impl {
            MPI_Group group;

            Impl(MPI_Comm comm) {
              MADNESS_MPI_TEST(MPI_Comm_group(comm, &group));
            }

            Impl(const Impl& other, int n, const int* ranks) {
              // MPI <3 interface lacks explicit const sanitation
              MADNESS_MPI_TEST(MPI_Group_incl(other.group, n, const_cast<int*>(ranks), &this->group));
            }

            ~Impl() {
              int initialized;  MPI_Initialized(&initialized);
              if (initialized) {
                MADNESS_MPI_TEST(MPI_Group_free(&group));
              }
            }

        };

        friend class Intracomm;
        Group() {}
        // only Intracomm will use this
        Group(MPI_Comm comm) : impl(new Impl(comm)) {
        }
        // only myself will use this
        Group(const std::shared_ptr<Impl>& i) : impl(i) {
        }

        std::shared_ptr<Impl> impl;
    };

    class Intracomm;
    extern Intracomm COMM_WORLD;

    ///  Wrapper around MPI_Comm. Has a shallow copy constructor; use Create(Get_group()) for deep copy
    class Intracomm {
    	struct Impl {
	        MPI_Comm comm;
	        int me;
    	    int numproc;

    	    volatile int utag;
    	    volatile int urtag;

    	    Impl(const MPI_Comm& c, int m, int n) :
    	        comm(c), me(m), numproc(n), utag(1024), urtag(1)
    	    { }

    	    ~Impl() {
              int initialized;  MPI_Initialized(&initialized);
              int finalized;  MPI_Finalized(&finalized);
              if (initialized && !finalized) {
                int compare_result;
                const int result = MPI_Comm_compare(comm, MPI_COMM_WORLD, &compare_result);
                if (result == MPI_SUCCESS && compare_result != MPI_IDENT)
                  MPI_Comm_free(&comm);
              }
    	    }

            /// Returns a unique tag for temporary use (1023<tag<=4095)

            /// These tags are intended for one time use to avoid tag
            /// collisions with other messages around the same time period.
            /// It simply increments/wraps a counter and returns the next
            /// legal value.
            ///
            /// So that send and receiver agree on the tag all processes
            /// need to call this routine in the same sequence.
            int unique_tag() {
              SAFE_MPI_GLOBAL_MUTEX;
              int result = utag++;
              if (utag >= 4095) utag = 1024;
              return result;
            }

            /// Returns a unique tag reserved for long-term use (0<tag<1000)

            /// Get a tag from this routine for long-term/repeated use.
            ///
            /// Tags in [1000,1023] are statically assigned.
            int unique_reserved_tag() {
                SAFE_MPI_GLOBAL_MUTEX;
                int result = urtag++;
                if (result >= 1000) MADNESS_EXCEPTION( "too many reserved tags in use" , result );
                return result;
            }

        };
    	std::shared_ptr<Impl> pimpl;

        friend void Init(void);
        friend void Init(int &, char **& );
        friend int Init_thread(int);
        friend int Init_thread(int &, char **&, int );

        // For internal use only. Do not try to call this constructor. It is
        // only used to construct Intarcomm in Create().
        Intracomm(const std::shared_ptr<Impl>& i) : pimpl(i) {
        }

    public:
        struct WorldInitObject;

        // For internal use only. Do not try to call this constructor. It is
        // only used to construct COMM_WORLD.
        Intracomm(const WorldInitObject&);

        explicit Intracomm(const MPI_Comm& comm) :
            pimpl()
        {
            int rank = -1;
            int size = -1;
            MADNESS_MPI_TEST(MPI_Comm_rank(comm, &rank));
            MADNESS_MPI_TEST(MPI_Comm_size(comm, &size));
            pimpl.reset(new Impl(comm, rank, size));
        }

        /**
         * This collective operation creates a new Intracomm from an Intracomm::Group object.
         * Must be called by all processes that belong to this communicator, but not all
         * must use the same \c group . Thus this Intracomm can be partitioned into several Intracomm objects
         * with one call.
         *
         * @param group Intracomm::Group describing the Intracomm object to be created (\sa Intracomm::Get_group()
         *              and Intracomm::Group::Incl() )
         * @return a new Intracomm object
         */
        Intracomm Create(Group group) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MPI_Comm group_comm;
            MADNESS_MPI_TEST(MPI_Comm_create(pimpl->comm, group.group(), &group_comm));
            int me; MADNESS_MPI_TEST(MPI_Comm_rank(group_comm, &me));
            int nproc; MADNESS_MPI_TEST(MPI_Comm_size(group_comm, &nproc));
            return Intracomm(std::shared_ptr<Impl>(new Impl(group_comm, me, nproc)));
        }

        bool operator==(const Intracomm& other) const {
            int compare_result;
            const int result = MPI_Comm_compare(pimpl->comm, other.pimpl->comm, &compare_result);
            return ((result == MPI_SUCCESS) && (compare_result == MPI_IDENT));
        }

        /**
         * This local operation returns the Intracomm::Group object corresponding to this intracommunicator
         * @return the Intracomm::Group object corresponding to this intracommunicator
         */
        Group Get_group() const {
            SAFE_MPI_GLOBAL_MUTEX;
            Group group(pimpl->comm);
            return group;
        }

        MPI_Comm& Get_mpi_comm() const {
            return pimpl->comm;
        }

        int Get_rank() const { return pimpl->me; }

        int Get_size() const { return pimpl->numproc; }

        Request Isend(const void* buf, const int count, const MPI_Datatype datatype, const int dest, const int tag) const {
            SAFE_MPI_GLOBAL_MUTEX;
            Request request;
            MADNESS_MPI_TEST(MPI_Isend(const_cast<void*>(buf), count, datatype, dest,tag, pimpl->comm, request));
            return request;
        }

        Request Irecv(void* buf, const int count, const MPI_Datatype datatype, const int src, const int tag) const {
            SAFE_MPI_GLOBAL_MUTEX;
            Request request;
            MADNESS_MPI_TEST(MPI_Irecv(buf, count, datatype, src, tag, pimpl->comm, request));
            return request;
        }

        void Send(const void* buf, const int count, const MPI_Datatype datatype, int dest, int tag) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Send(const_cast<void*>(buf), count, datatype, dest, tag, pimpl->comm));
        }

#ifdef MADNESS_USE_BSEND_ACKS
        void Bsend(const void* buf, size_t count, const MPI_Datatype datatype, int dest, int tag) const {
            SAFE_MPI_GLOBAL_MUTEX;
            if (count>10 || datatype!=MPI_BYTE) MADNESS_EXCEPTION("Bsend: this protocol is only for 1-byte acks", count );
            MADNESS_MPI_TEST(MPI_Bsend(const_cast<void*>(buf), count, datatype, dest, tag, pimpl->comm));
        }
#endif // MADNESS_USE_BSEND_ACKS

        void Recv(void* buf, const int count, const MPI_Datatype datatype, const int source, const int tag, MPI_Status& status) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Recv(buf, count, datatype, source, tag, pimpl->comm, &status));
        }

        void Recv(void* buf, const int count, const MPI_Datatype datatype, const int source, const int tag) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Recv(buf, count, datatype, source, tag, pimpl->comm, MPI_STATUS_IGNORE));
        }

        void Bcast(void* buf, size_t count, const MPI_Datatype datatype, const int root) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Bcast(buf, count, datatype, root, pimpl->comm));
        }

        void Reduce(const void* sendbuf, void* recvbuf, const int count, const MPI_Datatype datatype, const MPI_Op op, const int root) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Reduce(const_cast<void*>(sendbuf), recvbuf, count, datatype, op, root, pimpl->comm));
        }

        void Allreduce(const void* sendbuf, void* recvbuf, const int count, const MPI_Datatype datatype, const MPI_Op op) const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Allreduce(const_cast<void*>(sendbuf), recvbuf, count, datatype, op, pimpl->comm));
        }
        bool Get_attr(int key, void* value) const {
            int flag = 0;
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Comm_get_attr(pimpl->comm, key, value, &flag));
            return flag != 0;
        }

        void Abort(int code=1) const {
            MPI_Abort(pimpl->comm, code);
        }

        bool Is_initialized(void) const {
            int initialized;
            MPI_Initialized(&initialized);
            return (initialized==1);
        }

        void Barrier() const {
            SAFE_MPI_GLOBAL_MUTEX;
            MADNESS_MPI_TEST(MPI_Barrier(pimpl->comm));
        }

        /// Returns a unique tag for temporary use (1023<tag<=4095)

        /// These tags are intended for one time use to avoid tag
        /// collisions with other messages around the same time period.
        /// It simply increments/wraps a counter and returns the next
        /// legal value.
        ///
        /// So that send and receiver agree on the tag all processes
        /// need to call this routine in the same sequence.
        int unique_tag() {
          return pimpl->unique_tag();
        }

        /// Returns a unique tag reserved for long-term use (0<tag<1000)

        /// Get a tag from this routine for long-term/repeated use.
        ///
        /// Tags in [1000,1023] are statically assigned.
        int unique_reserved_tag() {
          return pimpl->unique_reserved_tag();
        }

        // Below here are extensions to MPI but have to be here since overload resolution
        // is not applied across different class scopes ... hence even having Isend with
        // a different signature in a derived class (WorldMpiInterface) hides this interface.
        // Thus, they must all be here.
        //
        // !! All of the routines below call the protected interfaces provided above.
        // !! Please ensure any additional routines follow this convention.
        /// Isend one element ... disabled for pointers to reduce accidental misuse.
        template <class T>
        typename madness::enable_if_c< !std::is_pointer<T>::value, Request>::type
        Isend(const T& datum, int dest, int tag=DEFAULT_SEND_RECV_TAG) const {
            return Isend(&datum, sizeof(T), MPI_BYTE, dest, tag);
        }

        /// Async receive data of up to lenbuf elements from process dest
        template <class T>
        Request
        Irecv(T* buf, int count, int source, int tag=DEFAULT_SEND_RECV_TAG) const {
            return Irecv(buf, count*sizeof(T), MPI_BYTE, source, tag);
        }


        /// Async receive datum from process dest with default tag=1
        template <class T>
        typename madness::enable_if_c< !std::is_pointer<T>::value, Request>::type
        Irecv(T& buf, int source, int tag=DEFAULT_SEND_RECV_TAG) const {
            return Irecv(&buf, sizeof(T), MPI_BYTE, source, tag);
        }


        /// Send array of lenbuf elements to process dest
        template <class T>
        void Send(const T* buf, long lenbuf, int dest, int tag=DEFAULT_SEND_RECV_TAG) const {
            Send((void*)buf, lenbuf*sizeof(T), MPI_BYTE, dest, tag);
        }


        /// Send element to process dest with default tag=1001

        /// Disabled for pointers to reduce accidental misuse.
        template <class T>
        typename madness::enable_if_c< !std::is_pointer<T>::value, void>::type
        Send(const T& datum, int dest, int tag=DEFAULT_SEND_RECV_TAG) const {
            Send((void*)&datum, sizeof(T), MPI_BYTE, dest, tag);
        }


        /// Receive data of up to lenbuf elements from process dest
        template <class T>
        void
        Recv(T* buf, long lenbuf, int src, int tag) const {
            Recv(buf, lenbuf*sizeof(T), MPI_BYTE, src, tag);
        }

        /// Receive data of up to lenbuf elements from process dest with status
        template <class T>
        void
        Recv(T* buf, long lenbuf, int src, int tag, Status& status) const {
            Recv(buf, lenbuf*sizeof(T), MPI_BYTE, src, tag, status);
        }


        /// Receive datum from process src
        template <class T>
        typename madness::enable_if_c< !std::is_pointer<T>::value, void>::type
        Recv(T& buf, int src, int tag=DEFAULT_SEND_RECV_TAG) const {
            Recv(&buf, sizeof(T), MPI_BYTE, src, tag);
        }


        /// MPI broadcast an array of count elements

        /// NB.  Read documentation about interaction of MPI collectives and AM/task handling.
        template <class T>
        void Bcast(T* buffer, int count, int root) const {
            Bcast(buffer,count*sizeof(T),MPI_BYTE,root);
        }


        /// MPI broadcast a datum

        /// NB.  Read documentation about interaction of MPI collectives and AM/task handling.
        template <class T>
        typename madness::enable_if_c< !std::is_pointer<T>::value, void>::type
        Bcast(T& buffer, int root) const {
            Bcast(&buffer, sizeof(T), MPI_BYTE,root);
        }

        int rank() const { return Get_rank(); }

        int nproc() const { return Get_size(); }

        int size() const { return Get_size(); }


        /// Construct info about a binary tree with given root

        /// Constructs a binary tree spanning the communicator with
        /// process root as the root of the tree.  Returns the logical
        /// parent and children in the tree of the calling process.  If
        /// there is no parent/child the value -1 will be set.
        void binary_tree_info(int root, int& parent, int& child0, int& child1);

    }; // class Intracomm

    inline int Init_thread(int & argc, char **& argv, int required) {
        int provided = 0;
        MADNESS_MPI_TEST(MPI_Init_thread(&argc, &argv, required, &provided));
        MADNESS_MPI_TEST(MPI_Comm_rank(COMM_WORLD.pimpl->comm, & COMM_WORLD.pimpl->me));
        MADNESS_MPI_TEST(MPI_Comm_size(COMM_WORLD.pimpl->comm, & COMM_WORLD.pimpl->numproc));
        if((provided < required) && (COMM_WORLD.Get_rank() == 0)) {
            std::cout << "!! Error: MPI_Init_thread did not provide requested functionality: "
                      << MPI_THREAD_STRING(required) << " (" << MPI_THREAD_STRING(provided) << "). \n"
                      << "!! Error: The MPI standard makes no guarantee about the correctness of a program in such circumstances. \n"
                      << "!! Error: Please reconfigure your MPI to provide the proper thread support. \n"
                      << std::endl;
            COMM_WORLD.Abort(1);
        } else if (provided > required && COMM_WORLD.Get_rank() == 0 ) {
            std::cout << "!! Warning: MPI_Init_thread provided more than the requested functionality: "
                      << MPI_THREAD_STRING(required) << " (" << MPI_THREAD_STRING(provided) << "). \n"
                      << "!! Warning: You are likely using an MPI implementation with mediocre thread support. \n"
                      << std::endl;
        }
#if defined(MVAPICH2_VERSION)
        char * mv2_string;
        int mv2_affinity = 1; /* this is the default behavior of MVAPICH2 */

        if ((mv2_string = getenv("MV2_ENABLE_AFFINITY")) != NULL) {
            mv2_affinity = atoi(mv2_string);
        }

        if (mv2_affinity!=0) {
            std::cout << "!! Error: You are using MVAPICH2 with affinity enabled, probably by default. \n"
                      << "!! Error: This will cause catastrophic performance issues in MADNESS. \n"
                      << "!! Error: Rerun your job with MV2_ENABLE_AFFINITY=0 \n"
                      << std::endl;
            COMM_WORLD.Abort(1);
        }
#endif // defined(MVAPICH2_VERSION)
        return provided;
    }

    inline int Init_thread(int required) {
        int argc = 0;
        char** argv = NULL;
        return Init_thread(argc, argv, required);
    }

    inline int Query_thread() {
      int provided;
      MADNESS_MPI_TEST(MPI_Query_thread(&provided));
      return provided;
    }

    inline void Init(int & argc, char **& argv) {
        MADNESS_MPI_TEST(MPI_Init(&argc, &argv));
        MADNESS_MPI_TEST(MPI_Comm_rank(COMM_WORLD.pimpl->comm, & COMM_WORLD.pimpl->me));
        MADNESS_MPI_TEST(MPI_Comm_size(COMM_WORLD.pimpl->comm, & COMM_WORLD.pimpl->numproc));
    }

    inline void Init() {
        int argc = 0;
        char** argv = NULL;
        Init(argc,argv);
    }

    inline int Finalize() { return MPI_Finalize(); }

    inline bool Is_finalized() {
        int flag = 0;
        MPI_Finalized(&flag);
        return flag != 0;
    }

    inline double Wtime() { return MPI_Wtime(); }

    inline void Attach_buffer(void* buffer, int size) {
        MADNESS_MPI_TEST(MPI_Buffer_attach(buffer, size));
    }


    inline int Detach_buffer(void *&buffer) {
        int size = 0;
        MPI_Buffer_detach(&buffer, &size);
        return size;
    }
}

#endif // MADNESS_WORLD_SAFEMPI_H__INCLUDED
