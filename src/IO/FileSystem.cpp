/*
 Copyright (C) 2010-2013 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Exceptions.h"
#include "IO/FileSystem.h"

#include <wx/dir.h>

#if defined __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#elif defined _WIN32
#include <Windows.h>
#elif defined __linux__
#endif

namespace TrenchBroom {
    namespace IO {
        Path FileSystem::findRootPath(const Path::List& rootPaths, const Path& relativePath) {
            if (relativePath.isAbsolute())
                throw FileSystemException("Cannot find root path of absolute path");
            
            Path::List::const_iterator it, end;
            for (it = rootPaths.begin(), end = rootPaths.end(); it != end; ++it) {
                const IO::Path& rootPath = *it;
                if (!rootPath.isAbsolute())
                    throw FileSystemException("Root path must be absolute");
                if (exists(rootPath + relativePath))
                    return rootPath;
            }
            return Path("");
        }

        Path FileSystem::resolvePath(const Path::List& rootPaths, const Path& path) {
            if (!path.isAbsolute()) {
                const IO::Path absolutePath = findRootPath(rootPaths, path) + path;
                return absolutePath.makeCanonical();
            } else {
                return path.makeCanonical();
            }
        }

        Path::List FileSystem::resolvePaths(const Path::List& rootPaths, const Path::List& paths) {
            Path::List result;
            Path::List::const_iterator it, end;
            for (it = paths.begin(), end = paths.end(); it != end; ++it) {
                const Path& path = *it;
                if (!path.isAbsolute()) {
                    const IO::Path absolutePath = findRootPath(rootPaths, path) + path;
                    result.push_back(absolutePath.makeCanonical());
                } else {
                    result.push_back(path.makeCanonical());
                }
            }
            return result;
        }

        bool FileSystem::isDirectory(const Path& path) const {
            return ::wxDirExists(path.asString());
        }

        bool FileSystem::exists(const Path& path) const {
            return ::wxFileExists(path.asString()) || ::wxDirExists(path.asString());
        }

        Path::List FileSystem::directoryContents(const Path& path, const FileSystemFilter contentFilter, const String& namePattern) const {
            if (!isDirectory(path.asString())) {
                FileSystemException e;
                e << path.asString() << " does not exist or is not a directory";
                throw e;
            }
            
            wxDir dir(path.asString());
            if (!dir.IsOpened()) {
                FileSystemException e;
                e << path.asString() << " could not be opened";
                throw e;
            }
            
            int flags = 0;
            switch (contentFilter) {
                case FSDirectories:
                    flags = wxDIR_DIRS;
                    break;
                case FSFiles:
                    flags = wxDIR_FILES;
                    break;
                case FSBoth:
                    flags = wxDIR_DIRS | wxDIR_FILES;
                    break;
            }
            
            Path::List result;
            
            wxString filename;
            if (dir.GetFirst(&filename, namePattern, flags)) {
                result.push_back(Path(filename.ToStdString()));
                
                while (dir.GetNext(&filename))
                    result.push_back(Path(filename.ToStdString()));
            }
            return result;
        }

        MappedFile::Ptr FileSystem::mapFile(const Path& path, const std::ios_base::openmode mode) const {
#ifdef _WIN32
            return MappedFile::Ptr(new WinMappedFile(path, mode));
#else
            return MappedFile::Ptr(new PosixMappedFile(path, mode));
#endif
        }

        
#if defined __APPLE__
        Path FileSystem::appDirectory() const {
            CFBundleRef mainBundle = CFBundleGetMainBundle ();
            CFURLRef bundleUrl = CFBundleCopyBundleURL(mainBundle);
            
            UInt8 buffer[1024];
            CFURLGetFileSystemRepresentation(bundleUrl, true, buffer, 1024);
            CFRelease(bundleUrl);
            
            StringStream result;
            for (size_t i = 0; i < 1024; i++) {
                UInt8 c = buffer[i];
                if (c == 0)
                    break;
                result << c;
            }
            
            return Path(result.str()).deleteLastComponent();
        }
#elif defined _WIN32
        Path FileSystem::appDirectory() const {
			TCHAR uAppPathC[MAX_PATH] = L"";
			DWORD numChars = GetModuleFileName(0, uAppPathC, MAX_PATH - 1);
            
			char appPathC[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, uAppPathC, numChars, appPathC, numChars, NULL, NULL);
			appPathC[numChars] = 0;
            
            const String appPathStr(appPathC);
            const Path appPath(appPathStr);
            return appPath.deleteLastComponent();
        }
#elif defined __linux__
        Path FileSystem::appDirectory() const {
            char buf[1024];
            const ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
            
            const String appPathStr(buf, len);
            const Path appPath(appPathStr);
            return appPath.deleteLastComponent();
        }
#endif

#if defined __APPLE__
        Path FileSystem::resourceDirectory() const {
            CFBundleRef mainBundle = CFBundleGetMainBundle ();
            CFURLRef resourcePathUrl = CFBundleCopyResourcesDirectoryURL(mainBundle);
            
            UInt8 buffer[1024];
            CFURLGetFileSystemRepresentation(resourcePathUrl, true, buffer, 1024);
            CFRelease(resourcePathUrl);
            
            StringStream result;
            for (size_t i = 0; i < 1024; i++) {
                UInt8 c = buffer[i];
                if (c == 0)
                    break;
                result << c;
            }
            
            return Path(result.str());
        }
#elif defined _WIN32
        Path FileSystem::resourceDirectory() const {
            return appDirectory() + Path("Resources");
        }
#elif defined __linux__
        Path FileSystem::resourceDirectory() const {
            return appDirectory() + Path("Resources");
        }
#endif
    }
}