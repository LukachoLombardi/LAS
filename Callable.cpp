#include "Callable.h"

void Callable::onFinish() {}

void Callable::finish() {
  taskPtr->isActive = false;
}

void Callable::operator()() {
  return run();
}