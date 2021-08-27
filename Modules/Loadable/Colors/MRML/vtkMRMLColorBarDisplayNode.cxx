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
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCallbackCommand.h>

// STD includes
#include <cstring>

//------------------------------------------------------------------------------
namespace
{

const char* COLOR_REFERENCE_ROLE = "colorRef";

} // namespace

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLColorBarDisplayNode);

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::vtkMRMLColorBarDisplayNode()
  :
  PositionPreset(Foreground),
  OrientationPreset(Vertical)
{
}

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::~vtkMRMLColorBarDisplayNode() = default;

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintEnumMacro(PositionPreset);
  vtkMRMLPrintEnumMacro(OrientationPreset);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  this->Superclass::WriteXML(of, nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLEnumMacro(PositionPreset, PositionPreset);
  vtkMRMLWriteXMLEnumMacro(OrientationPreset, OrientationPreset);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLEnumMacro(PositionPreset, PositionPreset);
  vtkMRMLReadXMLEnumMacro(OrientationPreset, OrientationPreset);
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
  vtkMRMLCopyEnumMacro(OrientationPreset);
  vtkMRMLCopyEndMacro();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetPositionPreset(int id)
{
  switch (id)
    {
    case 0:
      SetPositionPreset(Foreground);
      break;
    case 1:
    default:
      SetPositionPreset(Background);
      break;
    }
}

//---------------------------------------------------------------------------
const char* vtkMRMLColorBarDisplayNode::GetPositionPresetAsString(int id)
{
  switch (id)
    {
    case Foreground:
      return "Foreground";
    case Background:
    default:
      return "Background";
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

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetOrientationPreset(int id)
{
  switch (id)
    {
    case 0:
      SetOrientationPreset(Horizontal);
      break;
    case 1:
    default:
      SetPositionPreset(Vertical);
      break;
    }
}

//---------------------------------------------------------------------------
const char* vtkMRMLColorBarDisplayNode::GetOrientationPresetAsString(int id)
{
  switch (id)
    {
    case Horizontal:
      return "Horizontal";
    case Vertical:
    default:
      return "Vertical";
    }
}

//-----------------------------------------------------------
int vtkMRMLColorBarDisplayNode::GetOrientationPresetFromString(const char* name)
{
  if (name == nullptr)
    {
    // invalid name
    return -1;
    }
  for (int i = 0; i < OrientationPreset_Last; i++)
    {
    if (std::strcmp(name, GetOrientationPresetAsString(i)) == 0)
      {
      // found a matching name
      return i;
      }
    }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
vtkMRMLColorNode* vtkMRMLColorBarDisplayNode::GetColorNode()
{
  return vtkMRMLColorNode::SafeDownCast( this->GetNodeReference(COLOR_REFERENCE_ROLE) );
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetAndObserveColorNode(vtkMRMLColorNode* node)
{
  if (node && this->Scene != node->GetScene())
    {
    vtkErrorMacro("SetAndObserveColorNode: Cannot set reference, the referenced and referencing node are not in the same scene");
    return;
    }

  this->SetNodeReferenceID(COLOR_REFERENCE_ROLE, (node ? node->GetID() : nullptr));
}
