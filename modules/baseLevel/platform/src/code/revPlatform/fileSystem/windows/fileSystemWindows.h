//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows File system

#ifndef _REV_PLATFORM_FILESYSTEM_WINDOWS_FILESYSTEM_WINDOWS_H_
#define _REV_PLATFORM_FILESYSTEM_WINDOWS_FILESYSTEM_WINDOWS_H_

#ifdef _WIN32

#include "../fileSystem.h"

#include <cstdint>
#include <string>
#include <Windows.h>

namespace rev { namespace platform {

	class FileSystemWindows : public FileSystem
	{
	public:
		static FileSystemWindows* get();

		FileBuffer	getFileAsBuffer	(const char* _fileName);
		void		update			();
	private:
		static FileSystemWindows* sFileSystem;

		FileSystemWindows();
		void doFileChanged(const std::string& _fileName);
		void watchDirectory();

		static const int	mNotifBufferSize = 1024;
		HANDLE			mDirectoryHandle;
		HANDLE			mPortHandle;
		uint32_t		mNotificationBuffer[mNotifBufferSize];
		DWORD			mBytesRecieved;
		OVERLAPPED		mOverlapped;
		ULONG_PTR		mCompletionKey;
	};

}	// namespace platform
}	// namespace rev

#endif // _WIN32

#endif // _REV_PLATFORM_FILESYSTEM_WINDOWS_FILESYSTEM_WINDOWS_H_