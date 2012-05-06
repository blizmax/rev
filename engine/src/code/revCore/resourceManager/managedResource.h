////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on May 4th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Managed resource

#ifndef _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_
#define _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_

#include "resource.h"
#include "resourceManager.h"

namespace rev
{
	template<class _derivedT, class _keyT, bool _autocreate>
	class IManagedResource : public IResource
	{
	public:
		static _derivedT *	get				(_keyT);			// Get a resource by key (implies ownership)
		static void			registerResource(_derivedT*, _keyT);// Register a resource (does not imply ownership)
		static void			release			(_keyT);			// Release a resource by key
		static void			release			(_derivedT*);		// Release a resource given it's pointer

		using IResource::release;	// Unhide IResource::release methods

	public:
		typedef CResourceManager<_derivedT, _keyT>	TManager;
	private:
		static	TManager * manager();
		static 	TManager * sManager;
	};

	// Autocreate specialization
	template<class _derivedT, class _keyT>
	class IManagedResource<_derivedT, _keyT, true> : public IResource
	{
	public:
		static _derivedT *	get				(_keyT);			// Get a resource by key (implies ownership)
		static void			registerResource(_derivedT*, _keyT);// Register a resource (does not imply ownership)
		static void			release			(_keyT);			// Release a resource by key
		static void			release			(_derivedT*);		// Release a resource given it's pointer

		using IResource::release;	// Unhide IResource::release methods

	public:
		typedef CResourceManager<_derivedT, _keyT>	TManager;
	private:
		static	TManager * manager();
		static 	TManager * sManager;
	};

	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	_derivedT * IManagedResource<_derivedT,_keyT,_autocreate>::get(_keyT _x)
	{
		return manager()->get(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT> // Partial specialization for autocreate
	_derivedT * IManagedResource<_derivedT,_keyT,true>::get(_keyT _x)
	{
		TManager * mgr = manager(); // Get resource manager
		_derivedT * res = mgr->get(_x);
		if( ! res )
		{
			res = new _derivedT(_x);
			mgr->registerResource(res, _x);
		}
		return res;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::registerResource(_derivedT * _res, _keyT _x)
	{
		manager()->registerResource(_res, _x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::registerResource(_derivedT * _res, _keyT _x)
	{
		manager()->registerResource(_res, _x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::release(_keyT _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::release(_keyT _x)
	{
		manager()->release(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	void IManagedResource<_derivedT,_keyT,_autocreate>::release(_derivedT * _res)
	{
		manager()->release(_res);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	void IManagedResource<_derivedT,_keyT,true>::release(_derivedT * _res)
	{
		manager()->release(_res);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT, bool _autocreate>
	typename IManagedResource<_derivedT,_keyT,_autocreate>::TManager * IManagedResource<_derivedT,_keyT,_autocreate>::manager()
	{
		if(!sManager)
			sManager = new TManager();
		return sManager;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class _derivedT, class _keyT>
	typename IManagedResource<_derivedT,_keyT,true>::TManager * IManagedResource<_derivedT,_keyT,true>::manager()
	{
		if(!sManager)
			sManager = new TManager();
		return sManager;
	}

}	// namespace rev

#endif // _REV_CORE_RESOURCEMANAGER_MANAGEDRESOURCE_H_
