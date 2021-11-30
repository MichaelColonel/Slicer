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

#ifndef __qSlicerSubjectHierarchyColorBarPlugin_h
#define __qSlicerSubjectHierarchyColorBarPlugin_h

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyAbstractPlugin.h"

#include "qSlicerColorsSubjectHierarchyPluginsExport.h"

class qSlicerSubjectHierarchyColorBarPluginPrivate;
class vtkMRMLViewNode;
class vtkMRMLDisplayNode;
class vtkSlicerColorLogic;

/// \ingroup Slicer_QtModules_SubjectHierarchy_Widgets
class Q_SLICER_COLORS_SUBJECT_HIERARCHY_PLUGINS_EXPORT qSlicerSubjectHierarchyColorBarPlugin : public qSlicerSubjectHierarchyAbstractPlugin
{
public:
  Q_OBJECT

public:
  typedef qSlicerSubjectHierarchyAbstractPlugin Superclass;
  qSlicerSubjectHierarchyColorBarPlugin(QObject* parent = nullptr);
  ~qSlicerSubjectHierarchyColorBarPlugin() override;

public:
  /// Set Color module logic. Required for accessing display nodes and setting up color bar related node.
  void setColorLogic(vtkSlicerColorLogic* colorLogic);

  /// Get visibility context menu item actions to add to tree view.
  /// These item visibility context menu actions can be shown in the implementations of \sa showVisibilityContextMenuActionsForItem
  QList<QAction*> visibilityContextMenuActions() const override;

  /// Show visibility context menu actions valid for a given subject hierarchy item.
  /// \param itemID Subject Hierarchy item to show the visibility context menu items for
  void showVisibilityContextMenuActionsForItem(vtkIdType itemID) override;

  /// Show an item in a selected view.
  /// Calls Volumes plugin's showItemInView implementation and adds support for showing a color bar in 2D and 3D views.
  /// Returns true on success.
  bool showItemInView(vtkIdType itemID, vtkMRMLAbstractViewNode* viewNode, vtkIdList* allItemsToShow) override;

  /// Show/hide color bar in a view.
  /// If viewNode is nullptr then it is displayed in all views in the current layout.
  bool showColorBar(bool show, vtkIdType itemID, vtkMRMLViewNode* viewNode=nullptr);

protected slots:
  /// Toggle volume rendering option for current volume item
  void toggle2DVisibilityForCurrentItem(bool);
  void toggle3DVisibilityForCurrentItem(bool);
//  void toggleVolumeRenderingForCurrentItem(bool);
  /// Switch to Volume Rendering module and select current volume item
//  void showVolumeRenderingOptionsForCurrentItem();

protected:
  QScopedPointer<qSlicerSubjectHierarchyColorBarPluginPrivate> d_ptr;

//  void resetFieldOfView(vtkMRMLDisplayNode* displayNode, vtkMRMLViewNode* viewNode=nullptr);

private:
  Q_DECLARE_PRIVATE(qSlicerSubjectHierarchyColorBarPlugin);
  Q_DISABLE_COPY(qSlicerSubjectHierarchyColorBarPlugin);
};

#endif
