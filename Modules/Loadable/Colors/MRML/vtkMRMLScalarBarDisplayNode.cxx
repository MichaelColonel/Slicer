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

#include "vtkMRMLScalarBarDisplayNode.h"

// MRML includes
#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkCallbackCommand.h>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLScalarBarDisplayNode);

//-----------------------------------------------------------------------------
vtkMRMLScalarBarDisplayNode::vtkMRMLScalarBarDisplayNode()
  :
  vtkMRMLNode(),
  VisibilityOnSliceViewsFlag(false)
{
  this->SetSingletonTag("default");
}

//-----------------------------------------------------------------------------
vtkMRMLScalarBarDisplayNode::~vtkMRMLScalarBarDisplayNode() = default;

//----------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintBooleanMacro(VisibilityOnSliceViewsFlag);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  this->Superclass::WriteXML(of, nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLBooleanMacro(VisibilityOnSliceViewsFlag, VisibilityOnSliceViewsFlag);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLBooleanMacro(VisibilityOnSliceViewsFlag, VisibilityOnSliceViewsFlag);
  vtkMRMLReadXMLEndMacro();

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLScalarBarDisplayNode::CopyContent(vtkMRMLNode* anode, bool deepCopy/*=true*/)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::CopyContent(anode, deepCopy);

  vtkMRMLScalarBarDisplayNode* node = vtkMRMLScalarBarDisplayNode::SafeDownCast(anode);
  if (!node)
  {
    return;
  }

  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyBooleanMacro(VisibilityOnSliceViewsFlag);
  vtkMRMLCopyEndMacro();
}
