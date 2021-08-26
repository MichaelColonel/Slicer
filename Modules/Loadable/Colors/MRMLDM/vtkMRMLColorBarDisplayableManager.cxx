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
#include <vtkScalarBarRepresentation.h>
#include <vtkScalarBarWidget.h>
#include <vtkScalarBarActor.h>

// STD includes
#include <algorithm>
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

  // Build the color bars
  void BuildColorBar2D(vtkMRMLSliceNode* sliceNode = nullptr);
  void BuildColorBar3D(vtkMRMLViewNode* viewNode = nullptr);
  // Setup actor and widget / widget representation
  void SetupActor(vtkScalarBarActor*, vtkScalarBarWidget*);

  vtkMRMLColorBarDisplayableManager*        External;

  vtkSmartPointer<vtkScalarBarActor> ColorBarActorRed;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidgetRed;
  vtkSmartPointer<vtkScalarBarActor> ColorBarActorGreen;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidgetGreen;
  vtkSmartPointer<vtkScalarBarActor> ColorBarActorYellow;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidgetYellow;
  vtkSmartPointer<vtkScalarBarActor> ColorBarActor3D;
  vtkSmartPointer<vtkScalarBarWidget> ColorBarWidget3D;
  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ColorBarDisplayNode;
};


//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::vtkInternal(vtkMRMLColorBarDisplayableManager* external)
  :
  External(external),
  ColorBarActorRed(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidgetRed(vtkSmartPointer<vtkScalarBarWidget>::New()),
  ColorBarActorGreen(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidgetGreen(vtkSmartPointer<vtkScalarBarWidget>::New()),
  ColorBarActorYellow(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidgetYellow(vtkSmartPointer<vtkScalarBarWidget>::New()),
  ColorBarActor3D(vtkSmartPointer<vtkScalarBarActor>::New()),
  ColorBarWidget3D(vtkSmartPointer<vtkScalarBarWidget>::New())
{
 // vtkWarningWithObjectMacro(this->External, "vtkInternal::Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ColorBarWidgetRed->SetInteractor(nullptr);
  this->ColorBarWidgetGreen->SetInteractor(nullptr);
  this->ColorBarWidgetYellow->SetInteractor(nullptr);
  this->ColorBarWidget3D->SetInteractor(nullptr);
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::Destructor");
}

//---------------------------------------------------------------------------
vtkObserverManager* vtkMRMLColorBarDisplayableManager::vtkInternal::GetMRMLNodesObserverManager()
{
  return this->External->GetMRMLNodesObserverManager();
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::GetMRMLNodesObserverManager");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::Modified()
{
  return this->External->Modified();
//  vtkWarningWithObjectMacro(this->External, "vtkInternal::Modified");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::BuildColorBar3D(vtkMRMLViewNode* viewNode)
{
  if (!viewNode)
  {
    vtkMRMLNode* node = this->External->GetMRMLDisplayableNode();
    if (node)
    {
      viewNode = vtkMRMLViewNode::SafeDownCast(node);
    }
  }

  if (!viewNode)
  {
//    vtkErrorWithObjectMacro(this->External, "3D view node is invalid");
    return;
  }

//  vtkWarningWithObjectMacro( this->External, "View node name: " << viewNode->GetName());
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();
  if (!interactor)
  {
//    this->ColorBarWidget3D->SetEnabled(false);
    this->ColorBarWidget3D->Off();
    this->ColorBarWidget3D->SetInteractor(nullptr);
    return;
  }
//  vtkWarningWithObjectMacro(this->External, "Build color bar 3D");
  // Setup/update scalar bar actor
  if (this->ColorBarDisplayNode)
  {
    SetupActor(this->ColorBarActor3D, this->ColorBarWidget3D);

    this->ColorBarWidget3D->SetScalarBarActor(this->ColorBarActor3D);
    this->ColorBarWidget3D->SetInteractor(interactor);
//    this->ColorBarWidget3D->SetEnabled(this->ColorBarDisplayNode->GetVisibility3D());
    if (this->ColorBarDisplayNode->GetVisibility3D())
    {
      this->ColorBarWidget3D->On();
    }
    else
    {
      this->ColorBarWidget3D->Off();
    }
  }
  else
  {
//    vtkErrorWithObjectMacro(this->External, "Color bar display node is invalid");
    return;
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::BuildColorBar2D(vtkMRMLSliceNode* sliceNode)
{
  if (!sliceNode)
  {
    vtkMRMLNode* node = this->External->GetMRMLDisplayableNode();
    if (node)
    {
      sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
    }
  }

  if (!sliceNode)
  {
//    vtkErrorWithObjectMacro(this->External, "Slice node is invalid");
    return;
  }

//  vtkWarningWithObjectMacro( this->External, "Slice node name: " << sliceNode->GetName());
  vtkRenderWindowInteractor* interactor = this->External->GetInteractor();

  if (std::strcmp(sliceNode->GetName(), "Red") == 0)
  {
    if (!interactor)
    {
//      this->ColorBarWidgetRed->SetEnabled(false);
      this->ColorBarWidgetRed->Off();
      this->ColorBarWidgetRed->SetInteractor(nullptr);
      return;
    }
//    vtkWarningWithObjectMacro(this->External, "Build color bar 2D red");
    // Setup/update scalar bar actor
    if (this->ColorBarDisplayNode)
    {
      SetupActor(this->ColorBarActorRed, this->ColorBarWidgetRed);

      this->ColorBarWidgetRed->SetScalarBarActor(this->ColorBarActorRed);
      this->ColorBarWidgetRed->SetInteractor(interactor);
//      this->ColorBarWidgetRed->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
      if (this->ColorBarDisplayNode->GetVisibility2D())
      {
        this->ColorBarWidgetRed->On();
      }
      else
      {
        this->ColorBarWidgetRed->Off();
      }
    }
    else
    {
//      vtkErrorWithObjectMacro(this->External, "Color bar display node is invalid");
      return;
    }
  }
  else if (std::strcmp(sliceNode->GetName(), "Green") == 0)
  {
    if (!interactor)
    {
//      this->ColorBarWidgetGreen->SetEnabled(false);
      this->ColorBarWidgetGreen->Off();
      this->ColorBarWidgetGreen->SetInteractor(nullptr);
      return;
    }
//    vtkWarningWithObjectMacro(this->External, "Build color bar 2D green");
    // Setup/update scalar bar actor
    if (this->ColorBarDisplayNode)
    {
      SetupActor(this->ColorBarActorGreen, this->ColorBarWidgetGreen);

      this->ColorBarWidgetGreen->SetScalarBarActor(this->ColorBarActorGreen);
      this->ColorBarWidgetGreen->SetInteractor(interactor);
//      this->ColorBarWidgetGreen->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
      if (this->ColorBarDisplayNode->GetVisibility2D())
      {
        this->ColorBarWidgetGreen->On();
      }
      else
      {
        this->ColorBarWidgetGreen->Off();
      }
    }
    else
    {
//      vtkErrorWithObjectMacro(this->External, "Color bar display node is invalid");
      return;
    }
  }
  else if (std::strcmp(sliceNode->GetName(), "Yellow") == 0)
  {
    if (!interactor)
    {
//      this->ColorBarWidgetYellow->SetEnabled(false);
      this->ColorBarWidgetYellow->Off();
      this->ColorBarWidgetYellow->SetInteractor(nullptr);
      return;
    }
//    vtkWarningWithObjectMacro(this->External, "Build color bar 2D yellow");
    // Setup/update scalar bar actor
    if (this->ColorBarDisplayNode)
    {
      SetupActor(this->ColorBarActorYellow, this->ColorBarWidgetYellow);

      this->ColorBarWidgetYellow->SetScalarBarActor(this->ColorBarActorYellow);
      this->ColorBarWidgetYellow->SetInteractor(interactor);
//      this->ColorBarWidgetYellow->SetEnabled(this->ColorBarDisplayNode->GetVisibility2D());
      if (this->ColorBarDisplayNode->GetVisibility2D())
      {
        this->ColorBarWidgetYellow->On();
      }
      else
      {
        this->ColorBarWidgetYellow->Off();
      }
    }
    else
    {
//      vtkErrorWithObjectMacro(this->External, "Color bar display node is invalid");
      return;
    }
  }
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::vtkInternal::SetupActor(vtkScalarBarActor* actor,
  vtkScalarBarWidget* widget)
{
  vtkScalarBarRepresentation* repr = widget->GetScalarBarRepresentation();
  if (repr)
  {
//    vtkErrorWithObjectMacro(this->External, "SetupActor: representation is valid");
    repr->SetShowHorizontalBorder(true);
    repr->SetShowVerticalBorder(true);
    // only show the border when hovering over with the mouse
    repr->SetShowBorderToActive();
  }

  switch (this->ColorBarDisplayNode->GetPositionPreset())
  {
  case vtkMRMLColorBarDisplayNode::VerticalRight:
  case vtkMRMLColorBarDisplayNode::VerticalLeft:
    actor->SetOrientationToVertical();

//    actor->SetNumberOfLabels(11);
//    actor->SetTitle("(mm)");

    // it's a 2d actor, position it in screen space by percentages
//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.1);
//    actor->SetHeight(0.8);

    // By default, color swatch is too wide (especially when showing long color names),
    // therefore, set it to a bit narrower.
//    actor->SetBarRatio(0.15);

//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.1);
//    actor->SetHeight(0.8);
//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.1);
//    actor->SetHeight(0.8);
    break;
  case vtkMRMLColorBarDisplayNode::HorizontalTop:
  case vtkMRMLColorBarDisplayNode::HorizontalBottom:
    actor->SetOrientationToHorizontal();

//    actor->SetNumberOfLabels(11);
//    actor->SetTitle("(mm)");

    // it's a 2d actor, position it in screen space by percentages
//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.8);
//    actor->SetHeight(0.1);

    // By default, color swatch is too wide (especially when showing long color names),
    // therefore, set it to a bit narrower.
//    actor->SetBarRatio(0.15);

//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.8);
//    actor->SetHeight(0.1);
//    actor->SetPosition(0.1, 0.1);
//    actor->SetWidth(0.8);
//    actor->SetHeight(0.1);
    break;
  default:
    break;
  }
}

//---------------------------------------------------------------------------
// vtkMRMLColorBarDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::vtkMRMLColorBarDisplayableManager()
{
  this->Internal = new vtkInternal(this);
//  vtkWarningMacro("Constructor");
}

//---------------------------------------------------------------------------
vtkMRMLColorBarDisplayableManager::~vtkMRMLColorBarDisplayableManager()
{
  delete this->Internal;
//  vtkWarningMacro("Destructor");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::GetScalarBarWidgets(
  vtkScalarBarWidget* widget2D[3], vtkScalarBarWidget* widget3D) const
{
  widget2D[0] = this->Internal->ColorBarWidgetRed.GetPointer();
  widget2D[1] = this->Internal->ColorBarWidgetGreen.GetPointer();
  widget2D[2] = this->Internal->ColorBarWidgetYellow.GetPointer();
  widget3D = this->Internal->ColorBarWidget3D.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidgetRed() const
{
  return this->Internal->ColorBarWidgetRed.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidgetGreen() const
{
  return this->Internal->ColorBarWidgetGreen.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidgetYellow() const
{
  return this->Internal->ColorBarWidgetYellow.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarWidget* vtkMRMLColorBarDisplayableManager::GetScalarBarWidget3D() const
{
  return this->Internal->ColorBarWidget3D.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActorRed() const
{
  return this->Internal->ColorBarActorRed.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActorGreen() const
{
  return this->Internal->ColorBarActorGreen.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActorYellow() const
{
  return this->Internal->ColorBarActorYellow.GetPointer();
}

//---------------------------------------------------------------------------
vtkScalarBarActor* vtkMRMLColorBarDisplayableManager::GetScalarBarActor3D() const
{
  return this->Internal->ColorBarActor3D.GetPointer();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::Create()
{
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::AdditionalInitializeStep()
{
  // Build the initial scalar bar
  this->Internal->BuildColorBar2D();
  this->Internal->BuildColorBar3D();
//  vtkWarningMacro("AdditionalInitializeStep");
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
//  vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent");

  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(this->GetMRMLDisplayableNode());
  if (!viewNode)
  {
//    vtkErrorMacro("OnMRMLDisplayableNodeModifiedEvent: View node is invalid");
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  
  vtkMRMLColorTableNode* colorTableNode = nullptr;
  if (this->Internal->ColorBarDisplayNode)
  {
    vtkMRMLDisplayableNode* displayableNode = this->Internal->ColorBarDisplayNode->GetDisplayableNode();
    colorTableNode = this->Internal->ColorBarDisplayNode->GetColorTableNode();
    if (colorTableNode)
    {
      if (threeDViewNode)
      {
//        vtkWarningMacro("3D view node name: " << threeDViewNode->GetName());
        this->Internal->BuildColorBar3D(threeDViewNode);
      }
      else if (sliceNode)
      {
//        vtkWarningMacro("Slice vode name: " << sliceNode->GetName());
        this->Internal->BuildColorBar2D(sliceNode);
      }
    }
    else if (!colorTableNode && displayableNode)
    {
      vtkMRMLDisplayableNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(displayableNode);
      if (volumeNode)
      {
        vtkMRMLDisplayNode* volumeDisplayNode = volumeNode->GetDisplayNode();
        if (vtkMRMLColorNode* colorNode = volumeDisplayNode->GetColorNode())
        {
          if (threeDViewNode)
          {
//            vtkWarningMacro("3D view node name: " << threeDViewNode->GetName());
            this->Internal->BuildColorBar3D(threeDViewNode);
          }
          else if (sliceNode)
          {
//            vtkWarningMacro("Slice vode name: " << sliceNode->GetName());
            this->Internal->BuildColorBar2D(sliceNode);
          }

          colorTableNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
        }
/*
        switch (this->Internal->ColorBarDisplayNode->GetPositionPreset())
        {
        case vtkMRMLColorBarDisplayNode::VerticalRight:
        case vtkMRMLColorBarDisplayNode::VerticalLeft:
          this->Internal->ColorBarActor2D->SetOrientationToVertical();
          this->Internal->ColorBarActor3D->SetOrientationToVertical();
          this->Internal->ColorBarActor2D->SetPosition(0.1, 0.1);
          this->Internal->ColorBarActor2D->SetWidth(0.1);
          this->Internal->ColorBarActor2D->SetHeight(0.8);
          this->Internal->ColorBarActor3D->SetPosition(0.1, 0.1);
          this->Internal->ColorBarActor3D->SetWidth(0.1);
          this->Internal->ColorBarActor3D->SetHeight(0.8);
          break;
        case vtkMRMLColorBarDisplayNode::HorizontalTop:
        case vtkMRMLColorBarDisplayNode::HorizontalBottom:
          this->Internal->ColorBarActor2D->SetOrientationToHorizontal();
          this->Internal->ColorBarActor3D->SetOrientationToHorizontal();
          this->Internal->ColorBarActor2D->SetPosition(0.1, 0.1);
          this->Internal->ColorBarActor2D->SetWidth(0.8);
          this->Internal->ColorBarActor2D->SetHeight(0.1);
          this->Internal->ColorBarActor3D->SetPosition(0.1, 0.1);
          this->Internal->ColorBarActor3D->SetWidth(0.8);
          this->Internal->ColorBarActor3D->SetHeight(0.1);
          break;
        default:
          break;
        }
        vtkMRMLColorTableNode* colorTableNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
        if (colorTableNode)
        {
          this->Internal->ColorBarActor2D->SetLookupTable(colorTableNode->GetScalarsToColors());
          this->Internal->ColorBarActor3D->SetLookupTable(colorTableNode->GetScalarsToColors());
        }
        */
      }
    }
  }
  if (colorTableNode)
  {
    this->Internal->ColorBarActorRed->SetLookupTable(colorTableNode->GetScalarsToColors());
    this->Internal->ColorBarActorGreen->SetLookupTable(colorTableNode->GetScalarsToColors());
    this->Internal->ColorBarActorYellow->SetLookupTable(colorTableNode->GetScalarsToColors());
    this->Internal->ColorBarActor3D->SetLookupTable(colorTableNode->GetScalarsToColors());
  }
/*
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
            this->RequestRender();
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
            this->RequestRender();
          }
        }
      }
      else
      {
      }
    }
    vtkWarningMacro("OnMRMLDisplayableNodeModifiedEvent: 3D Name " << threeDViewNode->GetName());
  }
*/
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene())
  {
//    vtkErrorMacro("OnMRMLSceneNodeAdded: Invalid MRML scene or input node");
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
//  vtkWarningMacro("ProcessMRMLNodesEvents: process color bar node events");

  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  switch(event)
  {
  // Maybe something external removed the view/camera link, make sure the
  // view observes a camera node, create a new camera node if needed
  case vtkCommand::ModifiedEvent:
    {
      vtkMRMLColorBarDisplayNode* colorBarNode = vtkMRMLColorBarDisplayNode::SafeDownCast(caller);
      if (!this->Internal->ColorBarDisplayNode && colorBarNode)
      {
//        vtkErrorMacro("ProcessMRMLNodesEvents: Create new color bar");
        // Build new scalar bars
        this->Internal->ColorBarDisplayNode = colorBarNode;
//        this->RequestRender();
//        vtkWarningMacro("ProcessMRMLNodesEvents: Create or update color bar, send update event");
//        this->OnMRMLDisplayableNodeModifiedEvent(cbNode);
      }
      else if (this->Internal->ColorBarDisplayNode && colorBarNode)
      {
        // Update scalar bars using new prorepties of the bars
        if (this->Internal->ColorBarDisplayNode->GetPositionPreset() !=
          colorBarNode->GetPositionPreset())
        {
//          vtkErrorMacro("ProcessMRMLNodesEvents: Update color bar orientation");
        }
        this->Internal->ColorBarDisplayNode = colorBarNode;
      }
      else
      {
//        vtkErrorMacro("ProcessMRMLNodesEvents: Unable to create/update colour bar, colour bar display node is invalid");
      }
    }
    break;
  default:
    break;
  }
}
