
import crSimulator.core.*;

package crSimulator.crModules.DRM;

simple mysqlDRM extends BaseDrm like IDRM
{
    bool newData = default(true);
    @display("i=abstract/db");
    @class(mysqlDRM);
}