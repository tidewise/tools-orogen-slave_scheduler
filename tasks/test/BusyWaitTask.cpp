/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "BusyWaitTask.hpp"

using namespace slave_scheduler::test;

BusyWaitTask::BusyWaitTask(std::string const& name)
    : BusyWaitTaskBase(name)
{
}

BusyWaitTask::~BusyWaitTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See BusyWaitTask.hpp for more detailed
// documentation about them.

bool BusyWaitTask::configureHook()
{
    if (! BusyWaitTaskBase::configureHook())
        return false;
    return true;
}
bool BusyWaitTask::startHook()
{
    if (! BusyWaitTaskBase::startHook())
        return false;
    return true;
}
void BusyWaitTask::updateHook()
{
    BusyWaitTaskBase::updateHook();

    int32_t value;
    while (_in.read(value) != RTT::NewData) {
        usleep(10000);
    }
    if (value == 42) {
        return fatal();
    }
    _out.write(value);
}
void BusyWaitTask::errorHook()
{
    BusyWaitTaskBase::errorHook();
}
void BusyWaitTask::stopHook()
{
    BusyWaitTaskBase::stopHook();
}
void BusyWaitTask::cleanupHook()
{
    BusyWaitTaskBase::cleanupHook();
}
