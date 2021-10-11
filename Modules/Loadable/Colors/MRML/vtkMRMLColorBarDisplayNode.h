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

#ifndef __vtkMRMLColorBarDisplayNode_h
#define __vtkMRMLColorBarDisplayNode_h

// MRML includes
#include <vtkMRMLDisplayNode.h>
#include "vtkSlicerColorsModuleMRMLExport.h"

class vtkMRMLColorNode;

class VTK_SLICER_COLORS_MODULE_MRML_EXPORT vtkMRMLColorBarDisplayNode : public vtkMRMLDisplayNode
{
public:
  /// Color bar position preset on a view node
  enum OrientationType : int { Horizontal = 0, Vertical, Orientation_Last };

  static vtkMRMLColorBarDisplayNode *New();
  vtkTypeMacro(vtkMRMLColorBarDisplayNode,vtkMRMLDisplayNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkMRMLNode* CreateNodeInstance() override;

  /// Get node XML tag name (like Volume, Model)
  const char* GetNodeTagName() override { return "ColorBarDisplay"; }

  /// Read node attributes from XML file
  void ReadXMLAttributes(const char** atts) override;

  /// Write this node's information to a MRML file in XML format.
  void WriteXML(ostream& of, int indent) override;

  /// Copy node content (excludes basic data, such as name and node references).
  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentMacro(vtkMRMLColorBarDisplayNode);

  vtkGetMacro(Orientation, OrientationType);
  vtkSetMacro(Orientation, OrientationType);


  vtkGetVector2Macro(Position, float);
  vtkSetVector2Macro(Position, float);

  vtkGetMacro(Width, double);
  vtkSetMacro(Width, double);

  vtkGetMacro(Height, double);
  vtkSetMacro(Height, double);

  vtkGetMacro(TitleText, std::string);
  vtkSetMacro(TitleText, std::string);

  vtkGetVector3Macro(TitleFontColorRGB, double);
  vtkSetVector3Macro(TitleFontColorRGB, double);

  vtkGetMacro(TitleFontOpacity, double);
  vtkSetMacro(TitleFontOpacity, double);

  vtkGetMacro(TitleFontSize, int);
  vtkSetMacro(TitleFontSize, int);

  vtkGetMacro(TitleFontBold, bool);
  vtkSetMacro(TitleFontBold, bool);

  vtkGetMacro(TitleFontItalic, bool);
  vtkSetMacro(TitleFontItalic, bool);

  vtkGetMacro(TitleFontShadow, bool);
  vtkSetMacro(TitleFontShadow, bool);

  vtkGetVector3Macro(LabelFontColorRGB, double);
  vtkSetVector3Macro(LabelFontColorRGB, double);

  vtkGetMacro(LabelFontOpacity, double);
  vtkSetMacro(LabelFontOpacity, double);

  vtkGetMacro(LabelFontSize, int);
  vtkSetMacro(LabelFontSize, int);

  vtkGetMacro(LabelFontBold, bool);
  vtkSetMacro(LabelFontBold, bool);

  vtkGetMacro(LabelFontItalic, bool);
  vtkSetMacro(LabelFontItalic, bool);

  vtkGetMacro(LabelFontShadow, bool);
  vtkSetMacro(LabelFontShadow, bool);

  vtkGetMacro(LabelFormat, std::string);
  vtkSetMacro(LabelFormat, std::string);

  vtkGetMacro(MaxNumberOfColors, int);
  vtkSetMacro(MaxNumberOfColors, int);

  vtkGetMacro(NumberOfLabels, int);
  vtkSetMacro(NumberOfLabels, int);

  vtkGetMacro(UseColorNamesForLabels, bool);
  vtkSetMacro(UseColorNamesForLabels, bool);

  vtkGetMacro(CenterLabels, bool);
  vtkSetMacro(CenterLabels, bool);

protected:
  vtkMRMLColorBarDisplayNode();
  ~vtkMRMLColorBarDisplayNode() override;
  vtkMRMLColorBarDisplayNode(const vtkMRMLColorBarDisplayNode&);
  void operator=(const vtkMRMLColorBarDisplayNode&);

  static const char* GetOrientationAsString(int id);
  static int GetOrientationFromString(const char* name);
  void SetOrientation(int id);

private:
  OrientationType Orientation{ vtkMRMLColorBarDisplayNode::Vertical }; // Vertical or Horizontal
  float Position[2]{ 0.1, 0.1 }; // color bar position within view
  double Width{ 0.1 }; // color bar width within view
  double Height{ 0.8 }; // color bar height within view
  std::string TitleText; // color bar title
  struct FontProperties
  {
    double ColorRGB[3]{ 1., 1., 1. }; // text color [0., 1.]
    double Opacity{ 0. }; // text opacity
    double Size{ 12. }; // size of the font
    bool Bold{ false }; // bold font
    bool Italic{ false }; // italic font
    bool Shadow{ false }; // shadow font
  } TitleFontProperties, LabelFontProperties;

  double* TitleFontColorRGB;
  double& TitleFontOpacity;
  double& TitleFontSize;
  bool& TitleFontBold;
  bool& TitleFontItalic;
  bool& TitleFontShadow;

  double* LabelFontColorRGB;
  double& LabelFontOpacity;
  double& LabelFontSize;
  bool& LabelFontBold;
  bool& LabelFontItalic;
  bool& LabelFontShadow;

  std::string LabelFormat{ "%s" };
  int MaxNumberOfColors{ 5 };
  int NumberOfLabels{ 5 };
  bool UseColorNamesForLabels{ false };
  bool CenterLabels{ false };
};

#endif
