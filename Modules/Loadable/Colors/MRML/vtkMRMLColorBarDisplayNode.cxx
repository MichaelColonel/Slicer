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

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLColorBarDisplayNode);

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::vtkMRMLColorBarDisplayNode()
  :
  TitleFontColorRGB(TitleFontProperties.ColorRGB),
  TitleFontOpacity(TitleFontProperties.Opacity),
  TitleFontSize(TitleFontProperties.Size),
  TitleFontName(TitleFontProperties.FontName),
  TitleFontBold(TitleFontProperties.Bold),
  TitleFontItalic(TitleFontProperties.Italic),
  TitleFontShadow(TitleFontProperties.Shadow),
  LabelFontColorRGB(LabelFontProperties.ColorRGB),
  LabelFontOpacity(LabelFontProperties.Opacity),
  LabelFontSize(LabelFontProperties.Size),
  LabelFontName(LabelFontProperties.FontName),
  LabelFontBold(LabelFontProperties.Bold),
  LabelFontItalic(LabelFontProperties.Italic),
  LabelFontShadow(LabelFontProperties.Shadow)
{
}

//-----------------------------------------------------------------------------
vtkMRMLColorBarDisplayNode::~vtkMRMLColorBarDisplayNode() = default;

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintEnumMacro(Orientation);

  vtkMRMLPrintVectorMacro(Position, float, 2);
  vtkMRMLPrintFloatMacro(Width);
  vtkMRMLPrintFloatMacro(Height);
  vtkMRMLPrintStdStringMacro(TitleText);
  vtkMRMLPrintVectorMacro(TitleFontColorRGB, double, 3);
  vtkMRMLPrintFloatMacro(TitleFontOpacity);
  vtkMRMLPrintFloatMacro(TitleFontSize);
  vtkMRMLPrintStdStringMacro(TitleFontName);
  vtkMRMLPrintBooleanMacro(TitleFontBold);
  vtkMRMLPrintBooleanMacro(TitleFontItalic);
  vtkMRMLPrintBooleanMacro(TitleFontShadow);
  vtkMRMLPrintVectorMacro(LabelFontColorRGB, double, 3);
  vtkMRMLPrintFloatMacro(LabelFontOpacity);
  vtkMRMLPrintFloatMacro(LabelFontSize);
  vtkMRMLPrintStdStringMacro(LabelFontName);
  vtkMRMLPrintBooleanMacro(LabelFontBold);
  vtkMRMLPrintBooleanMacro(LabelFontItalic);
  vtkMRMLPrintBooleanMacro(LabelFontShadow);
  vtkMRMLPrintStdStringMacro(LabelFormat);
  vtkMRMLPrintIntMacro(MaxNumberOfColors);
  vtkMRMLPrintIntMacro(NumberOfLabels);
  vtkMRMLPrintBooleanMacro(UseColorNamesForLabels);
  vtkMRMLPrintBooleanMacro(CenterLabels);

  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  this->Superclass::WriteXML(of, nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLEnumMacro(Orientation, Orientation);
  vtkMRMLWriteXMLVectorMacro(Position, Position, float, 2);
  vtkMRMLWriteXMLFloatMacro(Width, Width);
  vtkMRMLWriteXMLFloatMacro(Height, Height);
  vtkMRMLWriteXMLStdStringMacro(TitleText, TitleText);
  vtkMRMLWriteXMLVectorMacro(TitleFontColorRGB, TitleFontColorRGB, double, 3);
  vtkMRMLWriteXMLFloatMacro(TitleFontOpacity, TitleFontOpacity);
  vtkMRMLWriteXMLFloatMacro(TitleFontSize, TitleFontSize);
  vtkMRMLWriteXMLStdStringMacro(TitleFontName, TitleFontName);
  vtkMRMLWriteXMLBooleanMacro(TitleFontBold, TitleFontBold);
  vtkMRMLWriteXMLBooleanMacro(TitleFontItalic, TitleFontItalic);
  vtkMRMLWriteXMLBooleanMacro(TitleFontShadow, TitleFontShadow);
  vtkMRMLWriteXMLVectorMacro(LabelFontColorRGB, LabelFontColorRGB, double, 3);
  vtkMRMLWriteXMLFloatMacro(LabelFontOpacity, LabelFontOpacity);
  vtkMRMLWriteXMLFloatMacro(LabelFontSize, LabelFontSize);
  vtkMRMLWriteXMLStdStringMacro(LabelFontName, LabelFontName);
  vtkMRMLWriteXMLBooleanMacro(LabelFontBold, LabelFontBold);
  vtkMRMLWriteXMLBooleanMacro(LabelFontItalic, LabelFontItalic);
  vtkMRMLWriteXMLBooleanMacro(LabelFontShadow, LabelFontShadow);
  vtkMRMLWriteXMLStdStringMacro(LabelFormat, LabelFormat);
  vtkMRMLWriteXMLIntMacro(MaxNumberOfColors, MaxNumberOfColors);
  vtkMRMLWriteXMLIntMacro(NumberOfLabels, NumberOfLabels);
  vtkMRMLWriteXMLBooleanMacro(UseColorNamesForLabels, UseColorNamesForLabels);
  vtkMRMLWriteXMLBooleanMacro(CenterLabels, CenterLabels);

  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLEnumMacro(Orientation, Orientation);
  vtkMRMLReadXMLVectorMacro(Position, Position, float, 2);
  vtkMRMLReadXMLFloatMacro(Width, Width);
  vtkMRMLReadXMLFloatMacro(Height, Height);
  vtkMRMLReadXMLStdStringMacro(TitleText, TitleText);
  vtkMRMLReadXMLVectorMacro(TitleFontColorRGB, TitleFontColorRGB, double, 3);
  vtkMRMLReadXMLFloatMacro(TitleFontOpacity, TitleFontOpacity);
  vtkMRMLReadXMLFloatMacro(TitleFontSize, TitleFontSize);
  vtkMRMLReadXMLStdStringMacro(TitleFontName, TitleFontName);
  vtkMRMLReadXMLBooleanMacro(TitleFontBold, TitleFontBold);
  vtkMRMLReadXMLBooleanMacro(TitleFontItalic, TitleFontItalic);
  vtkMRMLReadXMLBooleanMacro(TitleFontShadow, TitleFontShadow);
  vtkMRMLReadXMLVectorMacro(LabelFontColorRGB, LabelFontColorRGB, double, 3);
  vtkMRMLReadXMLFloatMacro(LabelFontOpacity, LabelFontOpacity);
  vtkMRMLReadXMLFloatMacro(LabelFontSize, LabelFontSize);
  vtkMRMLReadXMLStdStringMacro(LabelFontName, LabelFontName);
  vtkMRMLReadXMLBooleanMacro(LabelFontBold, LabelFontBold);
  vtkMRMLReadXMLBooleanMacro(LabelFontItalic, LabelFontItalic);
  vtkMRMLReadXMLBooleanMacro(LabelFontShadow, LabelFontShadow);
  vtkMRMLReadXMLStdStringMacro(LabelFormat, LabelFormat);
  vtkMRMLReadXMLIntMacro(MaxNumberOfColors, MaxNumberOfColors);
  vtkMRMLReadXMLIntMacro(NumberOfLabels, NumberOfLabels);
  vtkMRMLReadXMLBooleanMacro(UseColorNamesForLabels, UseColorNamesForLabels);
  vtkMRMLReadXMLBooleanMacro(CenterLabels, CenterLabels);
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
  vtkMRMLCopyEnumMacro(Orientation);
  vtkMRMLCopyVectorMacro(Position, float, 2);
  vtkMRMLCopyFloatMacro(Width);
  vtkMRMLCopyFloatMacro(Height);
  vtkMRMLCopyStdStringMacro(TitleText);
  vtkMRMLCopyVectorMacro(TitleFontColorRGB, double, 3);
  vtkMRMLCopyFloatMacro(TitleFontOpacity);
  vtkMRMLCopyFloatMacro(TitleFontSize);
  vtkMRMLCopyStdStringMacro(TitleFontName);
  vtkMRMLCopyBooleanMacro(TitleFontBold);
  vtkMRMLCopyBooleanMacro(TitleFontItalic);
  vtkMRMLCopyBooleanMacro(TitleFontShadow);
  vtkMRMLCopyVectorMacro(LabelFontColorRGB, double, 3);
  vtkMRMLCopyFloatMacro(LabelFontOpacity);
  vtkMRMLCopyFloatMacro(LabelFontSize);
  vtkMRMLCopyStdStringMacro(LabelFontName);
  vtkMRMLCopyBooleanMacro(LabelFontBold);
  vtkMRMLCopyBooleanMacro(LabelFontItalic);
  vtkMRMLCopyBooleanMacro(LabelFontShadow);
  vtkMRMLCopyStdStringMacro(LabelFormat);
  vtkMRMLCopyIntMacro(MaxNumberOfColors);
  vtkMRMLCopyIntMacro(NumberOfLabels);
  vtkMRMLCopyBooleanMacro(UseColorNamesForLabels);
  vtkMRMLCopyBooleanMacro(CenterLabels);
  vtkMRMLCopyEndMacro();
}

//---------------------------------------------------------------------------
void vtkMRMLColorBarDisplayNode::SetOrientation(int id)
{
  switch (id)
    {
    case 0:
      SetOrientation(vtkMRMLColorBarDisplayNode::Horizontal);
      break;
    case 1:
    default:
      SetOrientation(vtkMRMLColorBarDisplayNode::Vertical);
      break;
    }
}

//---------------------------------------------------------------------------
const char* vtkMRMLColorBarDisplayNode::GetOrientationAsString(int id)
{
  switch (id)
    {
    case vtkMRMLColorBarDisplayNode::Horizontal:
      return "Horizontal";
    case vtkMRMLColorBarDisplayNode::Vertical:
    default:
      return "Vertical";
    }
}

//-----------------------------------------------------------
int vtkMRMLColorBarDisplayNode::GetOrientationFromString(const char* name)
{
  if (name == nullptr)
    {
    // invalid name
    return -1;
    }
  for (int i = 0; i < vtkMRMLColorBarDisplayNode::Orientation_Last; i++)
    {
    if (std::strcmp(name, vtkMRMLColorBarDisplayNode::GetOrientationAsString(i)) == 0)
      {
      // found a matching name
      return i;
      }
    }
  // unknown name
  return -1;
}
