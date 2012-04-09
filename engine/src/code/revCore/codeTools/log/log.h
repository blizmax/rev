////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic log interface

#ifndef _REV_CORE_CODETOOLS_LOG_LOG_H_
#define _REV_CORE_CODETOOLS_LOG_LOG_H_

#include <revCore/string.h>
#include <vector.h>

namespace rev
{
	enum ELogChannel
	{
		eCommon,
		eError,
		eAssert,
		eVerbose,
		eParanoid,
		eGP1,
		eGP2,
		eGP3,

		eMaxLogChannel
	};

	namespace codeTools
	{
		class SLog
		{
		public:
			// Singleton life cycle
			static	void	init	();
			static	void	end		();
			static	SLog*	get		();

		public:
			// Common use interface
			template < class T >
			void	log		( T _msg, ELogChannel _out );
			void	newLine	(ELogChannel _out);

			// Extra management
			void	enableGlobalLog		()	{	mEnableGlobal = true;	}
			void	disableGlobalLog	()	{	mEnableGlobal = false;	}
			void	enableChannel		(ELogChannel _channel)	{ mEnableChannel[_channel] = true; }
			void	disableChannel		(ELogChannel _channel)	{ mEnableChannel[_channel] = false;}
			void	flush				();
			void	setBufferCapacity	(unsigned _capacity);

		private:
			typedef char*	messageT;	// Channel, message text
		
			bool					mEnableGlobal;
			bool					mEnableChannel[eMaxLogChannel];
			rtl::vector<messageT>	mBuffer;
			unsigned				mBufferCapacity;

		private:
			// Singleton instantiation
			SLog();
			~SLog();
			static SLog * sInstance;
		};
	

		//--------------------------------------------------------------------------------------------------------------
		template<class T>
		void SLog::log( T _msg, ELogChannel _channel )
		{
			if(mEnableGlobal && mEnableChannel[_channel])
			{
				mBuffer.push_back(makeString(_msg));
				if(mBuffer.size() >= mBufferCapacity)
					flush();
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		// const char * specialization
		//--------------------------------------------------------------------------------------------------------------
		template<>
		void SLog::log(const char * _msg, ELogChannel _channel);

	}	// namespace codeTools

#ifdef REV_LOG
//------------------------------------------------------------------------------------------------------------------
template<class T>
void	revLog	( T _msg, ELogChannel _channel = eCommon )
{
	codeTools::SLog * logger = codeTools::SLog::get();
	codeTools::revAssert(0 != logger);
	logger->log(_msg, _channel);
}
	
//------------------------------------------------------------------------------------------------------------------
template<class T>
void	revLogN	( T _msg, ELogChannel _channel = eCommon )
{
	codeTools::SLog * logger = codeTools::SLog::get();
	codeTools::revAssert(0 != logger);
	logger->log(_msg, _channel);
	logger->newLine(_channel);
}
#else // ! REV_LOG
	// When log is not enabled, delete it in preprocessing.
//------------------------------------------------------------------------------------------------------------------
template<class T>
void	revLog	( T _msg, ELogChannel _channel = eCommon )
{
	_msg;
	_channel;
}
	
//------------------------------------------------------------------------------------------------------------------
template<class T>
void	revLogN	( T _msg, ELogChannel _channel = eCommon )
{
	_msg;
	_channel;
}
#endif // ! REV_LOG

}	// namespace rev

#endif // _REV_CORE_CODETOOLS_LOG_LOG_H_
