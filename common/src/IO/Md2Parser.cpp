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

#include "Md2Parser.h"

#include "Exceptions.h"
#include "Assets/Texture.h"
#include "Assets/Md2Model.h"
#include "Assets/Palette.h"
#include "IO/CharArrayReader.h"
#include "IO/FileSystem.h"
#include "IO/ImageLoader.h"
#include "IO/IOUtils.h"
#include "IO/MappedFile.h"
#include "IO/Path.h"
#include "Renderer/IndexRangeMap.h"
#include "Renderer/IndexRangeMapBuilder.h"
#include "Renderer/Vertex.h"
#include "Renderer/VertexSpec.h"

namespace TrenchBroom {
    namespace IO {
        Md2Parser::Md2Parser(const String& name, const char* begin, const char* end, const Assets::Palette& palette, const FileSystem& fs) :
        Md2ParserBase(name, begin, end, palette, fs) {}
        
        // http://tfc.duke.free.fr/old/models/md2.htm
        Assets::EntityModel* Md2Parser::doParseModel() {
            CharArrayReader reader(m_begin, m_end);
            
            const int ident = reader.readInt<int32_t>();
            const int version = reader.readInt<int32_t>();
            
            if (ident != Md2Layout::Ident)
                throw AssetException() << "Unknown MD2 model ident: " << ident;
            if (version != Md2Layout::Version)
                throw AssetException() << "Unknown MD2 model version: " << version;
            
            /*const size_t skinWidth =*/ reader.skipSize<int32_t>();
            /*const size_t skinHeight =*/ reader.skipSize<int32_t>();
            /* const size_t frameSize =*/ reader.skipSize<int32_t>();
            
            const size_t skinCount = reader.readSize<int32_t>();
            const size_t frameVertexCount = reader.readSize<int32_t>();
            /* const size_t texCoordCount =*/ reader.skipSize<int32_t>();
            /* const size_t triangleCount =*/ reader.skipSize<int32_t>();
            const size_t commandLength = reader.readSize<int32_t>();
            const size_t frameCount = reader.readSize<int32_t>();
            
            const size_t skinOffset = reader.readSize<int32_t>();
            /* const size_t texCoordOffset =*/ reader.skipSize<int32_t>();
            /* const size_t triangleOffset =*/ reader.skipSize<int32_t>();
            const size_t frameOffset = reader.readSize<int32_t>();
            const size_t commandOffset = reader.readSize<int32_t>();

            const Md2SkinList skins = parseSkins(reader.view(skinOffset), skinCount);
            const Md2FrameList frames = parseFrames(reader.view(frameOffset), frameCount, frameVertexCount);
            const Md2MeshList meshes = parseMeshes(reader.view(commandOffset), commandLength);
            
            return buildModel(skins, frames, meshes);
        }
    }
}
