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

#ifndef Polyhedron_Subtract_h
#define Polyhedron_Subtract_h

template <typename T, typename FP, typename VP>
typename Polyhedron<T,FP,VP>::SubtractResult Polyhedron<T,FP,VP>::subtract(const Polyhedron& subtrahend) const {
    Callback c;
    return subtract(subtrahend, c);
}

template <typename T, typename FP, typename VP>
typename Polyhedron<T,FP,VP>::SubtractResult Polyhedron<T,FP,VP>::subtract(const Polyhedron& subtrahend, const Callback& callback) const {
    List result = doSubtract(subtrahend, callback);
         result = doMergeFragments(result, callback);
    return result;
}

template <typename T, typename FP, typename VP>
typename Polyhedron<T,FP,VP>::List Polyhedron<T,FP,VP>::doSubtract(const Polyhedron& subtrahend, const Callback& callback) const {
    Subtract subtract(*this, subtrahend, callback);
    return subtract.result();
}

template <typename T, typename FP, typename VP>
typename Polyhedron<T,FP,VP>::List Polyhedron<T,FP,VP>::doMergeFragments(const List& fragments, const Callback& callback) {
    // no-op
    return fragments;
}

template <typename T, typename FP, typename VP>
class Polyhedron<T,FP,VP>::Subtract {
private:
    const Polyhedron& m_minuend;
    Polyhedron m_subtrahend;
    const Callback& m_callback;
    List m_resultFragments;

private:
    static void subtract_r(const List& minutendFragments,
                           const std::vector<Plane<T,3>>& subtrahendPlanes,
                           const size_t currentPlaneIndex,
                           List* resultFragments) {
        if (currentPlaneIndex == subtrahendPlanes.size()) {
            // all of `minutendFragments` are now behind all of subtrahendPlanes
            // so they can be discarded.
            return;
        }
        
        const Plane<T,3> currentPlane = subtrahendPlanes[currentPlaneIndex];
        const Plane<T,3> currentPlaneInv = currentPlane.flipped();
        
        // clip the list of minutendFragments into a list of those in front of the
        // currentPlane, and those behind
        List backFragments;
        
        for (const auto& fragment : minutendFragments) {
            // the front fragments go directly into the result set.
            Polyhedron<T,FP,VP> fragmentInFront = fragment;
            const auto frontClipResult = fragmentInFront.clip(currentPlaneInv);
            
            if (!frontClipResult.empty()) // Polyhedron::clip() keeps the part behind the plane.
                resultFragments->push_back(fragmentInFront);
            
            // back fragments need to be clipped by the rest of the subtrahend planes
            Polyhedron<T,FP,VP> fragmentBehind = fragment;
            const auto backClipResult = fragmentBehind.clip(currentPlane);
            if (!backClipResult.empty())
                backFragments.push_back(fragmentBehind);
        }
        
        // recursively process the back fragments.
        subtract_r(backFragments, subtrahendPlanes, currentPlaneIndex + 1, resultFragments);
    }
    
    static std::vector<Plane<T,3>> planes(const Polyhedron& polyhedron, const typename Polyhedron::Callback& callback) {
        std::vector<Plane<T,3>> result;
        
        const Face* firstFace = polyhedron.faces().front();
        const Face* currentFace = firstFace;
        do {
            const Plane<T,3> plane = callback.plane(currentFace);
            result.push_back(plane);
            currentFace = currentFace->next();
        } while (currentFace != firstFace);
        
        return result;
    }
    
    static std::vector<Plane<T,3>> sortPlanes(const std::vector<Plane<T,3>>& planes) {
        using namespace Math;
        const T epsilon = Constants<T>::angleEpsilon();
        
        std::vector<Plane<T,3>> x, y, z, yz, xz, xy, other;
        
        for (const auto &plane : planes) {
            if (abs<T>(abs<T>(plane.normal.x()) - 1) < epsilon) {
                x.push_back(plane);
            } else if (abs<T>(abs<T>(plane.normal.y()) - 1) < epsilon) {
                y.push_back(plane);
            } else if (abs<T>(abs<T>(plane.normal.z()) - 1) < epsilon) {
                z.push_back(plane);
            } else if (abs<T>(plane.normal.x()) < epsilon) {
                yz.push_back(plane);
            } else if (abs<T>(plane.normal.y()) < epsilon) {
                xz.push_back(plane);
            } else if (abs<T>(plane.normal.z()) < epsilon) {
                xy.push_back(plane);
            } else {
                other.push_back(plane);
            }
        }
       
        std::vector<Plane<T,3>> result;
        VectorUtils::append(result, x);
        VectorUtils::append(result, y);
        VectorUtils::append(result, z);
        VectorUtils::append(result, yz);
        VectorUtils::append(result, xz);
        VectorUtils::append(result, xy);
        VectorUtils::append(result, other);
        assert(result.size() == planes.size());
        return result;
    }
    
    void subtract() {
        const std::vector<Plane<T,3>> subtrahendPlanes =
            sortPlanes(planes(m_subtrahend, m_callback));

        assert(m_resultFragments.empty());
        subtract_r(List{m_minuend}, subtrahendPlanes, 0, &m_resultFragments);
    }

public:
    Subtract(const Polyhedron& minuend, const Polyhedron& subtrahend, const Callback& callback) :
    m_minuend(minuend),
    m_subtrahend(subtrahend),
    m_callback(callback) {
        subtract();
    }
    
    const List result() {
        return m_resultFragments;
    }
};

#endif /* Polyhedron_Subtract_h */
