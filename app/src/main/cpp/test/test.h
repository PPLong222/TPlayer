//
// Created by 赵云龙 on 1/30/25.
//

#ifndef TPLAYER_TEST_H
#define TPLAYER_TEST_H

#include <thread>

class Test {
public:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    Test();
    static void Decode(std::shared_ptr<Test> that);
};


#endif //TPLAYER_TEST_H
