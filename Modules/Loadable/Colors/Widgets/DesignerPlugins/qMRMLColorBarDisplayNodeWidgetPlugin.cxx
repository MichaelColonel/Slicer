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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#include "qMRMLColorBarDisplayNodeWidgetPlugin.h"
#include "qMRMLColorBarDisplayNodeWidget.h"

//-----------------------------------------------------------------------------
qMRMLColorBarDisplayNodeWidgetPlugin::qMRMLColorBarDisplayNodeWidgetPlugin(QObject *newParent)
  : QObject(newParent)
{
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::group() const
{
  return "Slicer [MRML Widgets]";
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::whatsThis() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QWidget *qMRMLColorBarDisplayNodeWidgetPlugin::createWidget(QWidget *newParent)
{
  qMRMLColorBarDisplayNodeWidget* newWidget =
    new qMRMLColorBarDisplayNodeWidget(newParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::domXml() const
{
  return "<widget class=\"qMRMLColorBarDisplayNodeWidget\" \
          name=\"ColorBarDisplayNodeWidget\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>200</width>\n"
          "   <height>200</height>\n"
          "  </rect>\n"
          " </property>\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon qMRMLColorBarDisplayNodeWidgetPlugin::icon() const
{
  return QIcon(":/Icons/Colors.png");
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::includeFile() const
{
  return "qMRMLColorBarDisplayNodeWidget.h";
}

//-----------------------------------------------------------------------------
bool qMRMLColorBarDisplayNodeWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString qMRMLColorBarDisplayNodeWidgetPlugin::name() const
{
  return "qMRMLColorBarDisplayNodeWidget";
}
