
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An empty box for storing all the statistics of the CR node. To be implemented via the new "Signals" in OMNeT++, when needed.
 */

#ifndef CRSTATS_H_
#define CRSTATS_H_
#include "BaseStats.h"
class crStats : public BaseStats
{
    private:
        // Application Layer Stats
        simsignal_t appRequests;
    protected:
        virtual void initialize();
};

Define_Module(crStats);

#endif /* CRSTATS_H_ */
