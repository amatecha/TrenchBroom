/*
 Copyright (C) 2010-2017 Kristian Duske
 
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

#ifndef VertexHandleManager_h
#define VertexHandleManager_h

#include "VecMath.h"
#include "TrenchBroom.h"
#include "Model/Brush.h"
#include "Model/BrushFace.h"
#include "Model/Hit.h"
#include "Model/PickResult.h"
#include "Renderer/Camera.h"
#include "View/ViewTypes.h"

#include <algorithm>
#include <iterator>
#include <map>

namespace TrenchBroom {
    namespace Model {
        class Brush;
        class PickResult;
    }
    
    namespace Renderer {
        class Camera;
    }
    
    namespace View {
        template <typename H, typename C = std::less<H>>
        class VertexHandleManagerBase {
        public:
            typedef H Handle;
            typedef std::vector<H> HandleList;
        protected:
            struct HandleInfo {
                size_t count;
                bool selected;
                
                HandleInfo() :
                count(0),
                selected(false) {}
                
                bool select() {
                    const bool result = !selected;
                    selected = true;
                    return result;
                }
                
                bool deselect() {
                    const bool result = selected;
                    selected = false;
                    return result;
                }
                
                bool toggle() {
                    selected = !selected;
                    return selected;
                }
                
                void inc() {
                    ++count;
                }
                
                void dec() {
                    --count;
                }
            };
            
            typedef std::map<H, HandleInfo, C> HandleMap;
            typedef typename HandleMap::value_type HandleEntry;

            HandleMap m_handles;
            size_t m_selectedHandleCount;
        public:
            VertexHandleManagerBase() :
            m_selectedHandleCount(0) {}
            
            virtual ~VertexHandleManagerBase() {}
        public:
            size_t selectedHandleCount() const {
                return m_selectedHandleCount;
            }
            
            size_t unselectedHandleCount() const {
                return totalHandleCount() - selectedHandleCount();
            }
            
            size_t totalHandleCount() const {
                return m_handles.size();
            }
        public:
            
            HandleList allHandles() const {
                HandleList result;
                result.reserve(totalHandleCount());
                collectHandles([](const HandleInfo& info) { return true; }, std::back_inserter(result));
                return result;
            }
            
            HandleList selectedHandles() const {
                HandleList result;
                result.reserve(selectedHandleCount());
                collectHandles([](const HandleInfo& info) { return info.selected; }, std::back_inserter(result));
                return result;
            }
            
            HandleList unselectedHandles() const {
                HandleList result;
                result.reserve(unselectedHandleCount());
                collectHandles([](const HandleInfo& info) { return !info.selected; }, std::back_inserter(result));
                return result;
            }

        private:
            template <typename T, typename O>
            void collectHandles(const T& test, O out) const {
                for (const HandleEntry& entry : m_handles) {
                    const Handle& handle = entry.first;
                    const HandleInfo& info = entry.second;
                    if (test(info))
                        out = handle;
                }
            }
            
        public:
            bool selected(const Handle& handle) const {
                const auto it = m_handles.find(handle);
                if (it == std::end(m_handles))
                    return false;
                return it->second.selected;
            }
            
            bool anySelected() const {
                return selectedHandleCount() > 0;
            }
            
            bool allSelected() const {
                return selectedHandleCount() == totalHandleCount();
            }
        public:
            void add(const Handle& handle) {
                MapUtils::findOrInsert(m_handles, handle, HandleInfo())->second.inc();
            }
            
            void remove(const Handle& handle) {
                const auto it = m_handles.find(handle);
                if (it != std::end(m_handles)) {
                    HandleInfo& info = it->second;
                    info.dec();
                    
                    if (info.count == 0) {
                        deselect(info);
                        m_handles.erase(it);
                    }
                }
            }

            void clear() {
                m_handles.clear();
                m_selectedHandleCount = 0;
            }

            template <typename I>
            void select(I begin, I end) {
                std::for_each(begin, end, [this](const Handle& handle) { select(handle); });
            }
            
            void select(const Handle& handle) {
                const auto it = m_handles.find(handle);
                if (it != std::end(m_handles)) {
                    select(it->second);
                }
            }
            
            template <typename I>
            void deselect(I begin, I end) {
                std::for_each(begin, end, [this](const Handle& handle) { deselect(handle); });
            }
            
            void deselect(const Handle& handle) {
                const auto it = m_handles.find(handle);
                if (it != std::end(m_handles)) {
                    deselect(it->second);
                }
            }
            
            void deselectAll() {
                std::for_each(std::begin(m_handles), std::end(m_handles), [this](HandleEntry& entry) {
                    deselect(entry.second);
                });
            }
            
            template <typename I>
            void toggle(I begin, I end) {
                std::for_each(begin, end, [this](const Handle& handle) { toggle(handle); });
            }
            
            void toggle(const Handle& handle) {
                const auto it = m_handles.find(handle);
                if (it != std::end(m_handles)) {
                    toggle(it->second);
                }
            }
        private:
            void select(HandleInfo& info) {
                if (info.select()) {
                    assert(selectedHandleCount() < totalHandleCount());
                    ++m_selectedHandleCount;
                }
            }
            
            void deselect(HandleInfo& info) {
                if (info.deselect()) {
                    assert(m_selectedHandleCount > 0);
                    --m_selectedHandleCount;
                }
            }
            
            void toggle(HandleInfo& info) {
                if (info.toggle()) {
                    assert(selectedHandleCount() < totalHandleCount());
                    ++m_selectedHandleCount;
                } else {
                    assert(m_selectedHandleCount > 0);
                    --m_selectedHandleCount;
                }
            }
        public:
            template <typename P>
            void pick(const P& test, Model::PickResult& pickResult) const {
                std::for_each(std::begin(m_handles), std::end(m_handles), [&test, &pickResult](const HandleEntry& entry) {
                    const Model::Hit hit = test(entry.first);
                    if (hit.isMatch())
                        pickResult.addHit(hit);
                });
            }
        };

        class VertexHandleManager : public VertexHandleManagerBase<Vec3> {
        public:
            static const Model::Hit::HitType HandleHit;
        public:
            void pick(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult) const;
        public:
            template <typename I>
            Model::BrushSet findIncidentBrushes(const Handle& handle, I begin, I end) const {
                Model::BrushSet result;
                findIncidentBrushes(handle, begin, end, std::inserter(result, result.end()));
                return result;
            }
            
            template <typename I1, typename I2>
            Model::BrushSet findIncidentBrushes(I1 hBegin, I1 hEnd, I2 bBegin, I2 bEnd) const {
                Model::BrushSet result;
                auto out = std::inserter(result, std::end(result));
                std::for_each(hBegin, hEnd, [this, bBegin, bEnd, out](const Handle& handle) {
                    findIncidentBrushes(handle, bBegin, bEnd, out);
                });
                return result;
            }
        private:
            template <typename I, typename O>
            void findIncidentBrushes(const Handle& handle, I begin, I end, O out) const {
                std::copy_if(begin, end, out, [&handle](const Model::Brush* brush) { return brush->hasVertex(handle); });
            }
        public:
            template <typename I>
            void addHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { addHandles(brush); });
            }
            
            void addHandles(const Model::Brush* brush);
            
            template <typename I>
            void removeHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { removeHandles(brush); });
            }
            
            void removeHandles(const Model::Brush* brush);
        };
        
        class EdgeHandleManager : public VertexHandleManagerBase<Edge3> {
        public:
            static const Model::Hit::HitType HandleHit;
        public:
            void pick(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult) const;
        public:
            template <typename I>
            Model::BrushSet findIncidentBrushes(const Handle& handle, I begin, I end) const {
                Model::BrushSet result;
                findIncidentBrushes(handle, begin, end, std::inserter(result, result.end()));
                return result;
            }
            
            template <typename I1, typename I2>
            Model::BrushSet findIncidentBrushes(I1 hBegin, I1 hEnd, I2 bBegin, I2 bEnd) const {
                Model::BrushSet result;
                auto out = std::inserter(result, std::end(result));
                std::for_each(hBegin, hEnd, [this, bBegin, bEnd, out](const Handle& handle) {
                    findIncidentBrushes(handle, bBegin, bEnd, out);
                });
                return result;
            }
        private:
            template <typename I, typename O>
            void findIncidentBrushes(const Handle& handle, I begin, I end, O out) const {
                std::copy_if(begin, end, out, [&handle](const Model::Brush* brush) { return brush->hasEdge(handle); });
            }
        public:
            template <typename I>
            void addHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { addHandles(brush); });
            }
            
            void addHandles(const Model::Brush* brush);
            
            template <typename I>
            void removeHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { removeHandles(brush); });
            }
            
            void removeHandles(const Model::Brush* brush);
        };
        
        struct FaceHandleCmp {
            bool operator()(const Model::BrushFace* lhs, const Model::BrushFace* rhs) const;
        };
        
        class FaceHandleManager : public VertexHandleManagerBase<const Model::BrushFace*> {
        public:
            static const Model::Hit::HitType HandleHit;
        public:
            void pick(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult) const;
        public:
            template <typename I>
            Model::BrushSet findIncidentBrushes(const Handle& handle, I begin, I end) const {
                Model::BrushSet result;
                findIncidentBrushes(handle, begin, end, std::inserter(result, result.end()));
                return result;
            }
            
            template <typename I1, typename I2>
            Model::BrushSet findIncidentBrushes(I1 hBegin, I1 hEnd, I2 bBegin, I2 bEnd) const {
                Model::BrushSet result;
                auto out = std::inserter(result, std::end(result));
                std::for_each(hBegin, hEnd, [this, bBegin, bEnd, out](const Handle& handle) {
                    findIncidentBrushes(handle, bBegin, bEnd, out);
                });
                return result;
            }
        private:
            template <typename I, typename O>
            void findIncidentBrushes(const Handle& handle, I begin, I end, O out) const {
                std::copy_if(begin, end, out, [&handle](const Model::Brush* brush) { return brush->hasFace(handle->polygon()); });
            }
        public:
            template <typename I>
            void addHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { addHandles(brush); });
            }
            
            void addHandles(const Model::Brush* brush);
            
            template <typename I>
            void removeHandles(I begin, I end) {
                std::for_each(begin, end, [this](const Model::Brush* brush) { removeHandles(brush); });
            }
            
            void removeHandles(const Model::Brush* brush);
        };
    }
}

#endif /* VertexHandleManager_h */
