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

#ifndef __REAPER_HPP__
#define __REAPER_HPP__

#include <set>

#include <process/process.hpp>


namespace mesos { namespace internal { namespace slave {

class ProcessExitedListener : public process::Process<ProcessExitedListener>
{
public:
  virtual void processExited(pid_t pid, int status) = 0;
};


class Reaper : public process::Process<Reaper>
{
public:
  Reaper();
  virtual ~Reaper();

  void addProcessExitedListener(const process::PID<ProcessExitedListener>&);

protected:
  virtual void operator () ();

private:
  std::set<process::PID<ProcessExitedListener> > listeners;
};


}}} // namespace mesos { namespace internal { namespace slave {

#endif // __REAPER_HPP__
