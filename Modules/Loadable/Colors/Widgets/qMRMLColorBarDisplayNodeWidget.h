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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLColorBarDisplayNodeWidget_h
#define __qMRMLColorBarDisplayNodeWidget_h

// Slicer includes
#include <qMRMLWidget.h>

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

// Colors Widgets includes
#include "qSlicerColorsModuleWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLColorBarDisplayNode;
class qMRMLColorBarDisplayNodeWidgetPrivate;
class QAbstractButton;

/// \ingroup Slicer_QtModules_Colors
class Q_SLICER_MODULE_COLORS_WIDGETS_EXPORT qMRMLColorBarDisplayNodeWidget
  : public qMRMLWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qMRMLWidget Superclass;
  explicit qMRMLColorBarDisplayNodeWidget(QWidget *parent=0);
  ~qMRMLColorBarDisplayNodeWidget() override;

public slots:
  void setMRMLScene(vtkMRMLScene*) override;
  /// Set ColorBarDisplay MRML node (Parameter node)
  void setParameterNode(vtkMRMLNode* node);
  /// Update widget GUI from color bar parameters node
  void updateWidgetFromMRML();

  void onColorBarVisibilityToggled(bool);
  void onColorBarOrientationButtonClicked(QAbstractButton*);
  void onCheckedViewNodesChanged();
  void onPositionChanged(double*);
  void onWidthHeightChanged(double*);

  void onTitleTextChanged(const QString&);

  void onTitleColorChanged(const QColor&);
  void onTitleOpacityChanged(double);
  void onTitleFontNameChanged(const QString&);
  void onTitleFontShadowChanged(bool);
  void onTitleFontItalicChanged(bool);
  void onTitleFontBoldChanged(bool);
  void onTitleFontSizeChanged(double);

  void onLabelsFormatChanged(const QString&);

  void onLabelsColorChanged(const QColor&);
  void onLabelsOpacityChanged(double);
  void onLabelsFontNameChanged(const QString&);
  void onLabelsFontShadowChanged(bool);
  void onLabelsFontItalicChanged(bool);
  void onLabelsFontBoldChanged(bool);
  void onLabelsFontSizeChanged(double);

  void onMaximumNumberOfColorsChanged(int);
  void onNumberOfLabelsChanged(int);

  void onColorNameAsLabelToggled(bool);
  void onCenterLabelToggled(bool);
  void onLogarithmicScaleToggled(bool);

protected:
  QScopedPointer<qMRMLColorBarDisplayNodeWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLColorBarDisplayNodeWidget);
  Q_DISABLE_COPY(qMRMLColorBarDisplayNodeWidget);
};

#endif
