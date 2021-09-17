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
#include <iostream>
#include <algorithm>
#include <cstring>

//---------------------------------------------------------------------------
class vtkColorBarUpdateObserver : public vtkCommand
{
public:
  static vtkColorBarUpdateObserver *New()
  {
    return new vtkColorBarUpdateObserver;
  }
  vtkColorBarUpdateObserver()
  {
    this->DisplayableManager = nullptr;
  }
  void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event), void* vtkNotUsed(calldata)) override
  {
    if (this->DisplayableManager)
    {
      this->DisplayableManager->UpdateFromRenderer();
    }
  }
  vtkWeakPointer<vtkMRMLColorBarDisplayableManager> DisplayableManager;
};


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLColorBarDisplayableManager);

//---------------------------------------------------------------------------
class vtkMRMLColorBarDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkMRMLColorBarDisplayableManager * external);
  virtual ~vtkInternal();

  void AddRendererUpdateObserver(vtkRenderer* renderer);
  void RemoveRendererUpdateObserver();

  vtkObserverManager* GetMRMLNodesObserverManager();
  void Modified();

  // Build/update color bar
  void BuildColorBar();
  void UpdateColorBar();

  // Build/update actor and widget representation
  void BuildActor();
  void UpdateActor();

  vtkSlicerScalarBarActor* GetActor() const { return this->ColorBarActor; }
  vtkScalarBarWidget* GetWidget() const { return this->ColorBarWidget; }

  vtkMRMLColorBarDisplayableManager* External;

  vtkSmartPointer<vtkSlicerScalarBarActor> ColorBarActor;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidget;
  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ColorBarDisplayNode;

  vtkSmartPointer<vtkColorBarUpdateObserver> RendererUpdateObserver;
  int RendererUpdateObservationId;
  vtkWeakPointer<vtkRenderer> ObservedRenderer;

  bool WidgetInitiated;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLColorBarDisplayableManager* external)
  :
  External(external),
  ColorBarActor(vtkSmartPointer<vtkSlicerScalarBarActor>::New()),
  ColorBarWidget(vtkSmartPointer<vtkScalarBarWidget>::New()),
  RendererUpdateObserver(vtkSmartPointer<vtkColorBarUpdateObserver>::New()),
  RendererUpdateObservationId(0),
  WidgetInitiated(false)
{
  this->RendererUpdateObserver->DisplayableManager = this->External;
  this->ColorBarWidget->SetScalarBarActor(this->ColorBarActor);

  vtkScalarBarRepresentation* window = this->ColorBarWidget->GetScalarBarRepresentation();
  if (window)
  {
    window->SetShowHorizontalBorder(true);
    window->SetShowVerticalBorder(true);
    // show the border when hovering over with the mouse
    window->SetShowBorderToActive();
  }
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ColorBarWidget->SetScalarBarActor(nullptr);
  this->ColorBarWidget->SetInteractor(nullptr);
  WidgetInitiated = false;
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::AddRendererUpdateObserver(vtkRenderer* renderer)
{
  RemoveRendererUpdateObserver();
  if (renderer)
  {
    this->ObservedRenderer = renderer;
    this->RendererUpdateObservationId = renderer->AddObserver(vtkCommand::StartEvent, this->RendererUpdateObserver);
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::RemoveRendererUpdateObserver()
{
  if (this->ObservedRenderer)
  {
    this->ObservedRenderer->RemoveObserver(this->RendererUpdateObservationId);
    this->RendererUpdateObservationId = 0;
    this->ObservedRenderer = nullptr;
  }
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
void vtkMRMLColorBarDisplayableManager::vtkInternal::BuildColorBar()
{
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();
  if (!interactor)
  {
    // Interactor is invalid
    return;
  }
  else if (!this->ColorBarWidget->GetInteractor())
  {
    // Interactor is set
    // Widget initiated
    this->ColorBarWidget->SetInteractor(interactor);
    this->ColorBarWidget->Off();
    this->WidgetInitiated = true;
  }

  // In 3D viewers we need to follow the renderer and update the orientation marker accordingly
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(this->External->GetMRMLDisplayableNode());
  if (threeDViewNode)
  {
    this->AddRendererUpdateObserver(this->External->GetRenderer());
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::UpdateColorBar()
{
  vtkMRMLNode* node = this->External->GetMRMLDisplayableNode();

  if (!this->WidgetInitiated)
  {
    // Hide color bar
    this->ColorBarWidget->SetEnabled(false);
    this->ColorBarWidget->Off();
    return;
  }

  if (!node || !this->ColorBarDisplayNode)
  {
    return;
  }
  // Setup/update scalar bar actor
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(node);
  if (sliceNode && this->ColorBarDisplayNode->GetVisibility())
  {
    this->ColorBarWidget->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
  }
  else if (threeDViewNode && this->ColorBarDisplayNode->GetVisibility())
  {
    this->ColorBarWidget->SetEnabled(this->ColorBarDisplayNode->GetVisibility3D());
  }

}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::BuildActor()
{
  this->ColorBarActor->SetOrientationToVertical();
  this->ColorBarActor->SetBarRatio(0.15);
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::UpdateActor()
{
  if (!this->ColorBarDisplayNode || !this->WidgetInitiated)
  {
    return;
  }

  switch (this->ColorBarDisplayNode->GetOrientationPreset())
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

  vtkMRMLDisplayableNode* displayableNode = this->ColorBarDisplayNode->GetDisplayableNode();
  vtkMRMLColorNode* cNode = this->ColorBarDisplayNode->GetColorNode();
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
    vtkWarningWithObjectMacro(this->External, "vtkInternal::UpdateActor: Color and displayable nodes are invalid!");
    this->ColorBarWidget->Off();
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
        this->ColorBarActor->UseAnnotationAsLabelOn(); // Needed each time
        this->ColorBarActor->SetLookupTable(colorTableNode->GetLookupTable());
        this->ColorBarActor->SetNumberOfLabels(newNumberOfColors);
        this->ColorBarActor->SetMaximumNumberOfColors(newNumberOfColors);
        this->ColorBarActor->GetLookupTable()->ResetAnnotations();
        for ( int colorIndex = 0; colorIndex < newNumberOfColors; ++colorIndex)
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
void vtkMRMLColorBarDisplayableManager::Create()
{
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::AdditionalInitializeStep()
{
  // Build the initial scalar bar
  if (!this->Internal->WidgetInitiated)
  {
    this->Internal->BuildColorBar();
    this->Internal->BuildActor();
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::UpdateFromRenderer()
{
  // Rendering is performed, so let's re-render the marker with up-to-date orientation
  if (!this->Internal->WidgetInitiated)
  {
    this->Internal->UpdateColorBar();
    this->Internal->UpdateActor();
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
  if (!this->Internal->WidgetInitiated)
  {
    this->Internal->BuildColorBar();
    this->Internal->BuildActor();
  }
  this->Internal->UpdateColorBar();
  this->Internal->UpdateActor();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene())
  {
    vtkErrorMacro("OnMRMLSceneNodeRemoved: Invalid MRML scene or input node");
    return;
  }

  vtkMRMLColorBarDisplayNode* colorBarDisplayNode = vtkMRMLColorBarDisplayNode::SafeDownCast(node);
  if (colorBarDisplayNode && (this->Internal->ColorBarDisplayNode == colorBarDisplayNode))
  {
    this->Internal->ColorBarDisplayNode = nullptr;
    this->Internal->WidgetInitiated = false;
    this->Internal->ColorBarWidget->SetInteractor(nullptr);
    this->Internal->ColorBarActor->UseAnnotationAsLabelOff();
    this->Internal->UpdateColorBar();
    this->Internal->UpdateActor();
    this->RequestRender();
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
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
      this->Internal->ColorBarDisplayNode = vtkMRMLColorBarDisplayNode::SafeDownCast(caller);
      // Update scalar bars using new prorepties of the bars
      // Update color bar orientation
      if (!this->Internal->WidgetInitiated)
      {
        this->Internal->BuildColorBar();
        this->Internal->BuildActor();
      }
      this->Internal->UpdateColorBar();
      this->Internal->UpdateActor();
      this->RequestRender();
    }
    break;
  default:
    break;
  }
}
