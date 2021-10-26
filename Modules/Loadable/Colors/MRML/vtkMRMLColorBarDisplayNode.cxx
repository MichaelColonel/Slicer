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
  TitleTextColorRGB(TitleFontProperties.ColorRGB),
  TitleTextOpacity(TitleFontProperties.Opacity),
  TitleFontSize(TitleFontProperties.Size),
  TitleFontName(TitleFontProperties.FontName),
  TitleFontBold(TitleFontProperties.Bold),
  TitleFontItalic(TitleFontProperties.Italic),
  TitleFontShadow(TitleFontProperties.Shadow),
  LabelsTextColorRGB(LabelsFontProperties.ColorRGB),
  LabelsTextOpacity(LabelsFontProperties.Opacity),
  LabelsFontSize(LabelsFontProperties.Size),
  LabelsFontName(LabelsFontProperties.FontName),
  LabelsFontBold(LabelsFontProperties.Bold),
  LabelsFontItalic(LabelsFontProperties.Italic),
  LabelsFontShadow(LabelsFontProperties.Shadow)
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
  vtkMRMLPrintVectorMacro(TitleTextColorRGB, double, 3);
  vtkMRMLPrintFloatMacro(TitleTextOpacity);
  vtkMRMLPrintFloatMacro(TitleFontSize);
  vtkMRMLPrintStdStringMacro(TitleFontName);
  vtkMRMLPrintBooleanMacro(TitleFontBold);
  vtkMRMLPrintBooleanMacro(TitleFontItalic);
  vtkMRMLPrintBooleanMacro(TitleFontShadow);
  vtkMRMLPrintVectorMacro(LabelsTextColorRGB, double, 3);
  vtkMRMLPrintFloatMacro(LabelsTextOpacity);
  vtkMRMLPrintFloatMacro(LabelsFontSize);
  vtkMRMLPrintStdStringMacro(LabelsFontName);
  vtkMRMLPrintBooleanMacro(LabelsFontBold);
  vtkMRMLPrintBooleanMacro(LabelsFontItalic);
  vtkMRMLPrintBooleanMacro(LabelsFontShadow);
  vtkMRMLPrintStdStringMacro(LabelsFormat);
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
  vtkMRMLWriteXMLVectorMacro(TitleTextColorRGB, TitleTextColorRGB, double, 3);
  vtkMRMLWriteXMLFloatMacro(TitleTextOpacity, TitleTextOpacity);
  vtkMRMLWriteXMLFloatMacro(TitleFontSize, TitleFontSize);
  vtkMRMLWriteXMLStdStringMacro(TitleFontName, TitleFontName);
  vtkMRMLWriteXMLBooleanMacro(TitleFontBold, TitleFontBold);
  vtkMRMLWriteXMLBooleanMacro(TitleFontItalic, TitleFontItalic);
  vtkMRMLWriteXMLBooleanMacro(TitleFontShadow, TitleFontShadow);
  vtkMRMLWriteXMLVectorMacro(LabelsTextColorRGB, LabelsTextColorRGB, double, 3);
  vtkMRMLWriteXMLFloatMacro(LabelsTextOpacity, LabelsTextOpacity);
  vtkMRMLWriteXMLFloatMacro(LabelsFontSize, LabelsFontSize);
  vtkMRMLWriteXMLStdStringMacro(LabelsFontName, LabelsFontName);
  vtkMRMLWriteXMLBooleanMacro(LabelsFontBold, LabelsFontBold);
  vtkMRMLWriteXMLBooleanMacro(LabelsFontItalic, LabelsFontItalic);
  vtkMRMLWriteXMLBooleanMacro(LabelsFontShadow, LabelsFontShadow);
  vtkMRMLWriteXMLStdStringMacro(LabelsFormat, LabelsFormat);
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
  vtkMRMLReadXMLVectorMacro(TitleTextColorRGB, TitleTextColorRGB, double, 3);
  vtkMRMLReadXMLFloatMacro(TitleTextOpacity, TitleTextOpacity);
  vtkMRMLReadXMLFloatMacro(TitleFontSize, TitleFontSize);
  vtkMRMLReadXMLStdStringMacro(TitleFontName, TitleFontName);
  vtkMRMLReadXMLBooleanMacro(TitleFontBold, TitleFontBold);
  vtkMRMLReadXMLBooleanMacro(TitleFontItalic, TitleFontItalic);
  vtkMRMLReadXMLBooleanMacro(TitleFontShadow, TitleFontShadow);
  vtkMRMLReadXMLVectorMacro(LabelsTextColorRGB, LabelsTextColorRGB, double, 3);
  vtkMRMLReadXMLFloatMacro(LabelsTextOpacity, LabelsTextOpacity);
  vtkMRMLReadXMLFloatMacro(LabelsFontSize, LabelsFontSize);
  vtkMRMLReadXMLStdStringMacro(LabelsFontName, LabelsFontName);
  vtkMRMLReadXMLBooleanMacro(LabelsFontBold, LabelsFontBold);
  vtkMRMLReadXMLBooleanMacro(LabelsFontItalic, LabelsFontItalic);
  vtkMRMLReadXMLBooleanMacro(LabelsFontShadow, LabelsFontShadow);
  vtkMRMLReadXMLStdStringMacro(LabelsFormat, LabelsFormat);
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
  vtkMRMLCopyVectorMacro(TitleTextColorRGB, double, 3);
  vtkMRMLCopyFloatMacro(TitleTextOpacity);
  vtkMRMLCopyFloatMacro(TitleFontSize);
  vtkMRMLCopyStdStringMacro(TitleFontName);
  vtkMRMLCopyBooleanMacro(TitleFontBold);
  vtkMRMLCopyBooleanMacro(TitleFontItalic);
  vtkMRMLCopyBooleanMacro(TitleFontShadow);
  vtkMRMLCopyVectorMacro(LabelsTextColorRGB, double, 3);
  vtkMRMLCopyFloatMacro(LabelsTextOpacity);
  vtkMRMLCopyFloatMacro(LabelsFontSize);
  vtkMRMLCopyStdStringMacro(LabelsFontName);
  vtkMRMLCopyBooleanMacro(LabelsFontBold);
  vtkMRMLCopyBooleanMacro(LabelsFontItalic);
  vtkMRMLCopyBooleanMacro(LabelsFontShadow);
  vtkMRMLCopyStdStringMacro(LabelsFormat);
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
