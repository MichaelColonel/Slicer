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
#include "vtkMRMLScalarBarDisplayableManager.h"

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
#include <vtkMRMLWindowLevelWidget.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLScalarBarDisplayNode.h>

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
#include <vtkScalarBarWidget.h>
#include <vtkScalarBarActor.h>

// STD includes
#include <algorithm>
#include <cassert>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLScalarBarDisplayableManager );

//---------------------------------------------------------------------------
class vtkMRMLScalarBarDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkMRMLScalarBarDisplayableManager * external);
  virtual ~vtkInternal();

  vtkObserverManager* GetMRMLNodesObserverManager();
  void Modified();

  // Slice
  vtkMRMLSliceNode* GetSliceNode();
  void UpdateSliceNode();

  // Build the color scalar bar representation
  void BuildScalarBar();

  vtkMRMLScalarBarDisplayableManager*        External;

  vtkSmartPointer<vtkMRMLWindowLevelWidget> WindowLevelWidget;
/*  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor2D_Red;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget2D_Red;
  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor2D_Yellow;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget2D_Yellow;
  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor2D_Green;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget2D_Green;
  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor3D;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget3D;
*/
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLScalarBarDisplayableManager* external)
{
  this->External = external;
  this->WindowLevelWidget = vtkSmartPointer<vtkMRMLWindowLevelWidget>::New();
/*  this->ScalarBarActor2D_Red = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget2D_Red = vtkSmartPointer<vtkScalarBarWidget>::New();
  this->ScalarBarActor2D_Green = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget2D_Green = vtkSmartPointer<vtkScalarBarWidget>::New();
  this->ScalarBarActor2D_Yellow = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget2D_Yellow = vtkSmartPointer<vtkScalarBarWidget>::New();
  this->ScalarBarActor3D = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget3D = vtkSmartPointer<vtkScalarBarWidget>::New();
*/
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->WindowLevelWidget->SetMRMLApplicationLogic(nullptr);
  this->WindowLevelWidget->SetRenderer(nullptr);
  this->WindowLevelWidget->SetSliceNode(nullptr);
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::Destructor");
}

//---------------------------------------------------------------------------
vtkObserverManager* vtkMRMLScalarBarDisplayableManager::vtkInternal::GetMRMLNodesObserverManager()
{
  return this->External->GetMRMLNodesObserverManager();
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetMRMLNodesObserverManager");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::Modified()
{
  return this->External->Modified();
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::Modified");
}

//---------------------------------------------------------------------------
vtkMRMLSliceNode* vtkMRMLScalarBarDisplayableManager::vtkInternal::GetSliceNode()
{
  return this->External->GetMRMLSliceNode();
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetSliceNode");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::UpdateSliceNode()
{
  if (!this->External->GetMRMLScene())
  {
    this->WindowLevelWidget->SetSliceNode(nullptr);
    return;
  }

  vtkMRMLScalarBarDisplayNode* sbDisplayNode = this->External->GetScalarBarNode(this->External->GetMRMLScene());
  if (sbDisplayNode)
  {
    vtkWarningWithObjectMacro(this->External, "Display node is valid");
  }
  else
  {
    vtkWarningWithObjectMacro(this->External, "Display node is invalid");
  }

  if (!this->WindowLevelWidget->GetRenderer())
  {
    vtkMRMLApplicationLogic *mrmlAppLogic = this->External->GetMRMLApplicationLogic();
    this->WindowLevelWidget->SetMRMLApplicationLogic(mrmlAppLogic);
    this->WindowLevelWidget->CreateDefaultRepresentation();
    this->WindowLevelWidget->SetRenderer(this->External->GetRenderer());
  }
  this->WindowLevelWidget->SetSliceNode(this->GetSliceNode());
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::UpdateSliceNode");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::BuildScalarBar()
{
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();
  if (!interactor)
  {
    vtkWarningWithObjectMacro(this->External, "Interactor is invalid");
//    this->ScalarBarWidget2D_Green->SetInteractor(nullptr);
//    this->ScalarBarWidget2D_Yellow->SetInteractor(nullptr);
//   this->ScalarBarWidget2D_Red->SetInteractor(nullptr);
//    this->ScalarBarWidget3D->SetInteractor(nullptr);
    return;
  }

//  this->CrosshairMode = this->CrosshairNode->GetCrosshairMode();
//  Get scalar bar display node
// check scalar bar is visible
//  if (this->CrosshairNode->GetCrosshairMode() == vtkMRMLCrosshairNode::NoCrosshair)
  vtkMRMLScalarBarDisplayNode* sbDisplayNode = this->External->GetScalarBarNode(this->External->GetMRMLScene());

  if (!sbDisplayNode)
  {
//    this->ScalarBarWidget2D_Green->SetEnabled(false);
//    this->ScalarBarWidget2D_Yellow->SetEnabled(false);
//    this->ScalarBarWidget2D_Red->SetEnabled(false);
//    this->ScalarBarWidget3D->SetEnabled(false);
    vtkWarningWithObjectMacro(this->External, "No scalar bar display node");
    return;
  }
/*
  this->ScalarBarWidget2D_Yellow->SetScalarBarActor(this->ScalarBarActor2D_Yellow);
  this->ScalarBarWidget2D_Yellow->SetInteractor(interactor);
  this->ScalarBarWidget2D_Yellow->SetEnabled(true);

  this->ScalarBarWidget2D_Red->SetScalarBarActor(this->ScalarBarActor2D_Red);
  this->ScalarBarWidget2D_Red->SetInteractor(interactor);
  this->ScalarBarWidget2D_Red->SetEnabled(true);

  this->ScalarBarWidget2D_Green->SetScalarBarActor(this->ScalarBarActor2D_Green);
  this->ScalarBarWidget2D_Green->SetInteractor(interactor);
  this->ScalarBarWidget2D_Green->SetEnabled(true);

  this->ScalarBarWidget3D->SetScalarBarActor(this->ScalarBarActor3D);
  this->ScalarBarWidget3D->SetInteractor(interactor);
  this->ScalarBarWidget3D->SetEnabled(true);
*/
//  vtkWarningWithObjectMacro(this->External, "BuildScalarBar");

//  int *screenSize = interactor->GetRenderWindow()->GetScreenSize();

  // Handle size is defined a percentage of screen size to accommodate high-DPI screens
//  double handleSizeInScreenSizePercent = 5;
//  if (this->CrosshairNode->GetCrosshairMode() == vtkMRMLCrosshairNode::ShowSmallBasic
//    || this->CrosshairNode->GetCrosshairMode() == vtkMRMLCrosshairNode::ShowSmallIntersection)
//    {
//    handleSizeInScreenSizePercent = 2.5;
//    }
//  double handleSizeInPixels = double(screenSize[1])*(0.01*handleSizeInScreenSizePercent);
//  this->CrosshairRepresentation->SetHandleSize(handleSizeInPixels);

}

//---------------------------------------------------------------------------
// vtkMRMLScalarBarDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::vtkMRMLScalarBarDisplayableManager()
{
  this->Internal = new vtkInternal(this);
//  vtkWarningMacro("Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::~vtkMRMLScalarBarDisplayableManager()
{
  delete this->Internal;
//  vtkWarningMacro("Destructor");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::UpdateFromMRMLScene()
{
  this->Internal->UpdateSliceNode();
//  vtkWarningMacro("UpdateFromMRMLScene");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::UnobserveMRMLScene()
{
  this->Internal->WindowLevelWidget->SetSliceNode(nullptr);
//  vtkWarningMacro("UnobserveMRMLScene");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::Create()
{
  // Setup the SliceNode, ScalarBarNode
  this->Internal->UpdateSliceNode();
//  vtkWarningMacro("Create");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::AdditionalInitializeStep()
{
  // Build the initial scalar bar representation
  this->Internal->BuildScalarBar();
// vtkWarningMacro("AdditionalInitializeStep");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSliceNodeModifiedEvent()
{
//  vtkWarningMacro("OnMRMLSliceNodeModifiedEvent");

  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(this->GetMRMLDisplayableNode());
  if (!viewNode)
  {
//    vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: view node is invalid");
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  if (sliceNode)
  {
    vtkMRMLScalarBarDisplayNode* sbNode = this->GetScalarBarNode(this->GetMRMLScene());
    if (sbNode)
    {
      if (vtkMRMLColorTableNode* colorTableNode = sbNode->GetColorTableNode())
      {
        vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: Set lookup table");
//        this->Internal->ScalarBarActor2D_Green->SetLookupTable(colorTableNode->GetScalarsToColors());
//        this->Internal->ScalarBarActor2D_Red->SetLookupTable(colorTableNode->GetScalarsToColors());
//        this->Internal->ScalarBarActor2D_Yellow->SetLookupTable(colorTableNode->GetScalarsToColors());
      }
    }
    else
    {
//      vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: Invalid sb node");
    }
    
//    vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: Slice 2D Name " << sliceNode->GetName());
  }
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::CanProcessInteractionEvent(vtkMRMLInteractionEventData* eventData, double &closestDistance2)
{
  int eventid = eventData->GetType();
  if (eventid == vtkCommand::LeaveEvent)
  {
    this->Internal->WindowLevelWidget->Leave(eventData);
  }

  // Find/create active widget
  if (this->GetInteractionNode()->GetCurrentInteractionMode() == vtkMRMLInteractionNode::AdjustWindowLevel)
  {
    return this->Internal->WindowLevelWidget->CanProcessInteractionEvent(eventData, closestDistance2);
  }

  vtkMRMLSliceNode* sNode = this->GetMRMLSliceNode();
  if (sNode)
  {
//    vtkWarningMacro("CanProcessInteractionEvent: Slice 2D Name " << sNode->GetName());
  }

//  vtkWarningMacro("CanProcessInteractionEvent");
  return false;
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::ProcessInteractionEvent(vtkMRMLInteractionEventData* eventData)
{
  bool processed = this->Internal->WindowLevelWidget->ProcessInteractionEvent(eventData);
  if (this->Internal->WindowLevelWidget->GetNeedToRender())
  {
    this->RequestRender();
    this->Internal->WindowLevelWidget->NeedToRenderOff();
  }
//  vtkWarningMacro("ProcessInteractionEvent");
  return processed;
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::SetAdjustForegroundWindowLevelEnabled(bool enabled)
{
  this->Internal->WindowLevelWidget->SetForegroundVolumeEditable(enabled);
 // vtkWarningMacro("SetAdjustForegroundWindowLevelEnabled: Enable state " << enabled);
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::GetAdjustForegroundWindowLevelEnabled()
{
  return this->Internal->WindowLevelWidget->GetForegroundVolumeEditable();
//  vtkWarningMacro("GetAdjustForegroundWindowLevelEnabled");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::SetAdjustBackgroundWindowLevelEnabled(bool enabled)
{
  this->Internal->WindowLevelWidget->SetBackgroundVolumeEditable(enabled);
//  vtkWarningMacro("SetAdjustBackgroundWindowLevelEnabled: Enable state: " << enabled);
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::GetAdjustBackgroundWindowLevelEnabled()
{
  return this->Internal->WindowLevelWidget->GetBackgroundVolumeEditable();
//  vtkWarningMacro("GetAdjustBackgroundWindowLevelEnabled");
}

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayNode* vtkMRMLScalarBarDisplayableManager::GetScalarBarNode(vtkMRMLScene* scene)
{
  if (!scene)
  {
    return nullptr;
  }

  vtkSmartPointer<vtkCollection> barsCollection = vtkSmartPointer<vtkCollection>::New();
  barsCollection = vtkSmartPointer<vtkCollection>::Take(scene->GetNodesByClass("vtkMRMLScalarBarDisplayNode"));

  barsCollection->InitTraversal();
  for (int i = 0; i < barsCollection->GetNumberOfItems(); ++i)
  {
    vtkMRMLScalarBarDisplayNode* barNode = vtkMRMLScalarBarDisplayNode::SafeDownCast(barsCollection->GetItemAsObject(i));
    if (barNode && barNode->GetScalarBarTagName() == std::string("default"))
    {
      return barNode;
    }
  }
  return nullptr;
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneEndClose()
{
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneStartImport()
{

}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneEndImport()
{
  if (!this->GetMRMLScene())
    {
    return;
    }
  if (this->GetMRMLSliceNode())
    {
    }
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneEndRestore()
{
  if (this->GetMRMLSliceNode())
    {
    }

}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene())
  {
    vtkErrorMacro("OnMRMLSceneNodeAdded: Invalid MRML scene or input node");
    return;
  }

  if (node->IsA("vtkMRMLScalarBarDisplayNode"))
  {
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    vtkObserveMRMLNodeEventsMacro(node, events);
    vtkWarningMacro("OnMRMLSceneNodeAdded: Modified event observer added for scalar bar node");
  }
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
//  if (this->GetMRMLScene()->IsBatchProcessing() ||
//      !node->IsA("vtkMRMLScalarBarDisplayNode"))
    {
    return;
    }
//  if (node == this->Internal->CameraNode)
//    {
    // we need to find another camera node for the observed view node
 //   this->UpdateCameraNode();
//    }
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::ObserveMRMLScene()
{
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLNodeModified(vtkMRMLNode* node)
{
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::ProcessMRMLNodesEvents(vtkObject *caller, unsigned long event, void *callData)
{
  switch(event)
  {
  // Maybe something external removed the view/camera link, make sure the
  // view observes a camera node, create a new camera node if needed
  case vtkCommand::ModifiedEvent:
    {
      vtkMRMLScalarBarDisplayNode* node = vtkMRMLScalarBarDisplayNode::SafeDownCast(caller);
      if (node)
      {
        vtkWarningMacro("ProcessMRMLNodesEvents: got a scalar bar display node modified event");
      }
    }
    break;
  default:
    this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
    break;
  }
/*
  switch(event)
    {
    // Maybe something external removed the view/camera link, make sure the
    // view observes a camera node, create a new camera node if needed
    case vtkMRMLCameraNode::LayoutNameModifiedEvent:
      assert(vtkMRMLCameraNode::SafeDownCast(caller));
      this->UpdateCameraNode();
      vtkDebugMacro("ProcessingMRML: got a camera node modified event");
      break;
    case vtkMRMLCameraNode::ResetCameraClippingEvent:
      assert(vtkMRMLCameraNode::SafeDownCast(caller));
      vtkDebugMacro("ProcessingMRML: got a camera node modified event");
      if (this->GetRenderer())
        {
        this->GetRenderer()->ResetCameraClippingRange();
        this->GetRenderer()->UpdateLightsGeometryToFollowCamera();
        }
      else if (this->GetCameraNode() && this->GetCameraNode()->GetCamera())
        {
        vtkCamera* camera = this->GetCameraNode()->GetCamera();
        camera->SetClippingRange(0.1, camera->GetDistance()*2);
        }
      break;
    default:
      this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
      break;
    }
*/
}
