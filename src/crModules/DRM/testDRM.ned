import crSimulator.core.*;

package crSimulator.crModules.DRM;

simple testDRM extends BaseDrm like IDRM
{
    bool newData = default(true);
    @display("i=abstract/db");
    @class(testDRM);
}