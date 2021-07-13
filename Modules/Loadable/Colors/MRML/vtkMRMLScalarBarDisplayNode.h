/*==============================================================================

  Program: 3D Slicer

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __vtkMRMLScalarBarDisplayNode_h
#define __vtkMRMLScalarBarDisplayNode_h

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLDisplayNode.h>

class vtkMRMLDisplayableNode;

/// \brief MRML node to represent a display property for child nodes of a
///        subject hierarchy folder.
///
/// vtkMRMLFolderDisplayNode nodes stores display property of a generic
/// displayable node, so that it can override display properties of its
/// children when the user turns on that option.
///
/// The class has no additional features compared to its base class, but
/// since the base display node class is abstract, we need one that we can
/// instantiate.
///
/// Displayable managers must take into account parent folder display nodes as follows:
///
/// If the \sa ApplyDisplayPropertiesOnBranch flag is off, then the displayable node's own
/// Visibility and Opacity properties must be combined with the corresponding values stored
/// in the folder displayable (opacity = folderOpacity * ownOpacity;
/// visibility = folderVisibility && ownVisibility).
///
/// If the \sa ApplyDisplayPropertiesOnBranch flag is on, then displayable node's own display node
/// properties must be combined or replaced by the folder display nodes properties.
/// Properties to combine: Visibility, Visibility3D, Visibility2D, Opacity.
/// Properties to replace: all other properties stored in folder display node
/// (Color, EdgeColor, SelectedColor, Ambient, Specular, etc.).
///
class VTK_SLICER_COLORS_MODULE_MRML_EXPORT vtkMRMLScalarBarDisplayNode : public vtkMRMLNode
{
public:
  static vtkMRMLScalarBarDisplayNode *New();
  vtkTypeMacro(vtkMRMLScalarBarDisplayNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkMRMLNode* CreateNodeInstance() override;

  /// Get node XML tag name (like Volume, Model)
  const char* GetNodeTagName() override {return "ScalarBarDisplay";}

  /// Read node attributes from XML file
  void ReadXMLAttributes(const char** atts) override;

  /// Write this node's information to a MRML file in XML format.
  void WriteXML(ostream& of, int indent) override;

  /// Copy node content (excludes basic data, such as name and node references).
  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentMacro(vtkMRMLScalarBarDisplayNode);

  /// Get scalar bar visibility
  vtkGetMacro( VisibilityOnSliceViewsFlag, bool);
  /// Set scalar bar visibility
  vtkSetMacro( VisibilityOnSliceViewsFlag, bool);

protected:
  vtkMRMLScalarBarDisplayNode();
  ~vtkMRMLScalarBarDisplayNode() override;
  vtkMRMLScalarBarDisplayNode(const vtkMRMLScalarBarDisplayNode&);
  void operator=(const vtkMRMLScalarBarDisplayNode&);

private:
  bool VisibilityOnSliceViewsFlag;
};

#endif
