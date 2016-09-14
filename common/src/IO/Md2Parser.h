/*
 Copyright (C) 2010-2016 Kristian Duske
 
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
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TrenchBroom_Md2Parser
#define TrenchBroom_Md2Parser

#include "StringUtils.h"
#include "VecMath.h"
#include "Assets/AssetTypes.h"
#include "Assets/Md2Model.h"
#include "IO/Md2ParserBase.h"

#include <vector>

namespace TrenchBroom {
    namespace Assets {
        class EntityModel;
        class Palette;
    }
    
    namespace IO {
        class FileSystem;
        class Path;
        
        namespace Md2Layout {
            static const int Ident = (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I');
            static const int Version = 8;
        }

        class Md2Parser : public Md2ParserBase {
        private:
        public:
            Md2Parser(const String& name, const char* begin, const char* end, const Assets::Palette& palette, const FileSystem& fs);
        private:
            Assets::EntityModel* doParseModel();
        };
    }
}

#endif /* defined(TrenchBroom_Md2Parser) */
