//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Simple allocator using new and delete
#ifndef _REV_CORE_MEMORY_NEWALLOCATOR_INL_
#define _REV_CORE_MEMORY_NEWALLOCATOR_INL_

#include "newAllocator.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		template<class T_, typename ... Args_>
		T_*	NewAllocator::create(Args_ ... _args) {
			return new T_(_args...);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class T_>
		void NewAllocator::destroy(const T_* _ptr) {
			delete _ptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<>
		void * NewAllocator::allocate(unsigned _n) {
			void* buffer = new char[_n];
			return buffer;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_* NewAllocator::allocate(unsigned _n) {
			void* buffer = new char[_n*sizeof(T_)];
			return reinterpret_cast<T_*>(buffer);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class T_>
		void NewAllocator::deallocate(const T_* _ptr, unsigned) {
			delete[] reinterpret_cast<const char*>(_ptr);
		}

	} // namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_NEWALLOCATOR_INL_