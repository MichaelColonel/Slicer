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
    self.parent.contributors = ["Kevin Wang (PMH), Nicole Aucoin (BWH), Mikhail Polkovnikov (IHEP)"]
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
    # Timeout delay
    self.delayMs = 700

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_ColorsScalarBarSelfTest1()
    self.test_ColorsScalarBarSelfTest2()

  def test_ColorsScalarBarSelfTest1(self):

    self.delayDisplay("Starting the scalarbar test")

    import SampleData
    sampleDataLogic = SampleData.SampleDataLogic()
    ctVolumeNode = sampleDataLogic.downloadCTChest()
    self.assertIsNotNone( ctVolumeNode )

    logging.info('Processing started')

    # start in the colors module
    m = slicer.util.mainWindow()
    m.moduleSelector().selectModule('Colors')
    self.delayDisplay('In Colors module')

    colorBar = slicer.modules.colors.logic().CreateAndObserveColorBarDisplayNode(ctVolumeNode)
    self.assertIsNotNone( colorBar )

    self.delayDisplay('Show color bar on all views and slices!', self.delayMs)
    colorWidget = slicer.modules.colors.widgetRepresentation()
    # show the scalar bar widget
    activeColorNodeSelector = slicer.util.findChildren(colorWidget, 'ColorTableComboBox')[0]

    useSelectedColorsCheckBox = slicer.util.findChildren(colorWidget, 'UseSelectedColorsCheckBox')[0]
    self.assertIsNotNone( useSelectedColorsCheckBox )

    colorBarDisplayNodeWidget = slicer.util.findChildren(colorWidget, 'ColorBarDisplayNodeWidget')[0]
    self.assertIsNotNone( colorBarDisplayNodeWidget )

    # set vtkMRMLColorBarDisplayNode into qMRMLColorBarDisplayNodeWidget to control its parameters
    colorBarDisplayNodeWidget.setParameterNode(colorBar)

    colorBarVisibilityCheckBox = slicer.util.findChildren(colorBarDisplayNodeWidget, 'ColorBarVisibilityCheckBox')[0]
    self.assertIsNotNone( colorBarVisibilityCheckBox )
    colorBarVisibilityCheckBox.setChecked(True)

    # iterate over the color nodes and set each one active
    numColorNodes = slicer.mrmlScene.GetNumberOfNodesByClass('vtkMRMLColorNode')
    for n in range(numColorNodes):
      colorNode = slicer.mrmlScene.GetNthNodeByClass(n, 'vtkMRMLColorNode')
      print("%d/%d" % (n, numColorNodes-1))
      self.delayDisplay('Setting Color Node To %s' % colorNode.GetName())
      activeColorNodeSelector.setCurrentNodeID(colorNode.GetID())
      # use the delay display here to ensure a render
      self.delayDisplay('Set Color Node To %s' % colorNode.GetName())
      useSelectedColorsCheckBox.setChecked(False)
      self.delayDisplay('Toggled using selected color node')
      useSelectedColorsCheckBox.setChecked(True)

    logging.info('Processing completed')

    useSelectedColorsCheckBox.setChecked(False)
    colorBar.SetVisibility(False)

    self.delayDisplay('Test passed!')

  def test_ColorsScalarBarSelfTest2(self):

    self.delayDisplay("Starting the color bar displayable manager test")

    logging.info('Processing started')

    import SampleData
    sampleDataLogic = SampleData.SampleDataLogic()
    ctVolumeNode = sampleDataLogic.downloadCTChest()
    self.assertIsNotNone( ctVolumeNode )

    self.delayDisplay('Test color bar visibility', self.delayMs)
    colorBar = slicer.modules.colors.logic().CreateAndObserveColorBarDisplayNode(ctVolumeNode)

    self.assertIsNotNone( colorBar )

    self.delayDisplay('Show color bar on all views and slices!', self.delayMs)

    # signal to displayable manager to show a created color bar
    colorBar.SetMaxNumberOfColors(256)
    colorBar.SetVisibility(True)

    # get Red slice node
    sliceWidgetRed = slicer.app.layoutManager().sliceWidget('Red')
    sliceNodeRed = sliceWidgetRed.mrmlSliceNode()
    self.assertIsNotNone( sliceNodeRed )

    # get 3D view node
    threeDViewWidget = slicer.app.layoutManager().threeDWidget(0)
    threeDViewNode = threeDViewWidget.mrmlViewNode()

    self.assertIsNotNone( threeDViewNode )

    self.delayDisplay('Show color bar on the Red slice and on the 3D view', self.delayMs)

    colorBar.SetViewNodeIDs([sliceNodeRed.GetID(), threeDViewNode.GetID()])

    self.delayDisplay('Show color bar on the 3D view only', self.delayMs)

    colorBar.SetViewNodeIDs([threeDViewNode.GetID()])

    self.delayDisplay('Test color bar on 3D view finished!', self.delayMs)

    sliceNameColor = {
      'Red': [ 1., 0., 0. ],
      'Green': [ 0., 1., 0. ],
      'Yellow': [ 1., 1., 0.]
      }
    for sliceName, titleColor in sliceNameColor.items():
      self.delayDisplay('Test color bar on the ' + sliceName + ' slice view', self.delayMs)
      # get slice node
      sliceWidget = slicer.app.layoutManager().sliceWidget(sliceName)
      sliceNode = sliceWidget.mrmlSliceNode()
      # show color bar only on a single slice node
      colorBar.SetViewNodeIDs([sliceNode.GetID()])
      colorBar.SetTitleText(sliceName)
      colorBar.SetTitleTextColorRGB(titleColor)
      self.delayDisplay('Test color bar on the ' + sliceName + ' slice view finished!',self.delayMs)

    colorBar.SetVisibility(False)

    logging.info('Processing completed')
    self.delayDisplay('Test passed!')
