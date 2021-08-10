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
//#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLNode.h>

class vtkMRMLColorTableNode;

//class VTK_MRML_EXPORT vtkMRMLScalarBarDisplayNode : public vtkMRMLDisplayNode
class VTK_MRML_EXPORT vtkMRMLScalarBarDisplayNode : public vtkMRMLNode
{
public:
  static const char* COLOR_TABLE_REFERENCE_ROLE;

  static vtkMRMLScalarBarDisplayNode *New();
//  vtkTypeMacro(vtkMRMLScalarBarDisplayNode,vtkMRMLDisplayNode);
  vtkTypeMacro(vtkMRMLScalarBarDisplayNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkMRMLNode* CreateNodeInstance() override;

  /// Get node XML tag name (like Volume, Model)
//  const char* GetNodeTagName() override {return "ScalarBarDisplay";}
  const char* GetNodeTagName() override {return "ScalarBar";}

  /// Read node attributes from XML file
  void ReadXMLAttributes(const char** atts) override;

  /// Write this node's information to a MRML file in XML format.
  void WriteXML(ostream& of, int indent) override;

  /// Copy node content (excludes basic data, such as name and node references).
  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentMacro(vtkMRMLScalarBarDisplayNode);

  /// Get color table node
  vtkMRMLColorTableNode* GetColorTableNode();
  /// Set and observe color table node
  void SetAndObserveColorTableNode(vtkMRMLColorTableNode* node);

  /// Get scalar bar visibility
  vtkGetMacro(VisibilityOnSliceViewsFlag, bool);
  /// Set scalar bar visibility
  vtkSetMacro(VisibilityOnSliceViewsFlag, bool);

  char* GetScalarBarTagName() { return this->GetSingletonTag(); }

protected:
  vtkMRMLScalarBarDisplayNode();
  ~vtkMRMLScalarBarDisplayNode() override;
  vtkMRMLScalarBarDisplayNode(const vtkMRMLScalarBarDisplayNode&);
  void operator=(const vtkMRMLScalarBarDisplayNode&);

private:
  bool VisibilityOnSliceViewsFlag;
};

#endif
