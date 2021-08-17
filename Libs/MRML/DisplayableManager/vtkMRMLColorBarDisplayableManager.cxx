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
#include <vtkMRMLWindowLevelWidget.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLColorBarDisplayNode.h>

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
vtkStandardNewMacro(vtkMRMLColorBarDisplayableManager);

//---------------------------------------------------------------------------
class vtkMRMLColorBarDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkMRMLColorBarDisplayableManager * external);
  virtual ~vtkInternal();

  vtkObserverManager* GetMRMLNodesObserverManager();
  void Modified();

  // Slice
  vtkMRMLSliceNode* GetSliceNode();
  void UpdateSliceNode();

  // Build the color bar
  void BuildColorBar();

  vtkMRMLColorBarDisplayableManager*        External;

  vtkSmartPointer<vtkScalarBarActor> ScalarBarActor;
  vtkSmartPointer<vtkScalarBarWidget> ScalarBarWidget;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLColorBarDisplayableManager* external)
{
  this->External = external;
  this->ScalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
  this->ScalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();

  this->ScalarBarWidget->SetScalarBarActor(this->ScalarBarActor);

  vtkWarningWithObjectMacro(this->External, "vtkInternal::Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ScalarBarWidget->SetInteractor(nullptr);
  vtkWarningWithObjectMacro(this->External, "vtkInternal::Destructor");
}

//---------------------------------------------------------------------------
vtkObserverManager* vtkMRMLColorBarDisplayableManager::vtkInternal::GetMRMLNodesObserverManager()
{
  return this->External->GetMRMLNodesObserverManager();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetMRMLNodesObserverManager");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::Modified()
{
  return this->External->Modified();
  vtkWarningWithObjectMacro(this->External, "vtkInternal::Modified");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::BuildColorBar()
{
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();
  if (!interactor)
  {
    vtkWarningWithObjectMacro(this->External, "Interactor is invalid");
     this->ScalarBarWidget->SetInteractor(nullptr);
    return;
  }

//  vtkMRMLColorBarDisplayNode* node = this->External->GetColorBarNode(this->External->GetMRMLScene());

//  if (!node || (node && !node->GetVisibilityOnSliceViewsFlag()))
//  {
//    this->ScalarBarWidget->SetEnabled(false);
//    vtkWarningWithObjectMacro(this->External, "No scalar bar display node");
//    return;
//  }

  this->ScalarBarWidget->SetInteractor(interactor);
  this->ScalarBarWidget->SetEnabled(true);
}

//---------------------------------------------------------------------------
// vtkMRMLColorBarDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkMRMLColorBarDisplayableManager()
{
  this->Internal = new vtkInternal(this);
  vtkWarningMacro("Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::~vtkMRMLColorBarDisplayableManager()
{
  delete this->Internal;
  vtkWarningMacro("Destructor");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidget() const
{
  return this->Internal->ScalarBarWidget.GetPointer();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::Create()
{
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::AdditionalInitializeStep()
{
  // Build the initial scalar bar
  this->Internal->BuildColorBar();
 vtkWarningMacro("AdditionalInitializeStep");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
  vtkWarningMacro("OnMRMLSliceNodeModifiedEvent");

  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(this->GetMRMLDisplayableNode());
  if (!viewNode)
  {
    vtkWarningMacro("OnMRMLSliceNodeModifiedEvent: view node is invalid");
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  if (sliceNode)
  {
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Slice 2D Name " << sliceNode->GetName());
  }
  else if (threeDViewNode)
  {
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: 3D Name " << threeDViewNode->GetName());
  }

/*  if (sliceNode)
  {
    vtkMRMLScalarBarDisplayNode* sbNode = this->GetScalarBarNode(this->GetMRMLScene());
    if (sbNode)
    {
      if (vtkMRMLColorTableNode* colorTableNode = sbNode->GetColorTableNode())
      {
        this->Internal->BuildScalarBar();
        this->Internal->ScalarBarActor->SetLookupTable(colorTableNode->GetScalarsToColors());
      }
    }
    else
    {
    }
  }
*/
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
    vtkWarningMacro("OnMRMLSceneNodeAdded: Modified event observer added for color bar node");
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::ProcessMRMLNodesEvents(vtkObject *caller, unsigned long event, void *callData)
{
//  vtkWarningMacro("ProcessMRMLNodesEvents: process color bar node events");

  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  switch(event)
  {
  // Maybe something external removed the view/camera link, make sure the
  // view observes a camera node, create a new camera node if needed
  case vtkCommand::ModifiedEvent:
    {
      vtkMRMLColorBarDisplayNode* cbNode = vtkMRMLColorBarDisplayNode::SafeDownCast(caller);
      if (cbNode)
      {
        vtkWarningMacro("ProcessMRMLNodesEvents: Color bar node modified, visibility 2D " << cbNode->GetVisibility2D()
          << " visibility 3D " << cbNode->GetVisibility3D() << " preset position " << cbNode->GetPositionPreset());
      }
    }
    break;
  default:
    break;
  }
}
