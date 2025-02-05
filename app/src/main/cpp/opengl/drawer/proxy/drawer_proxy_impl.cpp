//
// Created by 赵云龙 on 2/5/25.
//

#include "drawer_proxy_impl.h"

void DrawerProxyImpl::AddDrawer(Drawer *drawer) {
    m_drawer.push_back(drawer);
}

void DrawerProxyImpl::Draw() {
    for (int i = 0; i < m_drawer.size(); ++i) {
        m_drawer[i]->Draw();
    }
}

void DrawerProxyImpl::Release() {
    for (int i = 0; i < m_drawer.size(); ++i) {
        m_drawer[i]->Release();
        delete m_drawer[i];
    }
    m_drawer.clear();
}
