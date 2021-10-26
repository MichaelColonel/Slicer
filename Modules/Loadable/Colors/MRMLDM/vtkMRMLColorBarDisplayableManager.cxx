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
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLColorBarDisplayNode.h>

#include <vtkSlicerScalarBarActor.h>

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

// STD includes
#include <tuple>
#include <map>
#include <cstring>

struct ColorBarWidgetData
{
  vtkSmartPointer<vtkSlicerScalarBarActor> Actor;
  vtkSmartPointer<vtkScalarBarWidget> Widget;
  bool InteractorInitiated{ false };
};

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
  vtkScalarBarWidget* GetWidget() const { return this->ColorBarWidget; }

  vtkWeakPointer<vtkSlicerScalarBarActor> GetActor(vtkMRMLColorBarDisplayNode*) const;
  vtkWeakPointer<vtkScalarBarWidget> GetWidget(vtkMRMLColorBarDisplayNode*) const;
  bool GetInitiatedFlag(vtkMRMLColorBarDisplayNode*) const;
  ColorBarWidgetData GetColorBarData(vtkMRMLColorBarDisplayNode*) const;

  vtkMRMLColorBarDisplayableManager* External;

  // Last modified/added color bar display node
  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ColorBarDisplayNode;
  // Actor and widget of last modified/added display node
  vtkWeakPointer<vtkSlicerScalarBarActor> ColorBarActor;
  vtkWeakPointer<vtkScalarBarWidget> ColorBarWidget;

  /// Map stores color bar display node ID, tuple
  std::map< std::string, ColorBarWidgetData > ColorBarTupleMap;
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
  this->ColorBarTupleMap.clear();
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

  if (this->ColorBarWidget && !this->ColorBarDisplayNode->GetVisibility())
    {
    this->ColorBarWidget->Off();
    return;
    }

  // Setup/update scalar bar widget
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(node);
  if (sliceNode && this->ColorBarDisplayNode->GetVisibility())
    {
//    this->ColorBarWidget->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
    }
  else if (threeDViewNode && this->ColorBarDisplayNode->GetVisibility())
    {
//    this->ColorBarWidget->SetEnabled(this->ColorBarDisplayNode->GetVisibility3D());
    }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::UpdateActor()
{
  if (!this->ColorBarDisplayNode)
    {
    return;
    }

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
    vtkMRMLDisplayableNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(displayableNode);
    if (volumeNode)
      {
      vtkMRMLDisplayNode* volumeDisplayNode = volumeNode->GetDisplayNode();
      if (vtkMRMLColorNode* colorNode = volumeDisplayNode->GetColorNode())
        {
        cNode = colorNode;
        }
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
      if (this->ColorBarActor->GetUseAnnotationAsLabel())
        {
        int newNumberOfColors = colorTableNode->GetNumberOfColors();

        // Update actor
        this->ColorBarActor->SetLookupTable(colorTableNode->GetLookupTable());
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
        this->ColorBarActor->SetLookupTable(colorTableNode->GetScalarsToColors());
        }
      }
    else if (!colorTableNode && procColorNode)
      {
      this->ColorBarActor->SetLookupTable(procColorNode->GetScalarsToColors());
      }
    }
}

//---------------------------------------------------------------------------
vtkWeakPointer<vtkSlicerScalarBarActor> vtkMRMLColorBarDisplayableManager::vtkInternal::GetActor(vtkMRMLColorBarDisplayNode* node) const
{
  vtkWeakPointer<vtkSlicerScalarBarActor> actor;
  actor = this->GetColorBarData(node).Actor;
  return actor;
}

//---------------------------------------------------------------------------
vtkWeakPointer<vtkScalarBarWidget> vtkMRMLColorBarDisplayableManager::vtkInternal::GetWidget(vtkMRMLColorBarDisplayNode* node) const
{
  vtkWeakPointer<vtkScalarBarWidget> widget;
  widget = this->GetColorBarData(node).Widget;
  return widget;
}

//---------------------------------------------------------------------------
bool vtkMRMLColorBarDisplayableManager::vtkInternal::GetInitiatedFlag(vtkMRMLColorBarDisplayNode* node) const
{
  bool initiated;
  initiated = this->GetColorBarData(node).InteractorInitiated;
  return initiated;
}

//---------------------------------------------------------------------------
ColorBarWidgetData vtkMRMLColorBarDisplayableManager::vtkInternal::GetColorBarData(vtkMRMLColorBarDisplayNode* node) const
{
  ColorBarWidgetData colorBarData;
  if (!node)
    {
    vtkErrorWithObjectMacro( this->External, "vtkInternal::GetColorBarData: Node is invalid");
    return colorBarData;
    }

  const auto it = this->ColorBarTupleMap.find(node->GetID());
  if (it != this->ColorBarTupleMap.end())
    {
    return it->second;
    }
  return colorBarData;
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

//-----------------------------------------------------------------------------

void vtkMRMLColorBarDisplayableManager::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("RegisterNodes: Invalid MRML scene");
    return;
    }
  if (!scene->IsNodeClassRegistered("vtkMRMLScalarVolumeNode"))
    {
    scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLScalarVolumeNode>::New());
    }
}


//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidget() const
{
  return this->Internal->GetWidget();
}

//---------------------------------------------------------------------------
vtkSlicerScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActor() const
{
  return this->Internal->GetActor();
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidget(vtkMRMLColorBarDisplayNode* dispNode) const
{
  return this->Internal->GetWidget(dispNode);
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

    vtkNew<vtkScalarBarWidget> scalarBarWidget;
    vtkNew<vtkSlicerScalarBarActor> scalarBarActor;

    scalarBarWidget->SetScalarBarActor(scalarBarActor);

    vtkScalarBarRepresentation* window = scalarBarWidget->GetScalarBarRepresentation();
    if (window)
      {
      window->SetShowHorizontalBorder(true);
      window->SetShowVerticalBorder(true);
      // show the border when hovering over with the mouse
      window->SetShowBorderToActive();
      }

    vtkRenderWindowInteractor* interactor = this->GetInteractor();
    if (interactor)
      {
      // Interactor is set
      // Widget initiated
      scalarBarWidget->SetInteractor(interactor);
      }

    std::string id(node->GetID());
    ColorBarWidgetData colorBarData;
    colorBarData.Actor = scalarBarActor;
    colorBarData.Widget = scalarBarWidget;
    colorBarData.InteractorInitiated = static_cast<bool>(interactor);
    this->Internal->ColorBarTupleMap[id] = colorBarData;
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
 
    auto it = this->Internal->ColorBarTupleMap.find(node->GetID());
    if (it != this->Internal->ColorBarTupleMap.end())
      {
//      vtkWeakPointer<vtkScalarBarWidget> scalarBarWidget;
//      vtkWeakPointer<vtkSlicerScalarBarActor> scalarBarActor;

      ColorBarWidgetData& colorBarData = it->second;
      if (colorBarData.Widget && colorBarData.Actor)
        {
        colorBarData.Widget->Off();
        this->GetRenderer()->RemoveVolume(colorBarData.Actor);
        colorBarData.Widget->Delete();
        colorBarData.Actor->Delete();

        colorBarData.InteractorInitiated = false;
        colorBarData.Widget = nullptr;
        colorBarData.Actor = nullptr;
        }
      
      if (dispNode && dispNode == this->Internal->ColorBarDisplayNode)
        {
        this->Internal->ColorBarDisplayNode = nullptr;
        this->Internal->ColorBarActor = nullptr;
        this->Internal->ColorBarWidget = nullptr;
        }
//      this->Internal->ColorBarTupleMap.erase(it);
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
        vtkWeakPointer<vtkScalarBarWidget> widget;
        vtkWeakPointer<vtkSlicerScalarBarActor> actor;
        ColorBarWidgetData colorBarData = this->Internal->GetColorBarData(dispNode);
        if (colorBarData.InteractorInitiated)
          {
          this->Internal->ColorBarDisplayNode = dispNode;
          this->Internal->ColorBarActor = colorBarData.Actor;
          this->Internal->ColorBarWidget = colorBarData.Widget;
          }
        }
      // Update scalar bars using new prorepties of the bars
      // Update color bar orientation
      this->Internal->UpdateColorBar();
      this->Internal->UpdateActor();
      this->RequestRender();
      }
      break;
    default:
      break;
    }
}
