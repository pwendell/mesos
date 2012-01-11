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

#include <sys/time.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
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
  fstream *filestr;

  ThreadArg(SleepExecutor* executor_, TaskDescription task_, int threadId_,
            double usec_, fstream *filestr_)
    : executor(executor_), task(task_), threadId(threadId_),
      usec(usec_), filestr(filestr_) {}
};

void* runTask(void* arg);

class SleepExecutor : public Executor
{
public:
  ExecutorDriver* driver;
  fstream filestr;
  virtual ~SleepExecutor() {}

  virtual void init(ExecutorDriver* driver, const ExecutorArgs &args) {
    this->driver = driver;
    filestr.open("/tmp/sparrow.txt", fstream::app);
  }

  virtual void launchTask(ExecutorDriver*, const TaskDescription& task) {
    double duration;
    istringstream in(task.data());

    cout << "Task: " << task.task_id().value() <<  "Sleep: " << duration << endl;
    in >> duration;

    if (duration < 0.0) { duration = 0.0; }
    double usec = duration * 1000.0;
   
    ThreadArg* arg = new ThreadArg(this, task, 0, usec, &filestr); 
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
  struct timeval start, end;
  gettimeofday(&start, NULL);
  usleep(static_cast<int>(arg->usec));
  TaskStatus status;
  status.mutable_task_id()->MergeFrom(arg->task.task_id());
  status.set_state(TASK_FINISHED);
  arg->executor->driver->sendStatusUpdate(status);
  gettimeofday(&end, NULL);
  long sec = end.tv_sec - start.tv_sec;
  long usec = end.tv_usec - start.tv_usec;
  cout << "START: " << (start.tv_sec * 1000) + (start.tv_usec / 1000) << endl;
  cout << "Diff: " << (sec * 1000)  + (usec / 1000) << " now " << (end.tv_sec * 1000) + (end.tv_usec / 1000) << endl;
  fflush(stdout);

}

int main(int argc, char** argv)
{
  SleepExecutor exec;
  MesosExecutorDriver driver(&exec);
  driver.run();
  return 0;
}
