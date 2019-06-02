#!/usr/bin/env kross

import os

import Kross

forms = Kross.module("forms")
assert 'createWidgetFromUIFile' in dir(forms)

dialog = forms.createDialog("TestGuiFormDialog")
dialog.setButtons("Ok|Cancel")
dialog.setFaceType("List")

options_page = dialog.addPage("Options", "Options", "configure")
options_ui = os.path.join(self.currentPath(), "testguiform.ui")
options_widget = forms.createWidgetFromUIFile(options_page, options_ui)

file_page = dialog.addPage("File", "Import From Image File", "fileopen")
file_widget = forms.createFileWidget(file_page, "kfiledialog:///mytestthingy1")
file_widget.setMode("Opening")
file_widget.setFilter("*.cpp|C++ Source Files\n*.h|Header files")

# Check radio buttons cannot be both checked
dialog['radioButton'].setChecked(True)
dialog['radioButton_2'].setChecked(True)
assert dialog['radioButton'].checked == 0
assert dialog['radioButton_2'].checked == 1

# Uncomment these to show the dialog.
# assert dialog.exec_loop() == 1
# assert dialog.result() == 'Ok'
