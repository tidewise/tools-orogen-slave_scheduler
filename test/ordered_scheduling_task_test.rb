# frozen_string_literal: true

using_task_library "slave_scheduler"

describe OroGen.slave_scheduler.OrderedSchedulingTask do
    run_live

    before do
        Syskit.conf.opportunistic_recovery_from_quarantine = false
    end

    after do
        Syskit.conf.opportunistic_recovery_from_quarantine = true
    end

    describe "configure" do
        attr_reader :task

        before do
            @task = syskit_deploy(OroGen.slave_scheduler.OrderedSchedulingTask
                                        .deployed_as("task_under_test"))
        end

        it "fails if the peer list is empty" do
            task.properties.scheduling_order = []
            expect_execution.scheduler(true).to { fail_to_start task }
        end

        it "fails if some of the peers do not exist" do
            task.properties.scheduling_order = %w[a x]
            expect_execution.scheduler(true).to { fail_to_start task }
        end
    end

    describe "triggering" do
        attr_reader :a_task, :b_task, :scheduler_task

        before do
            test_m = OroGen.slave_scheduler.test.BusyWaitTask
            syskit_stub_conf(test_m, "a")
            syskit_stub_conf(test_m, "b")

            @a_task, @b_task = syskit_deploy(
                test_m
                    .with_conf("a")
                    .use_deployment(OroGen::Deployments.ordered_scheduling_test)
                    .prefer_deployed_tasks("a"),
                test_m
                    .with_conf("b")
                    .use_deployment(OroGen::Deployments.ordered_scheduling_test)
                    .prefer_deployed_tasks("b")
            )
            refute_same @a_task, @b_task
            @scheduler_task, = a_task.each_child.first
        end

        after do
            if b_task.fatal_error?
                expect_execution { a_task.execution_agent.kill! }
                    .to do
                        emit a_task.aborted_event
                        emit scheduler_task.aborted_event
                        emit a_task.execution_agent.kill_event
                    end
            end
        end

        it "schedules tasks in the order given to the peer list" do
            scheduler_task.properties.scheduling_order = %w[a b]

            syskit_configure_and_start(scheduler_task)
            syskit_configure_and_start(a_task)
            syskit_configure_and_start(b_task)

            expect_execution.timeout(0.5).to do
                have_no_new_sample a_task.out_port
                have_no_new_sample b_task.out_port
            end
            expect_execution { syskit_write b_task.in_port, 10 }.timeout(0.5).to do
                have_no_new_sample a_task.out_port
                have_no_new_sample b_task.out_port
            end
            expect_execution { syskit_write a_task.in_port, 10 }.timeout(0.5).to do
                have_one_new_sample a_task.out_port
                have_one_new_sample b_task.out_port
            end
        end

        it "transitions to TRIGGER_FAILED if a task goes into FATAL, and "\
           "continues triggering other tasks" do
            scheduler_task.properties.scheduling_order = %w[a b]

            syskit_configure_and_start(scheduler_task)
            syskit_configure_and_start(a_task)
            syskit_configure_and_start(b_task)

            plan.add_permanent_task(scheduler_task)

            # Make 'b' go into FATAL so that we exit the scheduler's loop and
            # 'a' is next to be scheduled
            expect_execution do
                syskit_write a_task.in_port, 10
                syskit_write b_task.in_port, 42
            end.timeout(0.5).to do
                have_one_new_sample a_task.out_port
                emit b_task.fatal_error_event
                emit scheduler_task.trigger_failed_event
            end

            expect_execution { syskit_write a_task.in_port, 11 }.timeout(0.5).to do
                have_one_new_sample a_task.out_port
            end
        end

        it "does not emit TRIGGER_FAILED when already in that state" do
            scheduler_task.properties.scheduling_order = %w[a b]

            syskit_configure_and_start(scheduler_task)
            syskit_configure_and_start(a_task)
            syskit_configure_and_start(b_task)

            # Make 'b' go into FATAL so that we exit the scheduler's loop and
            # 'a' is next to be scheduled
            expect_execution do
                syskit_write a_task.in_port, 10
                syskit_write b_task.in_port, 42
            end.to do
                emit b_task.fatal_error_event
                emit scheduler_task.trigger_failed_event
            end

            expect_execution.to do
                poll do
                    # Needed to actually trigger
                    syskit_write a_task.in_port, 11
                end
                not_emit scheduler_task.trigger_failed_event, within: 5
                not_emit scheduler_task.running_event, within: 5
            end
        end
    end
end
