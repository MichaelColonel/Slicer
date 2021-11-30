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
class vtkTextProperty;

class VTK_SLICER_COLORS_MODULE_MRML_EXPORT vtkMRMLColorBarDisplayNode : public vtkMRMLDisplayNode
{
public:
  /// Color bar orientation preset on a view node
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

  vtkGetVector2Macro(Position, double);
  vtkSetVector2Macro(Position, double);

  vtkGetMacro(Width, double);
  vtkSetMacro(Width, double);

  vtkGetMacro(Height, double);
  vtkSetMacro(Height, double);

  vtkGetMacro(TitleText, std::string);
  vtkSetMacro(TitleText, std::string);

  vtkGetVector3Macro(TitleTextColorRGB, double);
  vtkSetVector3Macro(TitleTextColorRGB, double);

  vtkGetMacro(TitleTextOpacity, double);
  vtkSetMacro(TitleTextOpacity, double);

  vtkGetMacro(TitleFontSize, int);
  vtkSetMacro(TitleFontSize, int);

  vtkGetMacro(TitleFontName, std::string);
  vtkSetMacro(TitleFontName, std::string);

  vtkGetMacro(TitleFontBold, bool);
  vtkSetMacro(TitleFontBold, bool);

  vtkGetMacro(TitleFontItalic, bool);
  vtkSetMacro(TitleFontItalic, bool);

  vtkGetMacro(TitleFontShadow, bool);
  vtkSetMacro(TitleFontShadow, bool);

  vtkGetVector3Macro(LabelsTextColorRGB, double);
  vtkSetVector3Macro(LabelsTextColorRGB, double);

  vtkGetMacro(LabelsTextOpacity, double);
  vtkSetMacro(LabelsTextOpacity, double);

  vtkGetMacro(LabelsFontSize, int);
  vtkSetMacro(LabelsFontSize, int);

  vtkGetMacro(LabelsFontName, std::string);
  vtkSetMacro(LabelsFontName, std::string);

  vtkGetMacro(LabelsFontBold, bool);
  vtkSetMacro(LabelsFontBold, bool);

  vtkGetMacro(LabelsFontItalic, bool);
  vtkSetMacro(LabelsFontItalic, bool);

  vtkGetMacro(LabelsFontShadow, bool);
  vtkSetMacro(LabelsFontShadow, bool);

  vtkGetMacro(LabelsFormat, std::string);
  vtkSetMacro(LabelsFormat, std::string);

  vtkGetMacro(MaxNumberOfColors, int);
  vtkSetMacro(MaxNumberOfColors, int);

  vtkGetMacro(NumberOfLabels, int);
  vtkSetMacro(NumberOfLabels, int);

  vtkGetMacro(UseColorNamesForLabels, bool);
  vtkSetMacro(UseColorNamesForLabels, bool);

  vtkGetMacro(CenterLabels, bool);
  vtkSetMacro(CenterLabels, bool);

  vtkGetMacro(LogarithmicScale, bool);
  vtkSetMacro(LogarithmicScale, bool);
  vtkBooleanMacro(LogarithmicScale, bool);

  // Get vtkTextProperty from node
  void GetTitleTextProperty(vtkTextProperty*);
  void GetLabelsTextProperty(vtkTextProperty*);

  // Set node parameters from vtkTextProperty
  void SetTitleTextProperty(vtkTextProperty*);
  void SetLabelsTextProperty(vtkTextProperty*);

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
  double Position[2]{ 0.05, 0.4 }; // color bar position within view
  double Width{ 0.1 }; // color bar width within view
  double Height{ 0.45 }; // color bar height within view
  std::string TitleText; // color bar title
  struct FontProperties
  {
    double ColorRGB[3]{ 1., 1., 1. }; // text color [0., 1.]
    double Opacity{ 1. }; // text opacity
    double Size{ 12. }; // size of the font
    std::string FontName{ "Arial" }; // "Arial", "Courier", "Times" or "Unknown"
    bool Bold{ false }; // bold font
    bool Italic{ false }; // italic font
    bool Shadow{ false }; // shadow font
  } TitleFontProperties, LabelsFontProperties;

  double* TitleTextColorRGB;
  double& TitleTextOpacity;
  double& TitleFontSize;
  std::string& TitleFontName;
  bool& TitleFontBold;
  bool& TitleFontItalic;
  bool& TitleFontShadow;

  double* LabelsTextColorRGB;
  double& LabelsTextOpacity;
  double& LabelsFontSize;
  std::string& LabelsFontName;
  bool& LabelsFontBold;
  bool& LabelsFontItalic;
  bool& LabelsFontShadow;

  std::string LabelsFormat{ "%-#6.3g" };
  int MaxNumberOfColors{ 5 };
  int NumberOfLabels{ 5 };
  bool UseColorNamesForLabels{ false };
  bool CenterLabels{ false };
  bool LogarithmicScale{ false };
};

#endif
