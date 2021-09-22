import os
import time
import unittest
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging

#
# ColorsScalarBarSelfTest
#

class ColorsScalarBarSelfTest(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "ColorsScalarBarSelfTest"
    self.parent.categories = ["Testing.TestCases"]
    self.parent.dependencies = []
    self.parent.contributors = ["Kevin Wang (PMH), Nicole Aucoin (BWH)"]
    self.parent.helpText = """
    This is a test case for the new vtkSlicerScalarBarActor class.
    It iterates through all the color nodes and sets them active in the
    Colors module while the scalar bar widget is displayed.
    """
    self.parent.acknowledgementText = """
    This file was originally developed by Kevin Wang, PMH and was funded by CCO and OCAIRO.
""" # replace with organization, grant and thanks.

#
# ColorsScalarBarSelfTestWidget
#

class ColorsScalarBarSelfTestWidget(ScriptedLoadableModuleWidget):

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    # Instantiate and connect widgets ...

    #
    # Parameters Area
    #
    parametersCollapsibleButton = ctk.ctkCollapsibleButton()
    parametersCollapsibleButton.text = "Parameters"
    self.layout.addWidget(parametersCollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(parametersCollapsibleButton)

    # Apply Button
    #
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Run the algorithm."
    self.applyButton.enabled = True
    parametersFormLayout.addRow(self.applyButton)

    # connections
    self.applyButton.connect('clicked(bool)', self.onApplyButton)

    # Add vertical spacer
    self.layout.addStretch(1)

  def cleanup(self):
    pass

  def onApplyButton(self):
    test = ColorsScalarBarSelfTestTest()
    print("Run the test algorithm")
    test.test_ColorsScalarBarSelfTest1()


class ColorsScalarBarSelfTestTest(ScriptedLoadableModuleTest):
  """
  This is the test case for your scripted module.
  """

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_ColorsScalarBarSelfTest1()
    self.test_ColorsScalarBarSelfTest2()

  def test_ColorsScalarBarSelfTest1(self):

    self.delayDisplay("Starting the scalarbar test")

    logging.info('Processing started')

    # start in the colors module
    m = slicer.util.mainWindow()
    m.moduleSelector().selectModule('Colors')
    self.delayDisplay('In Colors module')

    colorWidget = slicer.modules.colors.widgetRepresentation()
    ctkScalarBarWidget = slicer.util.findChildren(colorWidget, name='VTKScalarBar')[0]
    # show the scalar bar widget
    ctkScalarBarWidget.setDisplay(1)
    activeColorNodeSelector = slicer.util.findChildren(colorWidget, 'ColorTableComboBox')[0]
    useColorNameAsLabelCheckbox = slicer.util.findChildren(colorWidget, 'UseColorNameAsLabelCheckBox')[0]
    checked = useColorNameAsLabelCheckbox.isChecked()
    # iterate over the color nodes and set each one active
    numColorNodes = slicer.mrmlScene.GetNumberOfNodesByClass('vtkMRMLColorNode')
    for n in range(numColorNodes):
      colorNode = slicer.mrmlScene.GetNthNodeByClass(n, 'vtkMRMLColorNode')
      useColorNameAsLabelCheckbox.setChecked(checked)
      print("%d/%d" % (n, numColorNodes-1))
      self.delayDisplay('Setting Color Node To %s' % colorNode.GetName())
      activeColorNodeSelector.setCurrentNodeID(colorNode.GetID())
      # use the delay display here to ensure a render
      self.delayDisplay('Set Color Node To %s' % colorNode.GetName())
      useColorNameAsLabelCheckbox.setChecked(not checked)
      self.delayDisplay('Toggled using names as labels')

    logging.info('Processing completed')

    self.delayDisplay('Test passed!')

  def test_ColorsScalarBarSelfTest2(self):

    self.delayDisplay("Starting the colorbar displayable manager test")

    logging.info('Processing started')

    import SampleData
    sampleDataLogic = SampleData.SampleDataLogic()
    ctVolumeNode = sampleDataLogic.downloadCTChest()
    self.assertIsNotNone( ctVolumeNode )

    colorBarTest = slicer.mrmlScene.AddNewNodeByClass( 'vtkMRMLColorBarDisplayNode', 'ColorBarTest')

    threeDViewWidget = slicer.app.layoutManager().threeDWidget(0)
    colorBarDisplayableManager3D = threeDViewWidget.threeDView().displayableManagerByClassName("vtkMRMLColorBarDisplayableManager")
    if colorBarDisplayableManager3D is None:
      logging.error("Failed to find the color bar displayable manager in 3D widget")

    redWidget = slicer.app.layoutManager().sliceWidget("Red")
    colorBarDisplayableManagerRed = redWidget.sliceView().displayableManagerByClassName("vtkMRMLColorBarDisplayableManager")
    if colorBarDisplayableManagerRed is None:
      logging.error("Failed to find the color bar displayable manager in red slice widget")

    greenWidget = slicer.app.layoutManager().sliceWidget("Green")
    colorBarDisplayableManagerGreen = greenWidget.sliceView().displayableManagerByClassName("vtkMRMLColorBarDisplayableManager")
    if colorBarDisplayableManagerGreen is None:
      logging.error("Failed to find the color bar displayable manager in green slice widget")

    yellowWidget = slicer.app.layoutManager().sliceWidget("Yellow")
    colorBarDisplayableManagerYellow = redWidget.sliceView().displayableManagerByClassName("vtkMRMLColorBarDisplayableManager")
    if colorBarDisplayableManagerYellow is None:
      logging.error("Failed to find the color bar displayable manager in yellow slice widget")

    logging.info('Processing completed')

    self.delayDisplay('Test passed!')
