/***************************************************************************
 * pythonmanager.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonmanager.h"

//#include <kexidb/drivermanager.h>
//#include <kexidb/driver.h>
//#include <kexidb/connection.h>

#include "../kexidb/pythonkexidb.h"
#include "../kexidb/pythonkexidbdriver.h"

using namespace Kross;

namespace Kross
{
    class PythonManagerPrivate
    {
        public:
            PyThreadState *gtstate;
    };
}

PythonManager::PythonManager(char* name)
{
    d = new PythonManagerPrivate();
    Py_SetProgramName(name);
    Py_Initialize();
    PyEval_InitThreads();
    d->gtstate = PyEval_SaveThread();
    if(! d->gtstate) {
        kdWarning() << "PyEval_SaveThread() failed" << endl;
        return;
    }
}

PythonManager::~PythonManager()
{
    if(d->gtstate) {
        PyEval_AcquireThread(d->gtstate);
        Py_Finalize();
    }
    delete d;
}

bool PythonManager::execute(const QString& execstring, const QStringList& modules)
{
    if(execstring.isEmpty() || ! d->gtstate) return false;

    PyEval_AcquireLock();
    PyThreadState *tstate = Py_NewInterpreter();
    if(! tstate) {
        kdWarning() << "Py_NewInterpreter() failed" << endl;
        return false;
    }

    //PySys_SetArgv(argc, argv);

    Kross::PythonKexiDB* kexidb = 0;

    if(modules.contains("kexidb")) {
        kexidb = new Kross::PythonKexiDB();
        //Py::Module module = kexidb->module();
        //std::cout << "module->as_string() = " << module.as_string() << std::endl;
    }

    PyRun_SimpleString(execstring.latin1());

    delete kexidb;

    Py_EndInterpreter(tstate);
    PyEval_ReleaseLock();

    return true;
}
