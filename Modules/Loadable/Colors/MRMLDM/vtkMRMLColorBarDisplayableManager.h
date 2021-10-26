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

#ifndef __vtkMRMLColorBarDisplayableManager_h
#define __vtkMRMLColorBarDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractDisplayableManager.h"
#include "vtkSlicerColorsModuleMRMLDisplayableManagerExport.h"

class vtkMRMLColorBarDisplayNode;
class vtkScalarBarWidget;
class vtkSlicerScalarBarActor;
class vtkMRMLScene;

/// \brief Displayable manager for the color scalar bars.
///
/// This displayable manager implements color scalar bar display in both 2D and 3D views.
class VTK_SLICER_COLORS_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLColorBarDisplayableManager :
  public vtkMRMLAbstractDisplayableManager
{
public:
  static vtkMRMLColorBarDisplayableManager* New();
  vtkTypeMacro(vtkMRMLColorBarDisplayableManager, vtkMRMLAbstractDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// \brief Get scalar bar widget of last modified display node
  /// \return scalar bar widget pointer
  vtkScalarBarWidget* GetScalarBarWidget() const;

  /// \brief Get scalar bar actor of last modified display node
  /// \return scalar bar actor pointer
  vtkSlicerScalarBarActor* GetScalarBarActor() const;

  /// \brief Get scalar bar widget
  /// \param dispNode - color bar display node
  /// \return scalar bar widget pointer
  vtkScalarBarWidget* GetScalarBarWidget(vtkMRMLColorBarDisplayNode* dispNode) const;

  /// \brief Get scalar bar actor
  /// \param dispNode - color bar display node
  /// \return scalar bar actor pointer
  vtkSlicerScalarBarActor* GetScalarBarActor(vtkMRMLColorBarDisplayNode* dispNode) const;

protected:
  vtkMRMLColorBarDisplayableManager();
  ~vtkMRMLColorBarDisplayableManager() override;

  /// Initialize the displayable manager based on its associated
  /// vtkMRMLSliceNode
  void Create() override;

  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  void RegisterNodes() override;

  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
  void ProcessMRMLNodesEvents(vtkObject *caller,
                                      unsigned long event,
                                      void *callData) override;

  /// Called when the SliceNode or Three3DViewNode are modified. May cause ColorBar to remap its position on screen.
  void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller) override;

  /// Method to perform additional initialization
  void AdditionalInitializeStep() override;

private:
  vtkMRMLColorBarDisplayableManager(const vtkMRMLColorBarDisplayableManager&) = delete;
  void operator=(const vtkMRMLColorBarDisplayableManager&) = delete;

  class vtkInternal;
  vtkInternal* Internal;
};

#endif
