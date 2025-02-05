//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_DRAWER_PROXY_IMPL_H
#define TPLAYER_DRAWER_PROXY_IMPL_H


#include "drawer_proxy.h"
#include <vector>

class DrawerProxyImpl : public DrawerProxy {
private:
    std::vector<Drawer *> m_drawer;
public:
    void AddDrawer(Drawer *drawer) override;
    void Draw() override;
    void Release() override;
};


#endif //TPLAYER_DRAWER_PROXY_IMPL_H
