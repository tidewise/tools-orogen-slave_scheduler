# Specific scheduling classes for oroGen components

The components in this package implement specific scheduling classes. They are
meant to be used as 'master' where the components to be scheduled are the
slaves.

To use them, you need to create specific deployments, and add the slave tasks as
`slave_of` the masters, for instance:

~~~ ruby
deployment 'control_deployment' do
    tasks = [
        task('joint_multiplexer', 'Multiplexer'),
        task('motor0', 'motors_elmo_ds402::Task'),
        task('motor1', 'motors_elmo_ds402::Task'),
        task('motor2', 'motors_elmo_ds402::Task'),
        task('motor3', 'motors_elmo_ds402::Task'),
        task('joint_demultiplexer', 'Demultiplexer')
    ]

    scheduler = task('control_scheduler', "slave_scheduler::OrderedSchedulingTask").
    	periodic(0.001)
    tasks.each { |t| t.slave_of(scheduler) }

    add_default_logger
end
~~~

Syskit has built-in support for these setups, i.e. it will auto-start the
scheduling component. 

## OrderedSchedulingTask

Runs its slaves in order. The `scheduling_order` property defines in which order
, the names being the task names defined in the deployment. All peers have to
specified in the order. Note that components that are stopped will be
triggered but won't do nothing (obviously).

The component will validate that all elements of `scheduling_order` exist, and
that all the master's slaves are triggered.
