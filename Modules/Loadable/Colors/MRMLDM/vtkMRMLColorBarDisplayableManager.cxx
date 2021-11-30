/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// MRMLDisplayableManager includes
#include "vtkMRMLColorBarDisplayableManager.h"

// MRML includes
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLInteractionEventData.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLLightBoxRendererManagerProxy.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLColorBarDisplayNode.h>

//#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLDisplayableNode.h>

// VTK includes
#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkCellArray.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProp.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkVersion.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkScalarBarWidget.h>
#include <vtkLookupTable.h>
#include <vtkTextProperty.h>

#include <vtkSlicerScalarBarActor.h>

// STD includes
#include <tuple>
#include <map>
#include <cstring>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLColorBarDisplayableManager);

//---------------------------------------------------------------------------
class vtkMRMLColorBarDisplayableManager::vtkInternal
{
public:

  vtkInternal(vtkMRMLColorBarDisplayableManager * external);
  virtual ~vtkInternal();

  vtkObserverManager* GetMRMLNodesObserverManager();
  void Modified();

  // Update color bar
  void UpdateColorBar();

  // Update actor and widget representation
  void UpdateActor();

  vtkSlicerScalarBarActor* GetActor() const { return this->ColorBarActor; }

  vtkWeakPointer<vtkSlicerScalarBarActor> GetActor(vtkMRMLColorBarDisplayNode*) const;

  vtkMRMLColorBarDisplayableManager* External;

  // Last modified/added color bar display node
  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ColorBarDisplayNode;
  // Actor of last modified/added display node
  vtkWeakPointer<vtkSlicerScalarBarActor> ColorBarActor;
  // vtkTextProperty of Title and Text, initiated in actor
  vtkWeakPointer<vtkTextProperty> TitleTextProperty;
  vtkWeakPointer<vtkTextProperty> LabelsTextProperty;

  /// Map stores color bar display node ID as a key, ScalarBarActor as a value
  std::map< std::string, vtkSmartPointer<vtkSlicerScalarBarActor> > ColorBarActorsMap;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLColorBarDisplayableManager* external)
  :
  External(external)
{
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ColorBarActorsMap.clear();
}

//---------------------------------------------------------------------------
vtkObserverManager* vtkMRMLColorBarDisplayableManager::vtkInternal::GetMRMLNodesObserverManager()
{
  return this->External->GetMRMLNodesObserverManager();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::Modified()
{
  return this->External->Modified();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::UpdateColorBar()
{
  vtkMRMLNode* node = this->External->GetMRMLDisplayableNode();

  if (!node || !this->ColorBarDisplayNode)
    {
    return;
    }

  if (!this->ColorBarDisplayNode->GetVisibility())
    {
    this->External->GetRenderer()->RemoveActor(this->ColorBarActor);
    return;
    }

  // Setup/update scalar bar actor visibility
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(node);

  bool visible = false;
  if (sliceNode && this->ColorBarDisplayNode->GetVisibility())
    {
    visible = this->ColorBarDisplayNode->GetVisibility2D() && this->ColorBarDisplayNode->IsDisplayableInView(sliceNode->GetID());
    }
  else if (threeDViewNode && this->ColorBarDisplayNode->GetVisibility())
    {
    visible = this->ColorBarDisplayNode->GetVisibility3D() && this->ColorBarDisplayNode->IsDisplayableInView(threeDViewNode->GetID());
    }

  if (visible)
    {
    this->External->GetRenderer()->AddActor2D(this->ColorBarActor);
    }
  else
    {
    this->External->GetRenderer()->RemoveActor(this->ColorBarActor);
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::UpdateActor()
{
  if (!this->ColorBarDisplayNode)
    {
    return;
    }

  std::string title = this->ColorBarDisplayNode->GetTitleText();
  this->ColorBarActor->SetTitle(title.c_str());
  this->ColorBarDisplayNode->GetTitleTextProperty(this->TitleTextProperty);
  this->ColorBarDisplayNode->GetLabelsTextProperty(this->LabelsTextProperty);

  this->ColorBarActor->SetPosition(this->ColorBarDisplayNode->GetPosition());
  this->ColorBarActor->SetWidth(this->ColorBarDisplayNode->GetWidth());
  this->ColorBarActor->SetHeight(this->ColorBarDisplayNode->GetHeight());

  std::string format = this->ColorBarDisplayNode->GetLabelsFormat();
  this->ColorBarActor->SetLabelFormat(format.c_str());
  this->ColorBarActor->SetNumberOfLabels(this->ColorBarDisplayNode->GetNumberOfLabels());
  this->ColorBarActor->SetMaximumNumberOfColors(this->ColorBarDisplayNode->GetMaxNumberOfColors());
  this->ColorBarActor->SetUseAnnotationAsLabel(this->ColorBarDisplayNode->GetUseColorNamesForLabels());
  this->ColorBarActor->SetCenterLabel(this->ColorBarDisplayNode->GetCenterLabels());

  switch (this->ColorBarDisplayNode->GetOrientation())
    {
    case vtkMRMLColorBarDisplayNode::Vertical:
      this->ColorBarActor->SetOrientationToVertical();
      break;
    case vtkMRMLColorBarDisplayNode::Horizontal:
      this->ColorBarActor->SetOrientationToHorizontal();
      break;
    default:
      break;
    }

  vtkMRMLColorNode* cNode = nullptr;
  vtkMRMLDisplayableNode* displayableNode = this->ColorBarDisplayNode->GetDisplayableNode();
  const char* cNodeID = this->ColorBarDisplayNode->GetColorNodeID();
  if (cNodeID)
    {
    cNode = vtkMRMLColorNode::SafeDownCast(this->External->GetMRMLScene()->GetNodeByID(cNodeID));
    }

  if (!cNode && displayableNode)
    {
    vtkMRMLDisplayNode* displayNode = displayableNode->GetDisplayNode();
    if (vtkMRMLColorNode* colorNode = displayNode->GetColorNode())
      {
      cNode = colorNode;
      }
    }
  else if (!cNode && !displayableNode)
    {
    // Color and displayable nodes are invalid!
    return;
    }

  if (cNode)
    {
    vtkMRMLColorTableNode* colorTableNode = vtkMRMLColorTableNode::SafeDownCast(cNode);
    vtkMRMLProceduralColorNode* procColorNode = vtkMRMLProceduralColorNode::SafeDownCast(cNode);

    if (colorTableNode && !procColorNode)
      {
      // Update LUT
      vtkLookupTable* LUT = colorTableNode->GetLookupTable();
      if (this->ColorBarActor->GetUseAnnotationAsLabel())
        {
        int newNumberOfColors = colorTableNode->GetNumberOfColors();

        this->ColorBarActor->SetLookupTable(LUT);
        this->ColorBarActor->SetNumberOfLabels(newNumberOfColors);
        this->ColorBarActor->SetMaximumNumberOfColors(newNumberOfColors);
        this->ColorBarActor->GetLookupTable()->ResetAnnotations();
        for (int colorIndex = 0; colorIndex < newNumberOfColors; ++colorIndex)
          {
          this->ColorBarActor->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(colorTableNode->GetColorName(colorIndex)));
          }
        }
      else
        {
        this->ColorBarActor->SetLookupTable(LUT);
        }
      }
    else if (!colorTableNode && procColorNode)
      {
      // Update LUT
      vtkLookupTable* LUT = procColorNode->GetLookupTable();
      // Scalars range in logarithmic scale == true, linear scale == false
      bool LogScale = this->ColorBarDisplayNode->GetLogarithmicScale();
      if (LUT && LogScale)
        {
        LUT->SetScaleToLog10();
        }
      else if (LUT && !LogScale)
        {
        LUT->SetScaleToLinear();
        }

      this->ColorBarActor->SetLookupTable(LUT);
      }
    }
}

//---------------------------------------------------------------------------
vtkWeakPointer<vtkSlicerScalarBarActor> vtkMRMLColorBarDisplayableManager::vtkInternal::GetActor(vtkMRMLColorBarDisplayNode* node) const
{
  if (!node)
    {
    vtkErrorWithObjectMacro( this->External, "vtkInternal::GetActor: Node is invalid");
    return nullptr;
    }

  vtkWeakPointer<vtkSlicerScalarBarActor> actor = nullptr;

  const auto it = this->ColorBarActorsMap.find(node->GetID());
  if (it != this->ColorBarActorsMap.end())
    {
    actor = it->second;
    }
  return actor;
}

//---------------------------------------------------------------------------
// vtkMRMLColorBarDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkMRMLColorBarDisplayableManager()
{
  this->Internal = new vtkInternal(this);
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::~vtkMRMLColorBarDisplayableManager()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkSlicerScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActor() const
{
  return this->Internal->GetActor();
}

//---------------------------------------------------------------------------
vtkSlicerScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActor(vtkMRMLColorBarDisplayNode* dispNode) const
{
  return this->Internal->GetActor(dispNode);
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::Create()
{
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::AdditionalInitializeStep()
{
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndCloseEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("RegisterNodes: Invalid MRML scene");
    return;
    }

  if (!scene->IsNodeClassRegistered("vtkMRMLColorBarDisplayNode"))
    {
    scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLColorBarDisplayNode>::New());
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::UpdateFromMRMLScene()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("UpdateFromMRMLScene: Invalid MRML scene");
    return;
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
  this->Internal->UpdateColorBar();
  this->Internal->UpdateActor();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  this->Superclass::OnMRMLSceneNodeAdded(node);

  if (!node || !this->GetMRMLScene())
    {
    vtkErrorMacro("OnMRMLSceneNodeAdded: Invalid MRML scene or input node");
    return;
    }

  if (node->IsA("vtkMRMLColorBarDisplayNode"))
    {
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    vtkObserveMRMLNodeEventsMacro(node, events);

    vtkNew<vtkSlicerScalarBarActor> scalarBarActor;

    std::string id(node->GetID());
    this->Internal->ColorBarActorsMap[id] = scalarBarActor;
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  this->Superclass::OnMRMLSceneNodeRemoved(node);

  if (!node || !this->GetMRMLScene())
    {
    vtkErrorMacro("OnMRMLSceneNodeRemoved: Invalid MRML scene or input node");
    return;
    }

  if (node->IsA("vtkMRMLColorBarDisplayNode"))
    {
    vtkUnObserveMRMLNodeMacro(node);

    vtkMRMLColorBarDisplayNode* dispNode = vtkMRMLColorBarDisplayNode::SafeDownCast(node);

    auto it = this->Internal->ColorBarActorsMap.find(node->GetID());
    if (it != this->Internal->ColorBarActorsMap.end())
      {
      if (dispNode && dispNode == this->Internal->ColorBarDisplayNode)
        {
        this->Internal->ColorBarDisplayNode = nullptr;
        this->Internal->ColorBarActor = nullptr;
        this->Internal->TitleTextProperty = nullptr;
        this->Internal->LabelsTextProperty = nullptr;
        }
      vtkWeakPointer<vtkSlicerScalarBarActor> actor;
      actor = it->second;
      this->GetRenderer()->RemoveActor(actor);
      this->Internal->ColorBarActorsMap.erase(it);
      }
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::ProcessMRMLNodesEvents(vtkObject *caller, unsigned long event, void *callData)
{
  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);

  switch (event)
    {
    case vtkCommand::ModifiedEvent:
      {
      vtkMRMLColorBarDisplayNode* dispNode = vtkMRMLColorBarDisplayNode::SafeDownCast(caller);
      if (dispNode)
        {
        vtkWeakPointer<vtkSlicerScalarBarActor> actor = this->Internal->GetActor(dispNode);
        if (actor)
          {
          this->Internal->ColorBarDisplayNode = dispNode;
          this->Internal->ColorBarActor = actor;
          this->Internal->TitleTextProperty = actor->GetTitleTextProperty();
          this->Internal->LabelsTextProperty = actor->GetLabelTextProperty();
          }
        else
          {
          this->Internal->ColorBarDisplayNode = dispNode;
          this->Internal->ColorBarActor = nullptr;
          this->Internal->TitleTextProperty = nullptr;
          this->Internal->LabelsTextProperty = nullptr;
          }

        // Update scalar bars using new prorepties of the bars
        // Update color bar orientation
        this->Internal->UpdateColorBar();
        this->Internal->UpdateActor();
        this->RequestRender();
        }
      }
      break;
    default:
      break;
    }
}
