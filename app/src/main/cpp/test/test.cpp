//
// Created by 赵云龙 on 1/30/25.
//

#include "test.h"
#include <thread>
#include <android/log.h>
#include <unistd.h>
#include "../util/logger.h"

Test::Test() {
    pthread_mutex_init(&m_mutex, nullptr);
    pthread_cond_init(&m_cond, nullptr);

    std::shared_ptr<Test> that(this);
    std::thread t(Decode, that);
    t.detach();
}

void Test::Decode(std::shared_ptr<Test> that) {

    usleep(9000*1000);
//    pthread_mutex_lock(&(that->m_mutex));
//    LOGI("123", "Begin Wait");
//    pthread_cond_wait(&(that->m_cond), &(that->m_mutex));
//    pthread_mutex_unlock(&(that->m_mutex));
    LOGI("123", "End Wait")
}
