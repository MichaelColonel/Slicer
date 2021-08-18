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

  vtkSmartPointer<vtkScalarBarActor> ColorBarActor2D;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidget2D;
  vtkSmartPointer<vtkScalarBarActor> ColorBarActor3D;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidget3D;
  vtkMRMLColorBarDisplayNode* ColorBarDisplayNode;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLColorBarDisplayableManager* external)
  :
  External(external),
  ColorBarActor2D(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidget2D(vtkSmartPointer<vtkScalarBarWidget>::New()),
  ColorBarActor3D(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidget3D(vtkSmartPointer<vtkScalarBarWidget>::New()),
  ColorBarDisplayNode(nullptr)
{
  this->ColorBarWidget2D->SetScalarBarActor(this->ColorBarActor2D);
  this->ColorBarWidget3D->SetScalarBarActor(this->ColorBarActor3D);

  vtkWarningWithObjectMacro(this->External, "vtkInternal::Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ColorBarWidget2D->SetInteractor(nullptr);
  this->ColorBarWidget3D->SetInteractor(nullptr);
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
//    vtkWarningWithObjectMacro(this->External, "Interactor is invalid");
    this->ColorBarWidget2D->SetInteractor(nullptr);
    return;
  }

  vtkMRMLNode* viewNode = this->External->GetMRMLDisplayableNode();
  if (viewNode)
  {
    vtkMRMLViewNode* vNode = vtkMRMLViewNode::SafeDownCast(viewNode);
    if (vNode)
    {
      vtkWarningWithObjectMacro(this->External, "View node name: " << vNode->GetName());
    }
  }
//  vtkMRMLColorBarDisplayNode* node = this->External->GetColorBarNode(this->External->GetMRMLScene());

  this->ColorBarWidget2D->SetInteractor(interactor);
  
  this->ColorBarWidget2D->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
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
  return this->Internal->ColorBarWidget2D.GetPointer();
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
void vtkMRMLColorBarDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller)
{
  vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent");

  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(caller);//this->GetMRMLDisplayableNode());
  if (!viewNode)
  {
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: view node is invalid");
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  if (sliceNode)
  {
    if (this->Internal->ColorBarDisplayNode)
    {
//      vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color bar node is valid");

      vtkMRMLDisplayableNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->Internal->ColorBarDisplayNode->GetDisplayableNode());
      if (volumeNode)
      {
//        vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Volume node is valid");
        vtkMRMLDisplayNode* displayNode = volumeNode->GetDisplayNode();
        if (vtkMRMLColorNode* colorNode = displayNode->GetColorNode())
        {
//          vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color node is valid");
          
          this->Internal->BuildColorBar();
          vtkMRMLColorTableNode* colorTableNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
          if (colorTableNode)
          {
//            vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color table node is valid");
            this->Internal->ColorBarActor2D->SetLookupTable(colorTableNode->GetScalarsToColors());
          }
        }
      }
      else
      {
      }
    }
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Slice 2D Name " << sliceNode->GetName());
  }
  else if (threeDViewNode)
  {
    if (this->Internal->ColorBarDisplayNode)
    {
//      vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color bar node is valid");

      vtkMRMLDisplayableNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->Internal->ColorBarDisplayNode->GetDisplayableNode());
      if (volumeNode)
      {
        vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Volume node is valid");
        vtkMRMLDisplayNode* displayNode = volumeNode->GetDisplayNode();
        if (vtkMRMLColorNode* colorNode = displayNode->GetColorNode())
        {
          vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color node is valid");
          
          this->Internal->BuildColorBar();
          vtkMRMLColorTableNode* colorTableNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
          if (colorTableNode)
          {
            vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: Color table node is valid");
            this->Internal->ColorBarActor2D->SetLookupTable(colorTableNode->GetScalarsToColors());
          }
        }
      }
      else
      {
      }
    }
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: 3D Name " << threeDViewNode->GetName());
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
        this->Internal->ColorBarDisplayNode = cbNode;
        vtkWarningMacro("ProcessMRMLNodesEvents: Color bar node modified, visibility 2D " << cbNode->GetVisibility2D()
          << " visibility 3D " << cbNode->GetVisibility3D() << " preset position " << cbNode->GetPositionPreset());
      }
    }
    break;
  default:
    break;
  }
}
