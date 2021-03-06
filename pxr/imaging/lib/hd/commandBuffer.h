//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef HD_COMMAND_BUFFER_H
#define HD_COMMAND_BUFFER_H


#include "pxr/imaging/hd/version.h"
#include "pxr/imaging/hd/drawBatch.h"
#include "pxr/imaging/hd/drawItemInstance.h"

#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/matrix4d.h"

class HdDrawItem;
class HdDrawItemInstance;

/// A buffer of commands (HdDrawItem or HdComputeItem objects) to be executed.
///
/// The HdCommandBuffer is responsible for accumulating draw items and sorting
/// them for correctness (e.g. alpha transparency) and efficiency (e.g. the
/// fewest number of GPU state changes).
///
class HdCommandBuffer {
public:
    HdCommandBuffer();
    ~HdCommandBuffer();

    /// Prepare the command buffer for draw
    void PrepareDraw(HdRenderPassStateSharedPtr const &renderPassState);

    /// Execute the command buffer
    void ExecuteDraw(HdRenderPassStateSharedPtr const &renderPassState);

    /// Cull drawItemInstances based on passed in combined view and projection matrix
    void FrustumCull(GfMatrix4d const &cullMatrix);

    /// Sync visibility state from RprimSharedState to DrawItemInstances.
    void SyncDrawItemVisibility(unsigned visChangeCount);

    /// Destructively swaps the contents of \p items with the internal list of
    /// all draw items. Culling state is reset, with no items visible.
    void SwapDrawItems(std::vector<HdDrawItem const*>* items,
                       unsigned currentShaderBindingsVersion);

    /// Rebuild all draw batches if any underlying buffer array is invalidated.
    void RebuildDrawBatchesIfNeeded(unsigned currentShaderBindingsVersion);

    /// Returns the total number of draw items, including culled items.
    size_t GetTotalSize() const { return _drawItems.size(); }

    /// Returns the number of draw items, excluding culled items.
    size_t GetVisibleSize() const { return _visibleSize; }

    /// Returns the number of culled draw items.
    size_t GetCulledSize() const { 
        return _drawItems.size() - _visibleSize; 
    }

private:
    void _RebuildDrawBatches();

    std::vector<HdDrawItem const*> _drawItems;
    std::vector<HdDrawItemInstance> _drawItemInstances;
    Hd_DrawBatchSharedPtrVector _drawBatches;
    HdShaderSharedPtrVector _shaders;
    size_t _visibleSize;
    unsigned _visChangeCount;
    unsigned _shaderBindingsVersion;
};

#endif //HD_COMMAND_BUFFER_H
