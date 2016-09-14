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

#ifndef CharArrayReader_h
#define CharArrayReader_h

#include "Macros.h"

#include "VecMath.h"

#include <cstdio>
#include <iostream>
#include <iterator>
#include <vector>

#ifdef _MSC_VER
#include <cstdint>
#elif defined __GNUC__
#include <stdint.h>
#endif

namespace TrenchBroom {
    namespace IO {
        class CharArrayReader {
        private:
            const char* const m_begin;
            const char* const m_end;
            const char* m_current;
        public:
            CharArrayReader(const char* begin, const char* end);

            CharArrayReader view(size_t offset) const;
            CharArrayReader view(size_t offset, size_t length) const;
            
            size_t size() const;
            void seekFromBegin(size_t offset);
            void seekFromEnd(size_t offset);
            void seekForward(size_t offset);

            void read(char* val, size_t size);
            void read(unsigned char* val, size_t size);
            bool canRead(size_t size) const;
            bool eof() const;

            template <typename T, typename R>
            R read() {
                T result;
                read(reinterpret_cast<char*>(&result), sizeof(T));
                return static_cast<R>(result);
            }
            
            template <typename T>
            void skip(size_t count = 1) {
                seekForward(count * sizeof(T));
            }

            template <typename T>
            char readChar() {
                return read<T, char>();
            }
            
            template <typename T>
            void skipChar() {
                skip<T>();
            }

            template <typename T>
            unsigned char readUnsignedChar() {
                return read<T, unsigned char>();
            }
            
            template <typename T>
            void skipUnsignedChar() {
                skip<T>();
            }

            template <typename T>
            int readInt() {
                return read<T, int>();
            }
            
            template <typename T>
            void skipInt() {
                skip<T>();
            }

            template <typename T>
            unsigned int readUnsignedInt() {
                return read<T, unsigned int>();
            }
            
            template <typename T>
            void skipUnsignedInt() {
                skip<T>();
            }

            template <typename T>
            size_t readSize() {
                return read<T, size_t>();
            }
            
            template <typename T>
            void skipSize() {
                skip<T>();
            }

            template <typename T>
            bool readBool() {
                return read<T, T>() != 0;
            }

            template <typename T>
            void skipBool() {
                skip<T>();
            }

            template <typename T>
            float readFloat() {
                return read<T, float>();
            }

            template <typename T>
            void skipFloat() {
                skip<T>();
            }
            
            template <typename T>
            double readDouble() {
                return read<T, double>();
            }

            template <typename T>
            void skipDouble() {
                skip<T>();
            }

            String readString(size_t size);
            void skipString(size_t size);

            template <typename R, size_t S, typename T>
            Vec<T,S> readVec() {
                Vec<T,S> result;
                for (size_t i = 0; i < S; ++i)
                    result[i] = read<T, R>();
                return result;
            }
            
            template <typename R, size_t S, typename T>
            void skipVec() {
                skip<T>(S);
            }

            template <typename C, typename T, typename R>
            void read(C& col, const size_t n) {
                read<T, R>(std::back_inserter(col), n);
            }
            
            template <typename T>
            void readRawVector(std::vector<T>& vec) {
                read(reinterpret_cast<char*>(vec.data()), vec.size() * sizeof(T));
            }

            template <typename C, typename T, typename R>
            void skip(C& col, const size_t n) {
                skip<T>(n);
            }
            
            template <typename T, typename R, typename I>
            void read(I out, const size_t n) {
                for (size_t i = 0; i < n; ++i)
                    out += read<T,R>();
            }
            
            template <typename T, typename R, typename I>
            void skip(I out, const size_t n) {
                skip<T>(n);
            }
        };
    }
}

#endif /* CharArrayReader_h */
