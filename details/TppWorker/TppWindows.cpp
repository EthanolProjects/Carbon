#include <functional>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "Task.hpp"
namespace Carbon {    
    namespace TppDetail {
        struct Task::Impl {

        };
        Task::Task() {}
        Task::~Task() {}
        Task::Task(Task&&) {}
        Task& Task::operator = (Task&&) {}
        void Task::run() {}
    }
}