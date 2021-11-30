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

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkScalarBarWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>

// CTK includes
#include <ctkVTKScalarBarWidget.h>
#include <ctkVTKTextPropertyWidget.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// Slicer Colors MRML includes
#include <vtkMRMLColorBarDisplayNode.h>

// Qt includes
#include <QDebug>
#include <QSpinBox>

// Parameters Widgets includes
#include "qMRMLColorBarDisplayNodeWidget.h"
#include "ui_qMRMLColorBarDisplayNodeWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Colors
class qMRMLColorBarDisplayNodeWidgetPrivate : public QWidget, public Ui_qMRMLColorBarDisplayNodeWidget
{
  Q_DECLARE_PUBLIC(qMRMLColorBarDisplayNodeWidget);
protected:
  qMRMLColorBarDisplayNodeWidget* const q_ptr;
  typedef QWidget Superclass;

public:
  qMRMLColorBarDisplayNodeWidgetPrivate(qMRMLColorBarDisplayNodeWidget& object);
  virtual void setupUi(qMRMLColorBarDisplayNodeWidget*);
  void init();

  /// Color bar MRML node containing shown parameters
  vtkWeakPointer<vtkMRMLColorBarDisplayNode> ParameterNode;
};

// --------------------------------------------------------------------------
qMRMLColorBarDisplayNodeWidgetPrivate::qMRMLColorBarDisplayNodeWidgetPrivate(
  qMRMLColorBarDisplayNodeWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidgetPrivate::setupUi(qMRMLColorBarDisplayNodeWidget* widget)
{
  this->Ui_qMRMLColorBarDisplayNodeWidget::setupUi(widget);
}

// --------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidgetPrivate::init()
{
  Q_Q(qMRMLColorBarDisplayNodeWidget);

  this->PositionCoordinatesWidget->setSingleStep(0.05);
  this->WidthHeightCoordinatesWidget->setSingleStep(0.05);

  // Radio buttons
  QObject::connect( this->ColorBarOrientationButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
    q, SLOT(onColorBarOrientationButtonClicked(QAbstractButton*)));

  // qMRMLComboBox
  QObject::connect( this->DisplayNodeViewComboBox, SIGNAL(checkedNodesChanged()),
    q, SLOT(onCheckedViewNodesChanged()));

  // Coordinates widgets
  QObject::connect( this->PositionCoordinatesWidget, SIGNAL(coordinatesChanged(double*)),
    q, SLOT(onPositionChanged(double*)));
  QObject::connect( this->WidthHeightCoordinatesWidget, SIGNAL(coordinatesChanged(double*)),
    q, SLOT(onWidthHeightChanged(double*)));

  // Title properties
  QObject::connect(this->TitleTextLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onTitleTextChanged(QString)));
  QObject::connect(this->TitleColorPickerButton, SIGNAL(colorChanged(QColor)),
                   q, SLOT(onTitleColorChanged(QColor)));
  QObject::connect(this->TitleOpacitySlider, SIGNAL(valueChanged(double)),
                   q, SLOT(onTitleOpacityChanged(double)));
  QObject::connect(this->TitleFontComboBox, SIGNAL(currentIndexChanged(QString)),
                   q, SLOT(onTitleFontNameChanged(QString)));
  QObject::connect(this->TitleBoldCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onTitleFontBoldChanged(bool)));
  QObject::connect(this->TitleItalicCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onTitleFontItalicChanged(bool)));
  QObject::connect(this->TitleShadowCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onTitleFontShadowChanged(bool)));
  QObject::connect(this->TitleSizeSlider, SIGNAL(valueChanged(double)),
                   q, SLOT(onTitleFontSizeChanged(double)));

  // Labels properties
  QObject::connect(this->LabelsTextLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onLabelsFormatChanged(QString)));
  QObject::connect(this->LabelsColorPickerButton, SIGNAL(colorChanged(QColor)),
                   q, SLOT(onLabelsColorChanged(QColor)));
  QObject::connect(this->LabelsOpacitySlider, SIGNAL(valueChanged(double)),
                   q, SLOT(onLabelsOpacityChanged(double)));
  QObject::connect(this->LabelsFontComboBox, SIGNAL(currentIndexChanged(QString)),
                   q, SLOT(onLabelsFontNameChanged(QString)));
  QObject::connect(this->LabelsBoldCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onLabelsFontBoldChanged(bool)));
  QObject::connect(this->LabelsItalicCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onLabelsFontItalicChanged(bool)));
  QObject::connect(this->LabelsShadowCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(onLabelsFontShadowChanged(bool)));
  QObject::connect(this->LabelsSizeSlider, SIGNAL(valueChanged(double)),
                   q, SLOT(onLabelsFontSizeChanged(double)));

  // QSpinBox
  QObject::connect( this->MaxNumberOfColorsSpinBox, SIGNAL(valueChanged(int)),
    q, SLOT(onMaximumNumberOfColorsChanged(int)));
  QObject::connect( this->NumberOfLabelsSpinBox, SIGNAL(valueChanged(int)),
    q, SLOT(onNumberOfLabelsChanged(int)));

  // QCheckBox
  QObject::connect( this->ColorBarVisibilityCheckBox, SIGNAL(toggled(bool)),
    q, SLOT(onColorBarVisibilityToggled(bool)));
  QObject::connect( this->UseColorNameAsLabelCheckBox, SIGNAL(toggled(bool)),
    q, SLOT(onColorNameAsLabelToggled(bool)));
  QObject::connect( this->CenterLabelCheckBox, SIGNAL(toggled(bool)),
    q, SLOT(onCenterLabelToggled(bool)));
  QObject::connect( this->LogarithmicScaleCheckBox, SIGNAL(toggled(bool)),
    q, SLOT(onLogarithmicScaleToggled(bool)));
}

//-----------------------------------------------------------------------------
// qSlicerColorBarPropertiesWidget methods

//-----------------------------------------------------------------------------
qMRMLColorBarDisplayNodeWidget::qMRMLColorBarDisplayNodeWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLColorBarDisplayNodeWidgetPrivate(*this) )
{
  Q_D(qMRMLColorBarDisplayNodeWidget);
  d->setupUi(this);
  d->init();
}

//-----------------------------------------------------------------------------
qMRMLColorBarDisplayNodeWidget::~qMRMLColorBarDisplayNodeWidget()
{
}


//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);
  this->Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::setParameterNode(vtkMRMLNode *node)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  vtkMRMLColorBarDisplayNode* parameterNode = vtkMRMLColorBarDisplayNode::SafeDownCast(node);
  // Each time the node is modified, the UI widgets are updated
  qvtkReconnect( d->ParameterNode, parameterNode, vtkCommand::ModifiedEvent,
    this, SLOT( updateWidgetFromMRML() ) );

  d->ParameterNode = parameterNode;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    // Disable all widgets
    d->TitleGroupBox->setEnabled(false);
    d->LabelsGroupBox->setEnabled(false);
    d->ColorBarVisibilityCheckBox->setEnabled(false);
    d->DisplayNodeViewComboBox->setEnabled(false);
    d->VerticalOrientationRadioButton->setEnabled(false);
    d->HorizontalOrientationRadioButton->setEnabled(false);
    d->PositionCoordinatesWidget->setEnabled(false);
    d->WidthHeightCoordinatesWidget->setEnabled(false);
    d->UseColorNameAsLabelCheckBox->setEnabled(false);
    d->CenterLabelCheckBox->setEnabled(false);
    d->MaxNumberOfColorsSpinBox->setEnabled(false);
    d->NumberOfLabelsSpinBox->setEnabled(false);
    d->LogarithmicScaleCheckBox->setEnabled(false);
    return;
    }

  // Enable all widgets
  d->PositionCoordinatesWidget->setEnabled(true);
  d->WidthHeightCoordinatesWidget->setEnabled(true);

  d->TitleGroupBox->setEnabled(true);
  d->LabelsGroupBox->setEnabled(true);
  d->ColorBarVisibilityCheckBox->setEnabled(true);
  d->DisplayNodeViewComboBox->setEnabled(true);
  d->VerticalOrientationRadioButton->setEnabled(true);
  d->HorizontalOrientationRadioButton->setEnabled(true);

  d->UseColorNameAsLabelCheckBox->setEnabled(true);
  d->CenterLabelCheckBox->setEnabled(true);
  d->MaxNumberOfColorsSpinBox->setEnabled(true);
  d->NumberOfLabelsSpinBox->setEnabled(true);
  d->LogarithmicScaleCheckBox->setEnabled(true);

  // Set visibility checkbox
  d->ColorBarVisibilityCheckBox->setChecked(d->ParameterNode->GetVisibility());

  // Setup color bar orientation
  switch (d->ParameterNode->GetOrientation())
    {
    case vtkMRMLColorBarDisplayNode::Vertical:
      d->VerticalOrientationRadioButton->setChecked(true);
      d->HorizontalOrientationRadioButton->setChecked(false);
      break;
    case vtkMRMLColorBarDisplayNode::Horizontal:
      d->HorizontalOrientationRadioButton->setChecked(true);
      d->VerticalOrientationRadioButton->setChecked(false);
      break;
    default:
      break;
    }

  d->DisplayNodeViewComboBox->setMRMLDisplayNode(d->ParameterNode);

  // Setup color bar position and size
  double position[2];
  d->ParameterNode->GetPosition(position);
  d->PositionCoordinatesWidget->setCoordinates(position);
  double size[2] = { d->ParameterNode->GetWidth(), d->ParameterNode->GetHeight() };
  d->WidthHeightCoordinatesWidget->setCoordinates(size);

  // Title parameters
  std::string title = d->ParameterNode->GetTitleText();
  d->TitleTextLineEdit->setText(title.c_str());

  double color[3] = {};
  d->ParameterNode->GetTitleTextColorRGB(color);
  QColor titleColor = QColor::fromRgbF( color[0], color[1], color[2]);
  d->TitleColorPickerButton->setColor(titleColor);

  d->TitleOpacitySlider->setValue(d->ParameterNode->GetTitleTextOpacity());
  d->TitleFontComboBox->setCurrentIndex(d->TitleFontComboBox->findText(d->ParameterNode->GetTitleFontName().c_str()));
  d->TitleBoldCheckBox->setChecked(d->ParameterNode->GetTitleFontBold());
  d->TitleItalicCheckBox->setChecked(d->ParameterNode->GetTitleFontItalic());
  d->TitleShadowCheckBox->setChecked(d->ParameterNode->GetTitleFontShadow());
  d->TitleSizeSlider->setValue(d->ParameterNode->GetTitleFontSize());

  // Labels parameters
  std::string format = d->ParameterNode->GetLabelsFormat();
  d->LabelsTextLineEdit->setText(format.c_str());

  d->ParameterNode->GetLabelsTextColorRGB(color);
  QColor labelsColor = QColor::fromRgbF( color[0], color[1], color[2]);
  d->LabelsColorPickerButton->setColor(labelsColor);

  d->LabelsOpacitySlider->setValue(d->ParameterNode->GetLabelsTextOpacity());
  d->LabelsFontComboBox->setCurrentIndex(d->LabelsFontComboBox->findText(d->ParameterNode->GetLabelsFontName().c_str()));
  d->LabelsBoldCheckBox->setChecked(d->ParameterNode->GetLabelsFontBold());
  d->LabelsItalicCheckBox->setChecked(d->ParameterNode->GetLabelsFontItalic());
  d->LabelsShadowCheckBox->setChecked(d->ParameterNode->GetLabelsFontShadow());
  d->LabelsSizeSlider->setValue(d->ParameterNode->GetLabelsFontSize());

  // Number of colors and labels
  d->MaxNumberOfColorsSpinBox->setValue(d->ParameterNode->GetMaxNumberOfColors());
  d->NumberOfLabelsSpinBox->setValue(d->ParameterNode->GetNumberOfLabels());

  // Labels naming and arrangment
  d->UseColorNameAsLabelCheckBox->setChecked(d->ParameterNode->GetUseColorNamesForLabels());
  d->CenterLabelCheckBox->setChecked(d->ParameterNode->GetCenterLabels());
  // Scalars range scale of LUT: logarithmic or linear
  d->LogarithmicScaleCheckBox->setChecked(d->ParameterNode->GetLogarithmicScale());
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onColorBarVisibilityToggled(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetVisibility(state);
}

//-----------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onColorBarOrientationButtonClicked(QAbstractButton* button)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);
  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  QRadioButton* radioButton = qobject_cast<QRadioButton*>(button);
  if (radioButton == d->HorizontalOrientationRadioButton)
    {
    double size[2] = { d->ParameterNode->GetWidth(), d->ParameterNode->GetHeight() };
    std::swap<double>( size[0], size[1]);
    QSignalBlocker blocker(d->WidthHeightCoordinatesWidget);
    d->WidthHeightCoordinatesWidget->setCoordinates(size);
    d->ParameterNode->DisableModifiedEventOn();
    d->ParameterNode->SetWidth(size[0]);
    d->ParameterNode->SetHeight(size[1]);
    d->ParameterNode->DisableModifiedEventOff();

    d->ParameterNode->SetOrientation(vtkMRMLColorBarDisplayNode::Horizontal);
    }
  else if (radioButton == d->VerticalOrientationRadioButton)
    {
    double size[2] = { d->ParameterNode->GetWidth(), d->ParameterNode->GetHeight() };
    std::swap<double>( size[0], size[1]);
    QSignalBlocker blocker(d->WidthHeightCoordinatesWidget);
    d->WidthHeightCoordinatesWidget->setCoordinates(size);
    d->ParameterNode->DisableModifiedEventOn();
    d->ParameterNode->SetWidth(size[0]);
    d->ParameterNode->SetHeight(size[1]);
    d->ParameterNode->DisableModifiedEventOff();

    d->ParameterNode->SetOrientation(vtkMRMLColorBarDisplayNode::Vertical);
    }
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onCheckedViewNodesChanged()
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onPositionChanged(double* position)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetPosition( position[0], position[1]);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onWidthHeightChanged(double* size)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetWidth(size[0]);
  d->ParameterNode->SetHeight(size[1]);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleTextChanged(const QString& titleText)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleText(titleText.toStdString());
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleColorChanged(const QColor& titleColor)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  qreal r,g,b;
  titleColor.getRgbF( &r, &g, &b);
  double rgb[3] = { double(r), double(g), double(b) };
  d->ParameterNode->SetTitleTextColorRGB(rgb);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleOpacityChanged(double opacity)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleTextOpacity(opacity);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleFontNameChanged(const QString& fontName)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleFontName(fontName.toStdString());
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleFontShadowChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleFontShadow(state);
}
//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleFontItalicChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleFontItalic(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleFontBoldChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleFontBold(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onTitleFontSizeChanged(double size)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetTitleFontSize(static_cast<int>(size));
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFormatChanged(const QString& labelsFormat)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFormat(labelsFormat.toStdString());
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsColorChanged(const QColor& labelsColor)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  qreal r,g,b;
  labelsColor.getRgbF( &r, &g, &b);
  double rgb[3] = { double(r), double(g), double(b) };
  d->ParameterNode->SetLabelsTextColorRGB(rgb);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsOpacityChanged(double opacity)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsTextOpacity(opacity);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFontNameChanged(const QString& fontName)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFontName(fontName.toStdString());
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFontShadowChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFontShadow(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFontItalicChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFontItalic(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFontBoldChanged(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFontBold(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLabelsFontSizeChanged(double size)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLabelsFontSize(static_cast<int>(size));
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onMaximumNumberOfColorsChanged(int maxNumberOfColors)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetMaxNumberOfColors(maxNumberOfColors);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onNumberOfLabelsChanged(int numberOfLabels)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetNumberOfLabels(numberOfLabels);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onColorNameAsLabelToggled(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  if (state)
    {
    // text string format
    d->ParameterNode->SetLabelsFormat(" %s");
    }
  else
    {
    // number format
    d->ParameterNode->SetLabelsFormat(" %-#6.3g");
    }

  d->ParameterNode->SetUseColorNamesForLabels(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onCenterLabelToggled(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetCenterLabels(state);
}

//-----------------------------------------------------------------------------
void qMRMLColorBarDisplayNodeWidget::onLogarithmicScaleToggled(bool state)
{
  Q_D(qMRMLColorBarDisplayNodeWidget);

  if (!d->ParameterNode)
    {
    qDebug() << Q_FUNC_INFO << ": Invalid parameter node";
    return;
    }

  d->ParameterNode->SetLogarithmicScale(state);
}
