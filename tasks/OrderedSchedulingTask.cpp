/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "OrderedSchedulingTask.hpp"

using namespace slave_scheduler;

OrderedSchedulingTask::OrderedSchedulingTask(std::string const& name)
    : OrderedSchedulingTaskBase(name)
{
}

OrderedSchedulingTask::OrderedSchedulingTask(std::string const& name, RTT::ExecutionEngine* engine)
    : OrderedSchedulingTaskBase(name, engine)
{
}

OrderedSchedulingTask::~OrderedSchedulingTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See OrderedSchedulingTask.hpp for more detailed
// documentation about them.

bool OrderedSchedulingTask::configureHook()
{
    if (! OrderedSchedulingTaskBase::configureHook())
        return false;
    return true;
}
bool OrderedSchedulingTask::startHook()
{
    if (! OrderedSchedulingTaskBase::startHook())
        return false;
    return true;
}
void OrderedSchedulingTask::updateHook()
{
    OrderedSchedulingTaskBase::updateHook();
}
void OrderedSchedulingTask::errorHook()
{
    OrderedSchedulingTaskBase::errorHook();
}
void OrderedSchedulingTask::stopHook()
{
    OrderedSchedulingTaskBase::stopHook();
}
void OrderedSchedulingTask::cleanupHook()
{
    OrderedSchedulingTaskBase::cleanupHook();
}
