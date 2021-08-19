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

#include "vtkMRMLColorBarDisplayNode.h"

// MRML includes
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCallbackCommand.h>

// STD includes
#include <cstring>

//------------------------------------------------------------------------------
namespace
{

const char* DISPLAYABLE_REFERENCE_ROLE = "displayableRef";

} // namespace

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLColorBarDisplayNode);

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::vtkMRMLColorBarDisplayNode()
  :
  PositionPreset(VerticalRight)
{
  this->SetVisibility2D(false);
  this->SetVisibility3D(false);
//  this->SetSingletonTag("default");
}

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::~vtkMRMLColorBarDisplayNode() = default;

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintEnumMacro(PositionPreset);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  this->Superclass::WriteXML(of, nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLEnumMacro(PositionPreset, PositionPreset);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLEnumMacro(PositionPreset, PositionPreset);
  vtkMRMLReadXMLEndMacro();

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::CopyContent(vtkMRMLNode* anode, bool deepCopy/*=true*/)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::CopyContent(anode, deepCopy);

  vtkMRMLColorBarDisplayNode* node = vtkMRMLColorBarDisplayNode::SafeDownCast(anode);
  if (!node)
  {
    return;
  }

  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyEnumMacro(PositionPreset);
  vtkMRMLCopyEndMacro();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetPositionPreset(int id)
{
  switch (id)
  {
  case 0:
    SetPositionPreset(HorizontalBottom);
    break;
  case 1:
    SetPositionPreset(HorizontalTop);
    break;
  case 2:
    SetPositionPreset(VerticalLeft);
    break;
  case 3:
  default:
    SetPositionPreset(VerticalRight);
    break;
  }
}

//---------------------------------------------------------------------------
const char* vtkMRMLColorBarDisplayNode::GetPositionPresetAsString(int id)
{
  switch (id)
  {
  case HorizontalTop:
    return "HorizontalTop";
  case HorizontalBottom:
    return "HorizontalBottom";
  case VerticalLeft:
    return "VerticalLeft";
  case VerticalRight:
  default:
    return "VerticalRight";
  }
}

//-----------------------------------------------------------
int vtkMRMLColorBarDisplayNode::GetPositionPresetFromString(const char* name)
{
  if (name == nullptr)
  {
    // invalid name
    return -1;
  }
  for (int i = 0; i < PositionPreset_Last; i++)
  {
    if (std::strcmp(name, GetPositionPresetAsString(i)) == 0)
    {
      // found a matching name
      return i;
    }
  }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
vtkMRMLDisplayableNode* vtkMRMLColorBarDisplayNode::GetDisplayableNode()
{
  return vtkMRMLDisplayableNode::SafeDownCast( this->GetNodeReference(DISPLAYABLE_REFERENCE_ROLE) );
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetAndObserveDisplayableNode(vtkMRMLDisplayableNode* node)
{
  if (node && this->Scene != node->GetScene())
  {
    vtkErrorMacro("SetAndObserveDisplayableNode: Cannot set reference, the referenced and referencing node are not in the same scene");
    return;
  }

  this->SetNodeReferenceID(DISPLAYABLE_REFERENCE_ROLE, (node ? node->GetID() : nullptr));
}
