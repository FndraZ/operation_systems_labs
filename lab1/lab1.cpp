#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex globalMutex;
condition_variable condVar;

bool ready = false;
int x = 0;

void Provide() {
    while (x < 10) {
        unique_lock<mutex> uniqueLock(globalMutex);
        while (ready) {
            condVar.wait(uniqueLock);
        }
        this_thread::sleep_for(chrono::seconds(1));
        x++;
        cout << "Sended  : " << x << " id: " << this_thread::get_id() << "\n";
        ready = true;
        condVar.notify_all();
        uniqueLock.unlock();
    }
}

void Consume() {
    while (x < 10) {
        unique_lock<mutex> uniqueLock(globalMutex);
        while (!ready)
        {
            condVar.wait(uniqueLock, []{ return ready; });
        }
        cout << "Recieved: " << x << " id: " << this_thread::get_id() << "\n";
        ready = false;
        condVar.notify_all();
    }
}

int main() {
    thread th1(Provide);
    thread th2(Consume);

    th1.join();
    th2.join();
}