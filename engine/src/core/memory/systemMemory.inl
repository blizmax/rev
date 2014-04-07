//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Manage memory at the base level
#ifndef _REV_CORE_MEMORY_SYSTEMMEMORY_INL_
#define _REV_CORE_MEMORY_SYSTEMMEMORY_INL_

#include <cassert>
#include "systemMemory.h"

namespace rev {
	namespace core {

		//----------------------------------------------------------------------------------------------------------------------
		inline SystemMemory* SystemMemory::get() {
			assert(sInstance);
			return sInstance;
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<typename Alloc_>
		void SystemMemory::startUp(Alloc_& _alloc) {
			assert(!sInstance);

			// Define local class using allocator
			class SystemMemoryFinal : public SystemMemory {
				Alloc_& mAlloc;
				// This should be "=delete", but Visual studio won't swallow that.
				SystemMemoryFinal& operator=(const SystemMemoryFinal&){ return *this; }
			public:
				SystemMemoryFinal(Alloc_* _alloc) : mAlloc(*_alloc) {}

				void*	allocBuffer(size_t _size) {
					return mAlloc.allocate<>(_size);
				}
				void	freeBuffer(const void* _ptr, size_t _size) {
					mAlloc.deallocate(_ptr, _size);
				}
			};

			sInstance = _alloc.create<SystemMemoryFinal>(&_alloc);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<typename Alloc_>
		void SystemMemory::shutDown(Alloc_& _alloc) {
			assert(sInstance);
			_alloc.destroy(sInstance);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_* SystemMemory::allocate(size_t _n) {
			assert(sInstance);
			void* buffer = sInstance->allocBuffer(_n * sizeof(T_));
			return reinterpret_cast<T_*>(buffer);
		}

		//----------------------------------------------------------------------------------------------------------------------
		template<class T_>
		void SystemMemory::deallocate(const T_* _ptr, size_t _n) {
			assert(sInstance);
			sInstance->freeBuffer(reinterpret_cast<const void*>(_ptr), _n);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_MEMORY_SYSTEMMEMORY_INL_