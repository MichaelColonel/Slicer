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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QInputDialog>

// CTK includes
#include <ctkVTKScalarsToColorsView.h>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerColorsModuleWidget.h"
#include "ui_qSlicerColorsModuleWidget.h"

// qMRMLWidget includes
#include "qMRMLColorModel.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLSliceWidget.h"

// Slicer logic includes
#include <vtkSlicerColorLogic.h>

// MRML includes
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLColorBarDisplayNode.h>
#include <vtkMRMLAbstractViewNode.h>
#include <vtkMRMLAbstractDisplayableManager.h>
#include <vtkMRMLColorBarDisplayableManager.h>

// VTK includes
#include <vtkBorderRepresentation.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarWidget.h>
#include <vtkSlicerScalarBarActor.h>

// STD includes
#include <cstring>

//-----------------------------------------------------------------------------
class qSlicerColorsModuleWidgetPrivate: public Ui_qSlicerColorsModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerColorsModuleWidget);
protected:
  qSlicerColorsModuleWidget* const q_ptr;

public:
  qSlicerColorsModuleWidgetPrivate(qSlicerColorsModuleWidget& obj);
  virtual ~qSlicerColorsModuleWidgetPrivate();
  vtkSlicerColorLogic* colorLogic()const;
  void setDefaultColorNode();

  /// Node to which color bar display node will be added
  vtkWeakPointer<vtkMRMLDisplayableNode> DisplayableNode;
  /// Actor and widget from displayable manager
  vtkWeakPointer<vtkSlicerScalarBarActor> ColorBarActor;
  vtkWeakPointer<vtkScalarBarWidget> ColorBarWidget;

  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ColorBarNode;
};

//-----------------------------------------------------------------------------
qSlicerColorsModuleWidgetPrivate::qSlicerColorsModuleWidgetPrivate(qSlicerColorsModuleWidget& object)
  :
  q_ptr(&object)
{
/*
  this->ScalarBarWidget = vtkScalarBarWidget::New();
  this->ScalarBarActor = vtkSlicerScalarBarActor::New();
  this->ScalarBarWidget->SetScalarBarActor(this->ScalarBarActor);
  this->ScalarBarActor->SetOrientationToVertical();
  this->ScalarBarActor->SetNumberOfLabels(11);
  this->ScalarBarActor->SetTitle("(mm)");

  // it's a 2d actor, position it in screen space by percentages
  this->ScalarBarActor->SetPosition(0.1, 0.1);
  this->ScalarBarActor->SetWidth(0.1);
  this->ScalarBarActor->SetHeight(0.8);

  // By default, color swatch is too wide (especially when showing long color names),
  // therefore, set it to a bit narrower.
  this->ScalarBarActor->SetBarRatio(0.15);

  // Allow resizing by clicking at the widget border
  vtkBorderRepresentation* border = this->ScalarBarWidget->GetBorderRepresentation();
  if (border)
    {
    border->SetShowHorizontalBorder(true);
    border->SetShowVerticalBorder(true);
    // only show the border when hovering over with the mouse
    border->SetShowBorderToActive();
    }
*/
}

//-----------------------------------------------------------------------------
qSlicerColorsModuleWidgetPrivate::~qSlicerColorsModuleWidgetPrivate()
{
/*
  if (this->ScalarBarWidget)
    {
    this->ScalarBarWidget->Delete();
    this->ScalarBarWidget = nullptr;
    }
  if (this->ScalarBarActor)
  {
    this->ScalarBarActor->Delete();
    this->ScalarBarActor = nullptr;
  }
*/
}

//-----------------------------------------------------------------------------
vtkSlicerColorLogic* qSlicerColorsModuleWidgetPrivate::colorLogic()const
{
  Q_Q(const qSlicerColorsModuleWidget);
  return vtkSlicerColorLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidgetPrivate::setDefaultColorNode()
{
  Q_Q(qSlicerColorsModuleWidget);
  if (!q->mrmlScene() ||
      !this->ColorTableComboBox ||
      this->ColorTableComboBox->currentNode() != nullptr)
    {
    return;
    }
  const char *defaultID = this->colorLogic()->GetDefaultLabelMapColorNodeID();
  vtkMRMLColorNode *defaultNode = vtkMRMLColorNode::SafeDownCast(
    q->mrmlScene()->GetNodeByID(defaultID));
  this->ColorTableComboBox->setCurrentNode(defaultNode);
}

//-----------------------------------------------------------------------------
qSlicerColorsModuleWidget::qSlicerColorsModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerColorsModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerColorsModuleWidget::~qSlicerColorsModuleWidget() = default;

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setup()
{
  Q_D(qSlicerColorsModuleWidget);

  d->setupUi(this);

  d->CopyColorNodeButton->setIcon(QIcon(":Icons/SlicerCopyColor.png"));

  connect(d->ColorTableComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onMRMLColorNodeChanged(vtkMRMLNode*)));
  connect(d->NumberOfColorsSpinBox, SIGNAL(editingFinished()),
          this, SLOT(updateNumberOfColors()));
  connect(d->LUTRangeWidget, SIGNAL(valuesChanged(double,double)),
          this, SLOT(setLookupTableRange(double,double)));
  connect(d->CopyColorNodeButton, SIGNAL(clicked()),
          this, SLOT(copyCurrentColorNode()));

  connect(d->UseColorNameAsLabelCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setUseColorNameAsLabel(bool)));
  connect(d->CenterLabelCheckBox, SIGNAL(toggled(bool)),
    this, SLOT(setCenterLabel(bool)));
/*
  qSlicerApplication * app = qSlicerApplication::application();
  if (app && app->layoutManager())
    {
    qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
    vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
    if (activeRenderer)
      {
      d->ScalarBarWidget->SetInteractor(activeRenderer->GetRenderWindow()->GetInteractor());
      }
    connect(d->VTKScalarBar, SIGNAL(modified()), threeDView, SLOT(scheduleRender()));
    }
*/
  double validBounds[4] = {VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 0., 1.};
  d->ContinuousScalarsToColorsWidget->view()->setValidBounds(validBounds);
  d->ContinuousScalarsToColorsWidget->view()->addColorTransferFunction(nullptr);

  connect( d->DisplayableNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onDisplayableNodeChanged(vtkMRMLNode*)));
  connect( d->ViewNodesComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onViewNodeChanged(vtkMRMLNode*)));
  connect( d->ColorBarVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(onColorBarVisibilityToggled(bool)));
  connect( d->UseSelectedColorsCheckBox, SIGNAL(toggled(bool)), this, SLOT(onUseSelectedColorsToggled(bool)));
  connect( d->ColorBarOrientationButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onColorBarOrientationButtonClicked(QAbstractButton*)));
  connect( d->VTKScalarBar, SIGNAL(modified()), this, SLOT(onScalarBarWidgetModified()));

  // Select the default color node
  d->setDefaultColorNode();
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setMRMLScene(vtkMRMLScene *scene)
{
  Q_D(qSlicerColorsModuleWidget);
  this->qSlicerAbstractModuleWidget::setMRMLScene(scene);
  d->setDefaultColorNode();
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setCurrentColorNode(vtkMRMLNode* colorNode)
{
  Q_D(qSlicerColorsModuleWidget);
  d->ColorTableComboBox->setCurrentNode(colorNode);
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setUseColorNameAsLabel(bool useColorName)
{
  Q_D(qSlicerColorsModuleWidget);

  if (d->ColorBarActor)
  {
    if (useColorName)
    {
      // text string format
      d->ColorBarActor->SetLabelFormat(" %s");
    }
    else
    {
      // number format
      d->ColorBarActor->SetLabelFormat(" %#8.3f");
    }

    d->ColorBarActor->SetUseAnnotationAsLabel(useColorName);
  }
}


//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setCenterLabel(bool centerLabel)
{
  Q_D(qSlicerColorsModuleWidget);

  if (d->ColorBarActor)
  {
    d->ColorBarActor->SetCenterLabel(centerLabel);
  }
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::onMRMLColorNodeChanged(vtkMRMLNode* newColorNode)
{
  Q_D(qSlicerColorsModuleWidget);

  vtkMRMLColorNode* colorNode = vtkMRMLColorNode::SafeDownCast(newColorNode);
  if (!colorNode)
    {
    d->NumberOfColorsSpinBox->setEnabled(false);
    d->NumberOfColorsSpinBox->setValue(0);
    d->LUTRangeWidget->setEnabled(false);
    d->LUTRangeWidget->setValues(0.,0.);
    d->CopyColorNodeButton->setEnabled(false);
    d->ContinuousScalarsToColorsWidget->setEnabled(false);
//    d->VTKScalarBar->setTitle("(mm)");
    return;
    }

  d->CopyColorNodeButton->setEnabled(true);

  vtkMRMLColorTableNode *colorTableNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
  vtkMRMLProceduralColorNode *procColorNode = vtkMRMLProceduralColorNode::SafeDownCast(colorNode);
  if (colorTableNode && !procColorNode)
    {
    // hide the procedural display, show the color table
    // freesurfer nodes are bit of a special case, they're defined
    // procedurally, but provide a look up table rather than a
    // color transfer function
    d->ContinuousDisplayCollapsibleButton->setCollapsed(true);
    d->ContinuousDisplayCollapsibleButton->setEnabled(false);
    d->ContinuousScalarsToColorsWidget->setEnabled(false);
    d->DisplayCollapsibleButton->setCollapsed(false);
    d->DisplayCollapsibleButton->setEnabled(true);

    // number of colors
    d->NumberOfColorsSpinBox->setEnabled(
      colorNode->GetType() == vtkMRMLColorTableNode::User);
    d->NumberOfColorsSpinBox->setValue(colorNode->GetNumberOfColors());
    Q_ASSERT(d->NumberOfColorsSpinBox->value() == colorNode->GetNumberOfColors());

    // set the range and the input for the scalar bar widget depending on if it's a freesurfer node or a color table node
    double *range = nullptr;
    d->LUTRangeWidget->setEnabled(colorNode->GetType() == vtkMRMLColorTableNode::User);
    if (colorTableNode && colorTableNode->GetLookupTable())
      {
      range = colorTableNode->GetLookupTable()->GetRange();
//      d->ScalarBarActor->SetLookupTable(colorTableNode->GetLookupTable());
      }
    disconnect(d->LUTRangeWidget, SIGNAL(valuesChanged(double, double)),
      this, SLOT(setLookupTableRange(double, double)));
    if (range)
      {
      // Make the range a bit (10%) larger than the values to allow some room for
      // adjustment. More adjustment can be done by manually setting the range on the GUI.
      double rangeMargin = (range[1] - range[0])*0.1;
      if (rangeMargin == 0)
        {
        rangeMargin = 10.0;
        }
      d->LUTRangeWidget->setRange(range[0] - rangeMargin, range[1] + rangeMargin);
      d->LUTRangeWidget->setValues(range[0], range[1]);
      }
    else
      {
      d->LUTRangeWidget->setEnabled(false);
      d->LUTRangeWidget->setValues(0.,0.);
      }
    connect(d->LUTRangeWidget, SIGNAL(valuesChanged(double, double)),
      this, SLOT(setLookupTableRange(double, double)));
    // update the annotations from the superclass color node since this is a
    // color table or freesurfer color node
    int numberOfColors = colorNode->GetNumberOfColors();
    vtkNew<vtkIntArray> indexArray;
    indexArray->SetNumberOfValues(numberOfColors);
    vtkNew<vtkStringArray> stringArray;
    stringArray->SetNumberOfValues(numberOfColors);
    for (int colorIndex=0; colorIndex<numberOfColors; ++colorIndex)
      {
      indexArray->SetValue(colorIndex, colorIndex);
      stringArray->SetValue(colorIndex, colorNode->GetColorName(colorIndex));
      }
//    if (d->ColorBarActor)
//    {
//      d->ColorBarActor->GetLookupTable()->SetAnnotations(indexArray.GetPointer(), stringArray.GetPointer());
//    }
    }
  else if (procColorNode && !colorTableNode)
    {
    // hide and disable the color table display, show the continuous one
    d->NumberOfColorsSpinBox->setEnabled(false);
    d->NumberOfColorsSpinBox->setValue(0);
    d->LUTRangeWidget->setEnabled(false);
    d->LUTRangeWidget->setValues(0.,0.);
    d->DisplayCollapsibleButton->setCollapsed(true);
    d->DisplayCollapsibleButton->setEnabled(false);
    d->ContinuousDisplayCollapsibleButton->setCollapsed(false);
    d->ContinuousDisplayCollapsibleButton->setEnabled(true);

    // set the color transfer function to the widget
    d->ContinuousScalarsToColorsWidget->view()->setColorTransferFunctionToPlots(procColorNode->GetColorTransferFunction());

    // only allow editing of user types
    d->ContinuousScalarsToColorsWidget->setEnabled(
        procColorNode->GetType() == vtkMRMLColorNode::User);

    // set the lookup table on the scalar bar widget actor
//    if (d->ColorBarActor && procColorNode->GetColorTransferFunction())
//      {
//      d->ColorBarActor->SetLookupTable(procColorNode->GetColorTransferFunction());
//      }
    }
  else
    {
    // not a valid type of color node
    d->LUTRangeWidget->setValues(0.,0.);
    }

  // add the color name to the scalar bar title
//  std::string title = std::string(colorNode->GetName()) + std::string(" (mm)");
//  d->VTKScalarBar->setTitle(title.c_str());
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::updateNumberOfColors()
{
  Q_D(qSlicerColorsModuleWidget);
  if (!d->NumberOfColorsSpinBox->isEnabled())
    {
    return;
    }
  int newNumber = d->NumberOfColorsSpinBox->value();
  vtkMRMLColorTableNode* colorTableNode = vtkMRMLColorTableNode::SafeDownCast(
    d->ColorTableComboBox->currentNode());
  if (colorTableNode)
    {
    colorTableNode->SetNumberOfColors(newNumber);
    }
  else
    {
    qWarning() << "updateNumberOfColors: please select a discrete color table node to adjust the number of colors";
    }
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setLookupTableRange(double min, double max)
{
  Q_D(qSlicerColorsModuleWidget);

  vtkMRMLNode *currentNode = d->ColorTableComboBox->currentNode();
  if (!currentNode)
    {
    return;
    }

  vtkMRMLColorNode* colorNode = vtkMRMLColorNode::SafeDownCast(currentNode);
  if (colorNode && colorNode->GetLookupTable())
    {
    colorNode->GetLookupTable()->SetRange(min, max);
    }
}

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::copyCurrentColorNode()
{
  Q_D(qSlicerColorsModuleWidget);
  vtkMRMLColorNode* currentNode = vtkMRMLColorNode::SafeDownCast(
    d->ColorTableComboBox->currentNode());
  Q_ASSERT(currentNode);
  QString newColorName = QInputDialog::getText(
    this, "Color node name",
    "Please select a new name for the color node copy",
    QLineEdit::Normal,
    QString(currentNode->GetName()) + QString("Copy"));
  if (newColorName.isEmpty())
    {
    return;
    }

  vtkMRMLColorNode *colorNode = nullptr;
  if (currentNode->IsA("vtkMRMLColorTableNode") ||
      currentNode->IsA("vtkMRMLFreeSurferProceduralColorNode"))
    {
    colorNode = d->colorLogic()->CopyNode(currentNode, newColorName.toUtf8());
    }
  else if (currentNode->IsA("vtkMRMLProceduralColorNode"))
    {
    colorNode = d->colorLogic()->CopyProceduralNode(currentNode, newColorName.toUtf8());
    }
  else
    {
    qWarning() << "CopyCurrentColorNode: current node not of a color node type "
               << "that can be copied. It's a " << currentNode->GetClassName()
               << ", not a procedural or color table node";
    return;
    }
  if (!this->mrmlScene()->AddNode(colorNode))
    {
    qWarning() << "CopyCurrentColorNode: failed to add new node to scene";
    }
  colorNode->Delete();
  if (colorNode->GetID())
    {
    d->ColorTableComboBox->setCurrentNode(colorNode);
    }
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onDisplayableNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerColorsModuleWidget);
  d->DisplayableNode = vtkMRMLDisplayableNode::SafeDownCast(node);
  if (!d->DisplayableNode)
  {
    d->ColorBarVisibilityCheckBox->setChecked(false);
    d->UseSelectedColorsCheckBox->setEnabled(false);
    d->VerticalOrientationRadioButton->setEnabled(false);
    d->HorizontalOrientationRadioButton->setEnabled(false);
    return;
  }

  vtkMRMLColorBarDisplayNode* colorBarNode = vtkMRMLColorBarDisplayNode::SafeDownCast(
    d->DisplayableNode->GetNodeReference(vtkMRMLColorBarDisplayNode::COLOR_BAR_REFERENCE_ROLE));
  if (colorBarNode)
  {
    d->ColorBarNode = colorBarNode;

    d->ColorBarVisibilityCheckBox->setChecked(true);
    d->VerticalOrientationRadioButton->setEnabled(true);
    d->HorizontalOrientationRadioButton->setEnabled(true);
    d->UseSelectedColorsCheckBox->setEnabled(true);
    return;
  }

  d->ColorBarVisibilityCheckBox->setChecked(false);
  d->UseSelectedColorsCheckBox->setEnabled(false);
  d->VerticalOrientationRadioButton->setEnabled(false);
  d->HorizontalOrientationRadioButton->setEnabled(false);
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onColorBarVisibilityToggled(bool toggled)
{
  Q_D(qSlicerColorsModuleWidget);

  if (!d->DisplayableNode)
  {
    qDebug() << Q_FUNC_INFO << "onAddColorBarButtonClicked: Displayable node is invalid";
    return;
  }

  if (toggled)
  {
    vtkNew<vtkMRMLColorBarDisplayNode> colorBarNode;
    this->mrmlScene()->AddNode(colorBarNode);

    d->ColorBarNode = colorBarNode;

    d->DisplayableNode->SetNodeReferenceID(vtkMRMLColorBarDisplayNode::COLOR_BAR_REFERENCE_ROLE, colorBarNode->GetID());
    colorBarNode->SetAndObserveDisplayableNode(d->DisplayableNode);

    d->VerticalOrientationRadioButton->setEnabled(true);
    d->HorizontalOrientationRadioButton->setEnabled(true);
    d->UseSelectedColorsCheckBox->setEnabled(true);
    colorBarNode->SetVisibility(true);
    colorBarNode->SetVisibility2D(true);
    colorBarNode->SetVisibility3D(true);
    colorBarNode->Modified();
  }
  else
  {
    vtkMRMLColorBarDisplayNode* colorBarNode = vtkMRMLColorBarDisplayNode::SafeDownCast(
      d->DisplayableNode->GetNodeReference(vtkMRMLColorBarDisplayNode::COLOR_BAR_REFERENCE_ROLE));
    if (colorBarNode)
    {
      this->mrmlScene()->RemoveNode(colorBarNode);
      d->DisplayableNode->SetNodeReferenceID( vtkMRMLColorBarDisplayNode::COLOR_BAR_REFERENCE_ROLE, nullptr);
      d->ColorBarNode = nullptr;

      d->VerticalOrientationRadioButton->setEnabled(false);
      d->HorizontalOrientationRadioButton->setEnabled(false);
      d->UseSelectedColorsCheckBox->setEnabled(false);
    }
  }
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onViewNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerColorsModuleWidget);

  // Configure scalar bar actor
  // Get scalar bar actor for a selected view
  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(node);
  if (!viewNode)
  {
    qDebug() << Q_FUNC_INFO << "onViewNodeChanged: Abstract view node is invalid";
    return;
  }

  // Get color bar displayable manager
  qSlicerApplication* app = qSlicerApplication::application();
  if (!app)
  {
    qCritical() << Q_FUNC_INFO << "onViewNodeChanged: Invalid Slicer application instance";
    return;
  }

  qSlicerLayoutManager* layoutManager = app->layoutManager();

  vtkMRMLSliceNode* selectedSliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  vtkMRMLViewNode* selected3dViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  if (selectedSliceNode)
  {
    // Iterate all slice views
    QStringList sliceViewNames = layoutManager->sliceViewNames();
    for (const QString& svName : sliceViewNames)
    {
      qMRMLSliceWidget* sliceWidget = layoutManager->sliceWidget(svName);
      qMRMLSliceView* sliceView = sliceWidget->sliceView();
      vtkMRMLSliceNode* widgetSliceNode = sliceWidget->mrmlSliceNode();
      if (!widgetSliceNode)
      {
        qWarning() << Q_FUNC_INFO << "onViewNodeChanged: Slice view node is invalid";
        break;
      }
      vtkMRMLAbstractDisplayableManager* displayManager = sliceView->displayableManagerByClassName("vtkMRMLColorBarDisplayableManager");
      if (std::strcmp(widgetSliceNode->GetID(), selectedSliceNode->GetID()) == 0)
      {
        vtkMRMLColorBarDisplayableManager* colorBarManager = vtkMRMLColorBarDisplayableManager::SafeDownCast(displayManager);
        d->ColorBarActor = colorBarManager->GetScalarBarActor();
        d->ColorBarWidget = colorBarManager->GetScalarBarWidget();
        d->VTKScalarBar->setScalarBarWidget(d->ColorBarWidget);
      }
    }
  }
  else if (selected3dViewNode)
  {
    // Iterate all 3D views
    for (int i = 0; i < layoutManager->threeDViewCount(); ++i)
    {
      qMRMLThreeDWidget* threeDWidget = layoutManager->threeDWidget(i);
      qMRMLThreeDView* threeDView = threeDWidget->threeDView();
      vtkMRMLViewNode* widgetViewNode = threeDView->mrmlViewNode();
      if (!widgetViewNode)
      {
        qWarning() << Q_FUNC_INFO << "onViewNodeChanged: 3D view node is invalid";
        break;
      }
      vtkMRMLAbstractDisplayableManager* displayManager = threeDView->displayableManagerByClassName("vtkMRMLColorBarDisplayableManager");
      if (std::strcmp(widgetViewNode->GetID(), selected3dViewNode->GetID()) == 0)
      {
        vtkMRMLColorBarDisplayableManager* colorBarManager = vtkMRMLColorBarDisplayableManager::SafeDownCast(displayManager);
        d->ColorBarActor = colorBarManager->GetScalarBarActor();
        d->ColorBarWidget = colorBarManager->GetScalarBarWidget();
        d->VTKScalarBar->setScalarBarWidget(d->ColorBarWidget);
      }
    }
  }
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onColorBarOrientationButtonClicked(QAbstractButton* button)
{
  Q_D(qSlicerColorsModuleWidget);
  QRadioButton* radioButton = qobject_cast<QRadioButton*>(button);
  if (radioButton == d->HorizontalOrientationRadioButton)
  {
    if (d->ColorBarNode)
    {
      d->ColorBarNode->SetOrientationPreset(vtkMRMLColorBarDisplayNode::Horizontal);
      d->ColorBarNode->Modified();
    }
  }
  else if (radioButton == d->VerticalOrientationRadioButton)
  {
    if (d->ColorBarNode)
    {
      d->ColorBarNode->SetOrientationPreset(vtkMRMLColorBarDisplayNode::Vertical);
      d->ColorBarNode->Modified();
    }
  }
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onScalarBarWidgetModified()
{
  Q_D(qSlicerColorsModuleWidget);

//  if (d->ColorBarNode)
//  {
//    d->ColorBarNode->Modified();
//  }
}

//-----------------------------------------------------------
void qSlicerColorsModuleWidget::onUseSelectedColorsToggled(bool toggled)
{
  Q_D(qSlicerColorsModuleWidget);

  if (!d->ColorBarNode)
  {
    qDebug() << Q_FUNC_INFO << "onUseSelectedColorsToggled: Color bar display node is invalid";
    return;
  }

  if (toggled)
  {
    vtkMRMLColorNode* colorNode = vtkMRMLColorNode::SafeDownCast(d->ColorTableComboBox->currentNode());
    if (colorNode)
    {
      d->ColorBarNode->SetAndObserveColorNode(colorNode);
    }
    else
    {
      qDebug() << Q_FUNC_INFO << "onUseSelectedColorsToggled: Color table node is invalid";
    }
  }
  else
  {
    d->ColorBarNode->SetAndObserveColorNode(nullptr);
  }
}

//-----------------------------------------------------------
bool qSlicerColorsModuleWidget::setEditedNode(vtkMRMLNode* node,
                                              QString role /* = QString()*/,
                                              QString context /* = QString()*/)
{
  Q_D(qSlicerColorsModuleWidget);
  Q_UNUSED(role);
  Q_UNUSED(context);
  if (vtkMRMLColorNode::SafeDownCast(node))
    {
    d->ColorTableComboBox->setCurrentNode(node);
    return true;
    }

  return false;
}

//-----------------------------------------------------------
vtkScalarBarWidget* qSlicerColorsModuleWidget::scalarBar()
{
  Q_D(qSlicerColorsModuleWidget);

  return d->ColorBarWidget;
}
