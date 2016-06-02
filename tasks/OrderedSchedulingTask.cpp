/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "OrderedSchedulingTask.hpp"
#include <algorithm>

using namespace std;
using RTT::Logger;

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

    typedef vector<string> PeerList;
    PeerList scheduling_order = _scheduling_order.get();

    // Add any peer not explicitely listed at the end of the scheduling order
    PeerList peers = this->getPeerList();
    if (peers.empty())
    {
        Logger::log(Logger::Error) << "OrderedSchedulingTask error: refusing to start with zero peers to trigger"
            << RTT::endlog();
        return false;
    }
    for (PeerList::const_iterator it = peers.begin(); it != peers.end(); it++)
    {
        if (find(scheduling_order.begin(), scheduling_order.end(), *it) == scheduling_order.end())
            scheduling_order.push_back(*it);
    }

    // Then resolve the names
    vector<TaskContext*> task_order;
    for (PeerList::const_iterator it = scheduling_order.begin(); it != scheduling_order.end(); it++)
    {
        TaskContext* peer = getPeer(*it);
        if (peer == NULL) {
            Logger::log(Logger::Error) << "OrderedSchedulingTask error: component '" << *it
                << "'in scheduling_order is not a peer."
                << RTT::endlog();
            return false;
        }
        task_order.push_back(peer);
    }

    this->scheduling_order = task_order;
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
    bool has_error = false;
    for (unsigned int i = 0; i < scheduling_order.size(); i++)
    {
        if (!scheduling_order[i]->update())
        {
            Logger::log(Logger::Error)
                << "OrderedSchedulingTask error: failed to trigger component #" << i
                << " '" << scheduling_order[i]->getName() << "'" << RTT::endlog();
            error(TRIGGER_FAILED);
            has_error = true;
        }
    }
    if (!has_error && state() == TRIGGER_FAILED)
        report(RUNNING);

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
