/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <time.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <mesos/executor.hpp>

using namespace mesos;
using namespace std;


class SleepExecutor; 

struct ThreadArg
{
  SleepExecutor* executor;
  TaskDescription task;
  int threadId;
  double usec;

  ThreadArg(SleepExecutor* executor_, TaskDescription task_, int threadId_,
            double usec_)
    : executor(executor_), task(task_), threadId(threadId_),
      usec(usec_) {}
};

void* runTask(void* arg);

class SleepExecutor : public Executor
{
public:
  ExecutorDriver* driver;

  virtual ~SleepExecutor() {}

  virtual void init(ExecutorDriver* driver, const ExecutorArgs &args) {
    this->driver = driver;
  }

  virtual void launchTask(ExecutorDriver*, const TaskDescription& task) {
    double duration;
    istringstream in(task.data());

    cout << "Task: " << task.task_id().value() <<  "Sleep: " << duration << endl;
    in >> duration;

    if (duration < 0.0) { duration = 0.0; }
    double usec = duration * 1000.0;
   
    ThreadArg* arg = new ThreadArg(this, task, 0, usec); 
    pthread_t thread;
    pthread_create(&thread, 0, runTask, arg);
    pthread_detach(thread);

    //TaskStatus status;
    //status.mutable_task_id()->MergeFrom(task.task_id());
    //status.set_state(TASK_RUNNING);

    //driver->sendStatusUpdate(status);
  }

  virtual void killTask(ExecutorDriver* driver, const TaskID& taskId) {}

  virtual void frameworkMessage(ExecutorDriver* driver,
                                const string& data) {}

  virtual void shutdown(ExecutorDriver* driver) {}

  virtual void error(ExecutorDriver* driver, int code,
                     const std::string& message) {}
};

void* runTask(void* threadArg) {
  ThreadArg* arg = (ThreadArg*) threadArg;
  cout << "Task: " << arg->task.task_id().value() <<  "Sleep: " << arg->usec << endl;
  
  usleep(static_cast<int>(arg->usec));
  TaskStatus status;
  status.mutable_task_id()->MergeFrom(arg->task.task_id());
  status.set_state(TASK_FINISHED);
  arg->executor->driver->sendStatusUpdate(status);

}

int main(int argc, char** argv)
{
  SleepExecutor exec;
  MesosExecutorDriver driver(&exec);
  driver.run();
  return 0;
}
