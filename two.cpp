#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
using namespace std;
std::mutex mymutex;
void sayHello() {
  int k = 0;
  unique_lock<mutex> lock(mymutex);
  while (k < 2) {
    k++;
    cout << endl << "hello" << endl;
    sleep(2);
  }
}
void sayWorld() {
  unique_lock<mutex> lock(mymutex);
  while (1) {
    cout << endl << "world" << endl;
    sleep(1);
  }
}
int main() {
  thread threadHello(&sayHello);
  thread threadWorld(&sayWorld);
  threadHello.join();
  threadWorld.join();
  return 0;
}