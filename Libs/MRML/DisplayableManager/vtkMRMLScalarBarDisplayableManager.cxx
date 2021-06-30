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
  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLScalarBarDisplayableManager* external)
{
  this->External = external;
  this->WindowLevelWidget = vtkSmartPointer<vtkMRMLWindowLevelWidget>::New();
  this->ScalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->WindowLevelWidget->SetMRMLApplicationLogic(nullptr);
  this->WindowLevelWidget->SetRenderer(nullptr);
  this->WindowLevelWidget->SetSliceNode(nullptr);
  vtkWarningWithObjectMacro(this->External, "vtkInternal::Destructor");
}

//---------------------------------------------------------------------------
vtkObserverManager* vtkMRMLScalarBarDisplayableManager::vtkInternal::GetMRMLNodesObserverManager()
{
  return this->External->GetMRMLNodesObserverManager();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetMRMLNodesObserverManager");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::Modified()
{
  return this->External->Modified();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::Modified");
}

//---------------------------------------------------------------------------
vtkMRMLSliceNode* vtkMRMLScalarBarDisplayableManager::vtkInternal::GetSliceNode()
{
  return this->External->GetMRMLSliceNode();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetSliceNode");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::UpdateSliceNode()
{
  if (!this->External->GetMRMLScene())
  {
    this->WindowLevelWidget->SetSliceNode(nullptr);
    return;
  }

  if (!this->WindowLevelWidget->GetRenderer())
  {
    vtkMRMLApplicationLogic *mrmlAppLogic = this->External->GetMRMLApplicationLogic();
    this->WindowLevelWidget->SetMRMLApplicationLogic(mrmlAppLogic);
    this->WindowLevelWidget->CreateDefaultRepresentation();
    this->WindowLevelWidget->SetRenderer(this->External->GetRenderer());
  }
  this->WindowLevelWidget->SetSliceNode(this->GetSliceNode());
  vtkWarningWithObjectMacro(this->External, "vtkInternal::UpdateSliceNode");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::vtkInternal::BuildScalarBar()
{
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();
  if (!interactor)
  {
    this->ScalarBarWidget->SetInteractor(nullptr);
    return;
  }

//  this->CrosshairMode = this->CrosshairNode->GetCrosshairMode();
//  Get scalar bar display node
// check scalar bar is visible
//  if (this->CrosshairNode->GetCrosshairMode() == vtkMRMLCrosshairNode::NoCrosshair)
  {
    this->ScalarBarWidget->SetEnabled(false);
    return;
  }
  this->ScalarBarWidget->SetScalarBarActor(this->ScalarBarActor);
  this->ScalarBarWidget->SetInteractor(interactor);
  this->ScalarBarWidget->SetEnabled(true);

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
  vtkWarningMacro("Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLScalarBarDisplayableManager::~vtkMRMLScalarBarDisplayableManager()
{
  delete this->Internal;
  vtkWarningMacro("Destructor");
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
  vtkWarningMacro("UpdateFromMRMLScene");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::UnobserveMRMLScene()
{
  this->Internal->WindowLevelWidget->SetSliceNode(nullptr);
  vtkWarningMacro("UnobserveMRMLScene");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::Create()
{
  // Setup the SliceNode, ScalarBarNode
  this->Internal->UpdateSliceNode();
  vtkWarningMacro("Create");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::AdditionalInitializeStep()
{
  // Build the initial crosshair representation
  this->Internal->BuildScalarBar();
  vtkWarningMacro("AdditionalInitializeStep");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::OnMRMLSliceNodeModifiedEvent()
{
  vtkWarningMacro("OnMRMLSliceNodeModifiedEvent");

  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(this->GetMRMLDisplayableNode());
  if (!viewNode)
  {
    vtkErrorMacro("OnMRMLSliceNodeModifiedEvent: view node is invalid");
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  if (sliceNode)
  {
    vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: Slice 2D Name " << sliceNode->GetName());
  }
  else if (threeDViewNode)
  {
    vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: 3D Name " << threeDViewNode->GetName());
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
    vtkWarningMacro("CanProcessInteractionEvent: Slice 2D Name " << sNode->GetName());
  }

  vtkWarningMacro("CanProcessInteractionEvent");
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
  vtkWarningMacro("ProcessInteractionEvent");
  return processed;
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::SetAdjustForegroundWindowLevelEnabled(bool enabled)
{
  this->Internal->WindowLevelWidget->SetForegroundVolumeEditable(enabled);
  vtkWarningMacro("SetAdjustForegroundWindowLevelEnabled: Enable state " << enabled);
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::GetAdjustForegroundWindowLevelEnabled()
{
  return this->Internal->WindowLevelWidget->GetForegroundVolumeEditable();
  vtkWarningMacro("GetAdjustForegroundWindowLevelEnabled");
}

//---------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayableManager::SetAdjustBackgroundWindowLevelEnabled(bool enabled)
{
  this->Internal->WindowLevelWidget->SetBackgroundVolumeEditable(enabled);
  vtkWarningMacro("SetAdjustBackgroundWindowLevelEnabled: Enable state: " << enabled);
}

//---------------------------------------------------------------------------
bool vtkMRMLScalarBarDisplayableManager::GetAdjustBackgroundWindowLevelEnabled()
{
  return this->Internal->WindowLevelWidget->GetBackgroundVolumeEditable();
  vtkWarningMacro("GetAdjustBackgroundWindowLevelEnabled");
}
