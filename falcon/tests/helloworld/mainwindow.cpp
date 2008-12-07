

#include <QVBoxLayout>
#include <QDebug>
#include "mainwindow.h"
 
#include <kross/core/manager.h>
#include <kross/core/action.h>
 
// the constructor.
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
  // Create the combobox where we display a list of
  // available scripts.
  cmbScripts = new QComboBox (this);
  cmbScripts->addItem("Choose script", "");
  
  // add some script that should be in this dir:
  
  cmbScripts->addItem( "Set property" );
  cmbScripts->addItem( "Show" );
  cmbScripts->addItem( "Hide" );
  cmbScripts->addItem( "Inspect" );
  cmbScripts->addItem( "setText" );

  // Connect the combobox signal with our slot to be able to
  // do something if the active item in the combobox changed.
  connect(cmbScripts, SIGNAL(activated(const QString &)),
          this, SLOT(scriptActivated(const QString &)));
 
  // The label we want to manipulate from within scripting
  // code.
  lblHello = new QLabel("Hello", this);
 
  // Put everything into a layout to have it shown in a
  // nice way.
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(cmbScripts);
  layout->addWidget(lblHello);
  setLayout(layout);
}
 
// this slot is called when the active item of the combobox changes
void MainWindow::scriptActivated(const QString &strSelectedScript)
{
  if(strSelectedScript.isEmpty())
  {
    // if no script was selected, we display nothing.
    lblHello->setText("-");
    return;
  }
 
  // Now let's create a Kross::Action instance which will act
  // as container for our script. You are also able to cache
  // that action, manipulate it on demand or execute it multiple
  // times.
  Kross::Action action(this, "MyScript");
  
  // Now let's set the scripting code that should be executed
  // depending on the choosen script. You are also able to
  // use action.setFile("/path/scriptfile") here to execute
  // an external scriptfile, as shown later in this tutorial.
  if(strSelectedScript == "Set property")
    action.setCode("MyLabel.text = 'Property changed'");
  else if(strSelectedScript == "Show")
    action.setCode("MyLabel.setVisible(true)");
  else if(strSelectedScript == "Hide")
    action.setCode("MyLabel.setVisible(false)");
  else if(strSelectedScript == "Inspect")
    action.setCode("inspect( MyLabel )");
  else if(strSelectedScript == "setText")
    action.setCode("MyLabel.setText( 'Text now changed' )");
  else
    return;
 
  // Set the name of the scripting engine that should be used to
  // evaluate the scripting code above.
  action.setInterpreter("falcon");
 
  // Now let's add the QLabel instance to let the scripting code
  // access it.
  action.addObject(lblHello, "MyLabel");
 
  // Finally execute the scripting code.
  action.trigger();
}

