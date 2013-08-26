

package crSimulator.crModules.DRM;

import crSimulator.core.*;

package crSimulator.crModules.DRM;

simple sqliteDRM extends BaseDrm like IDRM
{
    bool newData = default(true);
    @display("i=abstract/db");
    @class(sqliteDRM);
}