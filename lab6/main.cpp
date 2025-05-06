//
// Created by Michał on 06.05.2025.
//
#include <iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<atomic>
#include<random>
//Zadanie zrobione dwojako poniewaz nie rozumialem dokladnie polecenia

std::condition_variable waitForTask;
std::mutex taskMutex;
std::mutex completionMutex;
std::condition_variable waitForCompletion;
std::atomic<bool> completed{false};
std::atomic<bool> generated{false};
std::atomic<bool> timeout{false};
std::string generatedString;

void generator() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis('A', 'Z');

    {
        generatedString.clear();
        for (int i = 0; i < 4; ++i) {
            generatedString.push_back(static_cast<char>(dis(gen)));
        }
        std::cout << "Type this to win : " << generatedString << std::endl;
        generated.store(true);
    }
    waitForTask.notify_one();
    {   auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        std::unique_lock<std::mutex> lock(completionMutex);
        bool result= waitForCompletion.wait_until(lock,deadline,[](){return completed.load();});
        if (!result) {
            std::cout<<"U lose!";
            timeout.store(true);
        }
        else {
            std::cout<<"U win! \n";
            return;
        }
    }
}
void player() {
    std::unique_lock<std::mutex> lock(taskMutex);
    waitForTask.wait(lock, [] { return generated.load(); });
    generated.store(false);
    while (!timeout.load()) {
        std::string userInput;
       // std::cout << "Enter string: ";
        std::cin >> userInput;
        if (timeout.load()) {continue;}
        if (userInput == generatedString) {
            completed.store(true);
            waitForCompletion.notify_one();
            break;
        }
    }
}

class BankAccount {
    std::mutex lock;
    int saldo;
    int transactions;
    public:
    BankAccount(int saldo):saldo(saldo) {}
    bool getMoney(int amount) {
        lock.lock();
        if (saldo-amount>0) {
            saldo -= amount;
            std::cout<<"Wyplacono pieniedzy: "<<amount<<" Pozostalo: "<<saldo<<"\n";
            transactions+=1;
            lock.unlock();
            return true;
        }
        else if (saldo-amount==0) {
            saldo-=amount;
            std::cout<<"Wyplacono pieniedzy: "<<amount<<" Pozostalo: "<<saldo<<"\n";
            transactions+=1;
            lock.unlock();
            return true;
        }
        lock.unlock();
        return false;
    }
    void summarizeTransactions() {
        std::cout<<transactions<<"\n";
    }
};
#include <random>
#include<array>
void tryToGetMoney(std::stop_token st,BankAccount * account) {
    static std::array<int,3> amounts{100,200,300};
    while (true) {
        if (!account->getMoney(amounts[rand()%amounts.size()])) {
            break;
        }
    }
}

namespace versionOne {
    std::mutex streamMutex;
    std::mutex waitForReadyMtx;
    std::condition_variable readyExchange;
    std::atomic<bool> ready{false};
    void even() {
        for (int i=0;i<100;i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            streamMutex.lock();
            std::cout<<i*2<<std::endl;
            streamMutex.unlock();
            if (i==50) {
                ready.store(true);
                readyExchange.notify_all();
            }
        }

    }

    void odd() {
        for (int i=0;i<100;i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            streamMutex.lock();
            std::cout<<i*2+1<<std::endl;
            streamMutex.unlock();
            if (i==50) {
                ready.store(true);
                readyExchange.notify_all();
            }
        }
    }
}
namespace versionTwo {
    std::atomic<bool> ready{false};
    std::atomic<bool> turn{true};
    std::mutex streamMutexA;
    std::mutex streamMutexB;
    std::mutex waitForExchangemtx;
    std::condition_variable switchTurn;
    std::condition_variable readyExchange;
    //false
    void even() {
        for (int i=0;i<100;i++) {
                std::unique_lock<std::mutex> lock(streamMutexA);
                switchTurn.wait(lock,[](){return turn.load();});
                std::cout<<i*2<<"\n";

                turn.store(false);
                if (i==50) {
                    ready.store(true);
                    readyExchange.notify_all();
                }
                switchTurn.notify_all();

        }
        turn.store(false);
        switchTurn.notify_all();
    }
    void odd() {
        for (int i=0;i<100;i++) {
                std::unique_lock<std::mutex> lock(streamMutexB);
                switchTurn.wait(lock,[](){return !turn.load();});
                std::cout<<i*2+1<<"\n";
                turn.store(true);
            if (i==50) {
                ready.store(true);
                readyExchange.notify_all();
            }
                switchTurn.notify_all();

        }
        turn.store(true);
        switchTurn.notify_all();
    }
}
//#define version1
//#define version2
#define banks
//#define game
std::mutex managerControl;
int main() {
#ifdef version1
    std::thread evenThread(versionOne::even);
    std::thread oddThread(versionOne::odd);
    std::unique_lock<std::mutex> lock(versionOne::waitForReadyMtx);
    versionOne::readyExchange.wait(lock,[](){return versionOne::ready.load();});
    evenThread.swap(oddThread);
    evenThread.join();
    oddThread.join();
#endif
#ifdef version2
    std::thread evenThread2(versionTwo::even);
    std::thread oddThread2(versionTwo::odd);
    std::unique_lock<std::mutex> lock(versionTwo::waitForExchangemtx);
    versionTwo::readyExchange.wait(lock,[](){return versionTwo::ready.load();});
    evenThread2.swap(oddThread2);
    evenThread2.join();
    oddThread2.join();
#endif
#ifdef banks
    BankAccount account1(1000);
    std::jthread p1(tryToGetMoney,&account1);
    std::jthread p2(tryToGetMoney,&account1);
    std::jthread p3(tryToGetMoney,&account1);
    p1.join();
    p2.join();
    p3.join();
    account1.summarizeTransactions();
    std::cout<<"Bank account empty!";
#endif
#ifdef game
    while (true) {
        timeout.store(false);
        generated.store(false);
        completed.store(false);
        std::thread w1(player);
        std::thread w2(generator);
        w1.join();
        w2.join();
    }
#endif
    return 0;
}