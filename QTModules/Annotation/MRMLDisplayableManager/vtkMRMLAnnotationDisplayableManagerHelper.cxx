#include "MRMLDisplayableManager/vtkMRMLAnnotationDisplayableManagerHelper.h"

// AnnotationModule/MRML includes
#include "vtkMRMLAnnotationNode.h"

// VTK includes
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkAbstractWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSeedWidget.h>
#include <vtkHandleWidget.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSeedRepresentation.h>
#include <vtkProperty.h>

// MRML includes
#include <vtkMRMLSliceNode.h>

// STD includes
#include <vector>
#include <map>
#include <algorithm>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMRMLAnnotationDisplayableManagerHelper);
vtkCxxRevisionMacro (vtkMRMLAnnotationDisplayableManagerHelper, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

vtkMRMLAnnotationDisplayableManagerHelper::vtkMRMLAnnotationDisplayableManagerHelper()
{

  this->SeedWidget = 0;

}

vtkMRMLAnnotationDisplayableManagerHelper::~vtkMRMLAnnotationDisplayableManagerHelper()
{
  this->SeedWidget = 0;
}


//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::UpdateLocked(vtkMRMLAnnotationNode* node)
{
  // Sanity checks
  if (node == 0)
    {
    return;
    }

  vtkAbstractWidget * widget = this->GetWidget(node);
  // A widget is expected
  if(widget == 0)
    {
    return;
    }

  if (node->GetLocked())
    {
    widget->ProcessEventsOff();
    }
  else
    {
    widget->ProcessEventsOn();
    }
}

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::UpdateVisible(
    vtkMRMLAnnotationNode* node)
{
  // Sanity checks
  if (node == 0)
    {
    return;
    }

  vtkAbstractWidget * widget = this->GetWidget(node);
  // A widget is expected
  if(widget == 0)
    {
    return;
    }

  if (node->GetVisible())
    {
    widget->EnabledOn();
    }
  else
    {
    widget->EnabledOff();
    }
}

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::UpdateWidget(
    vtkMRMLAnnotationNode *node)
{
  if (!node)
    {
      return;
    }

  vtkAbstractWidget * widget = this->GetWidget(node);
  // Widget is expected to be valid
  if (widget == 0)
    {
    return;
    }

  this->UpdateLocked(node);
  this->UpdateVisible(node);

}

//---------------------------------------------------------------------------
vtkAbstractWidget * vtkMRMLAnnotationDisplayableManagerHelper::GetWidget(
    vtkMRMLAnnotationNode * node)
{
  if (!node)
    {
    return 0;
    }

  // Make sure the map contains a vtkWidget associated with this node
  WidgetsIt it = this->Widgets.find(node);
  if (it == this->Widgets.end())
    {
    return 0;
    }

  return it->second;
}

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::RemoveWidget(
    vtkMRMLAnnotationNode *node)
{
  if (!node)
  {
    return;
  }

  // Make sure the map contains a vtkWidget associated with this node
  WidgetsIt it = this->Widgets.find(node);
  if (it == this->Widgets.end()) {
    return;
  }

  // Delete and Remove vtkWidget from the map
  this->Widgets[node]->Delete();
  this->Widgets.erase(node);

  vtkMRMLAnnotationDisplayableManagerHelper::AnnotationNodeListIt it2 = std::find(
      this->AnnotationNodeList.begin(),
      this->AnnotationNodeList.end(),
      node);

  this->AnnotationNodeList.erase(it2);
}

//---------------------------------------------------------------------------
double vtkMRMLAnnotationDisplayableManagerHelper::GetSliceOffset(vtkMRMLSliceNode * sliceNode)
{
  //
  // - get the current translation in RAS space and convert it to Slice space
  //   by transforming it by the inverse of the upper 3x3 of SliceToRAS
  // - pull out the Z translation part
  //

  if ( !sliceNode )
    {
    vtkErrorMacro("GetSliceOffset: Could not get sliceNode.")
    return 0;
    }

  vtkSmartPointer<vtkMatrix4x4> sliceToRAS = vtkSmartPointer<vtkMatrix4x4>::New();
  sliceToRAS->DeepCopy( sliceNode->GetSliceToRAS() );
  for (int i = 0; i < 3; i++)
    {
    sliceToRAS->SetElement( i, 3, 0.0 );  // Zero out the tranlation portion
    }
  sliceToRAS->Invert();
  double v1[4], v2[4];
  for (int i = 0; i < 4; i++)
    { // get the translation back as a vector
    v1[i] = sliceNode->GetSliceToRAS()->GetElement( i, 3 );
    }
  // bring the translation into slice space
  // and overwrite the z part
  sliceToRAS->MultiplyPoint(v1, v2);

  return ( v2[2] );

}

//---------------------------------------------------------------------------
// Seeds for widget placement
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::PlaceSeed(double x, double y, vtkRenderWindowInteractor * interactor, vtkRenderer * renderer)
{
  vtkDebugMacro("PlaceSeed: " << x << ":" << y)

  if (!interactor)
    {
    vtkErrorMacro("PlaceSeed: No render interactor found.")
    }

  if (!renderer)
    {
    vtkErrorMacro("PlaceSeed: No render renderer found.")
    }

  if (!this->SeedWidget)
    {

    VTK_CREATE(vtkSphereHandleRepresentation, handle);
    handle->GetProperty()->SetColor(1,0,0);
    handle->SetHandleSize(5);

    VTK_CREATE(vtkSeedRepresentation, rep);
    rep->SetHandleRepresentation(handle);

    //seed widget
    vtkSeedWidget * seedWidget = vtkSeedWidget::New();
    seedWidget->SetRepresentation(rep);

    seedWidget->SetInteractor(interactor);
    seedWidget->SetCurrentRenderer(renderer);

    seedWidget->ProcessEventsOff();
    seedWidget->CompleteInteraction();

    this->SeedWidget = seedWidget;

    }

  // Seed widget exists here, just add a new handle at the position x,y

  double p[3];
  p[0]=x;
  p[1]=y;
  p[2]=0;

  //VTK_CREATE(vtkHandleWidget, newhandle);
  vtkHandleWidget * newhandle = this->SeedWidget->CreateNewHandle();
  vtkHandleRepresentation::SafeDownCast(newhandle->GetRepresentation())->SetDisplayPosition(p);

  this->HandleWidgetList.push_back(newhandle);

  this->SeedWidget->On();

}

//---------------------------------------------------------------------------
void vtkMRMLAnnotationDisplayableManagerHelper::RemoveSeeds()
{
  while(!this->HandleWidgetList.empty())
    {
    this->HandleWidgetList.pop_back();
    }
  if (this->SeedWidget)
    {
    this->SeedWidget->Off();
    this->SeedWidget = 0;
    }
}

//---------------------------------------------------------------------------
vtkHandleWidget * vtkMRMLAnnotationDisplayableManagerHelper::GetSeed(int index)
{
  if (this->HandleWidgetList.empty())
    {
    return 0;
    }

  return this->HandleWidgetList[index];
}
