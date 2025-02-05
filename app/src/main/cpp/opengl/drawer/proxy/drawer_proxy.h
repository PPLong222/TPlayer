//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_DRAWER_PROXY_H
#define TPLAYER_DRAWER_PROXY_H


#include "../drawer.h"

class DrawerProxy {
public:
    virtual void AddDrawer(Drawer *drawer) = 0;

    virtual void Draw() = 0;

    virtual void Release() = 0;

    virtual ~DrawerProxy() = default;
};


#endif //TPLAYER_DRAWER_PROXY_H
