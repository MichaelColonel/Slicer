/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyColorBarPlugin.h"
#include "qSlicerSubjectHierarchyVolumesPlugin.h"

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// Colors includes
#include "vtkSlicerColorLogic.h"

// MRML includes
#include <vtkMRMLCameraNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLViewLogic.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLColorBarDisplayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QSettings>

// MRML widgets includes
#include "qMRMLNodeComboBox.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Widgets
class qSlicerSubjectHierarchyColorBarPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyColorBarPlugin);
protected:
  qSlicerSubjectHierarchyColorBarPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyColorBarPluginPrivate(qSlicerSubjectHierarchyColorBarPlugin& object);
  ~qSlicerSubjectHierarchyColorBarPluginPrivate() override;
  void init();
public:
  vtkWeakPointer<vtkSlicerColorLogic> ColorLogic;

  QAction* ColorBarAction;
  QAction* ShowColorBarAction;
  QAction* ToggleColorBar2DAction;
  QAction* ToggleColorBar3DAction;
  QMenu* ColorBarMenu;
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyColorBarPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyColorBarPluginPrivate::qSlicerSubjectHierarchyColorBarPluginPrivate(qSlicerSubjectHierarchyColorBarPlugin& object)
  :
  q_ptr(&object),
  ColorBarAction(nullptr),
  ShowColorBarAction(nullptr),
  ToggleColorBar2DAction(nullptr),
  ToggleColorBar3DAction(nullptr),
  ColorBarMenu(nullptr)
{
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyColorBarPlugin);

  this->ShowColorBarAction = new QAction("Show color bar",q);
  QObject::connect(this->ShowColorBarAction, SIGNAL(toggled(bool)), q, SLOT(toggleVisibilityForCurrentItem(bool)));
  this->ShowColorBarAction->setCheckable(true);
  this->ShowColorBarAction->setChecked(false);

  this->ToggleColorBar2DAction = new QAction("Show color bar 2D",q);
  QObject::connect(this->ToggleColorBar2DAction, SIGNAL(toggled(bool)), q, SLOT(toggle2DVisibilityForCurrentItem(bool)));
  this->ToggleColorBar2DAction->setCheckable(true);
  this->ToggleColorBar2DAction->setChecked(false);

  this->ToggleColorBar3DAction = new QAction("Show color bar 3D",q);
  QObject::connect(this->ToggleColorBar2DAction, SIGNAL(toggled(bool)), q, SLOT(toggle3DVisibilityForCurrentItem(bool)));
  this->ToggleColorBar3DAction->setCheckable(true);
  this->ToggleColorBar3DAction->setChecked(false);

  this->ColorBarMenu = new QMenu();
  this->ColorBarMenu->addAction(this->ShowColorBarAction);
  this->ColorBarMenu->addAction(this->ToggleColorBar2DAction);
  this->ColorBarMenu->addAction(this->ToggleColorBar3DAction);

  this->ColorBarAction = new QAction("Color Scalar Bar",q);
  this->ColorBarAction->setMenu(this->ColorBarMenu);
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyColorBarPluginPrivate::~qSlicerSubjectHierarchyColorBarPluginPrivate() = default;

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyVolumeRenderingPlugin methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyColorBarPlugin::qSlicerSubjectHierarchyColorBarPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyColorBarPluginPrivate(*this) )
{
  this->m_Name = QString("ColorBar");

  Q_D(qSlicerSubjectHierarchyColorBarPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyColorBarPlugin::~qSlicerSubjectHierarchyColorBarPlugin() = default;

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPlugin::setColorLogic(vtkSlicerColorLogic* colorLogic)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);
  d->ColorLogic = colorLogic;
}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyColorBarPlugin::visibilityContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyColorBarPlugin);

  QList<QAction*> actions;
  actions << d->ColorBarAction;
  return actions;
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPlugin::showVisibilityContextMenuActionsForItem(vtkIdType itemID)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);

  if (!itemID)
    {
    // There are no scene actions in this plugin
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }
  qDebug() << Q_FUNC_INFO << ": Show visibility context menu action";
/*
  // Volume
  if (qSlicerSubjectHierarchyPluginHandler::instance()->pluginByName("Volumes")->canOwnSubjectHierarchyItem(itemID))
    {
    vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(shNode->GetItemDataNode(itemID));
    vtkMRMLColorBarDisplayNode* displayNode = nullptr;
    if (!volumeNode)
      {
      qCritical() << Q_FUNC_INFO << ": Failed to find scalar volume node associated to subject hierarchy item " << itemID;
      return;
      }
    if (d->VolumeRenderingLogic)
      {
      displayNode = d->VolumeRenderingLogic->GetFirstVolumeRenderingDisplayNode(volumeNode);
      }
    else
      {
      qWarning() << Q_FUNC_INFO << ": volume rendering logic is not set, cannot set up toggle volume rendering action";
      }

    d->ToggleVolumeRenderingAction->blockSignals(true);
    d->ToggleVolumeRenderingAction->setChecked(displayNode ? displayNode->GetVisibility() : false);
    d->ToggleVolumeRenderingAction->blockSignals(false);
    d->ToggleVolumeRenderingAction->setVisible(true);

    d->VolumeRenderingOptionsAction->setVisible(true);
    }
*/
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPlugin::toggleVisibilityForCurrentItem(bool on)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  if (currentItemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid current item";
    return;
    }
  qDebug() << Q_FUNC_INFO << ": Toggle total visibility";
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPlugin::toggle2DVisibilityForCurrentItem(bool on)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  if (currentItemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid current item";
    return;
    }
  qDebug() << Q_FUNC_INFO << ": Toggle 2D (Slices) visibility";
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyColorBarPlugin::toggle3DVisibilityForCurrentItem(bool on)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  if (currentItemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid current item";
    return;
    }
  qDebug() << Q_FUNC_INFO << ": Toggle 3D (Views) visibility";
}

//-----------------------------------------------------------------------------
bool qSlicerSubjectHierarchyColorBarPlugin::showItemInView(vtkIdType itemID, vtkMRMLAbstractViewNode* viewNode, vtkIdList* allItemsToShow)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);

  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(viewNode);
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);

  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return false;
    }
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!volumeNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to find scalar volume node associated to subject hierarchy item " << itemID;
    return false;
    }

  if (!d->ColorLogic)
    {
    qWarning() << Q_FUNC_INFO << ": color logic is not set, cannot set up toggle color bar action";
    return false;
    }
  bool wasVisible = false;
  vtkMRMLColorBarDisplayNode* displayNode = d->ColorLogic->GetFirstColorBarDisplayNode(volumeNode);
  if (displayNode)
    {
    wasVisible = displayNode->GetVisibility();
    }
  else
    {
    // if there is no color bar node => create it, get first color bar node otherwise
    displayNode = d->ColorLogic->CreateAndObserveColorBarDisplayNode(volumeNode);
    }
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to create color display node for scalar volume node " << volumeNode->GetName();
    return false;
    }

  if (viewNode)
    {
    // Show in specific view
    MRMLNodeModifyBlocker blocker(displayNode);
    // show
    if (!wasVisible)
      {
      displayNode->SetVisibility(true);
      }
    displayNode->AddViewNodeID(viewNode->GetID());
    }
  else if (sliceNode)
    {
    // Show in specific view
    MRMLNodeModifyBlocker blocker(displayNode);
    // show
    if (!wasVisible)
      {
      displayNode->SetVisibility(true);
      }
    displayNode->AddViewNodeID(sliceNode->GetID());
    }
  else
    {
    // Show in all views
    MRMLNodeModifyBlocker blocker(displayNode);
    displayNode->RemoveAllViewNodeIDs();
    displayNode->SetVisibility(true);
    }

  return true;
}

//---------------------------------------------------------------------------
bool qSlicerSubjectHierarchyColorBarPlugin::showColorBarInView( bool show, vtkIdType itemID, vtkMRMLViewNode* viewNode/*=nullptr*/)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);

  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return false;
    }
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!volumeNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to find scalar volume node associated to subject hierarchy item " << itemID;
    return false;
    }

  if (!d->ColorLogic)
    {
    qWarning() << Q_FUNC_INFO << ": color logic is not set, cannot set up toggle color bar action";
    return false;
    }
  bool wasVisible = false;
  vtkMRMLColorBarDisplayNode* displayNode = d->ColorLogic->GetFirstColorBarDisplayNode(volumeNode);
  if (displayNode)
    {
    wasVisible = displayNode->GetVisibility();
    }
  else
    {
    // there is no color bar display node
    if (!show)
      {
      // not visible and should not be visible, so we are done
      return true;
      }
    // if there is no color bar node => create it, get first color bar node otherwise
    displayNode = d->ColorLogic->CreateAndObserveColorBarDisplayNode(volumeNode);
    }
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to create color display node for scalar volume node " << volumeNode->GetName();
    return false;
    }

  if (viewNode)
    {
    // Show/hide in specific view
    MRMLNodeModifyBlocker blocker(displayNode);
    if (show)
      {
      // show
      if (!wasVisible)
        {
        displayNode->SetVisibility(true);
        // This was hidden in all views, show it only in the currently selected view
        displayNode->RemoveAllViewNodeIDs();
        }
      displayNode->AddViewNodeID(viewNode->GetID());
      }
    else
      {
      // This hides the volume rendering in all views, which is a bit more than asked for,
      // but since drag-and-drop to view only requires selective showing (and not selective hiding),
      // this should be good enough. The behavior can be refined later if needed.
      displayNode->SetVisibility(false);
      }
    }
  else
    {
    // Show in all views
    MRMLNodeModifyBlocker blocker(displayNode);
    displayNode->RemoveAllViewNodeIDs();
    displayNode->SetVisibility(show);
    }

  return true;
}

//---------------------------------------------------------------------------
bool qSlicerSubjectHierarchyColorBarPlugin::showColorBarInSlice( bool show, vtkIdType itemID, vtkMRMLSliceNode* sliceNode/*=nullptr*/)
{
  Q_D(qSlicerSubjectHierarchyColorBarPlugin);

  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return false;
    }
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!volumeNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to find scalar volume node associated to subject hierarchy item " << itemID;
    return false;
    }

  if (!d->ColorLogic)
    {
    qWarning() << Q_FUNC_INFO << ": color logic is not set, cannot set up toggle color bar action";
    return false;
    }
  bool wasVisible = false;
  vtkMRMLColorBarDisplayNode* displayNode = d->ColorLogic->GetFirstColorBarDisplayNode(volumeNode);
  if (displayNode)
    {
    wasVisible = displayNode->GetVisibility();
    }
  else
    {
    // there is no color bar display node
    if (!show)
      {
      // not visible and should not be visible, so we are done
      return true;
      }
    // if there is no color bar node => create it, get first color bar node otherwise
    displayNode = d->ColorLogic->CreateAndObserveColorBarDisplayNode(volumeNode);
    }
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to create color display node for scalar volume node " << volumeNode->GetName();
    return false;
    }

  if (sliceNode)
    {
    // Show/hide in specific view
    MRMLNodeModifyBlocker blocker(displayNode);
    if (show)
      {
      // show
      if (!wasVisible)
        {
        displayNode->SetVisibility(true);
        // This was hidden in all views, show it only in the currently selected view
        displayNode->RemoveAllViewNodeIDs();
        }
      displayNode->AddViewNodeID(sliceNode->GetID());
      }
    else
      {
      // This hides the volume rendering in all views, which is a bit more than asked for,
      // but since drag-and-drop to view only requires selective showing (and not selective hiding),
      // this should be good enough. The behavior can be refined later if needed.
      displayNode->SetVisibility(false);
      }
    }
  else
    {
    // Show in all views
    MRMLNodeModifyBlocker blocker(displayNode);
    displayNode->RemoveAllViewNodeIDs();
    displayNode->SetVisibility(show);
    }

  return true;
}
