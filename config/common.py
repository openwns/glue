import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'glue',
                            PROJMODULES    = ['TEST', 'BASE'],
                            FLATINCLUDES   = False,
                            LIBRARY        = True,
                            SHORTCUTS      = True,
                            DEFAULTVERSION = True,
			    REVISIONCONTROL = RCS.GNUArch('../', 'glue', 'unstable', '0.3'),
                            )
Return('commonEnv')
