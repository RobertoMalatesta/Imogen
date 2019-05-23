// https://github.com/CedricGuillemet/Imogen
//
// The MIT License(MIT)
//
// Copyright(c) 2019 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once

#include "imgui.h"
#include <vector>
#include <string>
#include <stdint.h>
#include "EvaluationStages.h"

struct UndoRedoHandler;
struct UndoRedo;

class GraphModel
{
public:
    GraphModel();
    ~GraphModel();

    struct Rug
    {
        ImVec2 mPos, mSize;
        uint32_t mColor;
        std::string mText;
    };

    struct Node
    {
        Node(int type, ImVec2 position) : mType(type), mPos(position), mbSelected(false)
        {
        }
        Node() : mbSelected(false)
        {
        }
        int mType;
        ImVec2 mPos;
        bool mbSelected;
    };

    struct Link
    {
        int mInputNodeIndex, mInputSlotIndex, mOutputNodeIndex, mOutputSlotIndex;
        bool operator==(const Link& other) const
        {
            return mInputNodeIndex == other.mInputNodeIndex && mInputSlotIndex == other.mInputSlotIndex &&
                   mOutputNodeIndex == other.mOutputNodeIndex && mOutputSlotIndex == other.mOutputSlotIndex;
        }
    };

    // Transaction
    void Clear();
    void BeginTransaction(bool undoable);
    bool InTransaction() const;
    void EndTransaction();

    // undo/redo
    void Undo();
    void Redo();

    // setters
    size_t AddNode(size_t type, ImVec2 position);
    void DelNode(size_t nodeIndex);
    void SelectNode(size_t nodeIndex, bool selected = true);
    void MoveSelectedNodes(const ImVec2 delta);
    void SetNodePosition(size_t nodeIndex, const ImVec2 position);
    void DeleteSelectedNodes();
    void AddLink(size_t inputNodeIndex, size_t inputSlotIndex, size_t outputNodeIndex, size_t outputSlotIndex);
    void DelLink(size_t linkIndex);
    void AddRug(const Rug& rug);
    void DelRug(size_t rugIndex);
    void SetRug(size_t rugIndex, const Rug& rug);
    void SetSamplers(size_t nodeIndex, const Samplers& sampler);
    void SetEvaluationOrder(const std::vector<size_t>& nodeOrderList);
    void SetParameter(size_t nodeIndex, const std::string& parameterName, const std::string& parameterValue);
    void SetParameters(size_t nodeIndex, const Parameters& parameters);
    void MakeKey(int frame, uint32_t nodeIndex, uint32_t parameterIndex);
    AnimTrack* GetAnimTrack(uint32_t nodeIndex, uint32_t parameterIndex);
    void SetIOPin(size_t nodeIndex, size_t io, bool forOutput, bool pinned);
    void SetParameterPin(size_t nodeIndex, size_t parameterIndex, bool pinned);
    void SetTimeSlot(size_t nodeIndex, int frameStart, int frameEnd);
    void SetKeyboardMouse(size_t nodeIndex, const UIInput& input);

	void SetParameterPins(const std::vector<uint32_t>& pins)
    {
        mEvaluationStages.SetParameterPins(pins);
    }
    void SetIOPins(const std::vector<uint32_t>& pins)
    {
        mEvaluationStages.SetIOPins(pins);
    }
    // transaction is handled is the function
    void NodeGraphLayout();

    // getters
    bool NodeHasUI(size_t nodeIndex) const;
    const std::vector<Rug>& GetRugs() const
    {
        return mRugs;
    }
    const std::vector<Node>& GetNodes() const
    {
        return mNodes;
    }
    const std::vector<Link>& GetLinks() const
    {
        return mLinks;
    }
    ImVec2 GetNodePos(size_t nodeIndex) const;
    bool IsIOUsed(size_t nodeIndex, int slotIndex, bool forOutput) const;
    bool IsIOPinned(size_t nodeIndex, size_t io, bool forOutput) const;
    bool IsParameterPinned(size_t nodeIndex, size_t parameterIndex) const;
    
    const EvaluationStages& GetEvaluationStages() const
    {
        return mEvaluationStages;
    }
    const std::vector<AnimTrack>& GetAnimTrack() const
    {
        return mEvaluationStages.GetAnimTrack();
    }
    void GetKeyedParameters(int frame, uint32_t nodeIndex, std::vector<bool>& keyed) const;
    const std::vector<uint32_t>& GetParameterPins() const
    {
        return mEvaluationStages.GetParameterPins();
    }
    const std::vector<uint32_t>& GetIOPins() const
    {
        return mEvaluationStages.GetIOPins();
    }
    const std::vector<unsigned char>& GetParameters(size_t nodeIndex) const;
    const Samplers& GetSamplers(size_t nodeIndex) const
    {
        return mEvaluationStages.mInputSamplers[nodeIndex];
    }
    ImRect GetNodesDisplayRect() const;
    ImRect GetFinalNodeDisplayRect() const;
    bool RecurseIsLinked(int from, int to) const;

    // dirty
    const std::vector<DirtyList>& GetDirtyList() const { return mDirtyList; }
    void ClearDirtyList() { mDirtyList.clear(); }

    // clipboard
    void CopySelectedNodes();
    void CutSelectedNodes();
    void PasteNodes(ImVec2 viewOffsetPosition);
    bool IsClipboardEmpty() const;

    EvaluationStages mEvaluationStages;

private:
    bool mbTransaction;
    UndoRedo* mUndoRedo;

    int mSelectedNodeIndex;
    std::vector<Node> mNodes;
    std::vector<Link> mLinks;
    std::vector<Rug> mRugs;


	std::vector<Node> mNodesClipboard;
	std::vector<EvaluationStage> mStagesClipboard;

    std::vector<DirtyList> mDirtyList;
    void SetDirty(size_t nodeIndex, DirtyFlag flags) { mDirtyList.push_back({nodeIndex, flags});}
    void AddLinkHelper(int nodeIndex);
    void DeleteLinkHelper(int nodeIndex);
    void AddNodeHelper(int nodeIndex);
    void DeleteNodeHelper(int nodeIndex);
    void RemoveAnimation(size_t nodeIndex);

    struct NodePosition
    {
        int mLayer;
        int mStackIndex;
    };

    void RecurseNodeGraphLayout(std::vector<NodePosition>& positions,
        std::map<int, int>& stacks,
        size_t currentIndex,
        int currentLayer);
};